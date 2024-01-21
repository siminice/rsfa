#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "db.h"

#define DB_ROWS         400
#define DB_COLS         60
#define DB_CELL         12

char db[DB_ROWS][DB_COLS][DB_CELL];
int nrows;

//--------------------------------------

int LoadDB(char *filename) {
  char s[5000], *tk[DB_COLS];
  nrows = 0;
  FILE *f = fopen(filename, "rt");
  if (f==NULL) { fprintf(stderr, "ERROR: database %s not found.\n", filename); return -1; }
  while (!feof(f)) {
    fgets(s, 5000, f);
    if (strlen(s) < 100) continue;
    tk[0] = strtok(s, ",\n");
    for (int j=1; j<DB_COLS; j++) tk[j]=strtok(NULL, ",\n");
    for (int j=0; j<DB_COLS; j++) {
      if (tk[j]!=NULL) strcpy(db[nrows][j], tk[j]);
      else strcpy(db[nrows][j], " ");
    }
    nrows++;
    s[0] = 0;
  }
  fclose(f);
  return 0;
}

int SaveDB(char *filename) {
  char nfilename[64];
  sprintf(nfilename, "%s.new", filename);
  FILE *f = fopen(filename, "wt");
  if (f==NULL) { fprintf(stderr, "ERROR: database %s not found.\n", filename); return -1; }
  for (int i=0; i<nrows; i++) {
    for (int j=0; j<DB_SCORE; j++) fprintf(f, "%s,", db[i][j]);
    fprintf(f, "    ,");
    for (int j=DB_SCORE; j<DB_COLS; j++) fprintf(f, "%s,", db[i][j]);
    fprintf(f, "\n");
  }
  fclose(f);
}

int main(int argc, char* argv[]) {
  if (argc < 2) { 
    printf("ERROR: No input file specified.\n"); 
    return -1; 
  }

  LoadDB(argv[1]);
  SaveDB(argv[1]);
  return 0;
}
