#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  char s[1024];
  FILE *f = fopen(argv[1], "rt");
  if (!f) return -1;
  int id = 0;
  while (!feof(f)) {
    fgets(s, 1000, f);
    printf("{\"_id\":%d,%s",id++,s+1);
  }
  fclose(f);
  return 0;
}
