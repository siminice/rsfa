#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char **club;
char **mnem;
int  NC;
int  id[32], win[32], drw[32], los[32], gsc[32], gre[32], pts[32], pen[32], pdt[32];
int  lid[32], lwin[32], ldrw[32], llos[32], lgsc[32], lgre[32], lpts[32], lpen[32], lpdt[32];
int  round[3][32][32], res[3][32][32], lround[3][32][32], lres[3][32][32];
int  n, r, ltbr, tbr, pr1, pr2, rel1, rel2, ppv, maxr1;

//--------------------------------------

void Load() {
  FILE *f;
  char s[500], *tok[20], *ystr, *name, *nick;
  f = fopen("teams.dat", "rt");
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
}

int PreloadFile(char *filename) {
  FILE *f;
  int h, i, j, x, y, r, z;
  int ln, lppv, lpr1, lpr2, lrel1, lrel2;
  maxr1 = 0;
  f = fopen(filename, "rt");
  if (NULL == f) { printf("File %s not found for preloading.\n", filename); return 0; }
  // Loading file
  char s[500], *tok[10];
  fscanf(f, "%d %d %d %d %d %d %d\n", &ln, &lppv, &ltbr, &lpr1, &lpr2, &lrel1, &lrel2);
  for (i=0; i<ln; i++) {
    fgets(s, 200, f);
    tok[0] = strtok(s, " ");
    for (int k=1; k<10; k++) tok[k] = strtok(NULL, " \n");
    lid[i] = atoi(tok[0]);
    lwin[i] = atoi(tok[2]);
    ldrw[i] = atoi(tok[3]);
    llos[i] = atoi(tok[4]);
    lgsc[i] = atoi(tok[5]);
    lgre[i] = atoi(tok[6]);
    lpts[i] = atoi(tok[7]);
    if (tok[8]) lpen[i] = atoi(tok[8]); else lpen[i] = 0;
    if (tok[9]) lpdt[i] = atoi(tok[9]); else lpdt[i] = 0;
  }
  int numr1 = 1 + ltbr/10;
  for (h=0; h<numr1; h++) {
   for (i=0; i<ln; i++) {
    for (j=0; j<ln; j++) {
      fscanf(f, "%d %d", &r, &z);
      lround[h][i][j] = r;
      if (r/1000 > maxr1) maxr1 = r/1000;
      lres[h][i][j] = z;
    }
    fscanf(f, "\n");
   }
  }
  fclose(f);
}

int LoadFile(char *filename) {
  FILE *f;
  int h, i, j, x, y, r, z;
  f = fopen(filename, "rt");
  if (NULL == f) { printf("File %s not found.\n", filename); return 0; }
  // Loading file
  char s[500], *tok[10];
  fscanf(f, "%d %d %d %d %d %d %d\n", &n, &ppv, &tbr, &pr1, &pr2, &rel1, &rel2);
  int *per = new int[n];
  for (i=0; i<n; i++) {
    fgets(s, 200, f);
    tok[0] = strtok(s, " ");
    for (int k=1; k<10; k++) tok[k] = strtok(NULL, " \n");
    id[i] = atoi(tok[0]);
    for (int k=0; k<n; k++) if (lid[k]==id[i]) per[i] = k;
    win[per[i]] = atoi(tok[2]);
    drw[per[i]] = atoi(tok[3]);
    los[per[i]] = atoi(tok[4]);
    gsc[per[i]] = atoi(tok[5]);
    gre[per[i]] = atoi(tok[6]);
    pts[per[i]] = atoi(tok[7]);
    if (tok[8]) pen[per[i]] = atoi(tok[8]); else pen[per[i]] = 0;
    if (tok[9]) pdt[per[i]] = atoi(tok[9]); else pdt[per[i]] = 0;
  }
  int numr2 = 1 + tbr/10;
  for (h=0; h<numr2; h++) {
   for (i=0; i<n; i++) {
    for (j=0; j<n; j++) {
      fscanf(f, "%d %d", &r, &z);
      round[h][per[i]][per[j]] = r;
      if (r>=0 && r/1000 <= maxr1) round[h][per[i]][per[j]] += maxr1*1000;
      res[h][per[i]][per[j]] = z;
    }
    fscanf(f, "\n");
   }
  }
  fclose(f);
  delete[] per;
}

void Dump() {
  int h, i, j;
  printf("%d %d %d %d %d %d %d\n", n, ppv, ltbr+10*(1+tbr/10), pr1, pr2, rel1, rel2);
  for (i=0; i<n; i++) {
    printf("%4d%4d%4d%4d%4d%4d%4d%4d",
      lid[i],
      lwin[i]+ldrw[i]+llos[i]+win[i]+drw[i]+los[i],
      lwin[i]+win[i],
      ldrw[i]+drw[i],
      llos[i]+los[i],
      lgsc[i]+gsc[i],
      lgre[i]+gre[i],
      lpts[i]+pts[i]);
    if (lpen[i]!=0) printf("%4d%4d", lpen[i], lpdt[i]);
    printf("\n");
  }
  int numr1 = 1 + ltbr/10;
  int numr2 = 1 + tbr/10;
  for (h=0; h<numr1; h++) {
   for (i=0; i<n; i++) {
    for (j=0; j<n; j++)
      printf("%6d%5d", lround[h][i][j], lres[h][i][j]);
    printf("\n");
   }
  }
  for (h=0; h<numr2; h++) {
   for (i=0; i<n; i++) {
    for (j=0; j<n; j++)
      printf("%6d%5d", round[h][i][j], res[h][i][j]);
    printf("\n");
   }
  }
}


//---------------------------------------------

int main(int argc, char* argv[]) {
  Load();  
  if (argc < 3) { printf("Usage: merge f1 f2\n"); return 1; }
  PreloadFile(argv[1]);
  LoadFile(argv[2]);
//  Merge();
  Dump();
  return 0;
}
