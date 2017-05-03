#include <limits.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>


#define MAX_COLORS 1024
#define MAX_COST 0xffffffff
#define PRUNE_HEURISTIC 20000


/**********************************************************************
 * structs
 **********************************************************************/

struct _galaxy_t; // fwd-declare cyclic struct

typedef struct {
    struct _galaxy_t *from;
    struct _galaxy_t *to;
    uint32_t color;
} wormhole_t ;

typedef struct {
    uint32_t color;
    uint32_t cost;
} energy_t ;

typedef struct _galaxy_t {
    int index;
    int num_energies;
    energy_t *energies;
    int num_wormholes;
    wormhole_t *wormholes;
    uint32_t min_cost;
    unsigned int visit_log[MAX_COLORS];
} galaxy_t ;

typedef struct{
    int case_number;
    int num_colors;
    uint32_t *colors;
    char **color_names;
    int num_galaxies;
    galaxy_t *galaxies;
} test_case_t;


/**********************************************************************
 * globals
 **********************************************************************/

test_case_t *test_cases;
int superset_size[MAX_COLORS];
int *superset[MAX_COLORS];

/**********************************************************************
 * constructors
 **********************************************************************/

void
init_galaxy(galaxy_t *galaxies, int index) {
    galaxies[index].index = index;
    galaxies[index].min_cost = MAX_COST;
    for (int i = 0; i < MAX_COLORS; i++) {
        galaxies[index].visit_log[i] = MAX_COST;
    }
}

void
init_supersets() {
    // a superset of color ci are all the colors cj that contain color ci
    // this helps when pruning galaxies.
    
    for (int i = 0; i < MAX_COLORS; i++) {
        superset_size[i] = 0;
        for (int j = 0; j < MAX_COLORS; j++) {
            // if color j contains i, j is a superset of i
            if ((j & i) == i) {
                superset_size[i]++;
            }
        }

        superset[i] = (int *) malloc(superset_size[i] * sizeof(int));
        superset_size[i] = 0;
        for (int j = 0; j < MAX_COLORS; j++) {
            if ((j & i) == i) {
                superset[i][superset_size[i]++] = j;
            }
        }
    }
}

/**********************************************************************
 * destructors
 **********************************************************************/

// that's what we want an os for


/**********************************************************************
 * parser
 **********************************************************************/

uint32_t
find_color(test_case_t *test_case, char *color) {
    for (int i = 0; i < test_case->num_colors; i++) {
        if (!strcmp(color, test_case->color_names[i])) {
            return test_case->colors[i];
        }
    }
    fprintf(stderr, "PANIC @find_color\n");
    exit(1);
}

