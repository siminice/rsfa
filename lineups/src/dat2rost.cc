#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db.h"

#define MAX_ROSTER 100
#define MNEM_LEN     7
#define FULL_LEN    64
#define MAX_ROUNDS  40
#define MAX_TEAMS	24
#define NUM_ORD		10
#define MAX_RR		 3
#define MAX_COL		50

#define DB_ROWS		400
#define DB_COLS		60
#define DB_CELL		20
#define EV_COLS	 	30

#define EV_GOAL          0
#define EV_OWNGOAL       1
#define EV_PKGOAL        2
#define EV_PKMISS        3
#define EV_YELLOW        4
#define EV_RED           5
#define EV_YELLOWRED     6
#define EV_INJURY	 7

#define DOB_DD_MM_YYYY  0
#define DOB_YYYYMMDD    1

const char *normal  = "\033[0m";
const char *red     = "\033[31m";
const char *green   = "\033[32m";
const char *yellow  = "\033[33m";
const char *blue    = "\033[34m";
const char *magenta = "\033[35m";
const char *cyan    = "\033[36m";
const char *white   = "\033[37m";
const char *hired     = "\033[31;1m";
const char *higreen   = "\033[32;1m";
const char *hiyellow  = "\033[33;1m";
const char *hiblue    = "\033[34;1m";
const char *himagenta = "\033[35;1m";
const char *hicyan    = "\033[36;1m";
const char *hiwhite   = "\033[37;1m";

int verbosity, lnerr;
int tm, NT, NR, NG, CS, FC;
int tmid, oppid;
char ofilename[FULL_LEN];
char cfilename[FULL_LEN];
char rfilename[FULL_LEN];
char sfilename[FULL_LEN];
int borna[256];

char **club;
char **mnem;
int  NC;
int year;
// --- catalog
int ncat;
char cat_mnem[MAX_ROSTER][MNEM_LEN];
int cat_pid[MAX_ROSTER];
int cat_mec[MAX_ROSTER];
int cat_tit[MAX_ROSTER];
int cat_rez[MAX_ROSTER];
int cat_gol[MAX_ROSTER];
int cat_min[MAX_ROSTER];
int cat_int[MAX_ROSTER];
int cat_ban[MAX_ROSTER];
int cat_gre[MAX_ROSTER];
// --- roster
int nros;
int ros_mec[MAX_ROSTER];
int ros_tit[MAX_ROSTER];
int ros_rez[MAX_ROSTER];
int ros_gol[MAX_ROSTER];
int ros_min[MAX_ROSTER];
int ros_int[MAX_ROSTER];
int ros_ban[MAX_ROSTER];
int ros_gre[MAX_ROSTER];
// --- schedule
char oppstr[MAX_ROUNDS][FULL_LEN];
int opp[MAX_ROUNDS];
int ha[MAX_ROUNDS];
int summ[MAX_ROUNDS];
int form[MAX_ROUNDS][23];
int oppfreq[MAX_TEAMS];
int hafreq[MAX_TEAMS];
char score[MAX_ROUNDS][7];
int nsub[MAX_ROUNDS], mid[MAX_ROUNDS], home[MAX_ROUNDS], post[MAX_ROUNDS];

char td[MAX_ROSTER][MAX_COL][20];
char db[DB_ROWS][DB_COLS][DB_CELL];
char edb[DB_ROWS][DB_COLS][DB_CELL];
const char *evsymb = "'`\"/   ";

struct alias {
  int   year;
  char *name;
  char *nick;
  alias(int, char*, char*);
  ~alias();
};

struct node {
  alias *data;
  node  *next;
  node(alias*, node*);
  ~node();
};
  
struct Aliases {
  node *list;
  Aliases();   
  ~Aliases();
  void Append(alias *a);
  char* GetName(int y);
  char* GetNick(int y);
};

//-------------------------------------

alias::alias(int y, char *s, char *n) {
  year = y;  
  name = (char*) malloc(strlen(s)+1);
  strcpy(name, s);
  if (n != NULL) {
    nick = (char*) malloc(strlen(n)+1);
    strcpy(nick, n);
  }
  else nick = NULL;
};

alias::~alias() {
  if (name) delete name;
  if (nick) delete nick;
};
  
node::node(alias *a, node *n) {
  data = a;
  next = n;
};
  
node::~node() {
  if (next) delete next;
  delete data;
};
  

Aliases::Aliases() {
  list = NULL;
};
  
Aliases::~Aliases() {
 delete list;
} 

void Aliases::Append(alias *a) {
  node *n = (node*) malloc(sizeof(node));
  n->data = a;
  n->next = list;
  list = n;
};
  
char* Aliases::GetName(int y) {
  if (!list) return NULL;
  node *n = list;
  char *s = n->data->name;
  int x = n->data->year;
  while (y < x && n->next != NULL) {
    n = n->next;
    s = n->data->name;
    x = n->data->year;
  }
  return s;
} 

char* Aliases::GetNick(int y) {
  if (!list) return NULL;
  node *n = list;
  char *s = n->data->nick;
  int x = n->data->year;
  while (y < x && n->next != NULL) {
    n = n->next;
    s = n->data->nick;
    x = n->data->year;
  }
  return s;
} 

Aliases **L;

//--------------------------------------

