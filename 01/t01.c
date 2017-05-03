#include <stdio.h>

int main() {
	int nc, c, na, a, ts, s;

	scanf("%d", &nc);

	for (c = 0; c < nc; c++) {
		scanf("%d", &na);
		ts = 0;
		for (a = 0; a < na; a++) {
			scanf("%d", &s);
			ts += s;
		}
		printf("Case #%d: %d\n", c+1, (ts + 7) / 8);
	}
	return 0;
}
