#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
  if (argc < 1) {
    return -1;
  }
  FILE *f = fopen(argv[1], "rt");
  if (!f) {
    return -2;
  }
  char *s = new char[32767];
  fgets(s, 32767, f);
  char *stt = strstr(s, "/equipa.php?id=");
  while (stt!=NULL) {
    char *stn = strstr(stt+1, "/equipa.php?id=");
    char *ste = strtok(stt, "\">");
    printf("%s\n", stt);
    stt = stn;
  }
  fclose(f);
  return 0;
}