int Load() {
  FILE *f;
  char s[2000], *tok[20], *ystr, *name, *nick;
  f = fopen("teams.dat", "rt");
  if (f==NULL) return 0;
  fscanf(f, "%d\n", &NC);
  club = new char*[NC];
  mnem = new char*[NC];
  L = new Aliases*[NC];
  for (int i=0; i<NC; i++) L[i] = new Aliases;
  for (int i=0; i<NC; i++) {
    fgets(s, 60, f);
    s[strlen(s)-1] = 0;
    mnem[i] = new char[16];
    club[i] = new char[32];
    memmove(mnem[i], s, 15); mnem[i][15] = 0;
    for (int j=0; j<30; j++) club[i][j] = 32;
    memmove(club[i], s+15, 30);
  }
  fclose(f);
  f = fopen("alias.dat", "rt");
  if (!f) return 0;
  for (int i=0; i<NC; i++) {
    if (feof(f)) continue;
    fgets(s, 2000, f);
    if (!s) continue; 
    if (strlen(s) < 3) continue;
    s[strlen(s)-1] = 0;
    tok[0] = strtok(s, "*");
    for (int j=1; j<20; j++)
      tok[j] = strtok(NULL, "*");
    int k=0;
    while(tok[k]) {
      ystr = strtok(tok[k], " ");
      name = strtok(NULL, "~");
      nick = strtok(NULL, "@");
      int y = atoi(ystr);
      alias *a = new alias(y, name, nick); 
      L[i]->Append(a);
//      fprintf(stderr, "Alias (%d,%s~%s) for %d.%s...\n", y, name, (nick!=NULL?nick:""), i, club[i]);
      k++;
    }
    s[0] = 0;
  }
  fclose(f);
  return 1;
}

   
char *NameOf(Aliases **L, int t, int y) {
  char *s = L[t]->GetName(y);
  if (!s) return club[t];   
  return s;
}

char *NickOf(Aliases **L, int t, int y) {
  char *s = L[t]->GetNick(y);
  if (!s) return mnem[t];   
  return s;
}

/* *************************************** */

#define MAX_NAMES	20000

int NP;
char **pmnem;
char **pname;
char **ppren;
char **pdob;
char **pcty;
char **ppob;
char **pjud;
//int phist[MAX_NAMES][30];

FILE *of;

void trim(char *s) {
  if (s==NULL) return;
  int l = strlen(s);
  if (l<=1) return;
  int j = l-1;
  while ((s[j]==' ' || s[j]=='\t') && j>0) j--;
  int i = 0; 
  while (s[i]==' ' && i<j) i++;
  if (i>0 || j<l-1) {
    s[j+1] = 0;
    memmove(s, s+i, j-i+1);
    s[j-i+1]=0;
  }
}

int isNumeric(char *s) {
  if (s==NULL) return 0;
  int l = strlen(s);
  if (l==0) return 0;
  for (int i=0; i<l; i++) if (s[i]<'0' || s[i]>'9') return 0;
  return 1;
}

int isNegative(char *s) {
  if (s==NULL) return 0;
  int l = strlen(s);
  if (l==0) return 0;
  if (s[0]!='-') return 0;
  for (int i=1; i<l; i++) if (s[i]<'0' || s[i]>'9') return 0;
  return 1;
}

void Mnem3(char *mn, char *m3) {
  int j = 0; 
  int k = 0;
  int l = strlen(mn);
  if (mn[0]=='F' && mn[1]=='C') { 
   while (j<l && mn[j]!=' ') j++; j++; 
  }
  while (k<3 && j<l) {
    if (mn[j]!='.' && mn[j]!=' ') { m3[k++] = mn[j]; } j++;
  }
  m3[k] = 0;
}

void Mnem4(char *mn, char *m3) {
  int j = 0; 
  int k = 0;
  int l = strlen(mn);
  if (mn[0]=='F' && mn[1]=='C') { 
   while (j<l && mn[j]!=' ') j++; j++; 
  }
  while (k<4 && j<l) {
    if (mn[j]!='.' && mn[j]!=' ') { m3[k++] = mn[j]; } j++;
  }
  m3[k] = 0;
}

int MIN(int x, int y) {
  if (x<y) return x;
  else return y;
}

int dist(char*a, char*b) {
  int i, j, modif, ins, del;
  int m = strlen(a);
  int n = strlen(b);
  int **d = new int*[m+1];
  for (i=0; i<=m; i++) {
    d[i] = new int[n+1];
    d[i][0] = i;
  }
  for (i=0; i<=n; i++)
    d[0][i] = i;
      
  for (i=1; i<=m; i++)
    for (j=1; j<=n; j++) {
      if (a[i-1] == b[j-1]) modif = d[i-1][j-1];
        else modif = d[i-1][j-1] + 1;
      del = MIN(d[i-1][j], d[i][j-1]) + 1;
      d[i][j] = MIN(modif, del);
    }
 
  int res = d[m][n];
  // cleanup;
  for (int i=0; i<=m; i++)
    delete[] d[i];
  delete[] d;
  return res;
}

int FindMnem(char *mnem) {
  int c = ((unsigned char)mnem[0]);
  for (int i=borna[c]; i<borna[c+1]; i++)
    if (strcmp(pmnem[i], mnem)==0) return i;
  return -1;
}

int GetLastName(char *s) {
  int opt[1000];
  int nopt = 0;
  int ch;
  for (int i=0; i<NP; i++) {
    if (strcmp(s, pname[i])==0) { 
       opt[nopt++] = i; 
    }
  }
  if (nopt==0) return -1;
  if (nopt==1) return opt[0];
  do {
    for (int i=0; i<nopt; i++) { 
      fprintf(stderr, "\n... [%d]. %s %s \t[%10s] ", i+1, ppren[opt[i]], pname[opt[i]], pdob[opt[i]]);
    }
    scanf("%d", &ch);
  } while (ch<=0 || ch>nopt);
  return opt[ch-1];
}

int FindName(char *name, char *pren, char *dob, char *cty) {
  return -1;
}

void GetInitial(int px, char *pini) {
    pini[1] = '.'; pini[2] = 0;
    if (px<0 || ppren[px]==NULL || ppren[px][0]==' ') pini[0] = 0;
    else if (strlen(ppren[px])>0) pini[0] = ppren[px][0];
    else pini[0] = 0;
}

char low(char c) {
  if (c=='ª') return 'º';
  if (c=='Þ') return 'þ';
  if (c=='Î') return 'î';
  if (c>='A' && c<='Z') return c+32;
  return c;
}

