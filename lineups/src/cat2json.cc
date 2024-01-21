#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db.h"

#define COLS 7
#define COL_MNEM 0
#define COL_LASTNAME 1
#define COL_FIRSTNAME 2
#define COL_DOB 3
#define COL_NAT 4
#define COL_POB 5
#define COL_COB 6

int mongo;

char *trim(char *s) {
  int len = strlen(s);
  int j=0; while (s[j]==' ' && j<len) j++;
  int k=len-1; while (s[k]==' ' && k>0) k--;
  s[k+1]=0;
  return s+j;
}

void extractDate(char *tok) {
  if (tok==NULL) return;
  if (strlen(tok)<=0) return;
  tok = trim(tok);
  char *tz=strtok(tok, "-./");
  char *tl=strtok(NULL, "-./");
  char *ta=strtok(NULL, "-./");
  if (tz==NULL) return;
  if (strlen(tz)==0) return;
  if (ta==NULL) {
    printf(",\"dob\":\"%s-01-01\"", tz);
    return;
  }
  if (ta!=NULL) {
    int zi = atoi(tz);
    int lu = atoi(tl);
    if (lu<12) {
      printf(",\"dob\":\"%s-%02s-%02s\"", ta, tl, tz);
    } else {
      printf(",\"dob\":\"%s-%02s-%02s\"", ta, tz, tl);
    }
  }
}

void extractField(char *fname, char *tok, char *sep) {
  if (tok==NULL) return;
  tok = trim(tok);
  if (strlen(tok)<=0) return;
  printf("%s\"%s\":\"%s\"", sep, fname, tok);
}

void extractJson(char *ltk[]) {
  extractField("mnem", ltk[COL_MNEM], "{");
  extractField("last", ltk[COL_LASTNAME], ",");
  extractField("first", ltk[COL_FIRSTNAME], ",");
  extractDate(ltk[COL_DOB]);
  extractField("nat", ltk[COL_NAT], ",");
  extractField("pob", ltk[COL_POB], ",");
  extractField("cob", ltk[COL_COB], ",");
  printf("}\n");
}


int main(int argc, char **argv) {
  FILE *f = fopen(argv[1], "rt");
  if (!f) {
    printf("ERROR: File %s not found.", argv[1]);
    return -1;
  }
  char s[2048], t[2048], *ltk[COLS];
  while (!feof(f)) {
    fgets(s, 2048, f);
    if (feof(f)) continue;
    ltk[0] = strtok(s, ",\n");
    for (int i=1; i<COLS; i++) ltk[i] = strtok(NULL, ",\n");
    extractJson(ltk);
  }
  fclose(f);
  return 0;
}
