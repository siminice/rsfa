#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db.h"
#include "rhead.h"
#include "catalog.hh"

#define MAX_COL	100

#define DB_ROWS		400
#define DB_COLS		60
#define DB_CELL		20

char db[DB_ROWS][DB_COLS][DB_CELL];
int col[MAX_COL];
char rfilename[128];
int NT;

//--------------------------------------

void LoadDB(int year) {
  char filename[64], s[5000], *tk[DB_COLS];
  sprintf(filename, "lineups-%d.db", year);
  FILE *f = fopen(filename, "rt");
  if (f==NULL) { fprintf(stderr, "ERROR: database %s not found.\n", filename); return; }
  for (int i=0; i<NT*(NT-1); i++) {
    fgets(s, 5000, f);
    tk[0] = strtok(s, ",\n");
    for (int j=1; j<DB_COLS; j++) tk[j]=strtok(NULL, ",\n");
    for (int j=0; j<DB_COLS; j++) {
      if (tk[j]!=NULL) strcpy(db[i][j], tk[j]);
      else strcpy(db[i][j], " ");
    }
  }
  fclose(f);
}

void PrintColumns(char *filename) {
  char s[5000], *tk[100];

  FILE *f = fopen(filename, "rt");
  if (f==NULL) {
    fprintf(stderr, "ERROR: Cannot open file %s.\n", filename);
    return;
  }

  do {
      fgets(s, 5000, f);
      tk[0] = strtok(s, "\t\n");
      for (int j=1; j<100; j++) tk[j] = strtok(NULL, "\t\n");
			for (int j=0; j<MAX_COL; j++) {
				if (col[j] && tk[j]!=NULL) {
					printf("%s\t", tk[j]);
				}
			}
			printf("\n");
  } while (!feof(f));
  fclose(f);
}

int main(int argc, char **argv) {
  strcpy(rfilename, "");
  if (argc>1) strcpy(rfilename, argv[1]);
	NT = 18;

  for (int k=2; k<argc; k++) {
		int v = atoi(argv[k]);
		col[v] = 1;
  }

  if (strlen(rfilename)==0) {
    fprintf(stderr, "ERROR: input file not specified.\n");
    return -1;
  }

	PrintColumns(rfilename);

  return 0;
}
