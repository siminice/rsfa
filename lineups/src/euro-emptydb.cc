#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "catalog.hh"
#include "db.h"

#define CTTY_ROM 39

FILE *of;
int year, nm, lasthm, lastaw, lastcr;
Locations ven, euroven;
char **ctty;
char **dir;
int NN;
char hname[256], gname[256];

//--------------------------------------

void Load() {
	FILE *f;
	char s[256], *tok[3];
  f = fopen("catalogs/country.dat", "rt");
	NN = 0;
  if (!f) return;
  fscanf(f, "%d\n", &NN);
  ctty = new char*[NN];
  dir = new char*[NN];
  for (int i=0; i<NN; i++) {
    if (feof(f)) continue;
    fgets(s, 200, f);
    s[strlen(s)-1] = 0;
    tok[0] = strtok(s, ":");
    tok[1] = strtok(NULL, "*");
    tok[2] = strtok(NULL, "*");
    dir[i] = strdup(tok[0]);
    ctty[i] = strdup(tok[2]+2);
    s[0] = 0;
  }
  fclose(f);
}

int FindCtty(char *s) {
	if (!s) return -1;
	for (int i=0; i<NN; ++i) {
		if (strcmp(dir[i], s)==0) return i;
	}
	return -1;
}

int EuroTID(char *name, char *fname) {
	char ls[256], s[256], *sd, *st;
	int cnc, ct;
	int rom = 0;
	char *pslash = strchr(name, '/');
	if (!pslash) {
		sprintf(ls, "rom/%s", name);
		rom = 1;
	}
	else strcpy(ls, name);
	int len = strlen(ls);
	for (int i=0; i<len; ++i) if (ls[i]=='_') ls[i] = ' ';
	sd = strtok(ls, "/");
	st = strtok(NULL, "/\n");
	if (st[0]>='a' && st[0]<'z') st[0] -= 32;
	ct = FindCtty(sd);
	if (ct<0) return -1;
  char filename[128];
  sprintf(filename, "/var/www/html/%s/euroteams.dat", sd);
  FILE *f = fopen(filename, "rt");
	fscanf(f, "%d\n", &cnc);
  for (int i=0; i<cnc; ++i) {
		fgets(s, 128, f);
	  s[15] = 0;
  	int k = 14; while (s[k]==' ') s[k--]=0;
		if (strstr(s, st)==s) {
			fclose(f);
			strcpy(fname, s);
			return 1000*ct+i+(1-rom);
		}
	}
  fclose(f);
  return -1;
}

void MakeEmptyLDB() {
	int h, g, sc, c, r, ret;
	char sh[256], sg[256];
	char z[32];
	for (int i=0; i<nm; i++) {
		printf("#%d\n", i+1);
		ret = 0;
		do {
			printf(" Home: "); scanf("%s", sh);
			ret = (strstr(sh, "ret")==sh);
			if (ret) {
				h = lastaw;
				strcpy(sg, hname);
				strcpy(hname, gname);
				strcpy(gname, sg);
			}
			else {
				h = EuroTID(sh, hname);
			}
		} while (h<0);
		printf(" -> %s\n", hname);
		if (h/1000==CTTY_ROM) h = h%1000;
		if (ret) {
			g = lasthm;
		}
		else {
			do {
				printf(" Away: "); scanf("%s", sg);
				g = EuroTID(sg, gname);
			} while (g<0);
		}
		if (g/1000==CTTY_ROM) g = g%1000;
		printf(" -> %s\n", gname);

		printf(" Score: "); scanf("%d", &sc);
		printf(" Date: "); scanf("%s", z);
		if (!ret) {
			printf(" Cup: "); scanf("%d", &c);
			printf(" Round: "); scanf("%d", &r);
		}

		lasthm = h;
		lastaw = g;
		lastcr = 100*c+r;

    fprintf(of, "%6d,%6d,%3d,%-16s,%d,%2d", h, g, sc, z, c, r);
    fprintf(of, ",      , , "); /* stad spect vreme */
    for (int k=0; k<22; k++) fprintf(of, ",      :0  ");
    fprintf(of, ",      ");
    for (int k=0; k<22; k++) fprintf(of, ",      :0  ");
    fprintf(of, ",      ");
    for (int k=0; k<4; k++) fprintf(of, ",      "); /* arbitri */
    fprintf(of, "\n");
	}
}

void MakeEmptyVDB() {
	for (int i=0; i<nm; i++) {
		for (int j=0; j<30; ++j) fprintf(of, " ,");
    fprintf(of, "\n");
	}
}


//---------------------------------------------

int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("ERROR: Usage int-emptydb <year> <nm>.\n");
    return -1;
  }
  year = strtol(argv[1], NULL, 10);
	nm = strtol(argv[2], NULL, 10);

	Load();
	ven.Load("catalogs/city.dat", "catalogs/stadium.dat");
	euroven.Load("catalogs/eurocity.dat", "catalogs/eurostadium.dat");
  char filename[128];
  sprintf(filename, "euro-lineups-%d.db", year);
  of = fopen(filename, "wt");
  if (of==NULL) return 1;
  MakeEmptyLDB();
	fclose(of);

  sprintf(filename, "euro-events-%d.db", year);
  of = fopen(filename, "wt");
  if (of==NULL) return 1;
  MakeEmptyVDB();
	fclose(of);

  return 0;
}
