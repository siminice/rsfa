#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* findLast(char *s) {
  int l = strlen(s) - 1;
  char *tb = NULL;
  char *tkf = strstr(s, "<span class=\"small_faded\">");
  if (tkf!=NULL) {
    tb = strstr(tkf, ">") + 1;
    tb[1] = 0;
    tkf[0] = 0;
    l = strlen(s) - 1;
  }
  while (l>0 && !(s[l-1]=='"' && s[l]=='>')) l--;
  char *tks = s+l+1;
  char *tkb = strstr(tks, "<b>");
  if (tkb!=NULL) tks = tkb+3;
  char *tke = strstr(tks+1, "<");
  if (tke) tke[0] = 0;
  if (tb) {
    strcat(tks, "-");
    strcat(tks, tb);
  }
  return tks;
}

void getTds(char *s) {
  char *nextcol = strstr(s, "<td");
  char *endcol;
  char *curr;
  do {
    curr = nextcol+1;
    endcol = strstr(curr, "</td");
    nextcol = strstr(curr, "<td ");
    endcol[0] = 0;
    char *tok = findLast(curr);
    fprintf(stdout, "%s    ", tok);
  } while (nextcol!=NULL);
  fprintf(stdout, "\n");
}

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "No input file specified.");
    return -1;
  }
  char *filename = argv[1];
  FILE *f = fopen(filename, "rt");
  if (!f) {
    fprintf(stderr, "File %s not found.", filename);
    return -2;
  }
  char *s = new char[100000];
  fgets(s, 100000, f);
  char *tab1 = strstr(s, "<tbody");
  char *tab2 = strstr(s, "</tbody");
  char *nextrow = strstr(tab1, "<tr");
  char *endrow;
  tab2[0] = 0;
  char *curr;
  do {
    curr = nextrow+1;
    endrow = strstr(curr, "</tr");
    nextrow = strstr(curr, "<tr ");
    endrow[0] = 0;
    getTds(curr);
  } while (nextrow!=NULL);
  fclose(f);
  return 0;
}
