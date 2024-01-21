#include <string.h>
#include <stdio.h>

char ctty[1000][4];
char country[1000][64];
int NC;

char* ltrim(char *s) {
  if (!s) return NULL;
  int i = 0;
  while (s[i]==' ' || s[i]=='\t') i++;
  return s+i;
}

int Load() {
  FILE *f = fopen("/var/www/html/country-eng.dat", "rt");
  char s[256];
  NC = 0;
  while (!feof(f)) {
    fgets(s, 256, f);
    if (strlen(s) > 3) {
      char *tkm = strtok(s, ",");
      char *tkc = strtok(NULL, ",\n");
      strncpy(ctty[NC], tkm, 16);
      strncpy(country[NC], tkc, 256);
      NC++;
    }
    s[0] = 0;
  }
  fclose(f);
}

int FindCtty(const char *s) {
  for (int i=0; i<NC; i++) {
    if (strcmp(country[i], s)==0) return i;
  }
  return -1;
}

int main(int argc, char **args) {
  Load();
  if (argc<1) { fprintf(stderr, "Error: no input file specified.\n"); return -1; }
  FILE *f = fopen(args[1], "rt");
  if (!f) { fprintf(stderr, "Error: file %s not found.\b", args[1]); return -2; }
  char s[2000], *tk[10], post;
  char name[256], fullname[256], dob[256], nat[256], pob[256], cob[256];
  strcpy(name, " ");
  strcpy(fullname, " ");
  strcpy(nat, " ");
  strcpy(dob, " ");
  strcpy(pob, " ");
  strcpy(cob, " ");
  while (!feof(f)) {
    fgets(s, 2000, f);
    if (strstr(s, "Complete name")!=NULL) {
      fgets(s, 2000, f);
      tk[0] = strtok(s, ">"); tk[1] = strtok(NULL, "<");
      char *tkfn = ltrim(tk[1]);
      strncpy(name, tkfn, 256);
      strncpy(fullname, tkfn, 256);
    }
    else if (strstr(s, ">Born:")!=NULL) {
      fgets(s, 2000, f);
      tk[0] = strtok(s, ">"); tk[1] = strtok(NULL, "<");
      strtok(tk[1], " ");
      strncpy(dob, tk[1], 256);
    }
    else if (strstr(s, "Nationality:")!=NULL) {
     fgets(s, 2000, f);
     fgets(s, 2000, f);
     char *alts = strstr(s, "alt");
      tk[0] = strtok(alts, "\""); tk[1] = strtok(NULL, "\"");
      int t = FindCtty(tk[1]);
      if (t>=0) strncpy(nat, ctty[t], 256);
    } else if (strstr(s, "Complete name")!=NULL) {
      fgets(s, 2000, f);
      strtok(s, ">");
      tk[0] = strtok(NULL, "<");
      strncpy(fullname, ltrim(tk[0]), 256);
    } else if (strstr(s, "Place of birth")!=NULL) {
     fgets(s, 2000, f);
     fgets(s, 2000, f);
     if (strchr(s, ',')!=NULL) {
       tk[0] = strtok(s, ",");
       strncpy(pob, ltrim(tk[0]), 256);
     }
     fgets(s, 2000, f);
     tk[0] = strtok(ltrim(s), "<");
     int t = FindCtty(tk[0]);
     if (t>=0) strncpy(cob, ctty[t], 256);
    }
  }
  fclose(f);
  char *pref = strtok(args[1], "/");
  pref = strtok(NULL, "-");
  char *link = strtok(NULL, ".");
  printf("%s,%s,%s,%s,%s,%s,%s\n", link, name, fullname, dob, nat, pob, cob);
  return 0;
}