int GetYear(char *dob, int fmt) {
  if (dob==NULL) return -1;
  char s[12];
  strcpy(s, dob);
  char *sd = strtok(s, "/.-");
  char *sm = strtok(NULL, "/.-");
  char *sy = strtok(NULL, "/.-");
  int xy = 0;
  if (sd!=NULL && sm==NULL && sy==NULL) xy = atoi(sd);
  if (sy!=NULL) xy = atoi(sy);
  if (xy>0 && xy<100) xy += 1900;
  return xy;
}

void LoadPlayers() {
  NP = 0;
  int c;
  for (c=0; c<257; c++) borna[c] = -1;
  FILE *f = fopen("players.dat", "rt");
  if (f==NULL) {
    fprintf(stderr, "ERROR: player database not found!\n");
    exit(0);
  }
  char s[100], *t[10];
  fgets(s, 100, f);
  sscanf(s, "%d", &NP);
  pname = new char*[MAX_NAMES];
  pmnem = new char*[MAX_NAMES];
  ppren = new char*[MAX_NAMES];
  pdob  = new char*[MAX_NAMES];
  pcty  = new char*[MAX_NAMES];
  ppob  = new char*[MAX_NAMES];
  pjud  = new char*[MAX_NAMES];
  for (int i=0; i<NP; i++) {
    fgets(s, 100, f);
//    fprintf(stderr, "... parsing %s", s);
    t[0] = strtok(s, ",\t\n");
     pmnem[i] = new char[7];
     strcpy(pmnem[i], t[0]);
    t[1] = strtok(NULL, ",\t\n");
     pname[i] = new char[strlen(t[1])+1];
     strcpy(pname[i], t[1]);
    t[2] = strtok(NULL, ",\t\n");
     ppren[i] = new char[strlen(t[2])+1];
     strcpy(ppren[i], t[2]);
    t[3] = strtok(NULL, ",\t\n");
     pdob[i] = new char[12];
     strcpy(pdob[i], t[3]);
    t[4] = strtok(NULL, ",\t\n");
     pcty[i] = new char[4];
     strcpy(pcty[i], t[4]);
    t[5] = strtok(NULL, ",\t\n");
     if (t[5]!=NULL) {
       ppob[i] = new char[strlen(t[5])+1];
       strcpy(ppob[i], t[5]);
     }
     else {
       ppob[i] = new char[2];
       strcpy(ppob[i], "");
     }
    t[6] = strtok(NULL, ",\t\n");
     if (t[6]!=NULL) {
       pjud[i] = new char[strlen(t[6])+1];
       strcpy(pjud[i], t[6]);
     }
     else {
       pjud[i] = new char[2];
       strcpy(pjud[i], "");
     }

     if (i==0 || (i>0 && pmnem[i-1][0]!=pmnem[i][0])) {
       c = ((unsigned char) pmnem[i][0]);
       borna[c] = i;
     }   

//     if (verbosity > 1) {
//       fprintf(stderr, "Loaded %s: %s %s [%s] (%s)\n", pmnem[i], ppren[i], pname[i], pdob[i], pcty[i]);
//     }
  }
  fclose(f);
  c = ((unsigned char)pmnem[NP-1][0]);
  borna[c+1] = NP;
  for (c=255; c>0; c--)
    if (borna[c]>=0 && borna[c-1]<0) borna[c-1] = borna[c];

//  fprintf(stderr, "Loaded %d names.\n", NP);
}


#define TD_MNEM          0
#define TD_PRENUME       1
#define TD_NUME          2
#define TD_DOB           3
#define TD_NAT           4  
#define TD_CLUB          5
#define TD_NR            6
#define TD_POST          7
#define TD_MECIURI       8
#define TD_TITULAR       9
#define TD_REZERVA      10  
#define TD_GOLURI       11  
#define TD_MINUTE       12
#define TD_INTEGRAL     13
#define TD_BANCA	14
#define TD_GOLPORTAR	15
#define TD_GALBENE      16
#define TD_ROSII        17
#define TD_NUM          TD_ROSII+1

char catd[MAX_ROSTER][TD_NUM][128];

void LoadCatalog(char *filename) {
  FILE *f = fopen(filename, "rt");
  if (f==NULL) {
    fprintf(stderr, "ERROR: Catalog %s not found.\n", filename);
    return;
  }
  ncat = 0;
  char s[1000], *tk[TD_NUM];
  while (!feof(f)) {
    fgets(s, 1000, f);
    if (strlen(s)<MNEM_LEN) continue;
    tk[0]=strtok(s, ",\n");
    for (int j=1; j<TD_NUM; j++) tk[j] = strtok(NULL, ",\n");
    for (int j=0; j<TD_NUM; j++) if (tk[j]!=NULL) strcpy(catd[ncat][j], tk[j]); else strcpy(catd[ncat][j], " ");
    if (tk[TD_MNEM]!=NULL) strcpy(cat_mnem[ncat], tk[TD_MNEM]);
    if (tk[TD_PRENUME]!=NULL) strcpy(cat_mnem[ncat], tk[TD_PRENUME]);
    if (tk[TD_NUME]!=NULL) strcpy(cat_mnem[ncat], tk[TD_NUME]);
    if (tk[TD_MECIURI]!=NULL) strcpy(cat_mnem[ncat], tk[TD_MECIURI]);
    if (tk[TD_TITULAR]!=NULL) strcpy(cat_mnem[ncat], tk[TD_TITULAR]);
    if (tk[TD_REZERVA]!=NULL) strcpy(cat_mnem[ncat], tk[TD_REZERVA]);
    if (tk[TD_GOLURI]!=NULL) strcpy(cat_mnem[ncat], tk[TD_GOLURI]);
    cat_pid[ncat] = FindMnem(tk[TD_MNEM]);
    if (cat_pid[ncat]<0) fprintf(stderr, "ERROR: Mnemonic %s not found.\n", tk[TD_MNEM]);
    ncat++;
    s[0] = 0;
  }
  fclose(f);
}

