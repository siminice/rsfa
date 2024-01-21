#include<string.h>
#include<stdlib.h>
#include<stdio.h>

int main(int argc, char **args) {
  if (argc < 2) {
    fprintf(stderr, "Error: no input file specified.\n");
    return 1;
  }
  FILE *f = fopen(args[1], "rt");
  if (!f) {
    fprintf(stderr, "Error: cannot open file %s.\n", args[1]);
    return 1;
  }
  char *s = new char[1000];
  char *t = new char[1000];
  t[0] = 0;
  while (!feof(f)) {
    fgets(s, 1000, f);
    if (!feof(f)) {
      s[strlen(s)-1] = 0;
      if (strstr(s, ":")!=NULL || strstr(s, "–")!=NULL ) {
        if (strlen(t) > 10) fprintf(stdout, "%s\n", t);
        strncpy(t, s, 1000);
      } else {
        strcat(t, s);
      }
    }
  }
  fprintf(stdout, "%s\n", t);
  fclose(f);
  return 0;
}
