#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DB_ROWS         400
#define DB_COLS         100
#define DB_CELL         12

char db[DB_ROWS][DB_COLS][DB_CELL];

void CheckDB(char *filename) {
  char s[5000], *tk[DB_COLS];
  FILE *f = fopen(filename, "rt");
  if (f==NULL) { fprintf(stderr, "ERROR: database %s not found.\n", filename); return; }
  int i = -1;
  while (!feof(f)) {
    fgets(s, 5000, f);
    i++;
    if (strlen(s) < 100) continue;
    tk[0] = strtok(s, ",\n");
    for (int j=1; j<DB_COLS; j++) tk[j]=strtok(NULL, ",\n");
    for (int j=0; j<DB_COLS; j++) {
      if (tk[j]!=NULL) strcpy(db[i][j], tk[j]);
      else strcpy(db[i][j], " ");
    }
    for (int j=1; j<=11; j++) {
      if (db[i][4+j][0]==' ') {
        fprintf(stderr, "Line #%3d, col %2d [%s-%s]: %s\n", i+1, j, db[i][0], db[i][1], db[i][4+j]);
      }
    }
    for (int j=1; j<=11; j++) {
      if (db[i][4+22+j][0]==' ') {
        fprintf(stderr, "Line #%3d, col %2d [%s-%s]: %s\n", i+1, 22+j, db[i][0], db[i][1], db[i][4+j]);
      }
    }
    s[0] = 0;
  }
  fclose(f);
}

int main(int argc, char* argv[]) {
  char filename[256];
  if (argc < 2) {
    printf("ERROR: No input file specified.\n");
    return -1;
  }

  CheckDB(argv[1]);
   
  return 0;
}
