#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "catalog.hh"
#include "aliases.hh"
#include "db.h"

#define CTTY_ROM 39

FILE *of;
int year, nm, lasthm, lastaw, lastcr;
Locations ven, euroven;
char **ctty;
char **dir;
int NN;
char hname[256], gname[256];
Aliases **CL;

//--------------------------------------

void Load() {
	FILE *f;
	char s[256], *tok[20], *ystr, *nick, *name;
  f = fopen("country.dat", "rt");
	NN = 0;
  if (!f) return;
  fscanf(f, "%d\n", &NN);
  CL = new Aliases*[NN];
  for (int i=0; i<NN; i++) CL[i] = new Aliases;
  ctty = new char*[NN];
  dir = new char*[NN];
  for (int i=0; i<NN; i++) {
    if (feof(f)) continue;
    fgets(s, 2000, f);
    s[strlen(s)-1] = 0;
    tok[0] = strtok(s, ":");
    dir[i] = strdup(tok[0]);
    tok[1] = strtok(NULL, "*");
    for  (int j=2; j<20; ++j) tok[j] = strtok(NULL, "*");
    int k = 2;
    while(tok[k]) {
      ystr = strtok(tok[k], " ");
      name = strtok(NULL, "~");
      nick = strtok(NULL, "@");
      int y = atoi(ystr);
      alias *a = new alias(y, name, name);
      CL[i]->Append(a);
      k++;
    }
    ctty[i] = strdup(name);
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

void MakeEmptyLDB() {
	int h, g, sc, c, r, ret;
	char sh[256], sg[256];
	char comp[60], round[60];
	char z[32];
	for (int i=0; i<nm; i++) {
		printf("#%d\n", i+1);
		ret = 0;
		do {
			printf(" Vs: "); scanf("%s", sh);
			if (sh[0]=='-') {
				h = FindCtty(sh+1);
				g = CTTY_ROM;
			}
			else {
				g = FindCtty(sh);
				h = CTTY_ROM;
			}
		} while (h<0 || g<0);

		printf(" %s - %s : ", CL[h]->GetName(year), CL[g]->GetName(year));
		scanf("%d", &sc);
		printf(" Date: "); scanf("%s", z);
/*
		printf(" Cup: "); scanf("%s", comp);
		printf(" Round: "); scanf("%s", round);
*/
		strcpy(comp, " A");
		strcpy(round, "  ");

    fprintf(of, "%6d,%6d, %4d,%-5s-%d@     ,%s,%s", h*1000, g*1000, sc, z, year, comp, round);
    fprintf(of, ", , , "); /* stad spect vreme */
    for (int k=0; k<22; k++) fprintf(of, ",      :0  ");
    fprintf(of, ",      ");
    for (int k=0; k<22; k++) fprintf(of, ",      :0  ");
    fprintf(of, ",      ");
    for (int k=0; k<4; k++) fprintf(of, ",      "); /* arbitri */
    fprintf(of, "\n");
	}
}

void LoadLDB() {
  char filename[128];
  sprintf(filename, "nat-lineups-%d.db", year);
  of = fopen(filename, "rt");
  if (of==NULL) return;
	char s[5000];
	nm = 0;
	while (!feof(of)) {
		fgets(s, 5000, of);
		if (strlen(s)>20) nm++;
		s[0] = 0;
	}
	fclose(of);
}

void MakeEmptyVDB() {
  char filename[128];
  sprintf(filename, "nat-events-%d.db", year);
  of = fopen(filename, "wt");
  if (of==NULL) return;
	for (int i=0; i<nm; i++) {
		for (int j=0; j<30; ++j) fprintf(of, " ,");
    fprintf(of, "\n");
	}
	fclose(of);
}


//---------------------------------------------

int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("ERROR: Usage nat-emptydb <year> <nm>.\n");
    return -1;
  }
  year = strtol(argv[1], NULL, 10);
	nm = strtol(argv[2], NULL, 10);

	Load();
	ven.Load("city.dat", "stadium.dat");
	euroven.Load("eurocity.dat", "eurostadium.dat");
  char filename[128];
  sprintf(filename, "nat-lineups-%d.db", year);
  of = fopen(filename, "wt");
  if (of==NULL) return 1;
  MakeEmptyLDB();
	fclose(of);

//	LoadLDB();
  MakeEmptyVDB();

  return 0;
}
