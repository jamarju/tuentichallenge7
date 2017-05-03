/**
 * This is the main program, it relies on nc for socket comms and
 * count.py for opencv coin-counting black magic.
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/wait.h>

#define Q "So, how much money$ do I have, bro?\n"
#define QLEN (strlen(Q))
#define A "Thank you, bro!\n"
#define ALEN (strlen(A))

#define ST_WAIT_MONEY 0
#define ST_WAIT_ANS 1
#define ST_DONE 2

#define TRACKER_HYST

typedef struct {
    int center;
    
} tracker_t;



int
process_image(int round) {
    uint8_t *img;
    char buf[128];
    FILE *fin, *fout;
    int width, height, maxval;
    sprintf(buf, "convert moneyz-%02d.jpg pgm:moneyz-%02d.pgm", round, round);
    system(buf);
    
    sprintf(buf, "moneyz-%02d.pgm", round);
    fin = fopen(buf, "r");
    fscanf(fin, "%s", buf); // P5
    fscanf(fin, "%d %d %d", &width, &height, &maxval);
    
    fgetc(fin);    // 0x0a
    
    img = (uint8_t *) malloc(width * height);
    for (int i = 0; i < width * height; i++) {
        img[i] = fgetc(fin);
    }
    fclose(fin);
    
    // unfuck pic
    int flipx = 40;
    int flipsize = 40;
    while (flipx < width) {
        if (flipx + flipsize >= width)
            flipsize = width - flipx;
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < flipsize / 2; x++) {
                uint8_t tmp = img[y * width + flipx + x];
                img[y * width + flipx + x] = img[y * width + flipx + flipsize - 1 - x];
                img[y * width + flipx + flipsize - 1 - x] = tmp;
            }
        }
        flipx += 80;
    }
    
    // save
    sprintf(buf, "fixed-%02d.pgm", round);
    fout = fopen(buf, "w");
    fprintf(fout, "P5\n%d %d\n255\n", width, height);
    
    for (int i = 0; i < width * height; i++) {
        fputc(img[i], fout);
    }
    fclose(fout);
    
    // find coins
    int cp[2];
    pipe(cp);
    pid_t pid;

    if ((pid == fork()) == 0) {
        // child
        close(1);
        dup(cp[1]);
        execlp("python", "python", "count.py", buf, (void *)0);
    }

    int ncin = cp[0];
    close(cp[1]);

    int buf_len = 0;
    while (read(ncin, buf + buf_len, 1) > 0) {
        buf_len++;
        if (buf[buf_len] == '\n') {
            buf[buf_len] = '\0';
            break;
        }
    }
    close(cp[0]);
    int st;
    wait(&st);
    return atoi(buf);    
}

int main(int argc, const char * argv[]) {
    int st = ST_WAIT_MONEY;
    char buffer[1024 * 1024];
    char ans[1024];
    int buffer_len = 0;
    FILE *fout;
    pid_t pid;
    int round = 0;
    
    int pc[2], cp[2];

    (void)pipe(pc);
    (void)pipe(cp);
    
    if ((pid = fork()) == 0) {
        // child
        close(0);   // close stdin
        close(1);   // close stdout
        dup(pc[0]);
        dup(cp[1]);
        
        execlp("nc", "nc", "52.49.91.111", "3456", (void *)0);
    }

    int ncin = cp[0];
    int ncout = pc[1];
    
    close(pc[0]);   // close receiver ends
    close(cp[1]);
    
    while (read(ncin, buffer + buffer_len, 1) > 0) {
        buffer_len++;
        switch (st) {
            case ST_WAIT_MONEY:
                if (buffer_len >= QLEN && memcmp(buffer + buffer_len - QLEN, Q, QLEN) == 0) {
                    char f[64];
                    sprintf(f, "moneyz-%02d.jpg", round);
                    fout = fopen(f, "w");
                    fwrite(buffer, 1, buffer_len - QLEN, fout);
                    fflush(fout);
                    fclose(fout);
                    
                    fwrite(buffer + buffer_len - QLEN, 1, QLEN, stdout);
                    
                    int moneyz = process_image(round);
                    
                    printf("OpenCV black magic says %d moneyz\n", moneyz);
                    sprintf(ans, "%d\n", moneyz);
                    write(ncout, ans, strlen(ans));
                    
                    buffer_len = 0;
                    round++;
                    
                    if (round <= 7) {                    
                        st = ST_WAIT_ANS;
                    } else {
                        st = ST_DONE;
                    }
                }
                break;
            case ST_WAIT_ANS:
                putchar(buffer[buffer_len - 1]);
                if (buffer[buffer_len - 1] == '\n') {
                    st = ST_WAIT_MONEY;
                    buffer_len = 0;
                }
                break;
            case ST_DONE:
                putchar(buffer[0]);
                buffer_len = 0;
        }
    }
}
