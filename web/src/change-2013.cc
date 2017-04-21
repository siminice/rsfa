#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LOG 10000
#define MAX_STR 512
#define MIN_STR 20
#define TK_MOVE   2
#define TK_OLDURL 1
#define TK_NEWURL 6

char logs[MAX_LOG][MAX_STR];
char ourl[MAX_STR], nurl[MAX_STR];
int nl;

void newURL(char *oldURL) {
  char t[MAX_STR], *tok[10], *w[20];
  strcpy(nurl, "");
  for (int i=0; i<nl; i++) {
    if (strstr(logs[i], oldURL)!=NULL) {
      strcpy(t, logs[i]);
      tok[0] = strtok(t, "\t\n");
      for (int j=1; j<10; j++) tok[j] = strtok(NULL, "\t\n");
      if (tok[TK_MOVE]==NULL) { fprintf(stderr, "ERROR: tokenizing %s.\n", oldURL); return; }
      w[0] = strtok(tok[TK_MOVE], " \t\n");
      for (int j=1; j<20; j++) w[j] = strtok(NULL, " \t\n");
      if (w[TK_NEWURL]==NULL) { fprintf(stderr, "ERROR: tokenizing %s.\n", tok[TK_MOVE]); return; }
      strcpy(nurl, w[TK_NEWURL]);
      return;
    }
  }
}

int main(int argc, char **argv) {
  FILE *lf = fopen(argv[1], "rt");
  if (lf==NULL) { fprintf(stderr, "ERROR: File %s not found.\n", argv[1]); return -1; }
  
  char s[MAX_STR], *tk[20], *tko;
  nl = 0;
  while(!feof(lf)) {
    fgets(s, MAX_STR, lf);
    if (strstr(s, "has moved")!=NULL) strcpy(logs[nl++], s);
    s[0] = 0;
  }
  fclose(lf);

  char ifilename[64];
  sprintf(ifilename, "2013/%s", argv[2]);
  FILE *inf = fopen(ifilename, "rt");
  FILE *of = fopen(argv[2], "wt");
  if (inf==NULL) { fprintf(stderr, "ERROR: File %s not found.\n", ifilename); return -1; }
  if (of==NULL) { fprintf(stderr, "ERROR: File %s not found.\n", argv[2]); return -1; }

  while (!feof(inf)) {
    fgets(s, MAX_STR, inf);
    if (strstr(s, "http")!=NULL) {
      tk[0] = strtok(s, " \t\n");
      for (int j=1; j<20; j++) {
        tk[j] = strtok(NULL, " \t\n");
      }
      if (tk[TK_OLDURL]==NULL) { fprintf(stderr, "ERROR: tokenizing %s.\n", s); continue; }
      // remove quotes
      int lt = strlen(tk[TK_OLDURL]);
      if (tk[TK_OLDURL][lt-1]=='"') tk[TK_OLDURL][lt-1] = 0;
      tk[TK_OLDURL]++;
      tko = strstr(tk[TK_OLDURL], "www");
      strcpy(ourl, tko);
      newURL(ourl);
      fprintf(stdout, "%s -> %s\n", ourl, nurl);
      if (strlen(nurl)>0) {
        fprintf(of, "%s \"http://www.soccerway.com%s\"", tk[0], nurl);
        for (int j=2; j<20; j++) if (tk[j]!=NULL) fprintf(of, " %s", tk[j]);
        fprintf(of, "\n");
      }
      else {
        fprintf(of, "%s", tk[0]);
        for (int j=1; j<20; j++) if (tk[j]!=NULL) fprintf(of, " %s", tk[j]);
        fprintf(of, "\n");
      }
    }
    else {
      fprintf(of, "%s", s);
    }
    s[0] = 0;
  }  

  fclose(of);
  fclose(inf);
}
