#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char **club;
char **mnem;
int  id[64], win[64], drw[64], los[64], gsc[64], gre[64];
int  pts[64], pen[64], pdt[64], rank[64], round[64][64], res[64][64];
int  round2[64][64], res2[64][64];
char inputfile[64], outputfile[64];
int NC, n, ppv, tbr, p1, p2, r1, r2;
char desc[64][32];
int reset;
int df, fd, syntax;

const char *LIT_JORNADA = "JORNADA ";
const char *LIT_JORNADA_NBSP = "JORNADA&nbsp;";
const char *LIT_AMPERSAND = "&";
const char *LIT_WIDTH = "size=\"4\">";
const char *LIT_FONT = "</font>";
const char *EOL = "\n";

void trim(char *s) {
  if (s) {
  if (strlen(s) > 1) {
    int j = strlen(s)-1;
    while ((s[j]==32 || s[j]=='\t' || s[j]=='\n') && j>0) j--;
    int i = 0; while ((s[i]==32 || s[i]=='\t' || s[i]=='\n') && i<strlen(s)-1) i++;
    if (i>0 || j<strlen(s)-1) {
      s[j+1] = 0;
      memmove(s, s+i, j-i+1);
      s[j-i+1]=0;
    }
  }
  }
}

void Load(const char *filename) {
  FILE *f;
  char s[2000], *tok[20], *ystr, *name, *nick;
  f = fopen("rsssfteams.dat", "rt");
  fscanf(f, "%d\n", &NC);
  club = new char*[NC];
  mnem = new char*[NC];
  for (int i=0; i<NC; i++) {
    fgets(s, 2000, f);
    s[strlen(s)-1] = 0;
    tok[0] = strtok(s, ",");
    tok[1] = strtok(NULL, "\n,");
    int l1 = strlen(tok[0]);
    int l2 = strlen(tok[1]);
    mnem[i] = new char[l1+1];
    club[i] = new char[l2+1];
    strcpy(mnem[i], tok[0]);
    strcpy(club[i], tok[1]);
  }
  fclose(f);
}

void Dump(char *outfile) {
  FILE *f;
  int i, j;
  char tempf[100];
  sprintf(tempf, "%s.old", outfile);
  rename(outfile, tempf);
  f = fopen(outfile, "wt");
  fprintf(f, "%d %d %d %d %d %d %d\n", n, ppv, tbr, p1, p2, r1, r2);
  for (i=0; i<n; i++) {
    fprintf(f, "%4d %4d %4d %4d %4d %4d %4d %4d ",
      id[i], win[i]+drw[i]+los[i], win[i], drw[i], los[i], gsc[i], gre[i], ppv*win[i]+drw[i]);
    if (pen[i]!=0) fprintf(f, "%4d %4d", pen[i], pdt[i]);
    if (strlen(desc[i])>0) {
      if (pen[i]==0)  fprintf(f, "%4d%4d", pen[i], pdt[i]);
      fprintf(f, " %s", desc[i]);
    }
    fprintf(f, "\n");
  }
  for (i=0; i<n; i++) {
    for (j=0; j<n; j++) 
      fprintf(f, " %5d %5d", round[i][j], res[i][j]);
    fprintf(f, "\n");
  }
  if (tbr>=10) {
    for (i=0; i<n; i++) {
      for (j=0; j<n; j++) 
        fprintf(f, " %5d %5d", round2[i][j], res2[i][j]);
      fprintf(f, "\n");
    }
  }
  fclose(f);
}

int Find(char* s) {
  if (!s) return -1;
  int found = 0;
  int i = 0;
  int multi = 0;
  int j;

  while (i < n && !found) {
//    if (strstr(mnem[id[i]], s)==mnem[id[i]]) return i;
    if (strcmp(mnem[id[i]], s)==0) return i;
    else i++;
  }
  return -1;
}

int sup(int i, int j) {
  int p1 = ppv*win[i]+drw[i];
  int p2 = ppv*win[j]+drw[j];
  if (p1 > p2) return 1;
  if (p1 < p2) return 0;
  if (gsc[i] - gre[i] > gsc[j] - gre[j]) return 1;
  if (gsc[i] - gre[i] < gsc[j] - gre[j]) return 0;
  if (gsc[i] > gsc[j]) return 1;
  if (gsc[i] < gsc[j]) return 0;   
  if (i > j) return 0;
  return 1;
}
  
void Ranking() {
  // BubbleSort
  int i, j;
  for (i=0; i<n; i++) rank[i] = i;
  int sorted;
  do {
    sorted = 1;
    for (i=0; i<n-1; i++)
      if (sup(rank[i+1], rank[i])) {
        sorted = 0;
        int aux = rank[i];
        rank[i] = rank[i+1];
        rank[i+1] = aux;
      }   
  } while (!sorted);
}
  
