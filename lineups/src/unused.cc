#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int tm, pl, cr;
int FY, LY;
int ECFY, ECLY;
int KFY, KLY;
int NFY, NLY;

char **club;
char **mnem;
int  NC;
int year;

//--------------------------------------

#define MAX_NAMES	20000

/* *************************************** */

int NP;
char **pmnem;
char **pname;
char **ppren;
char **pdob;
char **pcty;
char **ppob;
char **pjud;

int *used;

/* *************************************** */

int FindMnem(char *mnem) {
  for (int i=0; i<NP; i++)
    if (strcmp(pmnem[i], mnem)==0) return i;
  return -1;
}

void LoadPlayers() {
  NP = 0;
  FILE *f = fopen("catalogs/players.dat", "rt");
  if (f==NULL) {
    fprintf(stderr, "ERROR: player database not found!\n");
    exit(0);
  }
  char s[100], *t[10];
  fgets(s, 100, f);
  sscanf(s, "%d", &NP);
  pname = new char*[MAX_NAMES];
  pmnem = new char*[MAX_NAMES];
  ppren = new char*[MAX_NAMES];
  pdob  = new char*[MAX_NAMES];
  pcty  = new char*[MAX_NAMES];
  ppob  = new char*[MAX_NAMES];
  pjud  = new char*[MAX_NAMES];
  used  = new int[MAX_NAMES];

  for (int i=0; i<NP; i++) {
    fgets(s, 100, f);
    t[0] = strtok(s, ",\t\n");
     pmnem[i] = new char[7];
     strcpy(pmnem[i], t[0]);
    t[1] = strtok(NULL, ",\t\n");
     pname[i] = new char[strlen(t[1])+1];
     strcpy(pname[i], t[1]);
    t[2] = strtok(NULL, ",\t\n");
     ppren[i] = new char[strlen(t[2])+1];
     strcpy(ppren[i], t[2]);
    t[3] = strtok(NULL, ",\t\n");
     pdob[i] = new char[12];
     strcpy(pdob[i], t[3]);
    t[4] = strtok(NULL, ",\t\n");
     pcty[i] = new char[4];
     strcpy(pcty[i], t[4]);
    t[5] = strtok(NULL, ",\t\n");
     if (t[5]!=NULL) {
       ppob[i] = new char[strlen(t[5])+1];
       strcpy(ppob[i], t[5]);
     }
     else {
       ppob[i] = new char[2];
       strcpy(ppob[i], "");
     }
    t[6] = strtok(NULL, ",\t\n");
     if (t[6]!=NULL) {
       pjud[i] = new char[strlen(t[6])+1];
       strcpy(pjud[i], t[6]);
     }
     else {
       pjud[i] = new char[2];
       strcpy(pjud[i], "");
     }

     used[i] = 0;
  }
  fclose(f);
  fprintf(stderr, "Loaded %d names.\n", NP);
}

void Collect(int year) {
  char filename[128];
  char s[1000], *tok[100];
  int p;

  sprintf(filename, "catalogs/catalog-%d.dat", year);
  FILE *f = fopen(filename, "rt");
  if (f==NULL) {
    fprintf(stderr, "ERROR: Cannot open file %s.\n", filename);
    return;
  }
  
  do {
    fgets(s, 1000, f);
    if (strlen(s)<10) continue;
    tok[0] = strtok(s, ",\n");
    if (tok[0]!=NULL) p=FindMnem(tok[0]);
    if (p>=0) used[p] = 1;
    s[0] = 0;
  } while (!feof(f));

  fclose(f);
}

void CollectCup(int year) {
  char filename[128];
  char s[1000], *tok[100];
  int p;

  sprintf(filename, "catalogs/cupa-%d.dat", year);
  FILE *f = fopen(filename, "rt");
  if (f==NULL) {
    fprintf(stderr, "ERROR: Cannot open file %s.\n", filename);
    return;
  }
  
  do {
    fgets(s, 1000, f);
    if (strlen(s)<10) continue;
    tok[0] = strtok(s, ",\n");
    if (tok[0]!=NULL) p=FindMnem(tok[0]);
    if (p>=0) used[p] = 1;
    s[0] = 0;
  } while (!feof(f));
  fclose(f);
}

void CollectEC(int year) {
  char filename[128];
  char s[1000], *tok[100];
  int p;

  sprintf(filename, "catalogs/euro-%d.dat", year);
  FILE *f = fopen(filename, "rt");
  if (f==NULL) {
    fprintf(stderr, "ERROR: Cannot open file %s.\n", filename);
    return;
  }
  
  do {
    fgets(s, 1000, f);
    if (strlen(s)<10) continue;
    tok[0] = strtok(s, ",\n");
    if (tok[0]!=NULL) p=FindMnem(tok[0]);
    if (p>=0) used[p] = 1;
    s[0] = 0;
  } while (!feof(f));
  fclose(f);
}

void CollectNat(int year) {
  char filename[128];
  char s[1000], *tok[100];
  int p;

  sprintf(filename, "catalogs/nat-%d.dat", year);
  FILE *f = fopen(filename, "rt");
  if (f==NULL) {
    fprintf(stderr, "ERROR: Cannot open file %s.\n", filename);
    return;
  }
  
  do {
    fgets(s, 1000, f);
    if (strlen(s)<10) continue;
    tok[0] = strtok(s, ",\n");
    if (tok[0]!=NULL) p=FindMnem(tok[0]);
    if (p>=0) used[p] = 1;
    s[0] = 0;
  } while (!feof(f));
  fclose(f);
}

int main(int argc, char **argv) {

  FY   = 1933; LY   = 2018;
  KFY  = 1934; KLY  = LY;
  ECFY = 1957; ECLY = LY;
  NFY  = 1922; NLY  = LY;

  for (int k=1; k<argc; k++) {
    if (strcmp(argv[k], "-fy")==0 && k<argc-1)  {
      FY = atoi(argv[++k]);
    }
    else if (strcmp(argv[k], "-ly")==0 && k<argc-1)  {
      LY = atoi(argv[++k]);
    }
    else if (strcmp(argv[k], "-kfy")==0 && k<argc-1)  {
      KFY = atoi(argv[++k]);
    }
    else if (strcmp(argv[k], "-lfy")==0 && k<argc-1)  {
      KLY = atoi(argv[++k]);
    }
  }

  LoadPlayers();

  for (year=FY; year<=LY; year++) {
    Collect(year);
  }

  for (year=KFY; year<=KLY; year++) {
    CollectCup(year);
  }

  for (year=ECFY; year<=ECLY; year++) {
    CollectEC(year);
  }

  for (year=NFY; year<=NLY; year++) {
    CollectNat(year);
  }

  for (int p=0; p<NP; p++)
  if (used[p]==0) {
    printf("Unused %5d %s\n", p, pmnem[p]);
  }

  return 0;
}
