#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "db.h"

#define MAX_NAMES       20000
#define DB_ROWS         400
#define DB_COLS         60
#define DB_CELL         20
#define EV_COLS         30

int id[64], round[64][64], res[64][64];
int n, tbr, pr1, pr2, rel1, rel2, ppv, year, maxr;
char dbfilename[128];
char ssfilename[128];
char  db[DB_ROWS][DB_COLS][DB_CELL];
char edb[DB_ROWS][EV_COLS][DB_CELL];
bool winter;
int num_winter;
int *start_winter, *end_winter;

//--------------------------------------

int LoadLeague() {
  FILE *f;
  char s[2000], *tok[20], *ystr, *name, *nick;

  num_winter = 0;
  f = fopen("winter.dat", "rt");
  if (f!=NULL) {
    fscanf(f, "%d\n", &num_winter);
    start_winter = new int[num_winter];
    end_winter = new int[num_winter];
    for (int i=0; i<num_winter; i++) {
      fscanf(f, "%d %d\n", start_winter+i, end_winter+i);
    }
    fclose(f);
  }
}

int isWinterSeason(int y) { 
  for (int  i=0; i<num_winter; i++) {
    if (y>=start_winter[i] && y<=end_winter[i]) return 1;
  }
  return 0;
}

int getYear(int y, int r, int z) {
  if (isWinterSeason(y)) return y;
  if (z/50>7) return y-1;
  if (z/50==7) {
    if (r<n-1) return y-1;
  }
  return y;
}

int LoadSsn() {
  FILE *f;
  int i, j, x, y, r, z;
  f = fopen(ssfilename, "rt");
  if (NULL == f) return 0;
  char s[500], *tok[12];
  fscanf(f, "%d %d %d %d %d %d %d\n", &n, &ppv, &tbr, &pr1, &pr2, &rel1, &rel2);
  for (i=0; i<n; i++) {
    fgets(s, 200, f);
    tok[0] = strtok(s, " ");
    id[i] = atoi(tok[0]);
  }
  for (i=0; i<n; i++) {
    for (j=0; j<n; j++) {
      fscanf(f, "%d %d", &r, &z);
      round[i][j] = r;
      res[i][j] = z;
    }
    fscanf(f, "\n");
  }
  fclose(f);
  return 1;
}

void LoadDB() {
  char s[5000], *tk[DB_COLS];
  FILE *f;
  f = fopen(dbfilename, "rt");
  if (f==NULL) { fprintf(stderr, "ERROR: database %s not found.\n", dbfilename); return; }
  for (int i=0; i<n*(n-1); i++) {
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

int MatchID(int a, int b) {
  int mid = a*(n-1)+b-(b>a?1:0);
  return mid;
}

int GetHome(int mid) {
  return mid/n;
}

int GetAway(int mid) {
  int h = mid/n;
  int a = mid%n;
  if (a<h) return a;
  else return a+1;
}

int isBlank(char *s) {
  if (s==NULL) return 1;
  int l = strlen(s);
  for (int i=0; i<l; i++) {
	if (s[i]!=' ') return 0;
  }
  return 1;
}

void ChangeDB() {
  int m = 0;
  char sc[DB_CELL+1];
  for (int a=0; a<n; a++) {
	for (int b=0; b<n; b++) {
		if (a!=b) {
			if (isBlank(db[m][DB_SCORE])) {
				sprintf(sc, "%4d", res[a][b]);
				strcpy(db[m][DB_SCORE], sc);
			}
			m++;
		}
	}
  }
}

void SaveDB() {
  char s[5000], *tk[DB_COLS];
  FILE *f;
  char svfilename[128];
  sprintf(svfilename, "old-%s", dbfilename);
  rename(dbfilename, svfilename);

  f = fopen(dbfilename, "wt");
  if (f==NULL) { fprintf(stderr, "ERROR: Cannot open database %s.\n", dbfilename); return; }
  for (int i=0; i<n*(n-1); i++) {
    for (int j=0; j<DB_COLS; j++) {
      fprintf(f, "%s,", db[i][j]);
    }
    fprintf(f, "\n");
  }
  fclose(f);
}


//---------------------------------------------

int main(int argc, char* argv[]) {
  char s[128], *ystr;
  int year;

  if (argc < 2) {
    printf("ERROR: No input file specified.\n");
    return -1;
  }

  strcpy(dbfilename, argv[1]);

  for (int j=1; j<argc; j++) {
    if (strcmp(argv[j], "-y")==0) {
      if (j<argc-1) {
        year = atoi(argv[j+1]);
        if (year<1910 || year > 2099) {
		fprintf(stderr, "ERROR: Invalid year %d.\n", year);
		return -1;
	}
	sprintf(dbfilename, "lineups-%d.db", year);
	sprintf(ssfilename, "a.%d", year);
      }
    }
  }

  LoadSsn();
  LoadDB();
  ChangeDB();
  SaveDB();

  return 0;
}
