#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int isScore(char *s) {
  if (!s) return 0;
  int l = strlen(s);
  if (l<3) return 0;
  int x = 0;
  int i = 0;
  while (i<l) {
    if (s[i]!='-' && (s[i]<'0' || s[i]>'9')) return 0;
    if (x==0) {
      if (s[i]=='-') x = 1;
    }
    else if (x==1) {
      if (s[i]<'0' || s[i]>'9') return 0;
      else x = 2;
    }
    i++;
  }
 return x>1;
}

int isTime(char *s) {
  if (s==NULL) return 0;
  if (strlen(s) != 5) return 0;
  if (s[0]<'0' || s[0]>'9') return 0;
  if (s[1]<'0' || s[1]>'9') return 0;
  if (s[2]!=':') return 0;
  if (s[3]<'0' || s[3]>'9') return 0;
  if (s[4]<'0' || s[4]>'9') return 0;
  return 1;
}

int isRound(char *s) {
  if (!s) return 0;
  int l = strlen(s);
  if (l<2) return 0;
  if (s[0]!='J') return 0;
  for (int i=1; i<strlen(s); i++) {
    if (s[i]<'0' || s[i]>'9') return 0;
  }
  return 1;
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
  int second = 0;
  if (res[a][b] >= 0)  {
    if (res2[a][b] >= 0) {
      if (res[b][a] >= 0) {
        if (res2[b][a] >= 0) {
          printf("%s-%s already stored twice h&a.\n", club[id[a]], club[id[b]]);
          return 1;
        }
        else {
          round2[b][a] = r;
          res2[b][a] = 100*y+x;
          second = 2;
        }
      }
    }
    else second = 1;
  }
  if (second == 1) {
   if (tbr >= 10) {
    round2[a][b] = r;
    res2[a][b]   = 100*x+y;
   }
   else if (res[b][a] < 0) {
     round[b][a] = r;
     res[b][a] = 100*y+x;
   }
   else {
      printf("%s-%s already stored h&a.\n", club[id[a]], club[id[b]]);
   }
  }
  else if (second == 0) {
    if (round[a][b]<0) {
      round[a][b] = r;
      res[a][b] = 100*x+y;
    }
    else {
      round[b][a] = r;
      res[b][a] = 100*y+x;
    }
  }
  gsc[a] += x; gre[a] += y;
  gsc[b] += y; gre[b] += x;
  if (x>y) { win[a]++; los[b]++; }
    else if (x==y) { drw[a]++; drw[b]++; }
     else { los[a]++; win[b]++; }
  printf("R%2d %02d-%02d %-15s%d-%d %-15s\n", r/1000, r%50, (r%1000)/50,
          mnem[id[a]], x, y, mnem[id[b]]);
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
        round2[i][j] = -1;
        res2[i][j]   = -1;
      }
      fscanf(f, "\n");
    }
  }
  fclose(f);
  return 1;
}

void trim(char *s) {
  if (s) {
  if (strlen(s) > 1) {
    int j = strlen(s)-1;
    while ((s[j]==32 || s[j]=='\t') && j>0) j--;
    int i = 0; while (s[i]==32 && i<strlen(s)-1) i++;
    if (i>0 || j<strlen(s)-1) {
      s[j+1] = 0;
      memmove(s, s+i, j-i+1);
      s[j-i+1]=0;
    }
  }
  }
}

int Extract(char *filename) {
  FILE *f;
  char *s, *tok[32], *tkm, *tkz;
  int i, j, r, m, z, x, y;

  if (reset) {
    for (int j=0; j<n; j++) {
      for (int k=0; k<n; k++) {
        round[j][k] = -1;
        res[j][k] = -1;
        round2[j][k] = -1;
        res2[j][k] = -1;
      }
      win[j] = drw[j] = los[j] = gsc[j] = gre[j] = pts[j] = 0;
    }
  }

  f = fopen(filename, "rt");
  if (NULL == f) { printf("File %s not found.\n", filename); return 0; }

  s = new char[300];
  r = 0;
  m = fd/50;
  z = fd%50;
  char *home = new char[256];
  char *away = new char[256];

  while (!feof(f)) {
    fgets(s, 300, f);
    // syntax: h2h YYYY-MM-DD T1 x-y T2 Jnn FG
    if (strlen(s)>7) {
      tok[0] = strtok(s, " \t\n");
      for (int j=1; j<20; j++) tok[j] = strtok(NULL, " \t\n");
      int ntk = 0;
      while (ntk < 20 && tok[ntk]!=NULL) ntk++;
      int is = 3;
      while (is < ntk && !isScore(tok[is])) is++;
      int ir = is + 2;
      while (ir < ntk && !isRound(tok[ir])) ir++;
      if (strcmp(tok[1], "-")==0) {
        m = 1;
        z = 1;
      }  else {
        char *tko = strtok(tok[1], "-");
        char *tkm = strtok(NULL, "-");
        char *tkd = strtok(NULL, "-");
        m = atoi(tkm);
        z = atoi(tkd);
      }
      int ih = 2; if (isTime(tok[ih])) ih++;
      strncpy(home, tok[ih], 64);
      ih++; while (ih < is) { strcat(home, " "); strcat(home, tok[ih]); ih++; }
      char *tkx = strtok(tok[is], "-");
      char *tky = strtok(NULL, "-");
      x = atoi(tkx);
      y = atoi(tky);
      strncpy(away, tok[is+1], 64);
      int ia = is+2; while (ia < ir) { strcat(away, " "); strcat(away, tok[ia]); ia++; }
      if (tok[ir]) r = atoi(tok[ir]+1); else r = -1;
      int th = Find(home);
      int ta = Find(away);
      if (th>=0 && ta>=0) {
        AddResult(th, ta, r>0 ? 1000*r+50*m+z : -1, x, y);
      }
    }
    s[0] = 0;
  }

  fclose(f);
  printf("Input file closed.\n");
  return 1;
}

int main(int argc, char* argv[]) {
  printf("Usage: zerozero <a.2000>\nTeams in 'rsssfteams.dat'\n");
  if (argc < 2) { printf("No input file specified.\n"); return 0; }
  if (argc > 2) strcpy(inputfile, argv[argc-1]);
  else sprintf(inputfile, "html/%s", argv[1]);
  reset = 1;
  df = 0;
  fd = 51;
  syntax = 0;
  for (int j=2; j<argc-1; j++) {
    if (strstr(argv[j], "-nor")!=NULL) reset = 0;
    else if (strstr(argv[j], "-s")==argv[j]) {
      if (j+1<argc) syntax = atoi(argv[j+1]);
    }
  }

  Load("rsssfteams.dat");
  if (!LoadFile(argv[1])) return 0;
  Extract(inputfile);
  Ranking();
  Listing();
  Dump(argv[1]);
  return 1;
}