void Listing() {
  printf("\nStandings:\n");
  for (int i=0; i<n; i++) {
    int x = rank[i];
    int pts = ppv*win[x] + drw[x];
    printf("%2d.%-30s%2d%3d%3d%3d%4d-%2d%3d\n", i+1,
     club[id[x]], win[x]+drw[x]+los[x],
     win[x], drw[x], los[x], gsc[x], gre[x], pts);

     if (i==p1-1)
        printf("------------------------------------------------------\n");
     if (i==p1+p2-1 && p2>0)
        printf(" - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
     if (i==n-(r1+r2)-1 && r2>0)
        printf(" - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
     if (i==n-r1-1 && r1>0)
        printf("------------------------------------------------------\n");

  }
}

int AddResult(int a, int b, int r, int x, int y) {
  if (res[a][b] >= 0)  {
          printf("%s-%s already stored twice h&a.\n", club[id[a]], club[id[b]]);
          return 1;
  }
  if (round[a][b]<0) {
    round[a][b] = r;
    res[a][b] = 100*x+y;
    gsc[a] += x; gre[a] += y;
    gsc[b] += y; gre[b] += x;
    if (x>y) { win[a]++; los[b]++; }
      else if (x==y) { drw[a]++; drw[b]++; }
       else { los[a]++; win[b]++; }
    printf("R%2d %02d-%02d %-15s%d-%d %-15s\n", r/1000, r%50, (r%1000)/50,
            mnem[id[a]], x, y, mnem[id[b]]);
  }
  return 1;
}

int LoadFile(char *filename) {
  FILE *f; 
  int i, j, k, x, y, r, z;
//  printf("Reading from file %s...\n", filename);
  f = fopen(filename, "rt");
  if (NULL == f) { printf("File %s not found.\n", filename); return 0; }
  // Loading file
  fscanf(f, "%d %d %d %d %d %d %d\n", &n, &ppv, &tbr, &p1, &p2, &r1, &r2);
  char s[2000], *tok[20];
  for (i=0; i<n; i++) {
    fgets(s, 2000, f);
    tok[0] = strtok(s, " \t\n");
    for (int j=1; j<20; j++) tok[j] = strtok(NULL, " \t\n");
    id[i] = atoi(tok[0]);
    if (reset) {
      win[i] = drw[i] = los[i] = gsc[i] = gre[i] = pts[i] = pen[i] = pdt[i] = 0;
    }
    else {
      win[i] = atoi(tok[2]);
      drw[i] = atoi(tok[3]);
      los[i] = atoi(tok[4]);
      gsc[i] = atoi(tok[5]);
      gre[i] = atoi(tok[6]);
      pts[i] = atoi(tok[7]);
    }
    if (tok[8]) pen[i] = atoi(tok[8]); else pen[i] = 0;
    if (tok[9]) pdt[i] = atoi(tok[9]); else pdt[i] = 0;
    if (tok[10]) strcpy(desc[i], tok[10]); else strcpy(desc[i],"");
  }
  if (!reset) {
    for (i=0; i<n; i++) {
      for (j=0; j<n; j++) {
        fscanf(f, "%d %d", &r, &z);
        round[i][j]  = r;
        res[i][j]    = z;
      }
      fscanf(f, "\n");
    }
  }
  fclose(f);
  return 1;
}

void Reset() {
    for (int j=0; j<n; j++) {
      for (int k=0; k<n; k++) {
        round[j][k] = -1;
        res[j][k] = -1;
      }
      win[j] = drw[j] = los[j] = gsc[j] = gre[j] = pts[j] = 0;
    }
}

int ExtractInt(char *s, const char *ts, const char *te) {
  char t[1024];
  strncpy(t, s, 1000);
  char *s1 = strstr(t, ts);
  if (s1!=NULL) s1 += strlen(ts);
  char *s2 = strstr(t, te);
  if (s2 == NULL || s1 == NULL) return -1;
  s2[0] = 0;
  int v = atoi(s1);
  return v;
}

int ExtractDate(char *s) {
  char *t1 = strtok(s, "(");
  char *t2 = strtok(NULL, "-");
  char *t3 = strtok(NULL, "-");
  if (t3 == NULL) return -1;
  int z = atoi(t2);
  int m = atoi(t3);
  return 50*m + z;
}

int FindTeam(char *s, const char *ts, const char *te) {
  char *s1 = strstr(s, ts);
  if (s1 != NULL) s1 += strlen(ts);
  char *s2 = strstr(s, te);
  if (s2 == NULL || s1 == NULL) return -1;
  s2[0] = 0;
  trim(s1);
  int t = Find(s1);
  return t;
}

int ExtractScore(char *s, const char *ts, const char *te) {
  char *s1 = strstr(s, ts);
  if (s1 != NULL) s1 += strlen(ts);
  char *s2 = strstr(s, te);
  if (s2 == NULL || s1 == NULL) return -1;
  s2[0] = 0;
  trim(s1);
  char *tx = strtok(s1, "-");
  char *ty = strtok(NULL, "<");
  if (tx != NULL && ty != NULL) {
    int x = atoi(tx);
    int y = atoi(ty);
    return 100*x + y;
  }
  return -1;
}

int Extract(char *filename) {
  FILE *f;
  char *s, *ss, *tok[32], *tkm, *tkz;
  int i, j, r1, r2, m1, m2, z1, z2, d1, d2, x, y;

  f = fopen(filename, "rt");
  if (NULL == f) { printf("File %s not found.\n", filename); return 0; }

  s = new char[1024];
  ss = new char[1024];

  while (!feof(f)) {
    fgets(s, 1000, f);
    if (strstr(s, LIT_JORNADA)!=NULL) {
        r1 = ExtractInt(s, LIT_JORNADA, LIT_AMPERSAND);
        d1 = ExtractDate(s);
        if (d1<0) { fgets(s, 1000, f); d1 = ExtractDate(s); }
      fgets(s, 1000, f);
        r2 = ExtractInt(s, LIT_JORNADA_NBSP, LIT_AMPERSAND);
        if (r2<0) r2 = ExtractInt(s, LIT_JORNADA, LIT_AMPERSAND);
        d2 = ExtractDate(s);
        if (d2<0) { fgets(s, 1000, f); d2 = ExtractDate(s); }
    }
    else if (strstr(s, LIT_WIDTH)!=NULL) {
      i = FindTeam(s, LIT_WIDTH, LIT_FONT);
      if (i<0) {
        fgets(ss, 1000, f);
        trim(s); trim(ss);
        strcat(s, " "); strcat(s, ss);
        i = FindTeam(s, LIT_WIDTH, LIT_FONT);
      }
      fgets(s, 1000, f);
      x = ExtractScore(s, LIT_WIDTH,LIT_FONT);
      if (x<0) {
        fgets(ss, 1000, f);
        strcat(s, ss);
        x = ExtractScore(s, LIT_WIDTH,LIT_FONT);
      }
      fgets(s, 1000, f);
      j = FindTeam(s, LIT_WIDTH, LIT_FONT);
      if (j<0) {
        fgets(ss, 1000, f);
        trim(s); trim(ss);
        strcat(s, " "); strcat(s, ss);
        j = FindTeam(s, LIT_WIDTH, LIT_FONT);
      }
      fgets(s, 1000, f);
      if (i>=0 && j>=0 && x>=0) AddResult(i, j, 1000*r1+d1, x/100, x%100);

      fgets(s, 1000, f);
      i = FindTeam(s, LIT_WIDTH, LIT_FONT);
      if (i<0) {
        fgets(ss, 1000, f);
        trim(s); trim(ss);
        strcat(s, " "); strcat(s, ss);
        i = FindTeam(s, LIT_WIDTH, LIT_FONT);
      }
      fgets(s, 1000, f);
      x = ExtractScore(s, LIT_WIDTH,LIT_FONT);
      if (x<0) {
        fgets(ss, 1000, f);
        strcat(s, ss);
        x = ExtractScore(s, LIT_WIDTH,LIT_FONT);
      }
      fgets(s, 1000, f);
      j = FindTeam(s, LIT_WIDTH, LIT_FONT);
      if (j<0) {
        fgets(ss, 1000, f);
        trim(s); trim(ss);
        strcat(s, " "); strcat(s, ss);
        j = FindTeam(s, LIT_WIDTH, LIT_FONT);
      }
      fgets(s, 1000, f);
      if (i>=0 && j>=0 && x>=0) AddResult(i, j, 1000*r2+d2, x/100, x%100);
    } // strlen>7
  } // feof

  fclose(f);
  printf("Input file closed.\n");
  return 1;
}


int main(int argc, char* argv[]) {
  printf("Usage: arquero <a.2000>\n");
  if (argc < 2) { printf("No input file specified.\n"); return 0; }
  if (argc > 2) strcpy(inputfile, argv[argc-1]);
  else sprintf(inputfile, "html/%s.html", argv[1]);
  reset = 1;

  Load("rsssfteams.dat");
  if (!LoadFile(argv[1])) return 0;
  if (reset) Reset();
  Extract(inputfile);
  Ranking();
  Listing();
  Dump(argv[1]);
  return 1;
}
