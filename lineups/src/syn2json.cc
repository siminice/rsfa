#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_N           64
#define MAX_RR           4
#define NUM_ORD         10

int  id[MAX_N], win[MAX_N], drw[MAX_N], los[MAX_N], gsc[MAX_N], gre[MAX_N], pen[MAX_N], pdt[MAX_N];
int pts[MAX_N], rank[MAX_N];
int rnd[MAX_RR][MAX_N][MAX_N], res[MAX_RR][MAX_N][MAX_N];
int  n, r, lastr, lastrnd, tbr, pr1, pr2, rel1, rel2, ppv, numr, year, winter;
int level, group;
char desc[MAX_N][32], *league;

int LoadFile(char *filename) {
  FILE *f;
  int i, j, k, x, y, r, z;
  f = fopen(filename, "rt");
  if (NULL == f) return 0;
  char *sy = new char[128];
  strcpy(sy, filename);
  char *tl = strtok(sy, ".");
  level = ((char)(tl[0]))-96;
  char *tg = tl+1;
  if (tg[0]==0) group = 0; else group = atoi(tg);
  char *ty = strtok(NULL, ".");
  year = atoi(ty);

  winter = 0;
  char s[500], *tok[12];
  fscanf(f, "%d %d %d %d %d %d %d\n", &n, &ppv, &tbr, &pr1, &pr2, &rel1, &rel2);
  numr = tbr/NUM_ORD + 1;
  for (i=0; i<n; i++) {
    fgets(s, 200, f);
    tok[0] = strtok(s, " ");
    for (int k=1; k<12; k++) tok[k] = strtok(NULL, " \n");
    id[i] = atoi(tok[0]);
    win[i] = atoi(tok[2]);
    drw[i] = atoi(tok[3]);
    los[i] = atoi(tok[4]);
    gsc[i] = atoi(tok[5]);
    gre[i] = atoi(tok[6]);
    pts[i] = atoi(tok[7]);
    if (tok[8]) pen[i] = atoi(tok[8]); else pen[i] = 0;
    if (tok[9]) pdt[i] = atoi(tok[9]); else pdt[i] = 0;
    if (tok[10]) strcpy(desc[i], tok[10]); else strcpy(desc[i],"");
  }

  for (k=0; k<MAX_RR; k++)
    for (i=0; i<n; i++)
      for (j=0; j<n; j++) { rnd[k][i][j] = res[k][i][j] = -1; }

  for (k=0; k<numr; k++) {
    for (i=0; i<n; i++) {
      for (j=0; j<n; j++) {
        fscanf(f, "%d %d", &r, &z);
        rnd[k][i][j] = r;
        res[k][i][j] = z;
      }
      fscanf(f, "\n");
    }
  }
  fclose(f);
  return 1;
}

void extractJson() {
  printf("{");
  printf("\"league\":\"%s\"", league);
  printf(",\"level\":%d", level);
  if (group>0) printf(",\"group\":%d", group);
  printf(",\"season\":\"%d/%d\"", year-1, year);
  printf(",\"n\":%d", n);
  printf(",\"ppv\":%d", ppv);
  printf(",\"tbr\":%d", tbr%NUM_ORD);
  printf(",\"rr\":%d", numr);
  printf(",\"promo\":%d", pr1);
  printf(",\"promo_po\":%d", pr2);
  printf(",\"releg\":%d", rel1);
  printf(",\"releg_po\":%d", rel2);
  printf(",\n\"teams\":[");
  for (int i=0; i<n; i++) {
    printf("\n{\"id\":%d,\"pld\":%d,\"win\":%d,\"drw\":%d,\"los\":%d,\"gsc\":%d,\"gre\":%d,\"pts\":%d",
      id[i], win[i]+drw[i]+los[i], win[i], drw[i], los[i], gsc[i], gre[i], pts[i]);
    if (strlen(desc[i])>0) printf(",\"dec\":\"%s\"", desc[i]);
    printf("}");
    if (i<n-1) printf(",");
  }
  printf("],\n\"results\":[");
  for (int r=0; r<numr; r++) {
    for (int i=0; i<n; i++) {
      for (int j=0; j<n; j++) {
        if (res[r][i][j]>=0) {
          int dd = rnd[r][i][j]%1000;
          int dm = dd/50;
          int dz = dd%50;
          int dy = year;
          if (winter==0 && dm>6) dy--;
          int dr = rnd[r][i][j]/1000;
          printf("\n{\"date\":\"%d-%02d-%02d\",\"round\":%d,\"home\":%d,\"away\":%d,\"res\":%d},",
            dy, dm, dz, dr, id[i], id[j], res[r][i][j]);
        }
      }
    }
  }
  printf("],\n\"adjustments\":[");
  printf("]");
  printf("}\n");
}

int main(int argc, char **argv) {
  league = new char[64];
  strcpy(league, "rom");
  if (LoadFile(argv[1])) {
    extractJson();
  }
  return 0;
}
