#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "catalog.hh"
#include "db.h"

#define CAT_ROWS    2000
#define CAT_CELL     100
//burdca,Carol,Burdan,00/00/1912,ROM,173, , ,3,3,0,-11

Catalog *Pl;
int  NP, NC;

char cat[CAT_ROWS][CAT_COLS][CAT_CELL];

//--------------------------------------

int LoadCatalog(char *filename) {
  FILE *f = fopen(filename, "rt");
  if (f==NULL) {
    fprintf(stdout, "ERROR: Catalog %s not found.\n", filename);
    return 0;
  }
  NC = 0;
  char s[1000], *tk[CAT_COLS];
  while (!feof(f)) {
    fgets(s, 1000, f);
    if (strlen(s)<10) continue;
    tk[0]=strtok(s, ",\n");
    for (int j=1; j<CAT_COLS; j++) tk[j] = strtok(NULL, ",\n");
    for (int j=0; j<CAT_COLS; j++)
			if (tk[j]!=NULL) strcpy(cat[NC][j], tk[j]); else strcpy(cat[NC][j], " ");
    NC++;
    s[0] = 0;
  }
  fclose(f);
  return 1;
}

int SaveCatalog(char *filename) {
  char newfilename[255];
  sprintf(newfilename, "%s.new", filename);
  FILE *f = fopen(newfilename, "wt");
  if (f==NULL) {
    fprintf(stdout, "ERROR: Catalog %s not found.\n", newfilename);
    return 0;
  }
  for (int i=0; i<NC; i++) {
    for (int j=0; j<CAT_COLS; j++) {
      if (cat[i][j]!=NULL) fprintf(f, "%s", cat[i][j]);
      if (j<CAT_COLS-1) fprintf(f, ",");
    }
    fprintf(f, "\n");
  }
  fclose(f);
  return 1;
}



int isUnknown(char *dob) {
  if (dob==NULL) return 1;
  if (strlen(dob)==0) return 1;
  if (strcmp(dob, " ")==0) return 1;
  if (strcmp(dob, "0000-00-00")==0) return 1;
  return 0;
}

int yearOnly(char *dob) {
  if (dob==NULL) return 0;
  if (strlen(dob)==4) return atoi(dob);
  char s[255];
  strncpy(s, dob, 255);
  char *tky = strtok(s, "-");
  char *tkm = strtok(NULL, "-");
  char *tkd = strtok(NULL, "-");
  if (tky==NULL || tkm==NULL || tkd==NULL) return 0;
  int y = atoi(tky);
  int m = atoi(tkm);
  int d = atoi(tkd);
  if (m==0 && d==0) return y;
  return 0;
}

void InjectDob() {
  char dob1[255], dob2[255];
  for (int i=0; i<NC; i++) {
    int pid = Pl->FindMnem(cat[i][CAT_MNEM]);
    if (pid<0) {
      printf("Mnem %s not found.\n", cat[i][CAT_MNEM]);
    } else {
      strncpy(dob1, Pl->P[pid].dob, 255);
      strncpy(dob2, cat[i][CAT_DOB], 255);
      if (isUnknown(dob1) && isUnknown(dob2)) continue;
      int y1 = yearOnly(dob1);
      int y2 = yearOnly(dob2);
      if (y1==y2 && y1>0) continue;
      if (strcmp(dob1, dob2)!=0) {
        printf("%s: %s %s\n", cat[i][CAT_MNEM], Pl->P[pid].dob, cat[i][CAT_DOB]);
        if (!isUnknown(dob1)) strcpy(cat[i][CAT_DOB], dob1);
      }
    }
  }
}

//---------------------------------------------

int main(int argc, char* argv[]) {
  char filename[255];
  if (argc < 2) {
    fprintf(stdout, "ERROR: No file specified.\n");
    return -1;
  }
  strcpy(filename, argv[1]);
  int save = 0;
  if (argc > 2 && strcmp(argv[2], "-s")==0) save = 1;

  Pl = new Catalog();
  Pl->Load("players.dat");
  NP = Pl->Size();
  if (!LoadCatalog(filename)) {
    fprintf(stdout, "ERROR: loading file %s.\n", filename);
    return -2;
  }

  InjectDob();
  if (save) SaveCatalog(filename);
  return 0;
}
