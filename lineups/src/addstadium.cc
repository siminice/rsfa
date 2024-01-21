#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db.h"

#define MAX_TEAMS 2000
#define DB_ROWS		400
#define DB_COLS		60
#define DB_CELL		20

char db[DB_ROWS][DB_COLS][DB_CELL];
char defstad[MAX_TEAMS][12];
int  nrows;
int  NC; 
//--------------------------------------

int Load() {
  FILE *f;
  char s[2000], *tok[20], *ystr, *name, *nick;

  f = fopen("defaultstadium.dat", "rt");
  if (f==NULL) return 0;
  fscanf(f, "%d\n", &NC);
  for (int i=0; i<NC; i++) {
    fgets(s, 60, f);
    s[strlen(s)-1] = 0;
    memmove(defstad[i], s+15, 7);
  }
  fclose(f);
  return 1;
}

/* *************************************** */

void trim(char *s) {
  if (s==NULL) return;
  int l = strlen(s);
  if (l<=1) return;
  int j = l-1;
  while ((s[j]==' ' || s[j]=='\t') && j>0) j--;
  int i = 0;
  while (s[i]==' ' && i<j) i++;
  if (i>0 || j<l-1) {
    s[j+1] = 0;
    memmove(s, s+i, j-i+1);
    s[j-i+1]=0;
  }
}

void LoadDB(int year) {
  char filename[64], s[5000], *tk[DB_COLS];
  sprintf(filename, "lineups-%d.db", year);
  FILE *f = fopen(filename, "rt");
  if (f==NULL) { fprintf(stderr, "ERROR: database %s not found.\n", filename); return; }
	int i=0;
	while (!feof(f)) {
    fgets(s, 5000, f);
		if (strlen(s)<100) continue;
    tk[0] = strtok(s, ",\n");
    for (int j=1; j<DB_COLS; j++) tk[j]=strtok(NULL, ",\n");
    for (int j=0; j<DB_COLS; j++) {
      if (tk[j]!=NULL) strcpy(db[i][j], tk[j]);
      else strcpy(db[i][j], " ");
    }
		s[0] = 0;
		i++;
  }
	nrows = i;
  fclose(f);
}

void SaveDB(int year) {
  char oldfilename[64];
  char newfilename[64];
  char filename[64];
  sprintf(filename,    "lineups-%d.db",  year);
  sprintf(oldfilename, "lineups-%d.old", year);
  sprintf(newfilename, "lineups-%d.new", year);
  FILE *f = fopen(newfilename, "wt");
  if (f==NULL) { fprintf(stderr, "ERROR: database %s not found.\n", newfilename); return; }
  for (int i=0; i<nrows; i++) {
    for (int j=0; j<DB_COLS; j++) fprintf(f, "%s,", db[i][j]);
    fprintf(f, "\n");
  }
  fclose(f);
  rename(filename, oldfilename);
  rename(newfilename, filename);
}

void AddStad(int year) {
	for (int i=0; i<nrows; i++) {
			char *oldstad = db[i][DB_VENUE];
			int len = strlen(oldstad);
			int h = atoi(db[i][DB_HOME]);
			int sc = atoi(db[i][DB_SCORE]);
			if (h>=0 && h<NC && sc>=0 && oldstad[0]==' ') {
				strcpy(db[i][DB_VENUE], defstad[h]);
				fprintf(stderr, "%d %3d: @%s.\n", year, h, defstad[h]);
			}	else if (h<0 || h>=NC) {
				fprintf(stderr, "%d L%d: Unknown host %d.\n", year, i+1, h);
			}
	}
}

int main(int argc, char **argv) {
  Load();
	int FY = 1933;
	int LY = 2013;
	for (int year=FY; year<=LY; year++) {
	  LoadDB(year);
		AddStad(year);
  	SaveDB(year);
	}
  return 0;
}
