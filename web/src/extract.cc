#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *month[] = {"", "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                     "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

int Month(char *m) {
  for (int i=1; i<=12; i++)
   if (strstr(m, month[i])==m) return i;
  return 0;
}

int Extract(char *filename) {
  FILE *f = fopen(filename, "rt");
  if (f==NULL) return 2;
  char s[1024];
  int m, z, r, y;
  char *tk, *tkw, *tkm, *tkd, *tky, *tks, *t1, *t2;
  char Users[64], guest[64], score[64], dow[64];
  while (!feof(f)) {
    fgets(s, 1024, f);
    if (strstr(s, "dynres")!=NULL) {
      fgets(s, 1024, f); // dow
      tk = strstr(s, "<nobr>");
      if (tk==NULL) continue;
      tkw = tk+6;
      tk = strtok(tkw, "<");
      strcpy(dow, tkw);

      fgets(s, 1024, f);
      if (strstr(s, "dynresults2_date2")==NULL) continue;
      tk = strchr(s, '>');
      if (tk==NULL) continue;
      tk++;
      tkm = strtok(tk, " ,\n");
      tkd = strtok(NULL, ",<\n");
      tky = strtok(NULL, "<\n");
      if (tkm) m = Month(tkm);
      if (tkd) z = atoi(tkd);
      if (tky) y = atoi(tky);

      fgets(s, 1024, f);
      t1 = strstr(s, "/'>");
      if (t1==NULL) continue;
      t1 += 3;
      tk = strchr(t1, '<');
      if (tk==NULL) continue;
      tk[0] = 0;
      strcpy(Users, t1);

      fgets(s, 1024, f);
      tk = strstr(s, "_score");
      if (tk==NULL) continue;
      tks = strchr(tk, '>');
      if (tks==NULL) continue;
      tks++;
      tk = strchr(tks, '<');
      if (tk==NULL) continue;
      tk[0] = 0;
      strcpy(score, tks);      

      fgets(s, 1024, f);
      t2 = strstr(s, "/'>");
      if (t2==NULL) continue;
      t2 += 3;
      tk = strchr(t2, '<');
      if (tk==NULL) continue;
      tk[0] = 0;
      strcpy(guest, t2);

      printf("%-10s\t %s %2d, %02d\t %s\t %s\t %s\t i\n", dow, month[m], z, y, Users, score, guest);
    }
  }
}

int main(int argc, char **argv) {
  if (argc < 2) return 1;
  Extract(argv[1]);
  return 0;
}
