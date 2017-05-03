#include <stdio.h>
#include <stdint.h>

/*
 * This is the original carvedToWritten
 */
int64_t carvedToWritten(int64_t n)
{
    int64_t r = 0;
    for (int64_t i = 0; i < 64; ++i)
    {
        int64_t a = 0;
        for (int64_t j = n; j >= 0; --j)
        {
            int64_t b = 0;
            for (int64_t k = 0; k <= i; ++k)
            {
                int64_t c = a ^ ((i & (n ^ j) & 1) << k);
                a ^= (j & (1LL << k)) ^ b;
                b = (((c & j) | ((c ^ j) & b)) & (1LL << k)) << 1;
            }
        }
        r |= (a & (1LL << i));
    }
    return r;
}

/*
 * In the original algorithm the odd bits of r follow this pattern, this
 * is easy to spot after laying out the binary numbers on a spreadsheet
 *
 * r.i = (n + 1).(i+1)  for i%2==1
 *
 * For the even bits you have to unroll the k loop for a given i,
 * then it becomes clear that the boolean logic looks like a full
 * adder calculating a = a + j for j from 0 to n, which can be reduced
 * to n(n+1)/2, so now our function is O(1).
 *
 * r.i = ((n+1)*n/2).i  for i%2==0
 */
uint64_t carvedToWrittenSimp(uint64_t n)
{
    uint64_t r = 0;
    for (int i = 0; i < 64; i++) {
        if (i % 2 == 0) {   // 0, 2, 4, 6, ..
            r |= (n * (n + 1) / 2) & (1ULL << i);
        } else {    // 1, 3, 5, 7, ...
            r |= ((n + 1) >> 1) & (1ULL << i);
        }
    }
    return r;
}

/*
 * By reversing the formula above, even bits 'i' of (n+1) must satisfy:
 *
 * (n+1).i = r.(i-1) for i%2==0
 *
 * except for i=0, rince r.(0-1) isn't defined.
 *
 * So we'll bruteforce the even bits (n+1).i (i%2==0) plus (n+1).0 so that
 * the other equation holds.
 */
uint32_t writtenToCarved(uint64_t r) {
    
    uint32_t half_n_plus1 = ((uint32_t)r & 0xAAAAAAAA) << 1;
    for (uint32_t bf = 0; bf < 65536; bf++) {
        uint32_t n_plus1 = half_n_plus1;
        for (int i = 0; i < 16; i ++) {
            n_plus1 |= (bf & (1 << i)) << (i + 1);
        }
        if (carvedToWrittenSimp(n_plus1 - 1) == r)  return n_plus1 - 1;
        if (carvedToWrittenSimp(n_plus1) == r) return n_plus1;
    }
    return 0;
}

/*
 * Helper function to print binary
 */
void b(int64_t n) {
    for (int64_t i = 1LL << 31; i > 0; i >>= 1) {
        if (i & n) {
            putchar('1');
        } else {
            putchar('0');
        }
        putchar(',');
    }
}

int main(int argc, const char * argv[]) {
    int nc;
    scanf("%d", &nc);
    for (int c = 0; c < nc; c++) {
        uint64_t written;
        scanf("%llu", &written);
        uint32_t carved = writtenToCarved(written);
        printf("Case #%d: ", c + 1);
        if (carved == 0) {
            printf("IMPOSSIBLE\n");
        } else {
            printf("%u\n", carved);
        }
    }
    return 0;
}