void SaveCatalog() {
  char filename[128];
  sprintf(filename, "%s.new", cfilename);
  FILE *f = fopen(filename, "wt");
  if (f==NULL) { fprintf(stderr, "ERROR: Could not open file %s.\n", filename); return; }
  for (int i=0; i<ncat; i++) {
    for (int j=0; j<TD_NUM; j++) {
      fprintf(f, "%s", catd[i][j]);
      if (j<TD_NUM-1) fprintf(f, ","); else fprintf(f, "\n");
    }
  }
  fclose(f);
}

int id[MAX_TEAMS], res[MAX_RR][MAX_TEAMS][MAX_TEAMS], rnd[MAX_RR][MAX_TEAMS][MAX_TEAMS];

void LoadSeason() {
  char filename[64];
  sprintf(filename, "a.%d", year);
  FILE *f = fopen(filename, "rt");
  if (f==NULL) {
    fprintf(stderr, "ERROR: file %s not found.\n", filename);
    return;
  }
  char s[500], *tok[12];
  int i, j, k;
  int n, ppv, tbr, pr1, pr2, rel1, rel2, r, z;
  fscanf(f, "%d %d %d %d %d %d %d\n", &n, &ppv, &tbr, &pr1, &pr2, &rel1, &rel2);
  int numr = tbr/NUM_ORD + 1;
  for (i=0; i<n; i++) {
    fgets(s, 200, f);
    tok[0] = strtok(s, " ");
    id[i] = atoi(tok[0]);
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
}

int FindID(int t) {
  for (int i=0; i<NT; i++) 
    if (id[i]==t) return i;
  return -1;
}

void LoadDB() {
  char filename[64], s[5000], *tk[DB_COLS];
  sprintf(filename, "lineups-%d.db", year);
  FILE *f = fopen(filename, "rt");
  if (f==NULL) { fprintf(stderr, "ERROR: database %s not found.\n", filename); return; }
  for (int i=0; i<NT*(NT-1); i++) {
    fgets(s, 5000, f);
    tk[0] = strtok(s, ",\n");
    for (int j=1; j<DB_COLS; j++) tk[j]=strtok(NULL, ",\n");
    for (int j=0; j<DB_COLS; j++) {
      if (tk[j]!=NULL) strcpy(db[i][j], tk[j]);
      else strcpy(db[i][j], " ");
    }
  }
  fclose(f);
}

void SaveDB() {
  char oldfilename[64];
  char newfilename[64];
  char filename[64];
  sprintf(filename,    "lineups-%d.db",  year);
  sprintf(oldfilename, "lineups-%d.old", year);
  sprintf(newfilename, "lineups-%d.new", year);
  FILE *f = fopen(newfilename, "wt");
  if (f==NULL) { fprintf(stderr, "ERROR: database %s not found.\n", newfilename); return; }
  for (int i=0; i<NT*(NT-1); i++) {
    for (int j=0; j<DB_COLS; j++) fprintf(f, "%s,", db[i][j]);
    fprintf(f, "\n");
  }
  fclose(f);
  rename(filename, oldfilename);
  rename(newfilename, filename);
}

void LoadEvents() {
  char filename[64], s[5000], *tk[DB_COLS];
  FILE *f;
  sprintf(filename, "events-%d.db", year);
  f = fopen(filename, "rt");
  if (f==NULL) { fprintf(stderr, "ERROR: database %s not found.\n", filename); return; }
  for (int i=0; i<NT*(NT-1); i++) {
    fgets(s, 5000, f);
    tk[0] = strtok(s, ",\n");
    for (int j=1; j<EV_COLS; j++) tk[j]=strtok(NULL, ",\n");
    for (int j=0; j<EV_COLS; j++) {
      if (tk[j]!=NULL) strcpy(edb[i][j], tk[j]);
      else strcpy(edb[i][j], " ");
    }
  }
  fclose(f);
}

void SaveEvents() {
  char oldfilename[64];
  char newfilename[64];
  char filename[64];
  sprintf(filename,    "events-%d.db",  year);
  sprintf(oldfilename, "events-%d.old", year);
  sprintf(newfilename, "events-%d.new", year);
  sprintf(newfilename, "events-%d.new", year);
  FILE *f = fopen(newfilename, "wt");
  if (f==NULL) { fprintf(stderr, "ERROR: database %s not found.\n", newfilename); return; }
  for (int i=0; i<NT*(NT-1); i++) {
    for (int j=0; j<EV_COLS; j++) fprintf(f, "%s,", edb[i][j]);
    fprintf(f, "\n");
  }
  fclose(f);
  rename(filename, oldfilename);
  rename(newfilename, filename);
}

#define MAX_DIST	1000

int MatchOpponent(char *sopp) {
  int d[MAX_TEAMS];
  int min_dist = MAX_DIST;
  int best_match = -1;
  for (int i=0; i<NT; i++) {
    d[i] = dist(sopp, NameOf(L, id[i], year));
//    d[i] = dist(sopp, NickOf(L, id[i], year));
    if (d[i] < min_dist) {
      min_dist = d[i];
      best_match = i;
    }
  }
  return best_match;
}

int BestCatalogMatch(char *s) {
  int d[MAX_ROSTER];
  int min_dist = MAX_DIST;
  int best_match = -1;
  int d1, d2;
  
  char w[128];
  strcpy(w, s);
  char *s1 = strtok(w, ".");
  char *s2 = strtok(NULL, ".");
  
  for (int i=0; i<ncat; i++) {
    d1 = dist(s1, pname[cat_pid[i]]);
    if (s2!=NULL) {
      d1 = dist(s2, pname[cat_pid[i]]);
      if (s1[0]!=ppren[cat_pid[i]][0]) d1++;
    }
    d[i] = d1;
    if (d[i] < min_dist) {
      min_dist = d[i];
      best_match = i;
    }
  }
  return best_match;  
}

int MatchCatalogName(int si, char *s1, char *s2) {
  return si;
}

int FindHA(int a, int b, char *scr, int frq) {
  char s1[10], s2[10];
  sprintf(s1, "%d-%d", res[0][a][b]/100, res[0][a][b]%100);
  sprintf(s2, "%d-%d", res[0][b][a]%100, res[0][b][a]/100);
  if (strcmp(s1, s2)!=0) {
    if (strcmp(s1, scr)==0) return 0;
    if (strcmp(s2, scr)==0) return 1;
  }
  int r1 = rnd[0][a][b]/1000;
  int r2 = rnd[0][b][a]/1000;
  if (frq==1) {
    if (r1<r2) return 0;
    if (r2<r1) return 1;
  }
  else {
    if (r1<r2) return 1;
    if (r2<r1) return 0;
  }
  fprintf(stderr, "ERROR: Could not determine round for %s-%s [%s].\n", NickOf(L, id[a], year), NickOf(L, id[b], year), scr);
  int haw;
  fprintf(stderr, "Choose home/away for match #%d [0=home, 1=away]: ", frq);
  scanf("%d", &haw);
  return (haw!=0);
}

int FindOpp(int tmid, int r) {
   for (int i=0; i<NT; i++) {
      int r1 = rnd[0][tmid][i]/1000;
      int r2 = rnd[0][i][tmid]/1000;
      if (r1==r)  return i;
      if (r2==r)  return NT+i;
   }
   return -1;
}

void Extract(char *filename) {
  char s[5000], w[5000], u[5000], *tk[100], m3[5], smin[10];
  int pa, pb, sa[5], sb[5];
  int first, last, nopps;
  int postfreq[MAX_ROUNDS][22];

  FILE *f = fopen(filename, "rt");
  if (f==NULL) {
    fprintf(stderr, "ERROR: Cannot open file %s.\n", filename);
    return;
  }
  
  if (strchr(filename, '/')) {
    strcpy(s, filename);
    char *sy = strtok(s, "/");
    if (sy!=NULL) year = atoi(sy);
  }

  for (int i=0; i<MAX_ROUNDS; i++) {
    nsub[i] = 0;
    mid[i] = -1;
    home[i] = 0;
    post[i] = 0;
    summ[i] = 0;
    for (int j=0; j<22; j++) { 
      postfreq[i][j] = 0;
      form[i][j] = -1;
    }
  }

  do {
    fgets(w, 5000, f);
    if (w[0]=='#') {
      fgets(s, 5000, f);
      tk[0]= strtok(s, "\t");
      tm = atoi(tk[0]);
      tmid = FindID(tm);
      for (int i=1; i<50; i++) tk[i] = strtok(NULL, "\t\n");
      first = FC; 
      while (tk[first]!=NULL && (tk[first][0]==' ' || tk[first][0]=='_') && first < 50) first++;
      if (first>=50 || tk[first]==NULL) first = 4; /* default */
      for (int i=0; i<NT; i++) { oppfreq[i] = 0; hafreq[i] = 0; }
      nopps = 0;
      for (int i=0; i<NR; i++) {
        if (tk[first+i]) strcpy(oppstr[i], tk[first+i]); else strcpy(oppstr[i], " ");
        if (strcmp(oppstr[i], " ")!=0) nopps++;
        opp[i] = MatchOpponent(oppstr[i]);
      }

      fgets(s, 5000, f);
      tk[0]= strtok(s, "\t");
      for (int i=1; i<50; i++) tk[i] = strtok(NULL, "\t\n");
      first = FC; 
      while (tk[first]!=NULL && (tk[first][0]==' ' || tk[first][0]=='_') && first < 50) first++;
      if (first>=50 || tk[first]==NULL) first = 4;
      for (int i=0; i<NR; i++) { strcpy(score[i], tk[first+i]); }
      fprintf(stderr, "first column: %d\n", first);

      if (nopps==0) {
        fprintf(stderr, "Matching opponents by round #...\n");
        for (int i=0; i<NR; i++) {
          int xop = FindOpp(tmid, i+1);
          if (xop<0 || xop>=2*NT) { fprintf(stderr, "Could not find opponent in round %d.\n", i+1); }
          else if (xop<NT) {
            home[i] = 0;
            opp[i] = xop; oppid = xop;
            oppfreq[opp[i]]++;
            hafreq[oppid]++;
            strcpy(oppstr[i], NameOf(L, opp[i], year));
            mid[i] = tmid*(NT-1)+oppid-(oppid>tmid?1:0);
          }
          else {
            home[i] = 1;
            opp[i] = xop-NT; oppid = xop-NT;
            oppfreq[opp[i]]++;
            strcpy(oppstr[i], NameOf(L, opp[i], year));             
            mid[i] = oppid*(NT-1)+tmid-(tmid>oppid?1:0);
          }
        }
      }
      else {
        for (int i=0; i<NR; i++) {
          if (opp[i]>=0 && opp[i]<NT) {
            oppfreq[opp[i]]++;
            oppid = opp[i];
            if (tmid>=0 && oppid>=0) {
              int r1 = rnd[0][tmid][oppid]/1000;
              int r2 = rnd[0][oppid][tmid]/1000;
              int haw = FindHA(tmid, oppid, score[i], oppfreq[opp[i]]);
              if (haw==0) { mid[i] = tmid*(NT-1)+oppid-(oppid>tmid?1:0); home[i] = 0; hafreq[oppid]++;}
              if (haw==1) { mid[i] = oppid*(NT-1)+tmid-(tmid>oppid?1:0); home[i] = 1; }
            }
          }
        }
      }


      fgets(s, 5000, f);
      fgets(w, 5000, f);
 
      // --- Header info
      fprintf(stderr, "Roster %s %d.\n", NameOf(L, tm, year), year);

      fprintf(stderr, "Opponents: \n");
      for (int i=0; i<NR; i++) {
        Mnem4(NickOf(L, id[opp[i]], year), m3);
        fprintf(stderr, "%2d%c%4s %s, ", i+1, (home[i]?'-':'+'), m3, score[i]);
        if (i==NR/2-1) fprintf(stderr, "\n");
      }
      fprintf(stderr, "\n");
      for (int i=0; i<NT; i++)  {
        if (oppfreq[i]!=2 && id[i]!=tm) {
          fprintf(stderr, "%sWARNING: oppfreq[%s] = %d.\n", normal, NickOf(L, id[i], year), oppfreq[i]);
        }
      }
      for (int i=0; i<NT; i++)  {
        if (hafreq[i]!=1 && id[i]!=tm) {
          fprintf(stderr, "%sWARNING: hafreq[%s] = %d.\n", normal, NickOf(L, id[i], year), hafreq[i]);
        }
      }
      fprintf(stderr, "%s", yellow);

      for (int k=0; k<7+NR*3; k++) fprintf(stdout, "-"); fprintf(stdout, "\n");
      fprintf(stdout, " NUME |");
      for (int i=1; i<=NR; i++) fprintf(stdout, "%2d|", i); fprintf(stdout, "\n");
      for (int k=0; k<7+NR*3; k++) fprintf(stdout, "-"); fprintf(stdout, "\n");
      nros = 0;
    }

    if (w!=NULL && w[0]!=0) {

      tk[0] = strtok(w, "\t\n");
      for (int i=1; i<100; i++) tk[i] = strtok(NULL, "\t");

      if (w==NULL || s[0]==0) continue;
      pa = MatchCatalogName(nros, tk[1], tk[2]);
      if (pa<0) continue;

      int px = cat_pid[pa];
      strncpy(td[nros][0], ppren[px], 1);
      sprintf(td[nros][1], "%s", pname[px]);
      int py = GetYear(pdob[px], 0);
      int age = year - py;
      if (py<=0) age = 0;
      sprintf(td[nros][2], "%d", age);
      sprintf(td[nros][3], "%s", pcty[px]);

      last = 99;
      ros_mec[nros] = ros_tit[nros] = ros_rez[nros] = ros_gol[nros] = 0;
      ros_min[nros] = ros_ban[nros] = ros_int[nros] = ros_gre[nros] = 0;
      while (tk[last]==NULL) last--;
      fprintf(stdout, "%6s|", pmnem[px]);
      fprintf(stderr, "%6s|", pmnem[px]);

      for (int i=0; i<NR; i++) {
        int k = first+i;
        fprintf(stdout, "%2s|", tk[k]);
        strcpy(td[nros][4+i], tk[k]);
        if (strcmp(tk[k], "R")==0) {
          if (mid[i]>=0) {
            int col = DB_T1+23*home[i]+15+nsub[i];
            sprintf(db[mid[i]][col], "%s:0  ", pmnem[px]);
//            form[mid[i]][14+nsub[i]] = px;
            nsub[i]++;
          }
          ros_ban[nros]++;
        }
        else if (isNumeric(tk[k])) {
          post[i] = atoi(tk[k]);

          if (post[i]>=1 && post[i]<=14) {
            postfreq[i][post[i]]++;
//            form[mid[i]][post[i]=1] = px;
            if (postfreq[i][post[i]]>1) {
              char saux[32];
              int col = DB_T1+23*home[i]+post[i];
              strcpy(saux, db[mid[i]][col]);
              saux[6] = 0;
              int pax = FindMnem(saux);  
              fprintf(stderr, "ERROR: Multiple numbers \nR%-2d %s\tnr %-2d (%s, %s)\n", i+1, NickOf(L, tm, year), post[i], pname[pax], pname[px]);
            }
          }
          else {
            fprintf(stderr, "ERROR: Illegal number %d in R%d\n", post[i], i+1);
          }

          if (post[i]>=1 && post[i]<=11) {
             if (mid[i]>=0) {
               int col = DB_T1+23*home[i]+post[i];
               sprintf(db[mid[i]][col], "%s:", pmnem[px]);
             }
             ros_tit[nros]++;
          }
          else if (post[i]>=12 && post[i]<=17) {
             if (mid[i]>=0) {
               int col = DB_T1+23*home[i]+post[i];
               sprintf(db[mid[i]][col], "%s:", pmnem[px]);
             }
             ros_rez[nros]++;
          }
        }
      }

      ros_mec[nros] = ros_tit[nros] + ros_rez[nros];
      fprintf(stdout, "%3d %3d %3d\n", ros_mec[nros], ros_tit[nros], ros_rez[nros]);
      fprintf(stderr, "%3d %3d %3d\t ", ros_mec[nros], ros_tit[nros], ros_rez[nros]);
 
      fgets(u, 5000, f);
      tk[0] = strtok(u, "\t\n");
      for (int i=1; i<100; i++) tk[i] = strtok(NULL, "\t\n");
 
      last = 99;
      while (tk[last]==NULL) last--;
      if (isNegative(tk[last])) ros_gre[nros] = atoi(tk[last]);
      fprintf(stdout, "      |");
      for (int i=0; i<NR; i++) {
        int k = first+i;
        fprintf(stdout, "%2s|", tk[k]);
        if (strcmp(tk[k], "R")==0) {
          if (mid[i]>=0) {
            int col = DB_T1+23*home[i]+15+nsub[i];
            sprintf(db[mid[i]][col], "%s:0  ", pmnem[px]);
//            form[mid[i]][14+nsub[i]] = px;
            nsub[i]++;
          }
          ros_ban[nros]++;
        }
        else if (isNumeric(tk[k])) {
           int vmin = atoi(tk[k]);
           ros_min[nros] += vmin;
           summ[i] += vmin;
           if (vmin==90) ros_int[nros]++;
           strcpy(td[nros][4+i], tk[k]);
           if (mid[i]>=0) {
             sprintf(smin, "%-3d", vmin);
             strcat(db[mid[i]][DB_T1+23*home[i]+post[i]], smin);
           }
        }
      }
  
      fprintf(stdout, "%5d %3d %3d %3d\n", ros_min[nros], ros_int[nros], ros_ban[nros], ros_gre[nros]);
      fprintf(stderr, "%5d %3d %3d %3d\n", ros_min[nros], ros_int[nros], ros_ban[nros], ros_gre[nros]);
      sprintf(catd[nros][TD_MINUTE], "%6d", ros_min[nros]);
      sprintf(catd[nros][TD_INTEGRAL], "%d", ros_int[nros]);
      sprintf(catd[nros][TD_BANCA], "%d", ros_ban[nros]);
      sprintf(catd[nros][TD_GOLPORTAR], "%d", ros_gre[nros]);

      w[0] = 0;
      nros++;
    }

  } while (!feof(f));

  fclose(f);
  for (int k=0; k<7+NR*3; k++) fprintf(stdout, "-"); fprintf(stdout, "\n");
  fprintf(stderr, "Checksum: ");
  for (int k=0; k<NR; k++) { if (summ[k]!=990) fprintf(stderr, "%d:%d ", k+1, summ[k]); }
  fprintf(stderr, "\n");
}

void PrintScorer(int px, int evt, int gmk) {
  char pini[7];
  GetInitial(px, pini);
  if (evt==EV_PKGOAL) fprintf(stderr, "%s", cyan);
  else if (evt==EV_OWNGOAL) fprintf(stderr, "%s", magenta);
  else fprintf(stderr, "%s", normal);
  if (evt!=EV_OWNGOAL && lnerr==1) fprintf(stderr, "%s", hired);
  fprintf(stderr, "%s%s %c", pini, pname[px], evsymb[evt]);
  if (gmk<=0 || gmk>90) fprintf(stderr, "%s", yellow);
  fprintf(stderr, "%d ", gmk);
}

void GetEvents(char *filename) {
  char s[5000], *tk[100], *tkm[20], *tkp, *tkg[20], m4[5], pini[5];
  int pc, px, em, mn, mx, mr, x, y, ng, loc, gmk, unk;
  int evt;

  FILE *f = fopen(filename, "rt");
  if (f==NULL) {
    fprintf(stderr, "ERROR: Cannot open file %s.\n", filename);
    return;
  }
  
  do {
      fgets(s, 5000, f);
      tk[0] = strtok(s, "\t\n");
      for (int j=1; j<100; j++) tk[j] = strtok(NULL, "\t\n");
      if (tk[0]!=NULL && isNumeric(tk[0])) {
        mn = atoi(tk[0]);
        if (mn>=1 && mn<=NR) {
          if (home[mn-1]) {
            x = res[0][opp[mn-1]][tmid]%100;
            y = res[0][opp[mn-1]][tmid]/100;
            loc = y;
          }
          else {
            x = res[0][tmid][opp[mn-1]]/100;
            y = res[0][tmid][opp[mn-1]]%100;
            loc = 0;
          }
          Mnem4(NickOf(L, id[opp[mn-1]], year), m4);
          char scol[32];
          strcpy(scol, yellow);
          if (x>y) strcpy(scol, green);
          if (x<y) strcpy(scol, white);
          fprintf(stderr, "%s#%2d %c%4s %s%d-%d%s: ", yellow, mn, (home[mn-1]?'-':'+'), m4, scol, x, y, yellow);
          ng = 0;
          int len = strlen(tk[CS]);
          if (tk[CS]==NULL || len<3) { fprintf(stderr, "\n"); s[0] = 0; continue; }
          if (tk[CS][len-1] == '.') tk[CS][len-1] = 0;
          if (tk[CS][len-2] == ' ' && tk[CS][len-1] == 'p') { tk[CS][len-2] = 'p'; tk[CS][len-1] = 0; }
          tkm[0] = strtok(tk[CS], ";\t\n");
          for (int j=1; j<20; j++) tkm[j] = strtok(NULL, ";\t\n");
          for (int j=0; j<20; j++) if (tkm[j]!=NULL && ng<x) {
            tkp = strtok(tkm[j], " (");
            tkg[0] = strtok(NULL, " (,)\n");
            for (int k=1; k<20; k++) tkg[k] = strtok(NULL, " (,)\n");
            if (tkp==NULL) continue;
            lnerr = 0;
            pc = BestCatalogMatch(tkp);
            if (!isNumeric(td[pc][3+mn])) { lnerr = 1; }
            px = cat_pid[pc];
            GetInitial(px, pini);
            gmk = 999;
            int k = 0;
            evt = EV_GOAL;
            while (k<20 && tkg[k]!=NULL && ng<x) {
              evt = EV_GOAL;
              int len = strlen(tkg[k]);
              if (tkg[k][len-1] == 'p') { evt = EV_PKGOAL; tkg[k][len-1] = 0; }
              if (tkg[k][len-2]=='a' && tkg[k][len-1] == 'g') { 
                evt = EV_OWNGOAL; tkg[k][len-2] = 0; 
                px = GetLastName(tkp);
                if (px<0) px = 0;
                GetInitial(px, pini);
              }
              if (tkg[k][0]=='?') gmk = 999;
              if (isNumeric(tkg[k])) {
                gmk = atoi(tkg[k]);
                if (gmk<0 || gmk>120) gmk = 999;
              }
              PrintScorer(px, evt, gmk);
              sprintf(edb[mid[mn-1]][loc+ng], "%s%c%-3d", pmnem[px], evsymb[evt], gmk);
              ng++;
              k++;
            }
            if (k==0) {
              fprintf(stderr, "%s%s %c%d ", pini, pname[px], evsymb[evt], gmk);
              sprintf(edb[mid[mn-1]][loc+ng], "%s%c%-3d", pmnem[px], evsymb[evt], gmk);
              ng++;
            }
          }
          if (ng<x) { fprintf(stderr, " %s_____ ?%s", hired, normal); }
          fprintf(stderr, "\n");
        }
      s[0] = 0;
    }

  } while (!feof(f));
  fclose(f);
}

void HTMLHeader() {
  fprintf(of, "<HTML>\n<TITLE>Formaþii %s %d</TITLE>\n", NameOf(L, tm, year), year);
  fprintf(of, "<HEAD>\n<link href=\"css/seasons.css\" rel=\"stylesheet\" type=\"text/css\"/>\n");
  fprintf(of, "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-2\">\n");
  fprintf(of, "</HEAD>\n<BODY>\n");
}

void HTMLTable() {
  fprintf(of, "<script src=\"sorttable.js\"></script>\n");
  fprintf(of, "<H3>Formaþii %s %d</H3>\n", NameOf(L, tm, year), year);

  fprintf(of, "<TABLE class=\"sortable\" cellpadding=\"1\" frame=\"box\">\n");
  fprintf(of, "<THEAD><TR>\n");
  fprintf(of, "<TH>Pr</TH>");
  fprintf(of, "<TH>Nume</TH>");
  fprintf(of, "<TH>Vrs</TH>");
  fprintf(of, "<TH>Naþ</TH>");
  for (int i=0; i<NR; i++) { fprintf(of, "<TH>%d</TH>", i+1); }
  fprintf(of, "<TH>Mec</TH>");
  fprintf(of, "<TH>Tit</TH>");
  fprintf(of, "<TH>Sub</TH>");
  fprintf(of, "<TH>Rez</TH>");
  fprintf(of, "<TH>Min</TH>");
  fprintf(of, "<TH>Gol</TH>");
  fprintf(of, "<TH>Gpr</TH>");
  fprintf(of, "</TR></THEAD>\n");

  char m3[5];
  fprintf(of, "<TR>");
  for (int i=0; i<4; i++) fprintf(of, "<TD/>");
  for (int i=0; i<NR; i++) {
    Mnem4(NickOf(L, id[opp[i]], year), m3);
    fprintf(of, "<TD>%s</TD> ", m3);
  }
  for (int i=0; i<7; i++) fprintf(of, "<TD/>");
  fprintf(of, "</TR>");
  fprintf(of, "<TR>");
  for (int i=0; i<4; i++) fprintf(of, "<TD/>");
  for (int i=0; i<NR; i++) {
    fprintf(of, "<TD ALIGN=\"center\">%s</TD> ", score[i]);
  }
  for (int i=0; i<7; i++) fprintf(of, "<TD/>");
  fprintf(of, "</TR>");
}

void HTMLBody() {
  for (int i=0; i<nros; i++) {
    fprintf(of, "<TR");
    if (i%2==0) fprintf(of, " BGCOLOR=\"DDFFFF\"");
    fprintf(of, ">");
    for (int j=0; j<NR+4; j++) {
      fprintf(of, "<TD>%s</TD>", td[i][j]);
    }
    fprintf(of, "<TD ALIGN=\"right\">%d</TD>", ros_mec[i]);
    fprintf(of, "<TD ALIGN=\"right\">%d</TD>", ros_tit[i]);
    fprintf(of, "<TD ALIGN=\"right\">%d</TD>", ros_rez[i]);
    fprintf(of, "<TD ALIGN=\"right\">%d</TD>", ros_ban[i]);
    fprintf(of, "<TD ALIGN=\"right\">%d</TD>", ros_min[i]);
    fprintf(of, "<TD ALIGN=\"right\">%d</TD>", ros_gol[i]);
    fprintf(of, "<TD ALIGN=\"right\">%d</TD>", ros_gre[i]);
    fprintf(of, "</TR>\n"); 
  }
}

void HTMLFooter() {
  fprintf(of, "\n</TABLE>\n");
  fprintf(of, "</BODY>\n</HTML>");
  fclose(of);
}

int main(int argc, char **argv) {
  strcpy(rfilename, "");
  strcpy(cfilename, "");
  strcpy(sfilename, "");
  verbosity = 2;
  NT = 18; NR = 34; CS = 17; FC = 4;
  for (int k=1; k<argc; k++) {
    if (strcmp(argv[k], "-q")==0) verbosity = 0;
    else if (strcmp(argv[k], "-f")==0 && k<argc-1) strcpy(rfilename, argv[++k]);
    else if (strcmp(argv[k], "-c")==0 && k<argc-1) strcpy(cfilename, argv[++k]);
    else if (strcmp(argv[k], "-s")==0 && k<argc-1) strcpy(sfilename, argv[++k]);
    else if (strcmp(argv[k], "-x")==0 && k<argc-1) CS = atoi(argv[++k]);
    else if (strcmp(argv[k], "-k")==0 && k<argc-1) { FC = atoi(argv[++k]); }
    else if (strcmp(argv[k], "-n")==0 && k<argc-1) { NT = atoi(argv[++k]);   NR = 2*(NT-1); }
    else if (strcmp(argv[k], "-m")==0 && k<argc-1) { NR = atoi(argv[++k]); }
    else if (strcmp(argv[k], "-y")==0 && k<argc-1) year = atoi(argv[++k]);
  }
  if (FC < 0 || FC > NT) {
    fprintf(stderr, "ERROR: Illegal first column index %d.\n", FC);
    FC = 4;
  }
  if (strlen(rfilename)==0) {
    fprintf(stderr, "ERROR: input file not specified.\n");
    return -1;
  }
  if (NT < 12 || NT > 20) {
    fprintf(stderr, "ERROR: invalid number of teams %d.\n", NT);
    return -2;
  }

  Load();
  LoadPlayers();
  LoadSeason();
  LoadCatalog(cfilename);
  LoadDB();
  LoadEvents();

  Extract(rfilename);
  GetEvents(sfilename);

/*
  sprintf(ofilename, "roster-%d-%d.html", year, tm);
  of = fopen(ofilename, "wt");  
  HTMLHeader();
  HTMLTable();  
  HTMLBody();
  HTMLFooter();
  fclose(of);
*/

  SaveDB();
  SaveEvents();
  SaveCatalog();


  return 0;
}
