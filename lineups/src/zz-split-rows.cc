#include<string.h>
#include<stdio.h>

int main(int argc, char **argv) {
  if (argc < 2) {
    return -1;
  }
  char filename[64];
  strncpy(filename, argv[1], 64);
  FILE *f = fopen(filename, "rt");
  if (f==NULL) {
    return -2;
  }
  char *s = new char[250000];
  while (!feof(f)) {
    if (feof(f)) continue;
    fgets(s, 250000, f);
    char *tr = strstr(s, "</tr");
    while (tr!=NULL) {
      tr[0] = 0;
      s = tr+1;
      fprintf(stdout, "%s", s);
      tr = strstr(s, "</tr");
    }
    s[0] = 0;
  }
  return 1;
}
