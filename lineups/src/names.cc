#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAMES	20000

int NP;
char **pmnem;
char **pname;
char **ppren;
char **pdob;
char **pcty;


int FindMnem(char *mnem) {
  for (int i=0; i<NP; i++)
    if (strcmp(pmnem[i], mnem)==0) return i;
  return -1;
}

int FindName(char *name, char *pren, char *dob, char *cty) {
  for (int i=0; i<NP; i++)
    if (strstr(pname[i], name)!=NULL && (pren==NULL || strcmp(ppren[i],pren)==0) && 
        (dob==NULL || strcmp(pdob[i], dob)==0)) return i;
  return -1;
}

void Load() {
  NP = 0;
  FILE *f = fopen("players.dat", "rt");
  if (f==NULL) {
    printf("ERROR: player database not found!\n");
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
  for (int i=0; i<NP; i++) {
    fgets(s, 100, f);
//    printf("... parsing %s", s);
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
//     printf("Loaded %s: %s %s [%s] (%s)\n", pmnem[i], ppren[i], pname[i], pdob[i], pcty[i]);
  }
  fclose(f);
}

void Save() {
  FILE *f = fopen("players.new", "wt");
  fprintf(f, "%d\n", NP);
  for (int i=0; i<NP; i++) {
    fprintf(f, "%s,%s,%s,%s,%s\n", pmnem[i], pname[i], ppren[i], pdob[i], pcty[i]);
  }
  fclose(f);
  rename("players.dat", "players.old");
  rename("players.new", "players.dat");
}


void Extract() {
  char s[100], *t[4];
  char r;
  int da, db, cont;
  int pa, pb, sa[5], sb[5];
  do {
    printf("Last,first,dob,cty: ");
    fgets(s, 100, stdin);
    if (s==NULL || s[0]==0) continue;
    t[0] = strtok(s, ","),
    t[1] = strtok(NULL, ",");
    t[2] = strtok(NULL, ",");
    t[3] = strtok(NULL, ",\n");
    pa = FindName(t[0], t[1], t[2], t[3]);
    if (pa>=0) {
      printf("Found: %s %s [%s] (%s)\n", ppren[pa], pname[pa], pdob[pa], pcty[pa]);
    }
    else if (pa<0) {
      printf("Name %s %s [%s] (%s) not found. Add? [1]=yes ", t[1], t[0], t[2], t[3]);
      int add = 1;
      scanf("%d", &add);
      if (add==1) {
        pname[NP] = new char[strlen(t[0])+1];
        strcpy(pname[NP], t[0]);
        ppren[NP] = new char[strlen(t[1])+1];
        strcpy(ppren[NP], t[1]);
        pdob[NP] = new char[12];
        if (t[2]!=NULL) strcpy(pdob[NP], t[2]); else strcpy(pdob[NP], "00/00/0000");
        pcty[NP] = new char[4];
        if (t[3]!=NULL) strcpy(pcty[NP], t[3]); else strcpy(pcty[NP], "ROM");

        pmnem[NP] = new char[7];
        pmnem[NP][0] = pname[NP][0]+32;
        int d = 0; int k=1;
        while (k<4) {
          if (k>=strlen(t[0])) pmnem[NP][k]='x'; 
          else if (pname[NP][k+d]==' ') {d++; k--;}
          else pmnem[NP][k] = pname[NP][k+d];
          k++;
        }
        if (t[1]!=NULL) pmnem[NP][4] = ppren[NP][0]+32; else pmnem[NP][4] = '_';
        if (t[1]!=NULL) {
          if (strlen(t[1])<2) pmnem[NP][5] = '_';
          else if (t[1][1]>='A' && t[1][1]<='Z') pmnem[NP][5] = ppren[NP][1]+32; 
          else pmnem[NP][5] = ppren[NP][1];
        }
        pmnem[NP][6] = 0;
        if (FindMnem(pmnem[NP])>=0) {
          printf("...Mnemonic %s already in use.\n", pmnem[NP]);
          int k=0;
          do {
            k++;
            pmnem[NP][5] = (char) (k+48);
          } while (FindMnem(pmnem[NP])>=0);
        }
        pa = NP;
        printf(".Added player #%d, \033[33;1m%s\033[0m: %s %s [%s] (%s)\n", NP+1, pmnem[NP], ppren[NP], pname[NP], pdob[NP], pcty[NP]);
        NP++;
      }
    }
//    else printf("Identified: %s %s (%s)\n", ppren[pa], pname[pa], pdob[pa]);
    s[0] = 0;
    printf("Continue? [0/1] "); scanf("%d", &cont);
    fgets(s, 100, stdin);
  } while (cont==1);
}


int main(int argc, char **argv) {
  Load();
  Extract();
  Save();
  return 0;
}
