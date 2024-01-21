#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "db.h"

int id[64], round[4][64][64], res[4][64][64];
int n, rr, tbr, pr1, pr2, rel1, rel2, ppv, year, maxr;
int sfmt;
FILE *of;

//--------------------------------------

int LoadFile(char *filename) {
  FILE *f;
  int h, i, j, x, y, r, z;
  f = fopen(filename, "rt");
  if (NULL == f) return 0;
  // Loading file
  char s[500], *tok[12];
  fscanf(f, "%d %d %d %d %d %d %d\n", &n, &ppv, &tbr, &pr1, &pr2, &rel1, &rel2);
  rr = tbr/10 + 1;
  for (i=0; i<n; i++) {
    fgets(s, 200, f);
    tok[0] = strtok(s, " ");
    id[i] = atoi(tok[0]);
  }
  for (h=0; h<rr; h++) {
   for (i=0; i<n; i++) {
    for (j=0; j<n; j++) {
      fscanf(f, "%d %d", &r, &z);
      round[h][i][j] = r;
      res[h][i][j] = z;
    }
    fscanf(f, "\n");
   }
  }
  fclose(f);
  return 1;
}

void AddRow(int h, int i, int j) {
  int r = round[h][i][j]/1000;
  int z = round[h][i][j]%1000;
  int x = res[h][i][j]/100;
  int y = res[h][i][j]%100;
  if (sfmt==1) {
    if (res[h][i][j]>=0) {
      for (int i=0; i<x+y; i++) {
        fprintf(of, "      '0  ,");
      }
    }
    fprintf(of, "\n");
  }
  else {
    int mo = z/50;
    fprintf(of, "%4d,%4d, %4d, %04d-%02d-%02d@00:00,A,%2d", id[i], id[j], res[h][i][j], mo>7? year-1 : year, z/50, z%50, r);
    fprintf(of, ", , , "); /* stad spect vreme */
    for (int k=0; k<22; k++) fprintf(of, ",      :0  ");
    fprintf(of, ",      ");
    for (int k=0; k<22; k++) fprintf(of, ",      :0  ");
    fprintf(of, ",      ");
    for (int k=0; k<4; k++) fprintf(of, ",      "); /* arbitri */
    fprintf(of, "\n");
  }
}

void MakeEmptyDB() {
 for (int h=0; h<rr; h++) {
  for (int i=0; i<n; i++) {
   for (int j=0; j<n; j++) {
     if (i!=j) AddRow(h, i, j);
   }
  }
 }
}


//---------------------------------------------

int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("ERROR: No input file specified.\n");
    return -1;
  }
  sfmt = 0;
  if (argc > 2 && strcmp(argv[2], "-s")==0) { sfmt = 1; }

  maxr = 0;
  if (!LoadFile(argv[1])) {
    printf("ERROR: file %s not found.\n", argv[1]);
    return -2;
  }

  char *ystr = strtok(argv[1], ".");
  ystr = strtok(NULL, " ");
  if (ystr) year = atoi(ystr); else year = 0;
  char filename[128];
  if (sfmt) {
    sprintf(filename, "events-%d.db", year);
  }
  else {
    sprintf(filename, "lineups-%d.db", year);
  }
  of = fopen(filename, "wt");
  if (of==NULL) return 1;
  MakeEmptyDB();
  fclose(of);
  return 0;
}
