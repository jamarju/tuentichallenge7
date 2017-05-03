#include <stdio.h>

int nc, c, nr, pins, frame, roll, total_points;
int points[10], bonus[10];

int main() {
	scanf("%d", &nc);
	for (c = 0; c < nc; c++) {
		scanf("%d", &nr);
		for (frame = 0; frame < 10; frame++) {
			points[frame] = 0;
			for (roll = 0; roll < 2; roll++) {
				scanf("%d", &pins);

				points[frame] += pins;

				if (frame > 0 && bonus[frame - 1]) {
					bonus[frame - 1]--;
					points[frame - 1] += pins;
				}
				if (frame > 1 && bonus[frame - 2]) {
					bonus[frame - 2]--;
					points[frame - 2] += pins;
				}

				if (points[frame] == 10) {
					if (roll == 0)	{ 	// strike
						bonus[frame] = 2;
						break;			// skip next roll
					} else {			// spare
						bonus[frame] = 1;			
					}
				}
			}
		}
		while (bonus[9] > 0) {
			scanf("%d", &pins);
			if (bonus[8] > 0) {
				points[8] += pins;
				bonus[8]--;
			}
			points[9] += pins;
			bonus[9]--;
		}

		printf("Case #%d:", c + 1);
		total_points = 0;
		for (frame = 0; frame < 10; frame++) {
			total_points += points[frame];
			printf(" %d", total_points);
		}
		putchar('\n');
	}
	return 0;

}