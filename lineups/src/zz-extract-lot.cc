#include<stdio.h>
#include<stdlib.h>
#include<string.h>

void extractElements(char *pos, char *s) {
  char *scty = strstr(s, "title=");
  char *spl = strstr(scty, "player.php?");
  if (strcmp(pos, "C")==0) {
    spl = strstr(scty, "coach.php?");
  }
  char *snm = strstr(spl, ">");
  if (!snm) return;
  char *sag = strstr(scty, "<span>");
  strtok(scty+7, "\" ");
  strtok(spl, "\" ");
  strtok(snm, "<");
  char *cty = scty+7;
  char *pid = spl+14;
  if (strcmp(pos, "C")==0) {
    pid = spl+13;
  }
  char *name = snm+1;
  char *age = strdup(" ");
  if (sag) {
    strtok(sag+6, "<");
    age = sag+6;
  } else {
    //
  }
  printf("%s, ,%s,%s,%s,%s\n", pos,pid,name,cty,age);
}

void clipStaff(char *pos, char *s) {
  char *sst = strstr(s, "<div class=\"staff\">");
  while (sst!=NULL) {
    char *est = strstr(sst+1, "<div class=\"staff\">");
    if (est) {
      est[-1] = 0;
    }
    extractElements(pos, sst);
    sst = est;
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
  char *s = new char[50000];
  fgets(s, 50000, f);
  char *sts = strstr(s, "team_squad");
  if (!sts) {
    fprintf(stderr, "ERROR: team squad element not found.\n");
    return -3;
  }
  char *sgk = strstr(sts, ">Guarda Redes<");
  if (!sgk) {
    fprintf(stderr, "ERROR: goalkeepers not found.\n");
    return -3;
  }
  char *sdef = strstr(sgk, ">Defesa<");
  char *egk;
  if (!sdef) {
    fprintf(stderr, "ERROR: defenders not found.\n");
  } else {
    egk = sdef-1;
  }
  char *smid = strstr(sts, "dio<");
  if (!smid) {
    fprintf(stderr, "ERROR: midfielders not found.\n");
    return -3;
  }
  if (!sdef) egk = smid-1;
  egk[0] = 0;
  char *edef = smid-1;
  edef[0] = 0;
  char *sfwd = strstr(smid, ">Avan");
  if (!sfwd) {
    fprintf(stderr, "ERROR: forwards not found.\n");
    return -3;
  }
  char *emid = sfwd-1;
  emid[0] = 0;
  char *sjog = strstr(smid, ">Jogador de Campo<");
  if (sjog) {
    char *sfwe = sjog-1;
    sfwe[0] = 0;
  }

  char *s2 = new char[50000];
  fgets(s2, 50000, f);
  char *sch = strstr(s2, "Treinador");
  if (!sch) {
    fprintf(stderr, "Coaches not found.\n");
  } else {
    char *ech = strstr(sch, "Outros T");
    if (ech) ech[0] = 0;
    ech = strstr(sch, "irigentes");
    if (ech) ech[0] = 0;
  }
  fclose(f);

  if (sgk)  clipStaff("P", sgk);
  if (sdef) clipStaff("F", sdef);
  if (smid) clipStaff("M", smid);
  if (sfwd) clipStaff("A", sfwd);
  if (sjog) clipStaff("F", sjog);
  if (sch)  clipStaff("C", sch);
  return 0;
}
