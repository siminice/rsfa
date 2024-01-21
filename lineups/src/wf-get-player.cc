#include <string.h>
#include <stdio.h>

const char *wf_normal  = "\033[0m";
const char *wf_black   = "\033[30m";
const char *wf_red     = "\033[31m";
const char *wf_green   = "\033[32m";
const char *wf_yellow  = "\033[33m";
const char *wf_blue    = "\033[34m";
const char *wf_magenta = "\033[35m";
const char *wf_cyan    = "\033[36m";
const char *wf_white   = "\033[37m";
const char *wf_hired     = "\033[31;1m";
const char *wf_higreen   = "\033[32;1m";
const char *wf_hiyellow  = "\033[33;1m";
const char *wf_hiblue    = "\033[34;1m";
const char *wf_himagenta = "\033[35;1m";
const char *wf_hicyan    = "\033[36;1m";
const char *wf_hiwhite   = "\033[37;1m";

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
  char s[256];
  NC = 0;
  FILE *f = fopen("/var/www/html/country-eng.dat", "rt");
  if (!f) return 0;
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
  return 1;
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
  char name[256], dob[256], pob[256], nat[256], cob[256], fullname[256];;
  strcpy(name, " ");
  strcpy(nat, " ");
  strcpy(dob, " ");
  strcpy(pob, " ");
  strcpy(cob, " ");
  strcpy(fullname, " ");
  int start = 0;
  while (!feof(f)) {
    fgets(s, 2000, f);
    if (strstr(s, "itemprop")!=NULL) {
      if (strstr(s, "=\"birthDate\"")!=NULL) {
        start = 1;
        tk[0] = strtok(s, "\""); for (int i=1; i<4; i++) tk[i] = strtok(NULL, "\"");
        strncpy(dob, tk[3], 256);
      }
      else if (start>0 && strstr(s, "=\"name\"")!=NULL) {
        tk[0] = strtok(s, ">"); tk[1] = strtok(NULL, "<");
        strncpy(name, ltrim(tk[1]), 256);
      }
      else if (start>0 && strstr(s, "=\"nationality\"")!=NULL) {
        tk[0] = strtok(s, ">"); tk[1] = strtok(NULL, "<");
        int t = FindCtty(tk[1]);
        if (t>=0) strncpy(nat, ctty[t], 256);
      }
    } else if (start>0 && strstr(s, "Place of birth")!=NULL) {
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
    } else if (start>0 && strstr(s, "td colspan=\"2\" align=\"center\">")!=NULL) {
      strtok(s, ">");
      char *tkfn = strtok(NULL, "<");
      if (tkfn) strncpy(fullname, tkfn, 256);
    }
  }
  fclose(f);
  char *pref = strtok(args[1], "/");
  char *link = strtok(NULL, ".");
  printf("%s,%s,%s,%s,%s,%s,%s\n", link, name, fullname, dob, nat, pob, cob);
  return 0;
}
