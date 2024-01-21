#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define MAX_N 		64
#define MAX_RR 	 	 4
#define NUM_ORD 	10

#define DB_ROWS         600
#define DB_COLS         60
#define DB_CELL         40

#define DB_HOME      0
#define DB_GUEST     1
#define DB_SCORE     2
#define DB_DATE      3
#define DB_COMP      4
#define DB_ROUND     5

char **club;
char **mnem;
int  NC;
int  id[MAX_N], win[MAX_N], drw[MAX_N], los[MAX_N], gsc[MAX_N], gre[MAX_N], pen[MAX_N], pdt[MAX_N];
int pts[MAX_N], rank[MAX_N];
int rnd[MAX_RR][MAX_N][MAX_N], res[MAX_RR][MAX_N][MAX_N];
char filename[64], dbfile[64], *outputfile;
int n, ppv, tbr, numr, pr1, pr2, rel1, rel2, year;
char desc[MAX_N][32];
char db[DB_ROWS][DB_COLS][DB_CELL];
//--------------------------------------------------

int Load() {
  FILE *f;
  char s[2000], *tok[20];

  f = fopen("data/teams.dat", "rt");
  if (!f) return 0;
  fscanf(f, "%d\n", &NC);
  club = new char*[NC];
  mnem = new char*[NC];

  for (int i=0; i<NC; i++) {
    fgets(s, 60, f);
    s[strlen(s)-1] = 0;
    mnem[i] = new char[16];
    club[i] = new char[32];
    memmove(mnem[i], s, 15); mnem[i][15] = 0;
    for (int j=0; j<30; j++) club[i][j] = 32;
    memmove(club[i], s+15, 30);
  }
  fclose(f);
  return 1;
}

int LoadFile(char *filename) {
  FILE *f;
  f = fopen(filename, "rt");
  if (NULL == f) { printf("File %s not found.\n", filename); return 0; }

  int k, i, j, x, y, zi;
  char s[200], *tok[12];
  fscanf(f, "%d %d %d %d %d %d %d\n", &n, &ppv, &tbr, &pr1, &pr2, &rel1, &rel2);
  numr = tbr/NUM_ORD + 1;
  for (i=0; i<n; i++) {
    fgets(s, 200, f);
    tok[0] = strtok(s, " ");
    for (int k=1; k<12; k++) tok[k] = strtok(NULL, " \n");
    id[i] = atoi(tok[0]);
    win[i] = atoi(tok[2]);
    drw[i] = atoi(tok[3]);
    los[i] = atoi(tok[4]);
    gsc[i] = atoi(tok[5]);
    gre[i] = atoi(tok[6]);
    pts[i] = atoi(tok[7]);
    if (tok[8]) pen[i] = atoi(tok[8]); else pen[i] = 0;
    if (tok[9]) pdt[i] = atoi(tok[9]); else pdt[i] = 0;
    if (tok[10]) strcpy(desc[i], tok[10]); else strcpy(desc[i],"");
  }

  for (k=0; k<MAX_RR; k++)
    for (i=0; i<n; i++)
      for (j=0; j<n; j++) { rnd[k][i][j] = res[k][i][j] = -1; }

  for (k=0; k<numr; k++) {
    for (i=0; i<n; i++) {
      for (j=0; j<n; j++) {
        fscanf(f, "%d %d", &x, &y);
        rnd[k][i][j] = x;
        res[k][i][j] = y;
      }
      fscanf(f, "\n");
    }
  }
  fclose(f);
  return 1;
}

int LoadDB() {
  FILE *f;
  char s[1024], *tk[DB_COLS];
  f = fopen(dbfile, "rt");
  if (f==NULL) { fprintf(stderr, "ERROR: database %s not found.\n", dbfile); return 0; }
  for (int i=0; i<numr*n*(n-1); i++) {
    fgets(s, 1000, f);
    tk[0] = strtok(s, ",\n");
    for (int j=1; j<DB_COLS; j++) tk[j]=strtok(NULL, ",\n");
    for (int j=0; j<DB_COLS; j++) {
      if (tk[j]!=NULL) strcpy(db[i][j], tk[j]);
      else strcpy(db[i][j], " ");
    }
  }
  fclose(f);
  return 1;
}

