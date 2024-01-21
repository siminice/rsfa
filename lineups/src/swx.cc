#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int extract(char *filename) {
  FILE *f = fopen(filename, "rt");
  if (!f) {
    fprintf(stderr, "File %s not found.", filename);
    return -1;
  }
  char *s = new char[1024];
  char *roster[50], *tk[10];
  int p=0;
  while (!feof(f)) {
    fgets(s, 1000, f);
    if (strstr(s, "Coach")==s) continue;
    tk[0] = strtok(s, " \n"); for (int i=1; i<10; i++) tk[i] = strtok(NULL, " \n");
    int n = 0; for (int i=0; i<10; i++) if (tk[i]) n++;
    int t1 = 0;
    int t2 = n-1;
    if (tk[t1][0]>='0' && tk[t1][0]<='9') t1++;
    if (strlen(tk[t1])<3 && tk[t1][1]=='.') tk[t1][1]=' ';
    while (tk[t2][0]>='0' && tk[t2][0]<='9' && t2 >= t1) {
      t2--;
    };
    if (strstr(tk[t2], "Substituted")==tk[t2]) t2--;
    roster[p] = new char[100];
    strncpy(roster[p], tk[t1], 100);
    for (int j=t1+1; j<=t2; j++) {
      strcat(roster[p], " ");
      strcat(roster[p], tk[j]);
    }
    s[0] = 0;
    p++;
  }
  fprintf(stdout, "H: ");
  for (int p=0; p<11; p++) {
    fprintf(stdout, "%s, ", roster[p]);
  }
  fprintf(stdout, "\nA:");
  for (int p=11; p<22; p++) {
    fprintf(stdout, "%s, ", roster[p]);
  }
  fprintf(stdout, "\n");
  return 0;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    return -1;
  }
  extract(argv[1]);
  return 0;
}
