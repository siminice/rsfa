#include <string.h>
#include <stdio.h>

int main(int argc, char **args) {
  if (argc<1) { fprintf(stderr, "Error: no input file specified.\n"); return -1; }
  FILE *f = fopen(args[1], "rt");
  if (!f) { fprintf(stderr, "Error: file %s not found.\b", args[1]); return -2; }
  char s[2000], *tk[10], post, num[10];
  int n = 0;
  strcpy(num, " ");
  while (!feof(f)) {
    fgets(s, 2000, f);
    if (strstr(s, "colspan=\"7\" ")!=NULL) {
      if (strstr(s, ">Goalkeeper<")!=NULL) post='P';
      if (strstr(s, ">Defender<")!=NULL) post='F';
      if (strstr(s, ">Midfielder<")!=NULL) post='M';
      if (strstr(s, ">Forward<")!=NULL) post='A';
      if (strstr(s, ">Manager<")!=NULL) post='C';
      if (strstr(s, ">Ass. Manager<")!=NULL) { fclose(f); return 0; };
    } else if (strstr(s, "kleine_schrift")!=NULL) {
      char *tkn = strtok(strstr(s, "<span"), ">"); tkn = strtok(NULL, "<");
      if ((tkn!=NULL) && strstr(tkn, "span")==NULL) { strncpy(num, tkn, 3); }
    } else if (strstr(s, "/player_summary/")!=NULL) {
      n++;
      if (n%2==0) {
        tk[0] = strtok(s, "/");
        for (int i=1; i<3; i++) tk[i] = strtok(NULL, "/");
        printf("%c,%s,%s\n", post, num, tk[2]);
      }
    }
  }
  fclose(f);
  return 0;
}