int
parse(char *filename) {
    char buf[4096];
    FILE *fin = fopen(filename, "r");
    if (fin == NULL) {
        fprintf(stderr, "wrong filename\n");
        exit(1);
    }
    
    // read test cases
    int nc;
    fscanf(fin, "%d", &nc);
    test_cases = (test_case_t *) calloc(nc, sizeof(test_case_t));
    for (int c = 0; c < nc; c++) {
        test_case_t *tc = test_cases + c;
        tc->case_number = c + 1;
        
        // read colors
        int nk;
        fscanf(fin, "%d", &nk);
        tc->colors = (uint32_t *) calloc(nk, sizeof(uint32_t));
        tc->color_names = (char **) calloc(nk, sizeof(char *));
        uint32_t next_color = 1;
        for (int k = 0; k < nk; k++) {
            // read color
            fscanf(fin, "%s", buf);
            int num_primary_colors;
            fscanf(fin, "%d", &num_primary_colors);
            tc->color_names[k] = (char *) malloc(sizeof(char) * (strlen(buf) + 1));
            strcpy(tc->color_names[k], buf);
            if (num_primary_colors == 0) {
                // it's primary
                tc->colors[k] = next_color;
                next_color <<= 1;
            } else {
                // it's composite
                for (int pc = 0; pc < num_primary_colors; pc++) {
                    fscanf(fin, "%s", buf);
                    tc->colors[k] |= find_color(tc, buf);
                }
            }
            tc->num_colors++;
        }
        
        // read galaxies
        int ng;
        fscanf(fin, "%d", &ng);
        tc->num_galaxies = ng;
        tc->galaxies = (galaxy_t *) calloc(ng, sizeof(galaxy_t));
        for (int g = 0; g < ng; g++) {
            init_galaxy(tc->galaxies, g);
            galaxy_t *gal = tc->galaxies + g;
            // read galaxy
            int ne;
            fscanf(fin, "%d", &ne);
            gal->energies = (energy_t *) calloc(ne, sizeof(energy_t));
            gal->num_energies = 0;
            for (int e = 0; e < ne; e++) {
                fscanf(fin, "%s", buf);
                int color = find_color(tc, buf);
                int cost;
                fscanf(fin, "%d", &cost);
                gal->energies[e].color = color;
                gal->energies[e].cost = cost;
                gal->num_energies++;
            }
        }
        
        // read wormholes
        int nw;
        fscanf(fin, "%d", &nw);
        wormhole_t *tmp_wormholes = (wormhole_t *) calloc(nw, sizeof(wormhole_t));
        for (int w = 0; w < nw; w++) {
            fscanf(fin, "%s", buf);
            int color = find_color(tc, buf);
            int from, to;
            fscanf(fin, "%d%d", &from, &to);
            tc->galaxies[from].num_wormholes++;
            tmp_wormholes[w].color = color;
            tmp_wormholes[w].from = tc->galaxies + from;
            tmp_wormholes[w].to = tc->galaxies + to;
        }

        // assign wormholes to their origin galaxies
        for (int g = 0; g < ng; g++) {
            galaxy_t *gal = &tc->galaxies[g];
            int nw = gal->num_wormholes;
            if (nw > 0) {
                gal->num_wormholes = 0;
                gal->wormholes = (wormhole_t *) calloc(nw, sizeof(wormhole_t));
            }
        }
        for (int w = 0; w < nw; w++) {
            galaxy_t *gal = tmp_wormholes[w].from;
            gal->wormholes[gal->num_wormholes++] = tmp_wormholes[w];
        }
        free(tmp_wormholes);
        tmp_wormholes = 0;
    }
    
    return nc;
}

/**********************************************************************
 * solver
 **********************************************************************/

void
fix_energies(test_case_t *tc) {
    for (int g = 0; g < tc->num_galaxies; g++) {
        galaxy_t *gal = tc->galaxies + g;
        int ne = gal->num_energies;
        int new_ne = 0;
        energy_t *energies = gal->energies;
        energy_t *new_energies = (energy_t *) calloc(gal->num_energies, sizeof(energy_t));
        
        for (int i = 0; i < ne; i++) {
            int j;
            for (j = 0; j < ne; j++) {
                if ((energies[j].color & energies[i].color) == energies[i].color && energies[i].cost > energies[j].cost) {
                    /*
                    fprintf(stderr, "Idiotic energy in #%d: (%d %d) worse than (%d %d)\n",
                            tc->case_number,
                            energies[i].color, energies[i].cost,
                            energies[j].color, energies[j].cost);
                     */
                    break;
                }
            }
            if (j == ne) {    // keep
                new_energies[new_ne] = energies[i];
                new_ne++;
            }
        }
        gal->energies = new_energies;
        gal->num_energies = new_ne;
    }
}

int
energy_cmp(const void *a, const void *b) {
    energy_t *ea = (energy_t *)a;
    energy_t *eb = (energy_t *)b;
    return (ea->cost - eb->cost);
}

void
sort_energies(test_case_t *tc) {
    for (int g = 0; g < tc->num_galaxies; g++) {
        galaxy_t *gal = tc->galaxies + g;
        qsort(gal->energies, gal->num_energies, sizeof(energy_t), energy_cmp);
    }
}

