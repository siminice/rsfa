#include <string.h>
#include <stdio.h>

int main (int argc, char** argv) {
  FILE *f = fopen(argv[1], "rt");
  if (f==NULL) {
    fprintf(stderr, "ERROR: file %s not found.\n", argv[1]);
    return -1;
  }
  char s[5000];
  while (!feof(f)) {
    fgets(s, 5000, f);
    if (s[0]=='\t') printf("_");
    printf("%s", s);
    s[0] = 0;
  }
  fclose(f);
  return 0;
}
