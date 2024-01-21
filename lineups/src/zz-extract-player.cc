#include<stdio.h>
#include<stdlib.h>
#include<string.h>

void replace(char *s, char x, char y) {
  int len = strlen(s);
  for (int i=0; i<len; i++) {
    if (s[i]==x) s[i]=y;
  }
}

int main(int argc, char **argv) {
  if (argc<2) {
    fprintf(stderr, "ERROR: No input file specified.\n");
    return -1;
  }
  FILE *f = fopen(argv[1], "rt");
  if (!f) {
    fprintf(stderr, "ERROR: file %s not found.\n", argv[1]);
    return -2;
  }
  char *s1 = new char[30000];
  fgets(s1, 30000, f);
  char *stitle = strstr(s1, "factsheet");
  if (!stitle) {
    fprintf(stderr, "ERROR: factsheet not found.\n");
    return -3;
  }
  char *sdisp = strstr(stitle, "name");
  char *simp = strstr(sdisp, "span class=\"number");
  if (simp!=NULL) {
    sdisp = strstr(simp, "</span>");
  }
  char *tdisp = strstr(sdisp+4, ">")+1;
  strtok(tdisp, "<");

  char *s2 = new char[30000];
  fgets(s2, 30000, f);
  char *start = strstr(s2, "DADOS");
  if (!start) {
    fprintf(stderr, "ERROR: info element not found.\n");
    return -3;
  }

  char *sname = strstr(start, "<span>Nome");
  if (!sname) {
    fprintf(stderr, "ERROR: Name not found.\n");
    return -3;
  }
  char *sdob = strstr(start, "<span>Nascimento");
  if (!sdob) {
    fprintf(stderr, "ERROR: DOB not found.\n");
    return -3;
  }
  char *scob = strstr(start, "de Nascimento</span>");
  if (!scob) {
    fprintf(stderr, "ERROR: COB not found.\n");
    return -3;
  }
  char *snat = strstr(start, "<span>Nacionalidade");
  if (!snat) {
    fprintf(stderr, "ERROR: Nationality not found.\n");
    return -3;
  }
  char *spob = strstr(start, "<span>Naturalidade");
  if (!spob) {
//    fprintf(stderr, "POB not found.\n");
    spob = new char[30]; sprintf(spob, "%26s<", " ");
  }
  char *sindex = new char[12];
  strncpy(sindex, argv[1], 12);
  strtok(sindex, "-");
  char *tindex = strtok(NULL, ".");

  char *tname = sname+17;
  strtok(tname, "<");
  char *tdob = sdob+23;
  strtok(tdob, "<");
  char *tcob = strstr(scob, "title");
  tcob = tcob+7;
  strtok(tcob, "\"");
  char *tnat = strstr(snat, "title");
  tnat = tnat+7;
  strtok(tnat, "\"");
  char *tpob = spob+25;
  strtok(tpob, "<");
  replace(tpob, ',', ' ');
  fclose(f);

  printf("%s,%s,%s,%s,%s,%s,%s\n",tindex,tdisp,tname,tdob,tnat,tpob,tcob);
  return 0;
}
