#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db.h"

#define MAX_TEAMS  2000
#define DB_ROWS	   2000
#define DB_COLS		 60
#define DB_CELL		 60

char db[DB_ROWS][DB_COLS][DB_CELL];
int  NC;
int  nrows;
int  tid, fr, lr;
char *cmnem;
//--------------------------------------

int Load() {
  char *s = new char[256];
  char *err;
  cmnem = new char[10];
  printf("Coach mnem,TID,first round,last round: ");
  fgets(s, 256, stdin);
  cmnem = strtok(s, ",");
  char *st = strtok(NULL,  ",\n");
  char *sr1 = strtok(NULL,  ",\n");
  char *sr2 = strtok(NULL,  ",\n");
  if (st==NULL) return 0;
  tid = strtol(st, &err, 10);
  if (err!=NULL && err[0]!=0) return 0;
  if (sr1!=NULL) fr = strtol(sr1, &err, 10); else fr = 1;
  if (err!=NULL && err[0]!=0) return 0;
  if (sr2!=NULL) lr = strtol(sr2, &err, 10); else lr = 100;
  if (err!=NULL && err[0]!=0) return 0;
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

int LoadDB(int year) {
  char filename[64], s[5000], *tk[DB_COLS];
  sprintf(filename, "db/lineups-%d.db", year);
  FILE *f = fopen(filename, "rt");
  if (f==NULL) { fprintf(stderr, "ERROR: database %s not found.\n", filename); return 0; }
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
  return 1;
}

void SaveDB(int year) {
  char oldfilename[64];
  char newfilename[64];
  char filename[64];
  sprintf(filename,    "db/lineups-%d.db",  year);
  sprintf(oldfilename, "db/lineups-%d.old", year);
  sprintf(newfilename, "db/lineups-%d.new", year);
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

void AddCoach(int year) {
  int na = 0;
  for (int i=0; i<nrows; i++) {
    int h = atoi(db[i][DB_HOME]);
    int a = atoi(db[i][DB_AWAY]);
    int r = atoi(db[i][DB_ROUND]);
    if (h==tid) {
      if (db[i][DB_COACH1][0] == ' ' && r>=fr && r<=lr) {
        strncpy(db[i][DB_COACH1], cmnem, 6);
        na++;
      }
    }
    if (a==tid) {
      if (db[i][DB_COACH2][0] == ' ' && r>=fr && r<=lr) {
        strncpy(db[i][DB_COACH2], cmnem, 6);
        na++;
      }
    }
  }
  fprintf(stdout, "Added %d times.\n", na);
}

int main(int argc, char **argv) {
  if (argc < 1) { fprintf(stderr, "No year specified"); return -1; }
  int year = atoi(argv[1]);
  if (!Load()) { fprintf(stderr, "Bad input."); return -2; }
  if (!LoadDB(year)) { fprintf(stderr, "No database found for %d.", year); return -3; }
  AddCoach(year);
  SaveDB(year);
  return 0;
}
