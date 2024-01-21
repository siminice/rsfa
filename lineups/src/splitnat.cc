#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int NFY, NLY;

int main() {
  char s[5000], *tk[100];
  char filename[128];
  char lpr[64], lnm[64], ldb[64], *ndb;
 
  NFY = 1922; NLY = 2013;
  int n;

  ndb = new char[64];

  for (int y=NFY; y<=NLY; y++) {
  n = 0;
  fprintf(stderr, "Anul %d: ", y);
  FILE *f = fopen("nationala.tab", "rt");
  sprintf(filename, "nat-%d.tab", y);
  FILE *of = fopen(filename, "wt");
  while(!feof(f)) {
    fgets(s, 2000, f);
    if (strlen(s)<20) continue;
    tk[0] = strtok(s, "\t\n");
    for (int j=1; j<100; j++) tk[j] = strtok(NULL, "\t\n");

    if (strcmp(lpr, tk[1])==0 && strcmp(lnm, tk[2])==0) {
      if (tk[3][0]==' ') {
        strcpy(ndb, ldb);
        tk[3] = ndb;
      }
    }

    int ey = atoi(tk[6]);
    if (ey==y) {
      for (int j=0; j<100; j++) {
        if (tk[j]!=NULL) fprintf(of, "%s\t", tk[j]);
      }
      fprintf(of, "\n");
      n++;
    }

    strcpy(lpr, tk[1]);
    strcpy(lnm, tk[2]);
    strcpy(ldb, tk[3]);

    s[0] = 0;
  }
  fclose(f);
  fclose(of);
  fprintf(stderr, "%d prezenþe.\n", n);
  }
}
