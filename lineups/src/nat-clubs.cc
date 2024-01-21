#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db.h"

#define CAT_ROWS    2000
#define CAT_CELL    100

int  nccat, npcat, nncat;
int  year;

char ***currcat, ***prevcat, ***newcat;
char *currclub, *prevclub;

//--------------------------------------

void LoadCatalog(char *filename, char ***cat, int &ncat) {
  FILE *f = fopen(filename, "rt");
  if (f==NULL) {
    fprintf(stdout, "ERROR: Catalog %s not found.\n", filename);
    return;
  }
  ncat = 0;
  char s[1000], *tk[CAT_COLS];
  while (!feof(f)) {
    fgets(s, 1000, f);
    if (strlen(s)<10) continue;
    cat[ncat] = new char*[CAT_COLS];
    tk[0]=strtok(s, ",\n");
    for (int j=1; j<CAT_COLS; j++) tk[j] = strtok(NULL, ",\n");
    for (int j=0; j<CAT_COLS; j++) {
        cat[ncat][j] = new char[CAT_CELL];
		if (tk[j]!=NULL) strcpy(cat[ncat][j], tk[j]); else strcpy(cat[ncat][j], " ");
    }
    ncat++;
    s[0] = 0;
  }
  fclose(f);
}

int FindMnem(char *mnem, char ***cat, int ncat) {
	for (int i=0; i<ncat; ++i) {
		if (strcmp(cat[i][CAT_MNEM], mnem)==0) {
            return i;
        }
	}
    return -1;
}

void GetClubName(char *mnem, char ***cat, int ncat, char *club) {
  strcpy(club, "");
  for (int i=0; i<ncat; i++) {
    if (strcmp(cat[i][CAT_MNEM], mnem)==0) {
      if (strlen(club)==0) {
         strcat(club, cat[i][CAT_CLUB]);
      } else {
        strcat(club, "&");
        strcat(club, cat[i][CAT_CLUB]);
      }
    }
  }
}

void MakeNewCatalog() {
  for (int i=0; i<nncat; i++) {
    fprintf(stdout, "%s,%s,%s,", newcat[i][CAT_MNEM], newcat[i][CAT_PREN], newcat[i][CAT_NAME]);
    for (int j=CAT_DOB; j<CAT_CLUB; j++) {
      fprintf(stdout, "%s,", newcat[i][j]);
    }
        GetClubName(newcat[i][CAT_MNEM], prevcat, npcat, prevclub);
        GetClubName(newcat[i][CAT_MNEM], currcat, nccat, currclub);
        if (strcmp(prevclub, currclub)==0 || strlen(prevclub)<3) {
          fprintf(stdout, "%s,", currclub);
        } else if (strlen(currclub)<3) {
          fprintf(stdout, "%s,", prevclub);
        } else {
          fprintf(stdout, "%s + %s,", prevclub, currclub);
        }
    for (int j=CAT_CLUB+1; j<CAT_GOALS; j++) {
      fprintf(stdout, "%s,", newcat[i][j]);
    }
    fprintf(stdout, "%s\n", newcat[i][CAT_GOALS]);
  }
}

//---------------------------------------------

int main(int argc, char* argv[]) {
  if (argc < 2) {
    fprintf(stdout, "ERROR: No edition specified.\n");
    fprintf(stdout, "Usage: nat-clubs yyyy\n");
    return -1;
  }

  year = atoi(argv[1]);

  char filename[128];
  currclub = new char[256];
  prevclub = new char[256];
  currcat = new char**[CAT_ROWS];
  prevcat = new char**[CAT_ROWS];
  newcat = new char**[CAT_ROWS];
  sprintf(filename, "oldnat/nat-%d.dat", year-1);
  LoadCatalog(filename, prevcat, npcat);
  sprintf(filename, "oldnat/nat-%d.dat", year);
  LoadCatalog(filename, currcat, nccat);
  sprintf(filename, "nnat-%d.dat", year);
  LoadCatalog(filename, newcat, nncat);

  MakeNewCatalog();

  return 0;
}
