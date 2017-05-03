#include <stdio.h>
#include <stdint.h>

int main() {
	int nc, c, ncards;
	uint32_t p, i;

	scanf("%d", &nc);
	for (c = 0; c < nc; c++) {
		scanf("%u", &p);
		ncards = 0;
		i = 1;
		while (i <= p) {
			ncards++;
			i *= 2;
		}
		printf("Case #%d: %d\n", c + 1, ncards);
	}

}