#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db.h"

#define DB_COLS 60

int mongo;

char *trim(char *s) {
  int len = strlen(s);
  int j=0; while (s[j]==' ' && j<len) j++;
  int k=len-1; while (s[k]==' ' && k>0) k--;
  s[k+1]=0;
  return s+j;
}

char *ename(char et) {
  if (et=='"') return "pg";
  if (et=='`') return "og";
  if (et=='/') return "pm";
  return "g";
}

void extractDate(char *tok) {
  if (tok==NULL) return;
  char *td = strtok(tok, "@");
  char *tt = strtok(NULL, "@");
  td = trim(td);
  tt = trim(tt);
  if (strlen(td)<=0) return;
  char *tz=strtok(td, "-");
  char *tl=strtok(NULL, "-");
  char *ta=strtok(NULL, "@");
  if (tz==NULL || tl==NULL || ta==NULL) return;
  if (strlen(tz)==0 || strlen(tl)==0 || strlen(ta)==0) return;
  if (strcmp(tz,"1")==0) return;
  if (strcmp(tl,"00")==0) return;
  if (mongo) {
    printf(",\"date\":ISODate(\"%s-%s-%s", ta, tl, tz);
  } else {
    printf(",\"date\":\"%s-%s-%s", ta, tl, tz);
  }
  if (strlen(tt)>0) printf("T%s+02:00", tt);
  if (mongo) {
    printf("\")");
  } else {
    printf("\"");
  }
}

void extractField(char *fname, char *tok, char *sep, char *quote) {
  if (tok==NULL) return;
  tok = trim(tok);
  if (strlen(tok)<=0) return;
  printf("%s\"%s\":%s%s%s", sep, fname, quote, tok, quote);
}

void extractCap(char *tok, const char *sep) {
  if (tok==NULL) return;
  char *tn = strtok(tok, ":");
  char *tm = strtok(NULL, ":");
  tn = trim(tn);
  tm = trim(tm);
  if (strlen(tn)<=0 || strlen(tm)<=0) return;
  printf("%s{\"mnem\":\"%s\",\"min\":%s}", sep, tn, tm);
}

void extractEvent(char *tok, const char *sep) {
  if (tok==NULL) return;
  char et = tok[6];
  char *tn = new char[12];
  char *tm = new char[12];
  strncpy(tn, tok, 6); tn[6]=0;
  strncpy(tm, tok+7, 3); tm[3]=0;
  tn = trim(tn);
  tm = trim(tm);
  if (strlen(tn)<=0 || strlen(tm)<=0) return;
  printf("%s{\"type\":\"%s\",\"mnem\":\"%s\",\"min\":\"%s\"}", sep, ename(et), tn, tm);
}

void extractJson(char *ltk[], char *etk[]) {
  extractField("home", ltk[DB_HOME], "{", "");
  extractField("away", ltk[DB_AWAY], ",", "");
  extractField("score", ltk[DB_SCORE], ",", "");
  extractDate(ltk[DB_DATE]);
  extractField("comp", ltk[DB_COMP], ",", "\"");
  extractField("round", ltk[DB_ROUND], ",", "\"");
  extractField("venue", ltk[DB_VENUE], ",", "\"");
  extractField("att", ltk[DB_ATTEND], ",", "\"");
  extractField("weather", ltk[DB_WEATHER], ",", "\"");
  extractField("ref", ltk[DB_REF], ",", "\"");
  extractField("assist1", ltk[DB_ASIST1], ",", "\"");
  extractField("assist2", ltk[DB_ASIST2], ",", "\"");
  extractField("obs", ltk[DB_OBSERV], ",", "\"");
  printf(",\"rosters\":[");
  printf("{\"starters\":[");
  for (int i=DB_T1+1; i<=DB_T1+11; i++) {
    extractCap(ltk[i], (i==DB_T1+1? "" : ","));
  }
  printf("], \"subs\":[");
  for (int i=DB_T1+12; i<=DB_T1+22; i++) {
    extractCap(ltk[i], (i==DB_T1+12? "" : ","));
  }
  printf("]");
  extractField("coach", ltk[DB_COACH1], ",", "\"");
  printf("},{\"starters\":[");
  for (int i=DB_T2+1; i<=DB_T2+11; i++) {
    extractCap(ltk[i], (i==DB_T2+1? "" : ","));
  }
  printf("], \"subs\":[");
  for (int i=DB_T2+12; i<=DB_T2+22; i++) {
    extractCap(ltk[i], (i==DB_T2+12? "" : ","));
  }
  printf("]");
  extractField("coach", ltk[DB_COACH2], ",", "\"");
  printf("}],\"events\":[");
  for (int i=0; i<30; i++) {
    extractEvent(etk[i], (i==0?"":","));
  }
  printf("]}\n");
}


int main(int argc, char **argv) {
  FILE *f = fopen(argv[1], "rt");
  if (!f) {
    printf("ERROR: File %s not found.", argv[1]);
    return -1;
  }
  FILE *h = fopen(argv[2], "rt");
  if (!h) {
    printf("ERROR: File %s not found.", argv[2]);
    return -1;
  }
  mongo = argc>3;
  char s[2048], t[2048], *ltk[DB_COLS], *etk[DB_COLS];
  while (!feof(f)) {
    fgets(s, 2048, f);
    if (feof(f)) continue;
    ltk[0] = strtok(s, ",\n");
    for (int i=1; i<DB_COLS; i++) ltk[i] = strtok(NULL, ",\n");
    fgets(t, 2048, h);
    etk[0] = strtok(t, ",\n");
    for (int i=1; i<DB_COLS; i++) etk[i] = strtok(NULL, ",\n");
    extractJson(ltk, etk);
  }
  fclose(f);
  fclose(h);
  return 0;
}
