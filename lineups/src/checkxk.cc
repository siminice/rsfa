#include <string.h>
#include <stdio.h>

int main(int argc, char **argv) {
  FILE *f = fopen(argv[1], "rt");
  if (f==NULL) return -1;
  char s[1000];
  while (!feof(f)) {
    fgets(s, 1000, f);
    if (strlen(s)<20) fprintf(stdout, "%s", s);
  }
  fclose(f);
  return 0;
}
