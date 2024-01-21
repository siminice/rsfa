#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
	int prev_year = -1;
	int year;
	char s[500];
	FILE *f = fopen("cuparchive.dat", "rt");
	FILE *h;
	char filename[128];
	while(!feof(f)) {
		fgets(s, 500, f);
		if (strlen(s)<12) continue;
	  int year = 75*(s[0]-48) + (s[1]-48) + 1870;
		if (year != prev_year) {
			if (prev_year>0) fclose(h);
			sprintf(filename, "cuparchive-%d.dat", year);
			h = fopen(filename, "wt");
		}
		fprintf(h, "%s", s);
		prev_year = year;
		s[0] = 0;
	}
	fclose(h);
	fclose(f);
	return EXIT_SUCCESS;
}
