#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
  char *s = new char[1024];
  char ***r;
  char *tok[22];
  int *nl = new int[4];;
  r = new char**[2];
  r[0] = new char*[22];
  r[1] = new char*[22];
  int *nr = new int[2];
  nr[0] = 0;
  nr[1] = 0;
  for (int l=0; l<4; l++) {
    fgets(s, 1024, stdin);
    tok[0] = strtok(s, " \n");
    for (int i=1; i<11; i++) tok[i] = strtok(NULL,  " \n");
    nl[l] = 0; while (tok[nl[l]]!=NULL) nl[l]++;
    for (int i=0; i<nl[l]; i++) {
      if (tok[i][0]>='0' && tok[i][0]<='9') continue;
      if (strstr(tok[i], "aut.")!=NULL) continue;
      if (strstr(tok[i], "pen.")!=NULL) continue;
      r[l/2][nr[l/2]] = new char[128];
      strncpy(r[l/2][nr[l/2]], tok[i], strlen(tok[i]));
      nr[l/2]++;
    }
  }
  printf("%s: ", r[0][0]);
  for (int i=1; i<nr[0]; i++) {
    printf("%s", r[0][i]);
    if (i<nr[0]-1) {
      if (r[0][i+1][0]!='(') printf(", "); else printf(" ");
    }
  }
  printf(" | ");
  printf("%s: ", r[1][0]);
  for (int i=1; i<nr[1]; i++) {
    printf("%s", r[1][i]);
    if (i<nr[1]-1) {
      if (r[1][i+1][0]!='(') printf(", "); else printf(" ");
    }
  }
  printf("\n");
  return 0;
}
