#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_N 		64
#define MAX_RR 	 	 4
#define NUM_ORD 	10

char **club;
char **mnem;
int  NC;
int  id[MAX_N], win[MAX_N], drw[MAX_N], los[MAX_N], gsc[MAX_N], gre[MAX_N], pen[MAX_N], pdt[MAX_N];
int pts[MAX_N], rank[MAX_N];
int rnd[MAX_RR][MAX_N][MAX_N], res[MAX_RR][MAX_N][MAX_N];
char filename[64], dbfile[64], *outputfile, inputfolder[128];
int n, ppv, tbr, numr, pr1, pr2, rel1, rel2, year;
char desc[MAX_N][32];
//--------------------------------------------------

int Load() {
  FILE *f;
  char s[2000], *tok[20];

  f = fopen("lineups/data/webteams.dat", "rt");
  if (!f) return 0;
  fscanf(f, "%d\n", &NC);
  club = new char*[NC];
  mnem = new char*[NC];

  for (int i=0; i<NC; i++) {
    fgets(s, 60, f);
    s[strlen(s)-1] = 0;
    char *tkm = strtok(s, ",");
    char *tkc = strtok(NULL, "\n");
    mnem[i] = new char[64];
    club[i] = new char[64];
    strncpy(mnem[i], tkm, 64);
    strncpy(club[i], tkc, 64);
  }
  fclose(f);
  return 1;
}

int LoadFile(char *filename) {
  FILE *f;
  f = fopen(filename, "rt");
  if (NULL == f) { printf("File %s not found.\n", filename); return 0; }

  int k, i, j, x, y, zi;
  char s[200], *tok[12];
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
        fscanf(f, "%d %d", &x, &y);
        rnd[k][i][j] = x;
        res[k][i][j] = y;
      }
      fscanf(f, "\n");
    }
  }
  fclose(f);
  return 1;
}

void Reset() {
  for (int i=0; i<n; i++) {
    win[i] = drw[i] = los[i] = gsc[i] = gre[i] = pts[i] = 0;
  }
  for (int h=0; h<numr; h++) {
    for (int i=0; i<n; i++) {
      for (int j=0; j<n; j++) {
        rnd[h][i][j] = -1;
        res[h][i][j] = -1;
      }
    }
  }
}

void AddResult(int a, int b, int r, int d, int x, int y) {
  printf("R%02d %02d/%02d %-16s %2d-%-2d %s\n", r, d%50, d/50, mnem[id[a]], x, y, mnem[id[b]]);
  if (rnd[0][a][b] < 0) {
    rnd[0][a][b] = 1000*r+d;
    res[0][a][b] = 100*x+y;
  } else {
    rnd[1][a][b] = 1000*r+d;
    res[1][a][b] = 100*x+y;
  }
  gsc[a] += x; gre[b] += x;
  gre[a] += y; gsc[b] += y;
 if (x>y) {
   win[a]++; los[b]++; pts[a] += ppv;
 } else if (x==y) {
   drw[a]++; drw[b]++; pts[a]++; pts[b]++;
 } else {
   los[a]++; win[b]++; pts[b] += ppv;
 }
}

void Save() {
  FILE *f;
  int k, i, j;
  outputfile = new char[strlen(filename)+5];
  strcpy(outputfile, filename);
  strcat(outputfile, ".old");
  rename(filename, outputfile);
  f = fopen(filename, "wt");
  fprintf(f, "%d %d %d %d %d %d %d\n", n, ppv, tbr, pr1, pr2, rel1, rel2);
  for (i=0; i<n; i++) {
    fprintf(f, "%4d %3d %3d %3d %3d %3d %3d %3d",
      id[i], win[i]+drw[i]+los[i], win[i], drw[i], los[i], gsc[i], gre[i], pts[i]);
    if (pen[i]!=0) fprintf(f, " %3d %3d", pen[i], pdt[i]);
    if (strlen(desc[i])>0) {
      if (pen[i]==0)  fprintf(f, " %3d %3d", pen[i], pdt[i]);
      fprintf(f, " %s", desc[i]);
    }
    fprintf(f, "\n");
  }

  for (k=0; k<numr; k++) {
    for (i=0; i<n; i++) {
      for (j=0; j<n; j++) {
        fprintf(f, "%6d%5d", rnd[k][i][j], res[k][i][j]);
      }
      fprintf(f, "\n");
    }
  }
  fclose(f);
}

int FindWebTeam(char *s) {
  if (s==NULL) return -1;
  for (int i=0; i<n; i++) {
    if (strcmp(s, club[id[i]])==0) return i;
  }
  return -1;
}

int ExtractResults(char *inputfolder, int nr) {
  int numAdded = 0;
  char s[1024];
  int home, away, md, rxy;

  for (int r=1; r<=nr; r++) {

  char *inputfile = new char[128];
  sprintf(inputfile, "%s/r%d.html", inputfolder, r);
  FILE *f = fopen(inputfile, "rt");
  if (!f) continue;

  do {
    fgets(s, 1024, f);
  } while (!(feof(f)) && strstr(s, "<table class=\"standard_tabelle")==NULL);
  if (feof(f)) return numAdded;

  int nc = 0;
  md = -1;
  do {
    fgets(s, 1024, f);
    if (strstr(s, "<tr")!=NULL) {
       nc = 0; home = away = rxy = -1;
    }
    else if (strstr(s, "<td")!=NULL) {
      nc++;
      if (nc==1) {
        char *stz = strstr(s, "nowrap\">");
        if (stz!=NULL) {
          char *st0 = strtok(stz, ">");
          char *std = strtok(NULL, "/");
          char *stm = strtok(NULL, "/");
          if (std!=NULL && stm!=NULL) {
            int d = atoi(std);
            int m = atoi(stm);
            if (d>0 && m>0) md = 50*m+d;
          }
        }
      }
    }
    else {
      char *stt = strstr(s, "/teams/");
      char *str = strstr(s, ":");
      if (stt!=NULL) {
        char *tkt = strtok(stt+7, "/");
        if (nc==3) home = FindWebTeam(tkt);
        if (nc==5) away = FindWebTeam(tkt);
      }
      else if (nc==6 && str!=NULL) {
        int k=1;
        while (str[k]>='0' && str[k]<='9') k++;
        str[k] = 0;
        k = -1;
        while (str[k]>='0' && str[k]<='9') k--;
        char *tkx = strtok(str+k+1, ":");
        char *tky = strtok(NULL, ":");
        int x = atoi(tkx);
        int y = atoi(tky);
        if (home>=0 && away>=0 && x>=0 && y>=0) {
          AddResult(home, away, r, md, x, y);
          numAdded++;
        }
      }
    }
  } while (!(feof(f)) && strstr(s, "</table")==NULL);
  fclose(f);
  }
  return numAdded;
}

//---------------------------------------------

int main(int argc, char* argv[]) {
  if (argc < 4) { printf("No input files specified.\n"); return 0; }

  if (!Load()) { return -1; }
  sprintf(filename, "%s", argv[1]);
  if (!LoadFile(filename)) { return -2; }
  sprintf(inputfolder, "%s", argv[2]);
  int nr = atoi(argv[3]);

  Reset();
  if (ExtractResults(inputfolder, nr) > 0) {
    Save();
  }
  return 1;
}
