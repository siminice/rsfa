#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int  id[64], win[64], drw[64], los[64], gsc[64], gre[64], pen[64], pdt[64];
int pts[64], rank[64], rnd[64][64], res[64][64];
int rnd2[64][64], res2[64][64];
char inputfile[64], *outputfile;
int n, ppv, tbr, pr1, pr2, rel1, rel2;
char desc[64][32];

//--------------------------------------------------

void Dump() {
  FILE *f;
  int i, j;
  outputfile = new char[strlen(inputfile)+5];
  strcpy(outputfile, inputfile);
  strcat(outputfile, ".old");
  rename(inputfile, outputfile);
  f = fopen(inputfile, "wt");
  fprintf(f, "%d %d %d %d %d %d %d\n", n, ppv, tbr, pr1, pr2, rel1, rel2);
  for (i=0; i<n; i++) {
    fprintf(f, "%4d%4d%4d%4d%4d%4d%4d%4d",
      id[i], win[i]+drw[i]+los[i], win[i], drw[i], los[i], gsc[i], gre[i], pts[i]);
    if (pen[i]!=0) fprintf(f, "%4d%4d", pen[i], pdt[i]);
    if (strlen(desc[i])>0) {
      if (pen[i]==0)  fprintf(f, "%4d%4d", pen[i], pdt[i]);
      fprintf(f, " %s", desc[i]);
    }
    fprintf(f, "\n");
  }
  for (i=0; i<n; i++) {
    for (j=0; j<n; j++)
      fprintf(f, "%6d%5d", rnd[i][j], res[i][j]);
    fprintf(f, "\n");
  }
  if (tbr>=10) {
    for (i=0; i<n; i++) {
      for (j=0; j<n; j++)
        fprintf(f, "%6d%5d", rnd2[i][j], res2[i][j]);
      fprintf(f, "\n");
    }
  }
  fclose(f);
}

int LoadFile(char *filename) {
  FILE *f;
  int i, j, x, y, r, z;
  f = fopen(filename, "rt");
  if (f==NULL) return 0;
  // Loading file
  char s[200], *tok[20];
  fscanf(f, "%d %d %d %d %d %d %d\n", &n, &ppv, &tbr, &pr1, &pr2, &rel1, &rel2);
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
  for (i=0; i<n; i++) {
    for (j=0; j<n; j++) {
      fscanf(f, "%d %d", &r, &z);
      rnd[i][j] = r;
      res[i][j]   = z;
      rnd2[i][j] = -1;
      res2[i][j] = -1;
    }
    fscanf(f, "\n");
  }

  if (tbr>=10) {
    for (i=0; i<n; i++) {
      for (j=0; j<n; j++) {
        fscanf(f, "%d %d", &r, &z);
        rnd2[i][j] = r;
        res2[i][j] = z;
      }
      fscanf(f, "\n");
    }
  }

  fclose(f);
  return 1;
}

int Shift() {
  int aux = -1;
  for (int i=0; i<n; i++) {
    for (int j=0; j<n; j++) {
      if (rnd[i][j]<0 && rnd2[i][j]>0) {
        aux = rnd[i][j]; rnd[i][j] = rnd2[i][j]; rnd2[i][j] = aux;
        aux = res[i][j]; res[i][j] = res2[i][j]; res2[i][j] = aux;
      }
    }
  }
}

//---------------------------------------------

int main(int argc, char* argv[]) {
  if (argc < 2) { printf("No input file specified.\n"); return 0; }
  strcpy(inputfile, argv[1]);
  if (!LoadFile(inputfile)) {
    printf("File %s not found.\n", inputfile);
    return 0;
  }
  Shift();
  Dump();
  return 1;
}
