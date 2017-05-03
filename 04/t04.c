#include <limits.h>
#include <stdio.h>
#include <stdlib.h> 
#include <stdint.h>

/* qsort int comparison function */ 
int uint64_cmp(const void *a, const void *b) 
{ 
    return *(const uint64_t *)a - *(const uint64_t *)b; 
} 
 
int main() {
	int nc, c, nl, l, x, y, z;
	uint64_t min;
	uint64_t le[100000];

	scanf("%d", &nc);
	for (c = 0; c < nc; c++) {
		min = ULLONG_MAX;
		scanf("%u", &nl);

		for (l = 0; l < nl; l++) {
			scanf("%llu", &le[l]);
		}

		qsort(le, nl, sizeof(uint64_t), uint64_cmp);

		for (x = 0; x < nl - 2; x++) {
			if (le[x] >= min) 
				break;
			for (y = x + 1; y < nl - 1; y++) {
				if (le[x] + le[y] >= min) 
					break;
				for (z = y + 1; z < nl; z++) {
					if (le[x] + le[y] + le[z] >= min)
						break;
					if (le[x] + le[y] > le[z] && le[x] + le[z] > le[y] && le[y] + le[z] > le[x]) {
						min = le[x] + le[y] + le[z];
						break;
					}
				}
			}
		}

		printf("Case #%d: ", c + 1);
		if (min == ULLONG_MAX) {
			puts("IMPOSSIBLE");
		} else {
			printf("%llu\n", min);
		}
	}
}