void Save() {
  FILE *f;
  int k, i, j;
  outputfile = new char[strlen(filename)+5];
  strcpy(outputfile, filename);
  strcat(outputfile, ".old");
  rename(filename, outputfile);
  f = fopen(filename, "wt");
  fprintf(f, "%d %d %d %d %d %d %d\n", n, ppv, tbr, pr1, pr2, rel1, rel2);
  for (i=0; i<n; i++) {
    fprintf(f, "%4d %3d %3d %3d %3d %3d %3d %3d",
      id[i], win[i]+drw[i]+los[i], win[i], drw[i], los[i], gsc[i], gre[i], pts[i]);
    if (pen[i]!=0) fprintf(f, " %3d %3d", pen[i], pdt[i]);
    if (strlen(desc[i])>0) {
      if (pen[i]==0)  fprintf(f, " %3d %3d", pen[i], pdt[i]);
      fprintf(f, " %s", desc[i]);
    }
    fprintf(f, "\n");
  }

  for (k=0; k<numr; k++) {
    for (i=0; i<n; i++) {
      for (j=0; j<n; j++) {
        fprintf(f, "%6d%5d", rnd[k][i][j], res[k][i][j]);
      }
      fprintf(f, "\n");
    }
  }
  fclose(f);
}

int CheckDates() {
  int er = 0;
  char dbd[DB_CELL];
  char dbs[DB_CELL];
  for (int h=0; h<numr; h++) {
    for (int i=0; i<n; i++) {
      for (int j=0; j<n; j++) {
        if (i!=j) {
          int d = rnd[h][i][j];
          int r = d/1000;
          int z = d%1000;
          int m = h*n*(n-1) + (n-1)*i + j-(i<j? 1:0);
          int dbr = atoi(db[m][DB_ROUND]);
          strncpy(dbd, db[m][DB_DATE], DB_CELL);
          strncpy(dbs, db[m][DB_SCORE], DB_CELL);
          int sc = atoi(dbs);
          char *dby = strtok(dbd, "-");
          char *dbm = strtok(NULL, "-");
          char *dbz = strtok(NULL, "-@");
          int dm = atoi(dbm);
          int dz = atoi(dbz);
          if (r != dbr || z != 50*dm+dz) {
            er++;
            printf(" [M%3d] %-16s - %-16s %d-%d: R%2d %02d/%02d -> R%2d %02d/%02d %d-%d.\n",
              m+1, mnem[id[i]], mnem[id[j]], res[h][i][j]/100, res[h][i][j]%100, r, z/50, z%50, dbr, dm, dz, sc/100, sc%100);
            res[h][i][j] = sc;
            if (h<2) {
              rnd[h][i][j] = dbr*1000 + 50*dm + dz;
            } else {
              rnd[h][i][j] = (rnd[h][i][j]/1000)*1000 + 50*dm + dz;
            }
          }
/*
          if (res[h][i][j]<0 && sc>=0) {
            er++;
            printf(" [M%3d] %-16s - %-16s = %d-%d.\n",
              m+1, mnem[id[i]], mnem[id[j]], sc/100, sc%100);
            res[h][i][j] = sc;
          }
*/
        }
      }
    }
  }
  return er;
}

//---------------------------------------------

int main(int argc, char* argv[]) {
  if (argc < 2) { printf("No input file specified.\n"); return 0; }
  char sarg1[128];
  strncpy(sarg1, argv[1], 128);
  char *ystr = strtok(sarg1, ".");
  ystr = strtok(NULL, " ");
  if (ystr) year = atoi(ystr); else year = 0;
  sprintf(dbfile, "db/lineups-%d.db", year);

  if (!Load()) { return -1; }
  sprintf(filename, "data/%s", argv[1]);
  if (!LoadFile(filename)) { return -2; }
  if (!LoadDB()) { return -3; }
  if (CheckDates() > 0) {
    Save();
  }
  return 1;
}