int
prune_galaxy(galaxy_t *galaxy, int my_color, int cost) {
    
    // Prune this path if it has a higher cost than the heuristic value.
    if (cost >= PRUNE_HEURISTIC)
        return 1;

    // Prune this path if we arrived in a worse condition
    // Our condition is worse than a previous one if we arrive with a subset
    // of previous colors and a higher cost.
    for (int i = 0; i < superset_size[my_color]; i++) {
        if (cost >= galaxy->visit_log[superset[my_color][i]]) {
            return 1;
        }
    }
    
    // Log our visit
    galaxy->visit_log[my_color] = cost;
    
    return 0;
}

// fwd declare cyclic recursive functions
void enter_galaxy(galaxy_t *, uint32_t, int, int);

void
recurse_energies(galaxy_t *galaxy, int i_energy, int my_color, int cost, int recursion_level, uint32_t combination) {
    // Find the first useful color
    energy_t *ee = galaxy->energies;
    int ne = galaxy->num_energies;
    
    while (i_energy < ne && (my_color & ee[i_energy].color) == ee[i_energy].color) {
        i_energy++;
        combination <<= 1;
        combination++;
    }
    
    // Enter galaxy if we have a valid combination
    if (i_energy >= ne) {
        if (recursion_level == 0) {
            fprintf(stderr, "%d%%\r", (int)(100LL * combination / (1 << ne)));
            fflush(stderr);
        }
        for (int w = 0; w < galaxy->num_wormholes; w++) {
            wormhole_t *wormhole = &galaxy->wormholes[w];
            if ((my_color & wormhole->color) == wormhole->color) {
                enter_galaxy(wormhole->to, my_color & ~wormhole->color, cost, recursion_level + 1);
            }
        }
    } else {
        // Recurse both options: do not take/take
        energy_t *e = ee + i_energy;
        
        recurse_energies(galaxy, i_energy + 1, my_color, cost, recursion_level, combination << 1);
        recurse_energies(galaxy, i_energy + 1, my_color | e->color, cost + e->cost, recursion_level, (combination << 1) + 1);
    }
}

void
enter_galaxy(galaxy_t *galaxy, uint32_t my_color, int cost, int recursion_level) {
    
    // Update galaxy cost
    if (cost < galaxy->min_cost) {
        galaxy->min_cost = cost;
    }
    
    if (! prune_galaxy(galaxy, my_color, cost)) {
        recurse_energies(galaxy, 0, my_color, cost, recursion_level, 0);
    }
}

void
solve(test_case_t *tc) {
    fix_energies(tc);
    //sort_energies(tc);
    
    enter_galaxy(tc->galaxies, 0, 0, 0);
}

void
write_results(test_case_t *tc, FILE *fout) {
    fprintf(fout, "Case #%d:", tc->case_number);
    for (int g = 0; g < tc->num_galaxies; g++) {
        fprintf(fout, " %d", tc->galaxies[g].min_cost);
    }
    fputc('\n', fout);
}

/**********************************************************************
 * main
 **********************************************************************/

int
main(int argc, char **argv) {
    char *filename_in = NULL;
    FILE *fout;
    struct timeval stop, start;
    int c0, c1;
    
    if (argc <= 1) {
        fprintf(stderr, "t11 input [output [tc0 [tc1]]]\n");
        exit(1);
    }
    if (argc > 1) {
        filename_in = argv[1];
    }
    if (argc > 2) {
        fout = fopen(argv[2], "w");
    } else {
        fout = stdout;
    }
    
    gettimeofday(&start, NULL);

    init_supersets();

    c0 = 1;
    c1 = parse(filename_in);
    
    if (argc > 3) {
        c0 = atoi(argv[3]);
    }
    
    if (argc > 4) {
        c1 = atoi(argv[4]);
    }

    for (int c = c0; c <= c1; c++) {
        test_case_t *tc = test_cases + c - 1;
        solve(tc);
        write_results(tc, fout);
        fprintf(stderr, "%d/%d\n", tc->case_number, c1);
    }
    fclose(fout);

    gettimeofday(&stop, NULL);
    
    long ms = (stop.tv_sec - start.tv_sec) * 1000 + (stop.tv_usec - start.tv_usec) / 1000;
    
    fprintf(stderr, "Took %ld ms\n", ms);
}
