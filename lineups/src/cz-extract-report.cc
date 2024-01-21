#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "zz-catalog.hh"

#define ROSTER_SIZE   22
#define MAX_EVENTS    40
#define MAX_CAT     1000
#define CAT_COLS      20
#define CAT_CELL      64
#define MAX_ROUNDS  40
#define MAX_TEAMS   24
#define NUM_ORD     10
#define MAX_RR       3

#define EV_GOAL      0
#define EV_OWNGOAL   1
#define EV_PKGOAL    2
#define EV_PKMISS    3
#define EV_YELLOW    4
#define EV_RED       5
#define EV_YELLOWRED 6
#define EV_PKSAVED   7
#define EV_COLS     30

#define DB_ROWS      600
#define DB_COLS       60
#define DB_CELL       30
#define DB_HOME          0
#define DB_AWAY          1
#define DB_SCORE         2
#define DB_DATE          3
#define DB_COMP          4
#define DB_ROUND         5
#define DB_VENUE         6
#define DB_ATTEND        7
#define DB_WEATHER       8
#define DB_ROSTER1       9
#define DB_COACH1       31
#define DB_ROSTER2      32
#define DB_COACH2       54
#define DB_REF          55
#define DB_ASIST1       56
#define DB_ASIST2       57
#define DB_OBSERV       58
#define DB_T1            8
#define DB_T2           31

extern int cdist(char*a, char*b);

const char *tnormal  = "\033[0m";
const char *thiyellow  = "\033[33;1m";

const char *months[] = {
"", "Janeiro", "Fevreiro", "Março", "Abril", "Maio", "Junho",
    "Julho", "Agosto", "Setembro", "Outibro", "Novembro", "Dezembro"};

const char* evsymb = "'`\"/#!*\\";
int round = -1;
int minu = 0;
int hour = 0;
int day = 0;
int month = 0;
int year = 0;
char ldate[24];
int season = 0;
int city = -1;
int stad = -1;
int ref = -1;
int home = -1;
int away = -1;
int score = -1;
int hsc = -1;
int asc = -1;
int hid = -1;
int aid = -1;
int hx = -1;
int ax = -1;
int hpid[ROSTER_SIZE];
int apid[ROSTER_SIZE];
int hpln[ROSTER_SIZE];
int apln[ROSTER_SIZE];
int hmin[ROSTER_SIZE];
int amin[ROSTER_SIZE];
int hco = -1;
int aco = -1;
int nev;
char evs[MAX_EVENTS][128];
int evp[MAX_EVENTS];
int evt[MAX_EVENTS];
int evm[MAX_EVENTS];
int evy[MAX_EVENTS];
int nevy;
char roster[ROSTER_SIZE][2][128];
char smin[ROSTER_SIZE][2][128];
int card[ROSTER_SIZE][2];
int subx[ROSTER_SIZE][2];
int suby[ROSTER_SIZE][2];
char coach[2][128];
char referee[128];
char venue[128];
char attend[128];
char *sbuff = new char[4096];
char *seuff = new char[1024];
char *spg = new char[256];

Catalog *Pl = new Catalog();
Catalog *Co = new Catalog();
Catalog *Rf = new Catalog();
Locations *Loc = new Locations();
char cat[MAX_CAT][CAT_COLS][CAT_CELL];

char fns[256];
char lns[256];

// Ugly globals
FILE *f;
char *s, *tk, *tok[20], *ystr, *name, *nick;

int FindMonth(char *s) {
  if (!s) return 0;
  for (int i=1; i<=12; i++) {
    if (strcmp(s, months[i])==0) return i;
  }
  return 0;
}

char* ltrim(char *s) {
  if (!s) return NULL;
  int i = 0;
  while (s[i]==' ' || s[i]=='\t') i++;
  return s+i;
}

void rtrim(char *s) {
  if (!s) return;
  int i = strlen(s) - 1;
  while (i>0 && (s[i]==' ' || s[i]=='\t' || s[i]=='\n')) i--;
  s[i+1] = 0;
}

int isUpper(char *s) {
  if (!s) return 0;
  int l = strlen(s);
  for (int i=0; i<l; i++) {
    if ((s[i]>='a' && s[i]<='z') || s[i]=='?') return 0;
  }
  return 1;
}

void toCamel(char *s) {
  if (!s) return;
  int l = strlen(s);
  for (int i=1; i<l; i++) {
    if (s[i]>='A' && s[i]<='Z') s[i] += 32;
  }
}

void sanitize(char *s) {
  // 012345678901
  // D&#39;Aquino
  char *pos;
  while ((pos=strstr(s, "&#39;"))!=NULL) {
    int l = strlen(pos); // 11
    pos[0] = 39; // D'#39;Aquino
    int k = 5;
    while (k<=l && pos[k]!=0) {
      pos[k-4] = pos[k]; // D'Aquino
      k++;
    }
    pos[k-4] = 0;
  }
}

void Tokenize(char *s) {
  char *tk[6];
  sanitize(s);
  tk[0] = strtok(s, " ");
  fns[0] = 0; lns[0] = 0;
  for (int j = 1; j<6; j++) tk[j] = strtok(NULL, " ");
  int ntf = 0;
  int ntl = 0;
  for (int j=0; j<6; j++) {
    if (tk[j]==NULL) continue;
    if (isUpper(tk[j])) {
      if (ntl>0) { strcat(lns, " "); }
      toCamel(tk[j]);
      strcat(lns, tk[j]);
      ntl++;
    } else {
      if (ntf>0) { strcat(fns, " "); }
      strcat(fns, tk[j]);
      ntf++;
    }
  }
}

char **club;
char **mnem;
int  NC, NT, ncat;
int n, ppv, tbr, pr1, pr2, rel1, rel2, r, z, numr;

char ldb[DB_ROWS][DB_COLS][DB_CELL];
char edb[DB_ROWS][EV_COLS][DB_CELL];

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
  f = fopen("data/zteams.dat", "rt");
  if (f==NULL) return 0;
  fscanf(f, "%d\n", &NC);
  club = new char*[NC];
  mnem = new char*[NC];
  L = new Aliases*[NC];
  for (int i=0; i<NC; i++) L[i] = new Aliases;
  char *s = new char[128];
  for (int i=0; i<NC; i++) {
    fgets(s, 60, f);
    s[strlen(s)-1] = 0;
    mnem[i] = new char[16];
    club[i] = new char[64];
    strtok(s,",");
    strncpy(mnem[i], s, 16);
    char *wft = strtok(NULL, "\n");
    strncpy(club[i], wft, 64);
  }
  fclose(f);
  f = fopen("data/alias.dat", "rt");
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

int id[MAX_TEAMS], res[MAX_RR][MAX_TEAMS][MAX_TEAMS], rnd[MAX_RR][MAX_TEAMS][MAX_TEAMS];

void LoadSeason(int year) {
  char filename[64];
  sprintf(filename, "data/a.%d", year);
  FILE *f = fopen(filename, "rt");
  if (f==NULL) {
    fprintf(stderr, "ERROR: file %s not found.\n", filename);
    return;
  }
  char s[500], *tok[12];
  int i, j, k;
  fscanf(f, "%d %d %d %d %d %d %d\n", &n, &ppv, &tbr, &pr1, &pr2, &rel1, &rel2);
  numr = tbr/NUM_ORD + 1;
  NT = n;
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

int FindId(int t) {
  for (int i=0; i<NT; i++)
    if (id[i]==t) return i;
  return -1;
}

int FindWebTeam(char *s) {
  for (int i=0; i<NT; i++) {
    if (strcmp(s, club[id[i]])==0) return i;
  }
  return -1;
}

int FindNick(char *s, int season) {
  for (int i=0; i<NT; i++) {
    if (strcmp(s, NickOf(L, id[i], season))==0) return i;
  }
  return -1;
}

int Mid(int h, int a, int r, int d) {
  int m1 = (NT-1)*h + a - (h<a? 1 : 0);
  int m2 = m1 + NT*(NT-1);
  if (r==rnd[0][h][a]/1000) return m1;
  return m2;
}

void LoadDB(int year) {
  char filename[64], s[5000], *tk[DB_COLS];
  sprintf(filename, "db/lineups-%d.db", year);
  FILE *f = fopen(filename, "rt");
  if (f==NULL) { fprintf(stderr, "ERROR: database %s not found.\n", filename); return; }
  for (int i=0; i<numr*NT*(NT-1); i++) {
    fgets(s, 5000, f);
    tk[0] = strtok(s, ",\n");
    for (int j=1; j<DB_COLS; j++) tk[j]=strtok(NULL, ",\n");
    for (int j=0; j<DB_COLS; j++) {
      if (tk[j]!=NULL) strcpy(ldb[i][j], tk[j]);
      else strcpy(ldb[i][j], " ");
    }
  }
  fclose(f);
}

void LoadEvents(int year) {
  char filename[64], s[5000], *tk[DB_COLS];
  sprintf(filename, "db/events-%d.db", year);
  FILE *f = fopen(filename, "rt");
  if (f==NULL) { fprintf(stderr, "ERROR: database %s not found.\n", filename); return; }
  for (int i=0; i<numr*NT*(NT-1); i++) {
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

void SaveDB(int year) {
  char oldfilename[64];
  char newfilename[64];
  char filename[64];
  sprintf(filename,    "db/lineups-%d.db",  year);
  sprintf(oldfilename, "db/lineups-%d.old", year);
  sprintf(newfilename, "db/lineups-%d.new", year);
  FILE *f = fopen(newfilename, "wt");
  if (f==NULL) { fprintf(stderr, "ERROR: database %s not found.\n", newfilename); return; }
  for (int i=0; i<numr*NT*(NT-1); i++) {
    for (int j=0; j<DB_COLS; j++) fprintf(f, "%s,", ldb[i][j]);
    fprintf(f, "\n");
  }
  fclose(f);
  rename(filename, oldfilename);
  rename(newfilename, filename);
}

void SaveEvents(int year) {
  char oldfilename[64];
  char newfilename[64];
  char filename[64];
  sprintf(filename,    "db/events-%d.db",  year);
  sprintf(oldfilename, "db/events-%d.old", year);
  sprintf(newfilename, "db/events-%d.new", year);
  FILE *f = fopen(newfilename, "wt");
  if (f==NULL) { fprintf(stderr, "ERROR: database %s not found.\n", newfilename); return; }
  for (int i=0; i<numr*NT*(NT-1); i++) {
    for (int j=0; j<EV_COLS; j++) fprintf(f, "%s,", edb[i][j]);
    fprintf(f, "\n");
  }
  fclose(f);
  rename(filename, oldfilename);
  rename(newfilename, filename);
}

int LoadCatalog(int year) {
//burdca,Carol,Burdan,00/00/1912,ROM,173,#,A,3,3,0,-11,carol-burdan
  char filename[64], s[5000], *tk[CAT_COLS];
  FILE *f;
  sprintf(filename, "catalogs/catalog-%d.dat", year);
  f = fopen(filename, "rt");
  if (f==NULL) { fprintf(stderr, "ERROR: catalog %s not found.\n", filename); return 0; }
  int i = 0;
  while (!feof(f)) {
    fgets(s, 5000, f);
    if (strlen(s)<20) continue;
    tk[0] = strtok(s, ",\n");
    for (int j=1; j<CAT_COLS; j++) tk[j]=strtok(NULL, ",\n");
        for (int j=0; j<CAT_COLS; ++j) {
            if (tk[j]) strcpy(cat[i][j], tk[j]); else strcpy(cat[i][j], " ");
        }
    s[0] = 0;
    i++;
  }
  fclose(f);
  ncat = i;
//  fprintf(stderr, "Loaded %d catalog entries from %s.\n", ncat, filename);
  return 1;
}

int FindInCatalog(char *s, int t) {
  for (int i=0; i<ncat; i++) {
    int ct = atoi(cat[i][5]);
    if (t==-1 || t==ct) {
      if (strcmp(cat[i][12], s)==0) {
        return i;
      }
    }
  }
  return -1;
}

int FindCoach(char *s) {
  return Co->FindLink(s);
}

int FindReferee(char *s) {
  return Rf->FindIndex(s);
}

int FindCity(char *s) {
  return Loc->FindCityName(s);
}

void ErrorMsg(const char *et, char *es) {
  fprintf(stderr, "%sError: %s %s not found%s.\n", thiyellow, et, es, tnormal);
}

void ExtractRound(char *s) {
  strtok(s, ":");
  char *tr = strtok(NULL, ".");
  round = atoi(ltrim(tr));
}

void ExtractDate(char *s) {
  strtok(s, ":");
  char *ty = strtok(NULL, "-");
  char *tm = strtok(NULL, "-");
  char *td = strtok(NULL, "@");
  char *th = strtok(NULL, ":");
  char *ti = strtok(NULL, ":");
  day = atoi(td);
  month = atoi(tm);
  int lyr = atoi(ty);
  hour = atoi(th);
  minu = atoi(ti);
  sprintf(ldate, "%04d-%02d-%02d@%02d:%02d", lyr, month, day, hour, minu);
}

void ExtractVenue(char *s) {
}

int ExtractTid(char *s) {
  strtok(s, ":");
  char *tt = strtok(NULL, "\n");
  int hh = -1;
  int hd = 1000;
  for (int i=0; i<NT; i++) {
    int idist = cdist(tt, NameOf(L, id[i], season));
    if (idist == 0) return i;
    if (idist < hd) {
      hd = idist;
      hh = i;
    }
  }
  return hh;
}

int ExtractScore(char *s) {
  strtok(s, ":");
  char *ts = strtok(NULL, "\n");
  char *tx = strtok(ts, "-");
  char *ty = strtok(NULL, "-");
  int x = atoi(tx);
  int y = atoi(ty);
  return 100*x + y;
}

int FindOwn(char *s) {
  for (int i=0; i<ROSTER_SIZE; i++) {
    if (hpid[i] >=0 && strcmp(Pl->P[hpid[i]].name,s)==0) return hpid[i];
    if (apid[i] >=0 && strcmp(Pl->P[apid[i]].name,s)==0) return apid[i];
  }
  return -1;
}

int FindPindex(char *s) {
  char *spr = strtok(s, " ");
  char *sln = strtok(NULL, "\n");
  for (int i=0; i<ROSTER_SIZE; i++) {
    if (hpid[i] >=0 && strcmp(Pl->P[hpid[i]].name,sln)==0 && strcmp(Pl->P[hpid[i]].pren,spr)==0) return hpid[i];
    if (apid[i] >=0 && strcmp(Pl->P[apid[i]].name,sln)==0 && strcmp(Pl->P[apid[i]].pren,spr)==0) return apid[i];
  }
  return -1;
}

int FindInRoster(char *s) {
  char *spr = strtok(s, " ");
  char *sln = strtok(NULL, "\n");
  rtrim(sln);
  for (int i=0; i<ROSTER_SIZE; i++) {
    int hpi = hpid[i];
    int api = apid[i];
    if (hpi >=0 && strcmp(Pl->P[hpi].name,sln)==0 && strcmp(Pl->P[hpi].pren,spr)==0) return i;
    if (api >=0 && strcmp(Pl->P[api].name,sln)==0 && strcmp(Pl->P[api].pren,spr)==0) return i + ROSTER_SIZE;
  }
  return -1;
}

void ExtractSentOff(char *s) {
  if (!s || strlen(s)<3) return;
  char *tk[5];
  tk[0] = strtok(s, ",");
  for (int i=1; i<5; i++) tk[i] = strtok(NULL, ",");
  for (int i=0; i<5; i++) {
    if (tk[i]) {
      char *tkm = strtok(tk[i], " ");
      char *tkp = strtok(NULL, "\n");
      int som = atoi(tkm);
      int pos = FindInRoster(tkp);
      if (pos >=0) {
        if (pos < ROSTER_SIZE) {
          hmin[pos] -= (91-som);
          evp[nev] = hpid[pos];
        } else {
          amin[pos-ROSTER_SIZE] -= (91-som);
          evp[nev] = apid[pos-ROSTER_SIZE];
        }
        evt[nev] = EV_RED;
        evm[nev] = som;
        nev++;
      }
    }
  }
}

void ExtractPkmiss(char *s) {
  char *tkp = strtok(s, "(");
  char *tkm = strtok(NULL, ".");
  int gmin = atoi(tkm);
  rtrim(tkp);
  int pix = FindPindex(tkp);
  evp[nev] = pix;
  evt[nev] = EV_PKMISS;
  evm[nev] = gmin;
  nev++;
}

int SafeFind(Catalog *C, char *pr, char *ln) {
  if (ln == NULL) {
    ln = strdup(pr);
    pr = strdup(" ");
  }
  return C->FindName(pr, ln);
}

void ExtractEvents() {
  char *s = new char[512];
  char *t = new char[512];
  char *tk[5];
  do {
    fgets(t, 512, f);
    strncpy(s, t, 512);
    tk[0] = strtok(s, ":");
    for (int i=1; i<5; i++) tk[i] = strtok(NULL, ",\n");
    if (strstr(t,"Goal:")!=NULL) {
      char *tkm = strtok(tk[1], ".");
      int gmin = atoi(tkm);
      int px = atoi(tk[3]);
      if (px==-2) {
        char *spr = strtok(tk[4], " ");
        char *sln = strtok(NULL, " ,(\n");
        int pix = SafeFind(Pl, spr, sln);
        px = pix;
      } else {
        int pix = Pl->FindIndex(px);
        px = pix;
      }
      evp[nev] = px;
      char *spen = strstr(tk[4], "(pen)");
      evt[nev] = spen == NULL ? EV_GOAL : EV_PKGOAL;
      evm[nev] = gmin;
      nev++;
    }
    else if (strstr(t,"Own:")!=NULL) {
      int gmin = atoi(tk[1]);
      int pix = FindOwn(tk[3]);
      evp[nev] = pix;
      evt[nev] = EV_OWNGOAL;
      evm[nev] = gmin;
      nev++;
    }
    else if (strstr(t,"Penalty:")!=NULL) {
      for (int i=1; i<5; i++) if (tk[i]) {
        ExtractPkmiss(ltrim(tk[i]));
      }
    }
    else if (strstr(t,"Sent off:")!=NULL) {
      char *so1 = strtok(tk[1], "-\n");
      char *so2 = strtok(NULL, "\n");
      ExtractSentOff(so1);
      ExtractSentOff(so2);
    }
    else if (strstr(t,"Attendance:")!=NULL) {
      strncpy(attend, tk[1], 6);
    }
    else if (strstr(t,"Referee:")!=NULL) {
      if (strcmp(tk[1],"-2")!=0) {
        ref = FindReferee(tk[1]);
      } else {
        char *spr = strtok(tk[2], " ");
        char *sln = strtok(NULL, " ,(\n");
        int rix = SafeFind(Rf, spr, sln);
        ref = rix;
      }
    }
    t[0] = 0;
  } while (!feof(f));
  for (int i=0; i<nevy; i++) {
    evp[nev] = evy[i];
    evt[nev] = EV_YELLOW;
    evm[nev] = 90;
    nev++;
  }
}

int ExtractCoach(char *s) {
  char *ci = strtok(s,":");
  char *sc = strtok(NULL, "\n");
  char *sco = strtok(sc,",");
  if (strcmp(sco,"-2")==0) {
    char *snm = strtok(NULL, "\n");
    char *spr = strtok(snm, " ");
    char *sln = strtok(NULL, " \n");
    int cx = SafeFind(Co, spr, sln);
    return cx;
  }
  int cix = Co->FindIndex(sco);
  return cix;
}

void ExtractInfo(int pos, char *info) {
  if (info == NULL) return;
  char *tk[5];
  tk[0] = strtok(info, " \n");
  for (int i=1; i<5; i++) tk[i] = strtok(NULL, " \n");
  int ntk = 0; for (int i=0; i<5; i++) if (tk[i]!=NULL) ntk++;
  for (int i=0; i<ntk; i++) {
    if (tk[i][0] == '#') {
      evy[nevy] = pos < 23 ? hpid[pos-1] : apid[pos-23];
      nevy++;
    }
    else if (tk[i][0] == '!') {
    }
    else if (tk[i][0] == '-') {
      char *tkj = strtok(tk[i], "(");
      char *tkm = strtok(NULL, ".");
      int imin = atoi(tkm);
      if (pos < 23) hmin[pos-1] -= (90 - (imin-1));
      else amin[pos-23] -= (90 - (imin-1));
    }
    else if (tk[i][0] == '<') {
      char *tkj = strtok(tk[i], "(");
      char *tkm = strtok(NULL, ".");
      int imin = atoi(tkm);
      if (pos < 23) hmin[pos-1] = (90 - (imin-1));
      else amin[pos-23] = (90 - (imin-1));
    }
  }
}

void ExtractRosters() {
  char *s = new char[512];
  char *tk[5];
  fgets(s, 512, f);
  if (s[0]=='-') fgets(s, 512, f);
  do {
    if (strstr(s, "Pos:")!=NULL) {
      char *si = strtok(s,":");
      char *sp = strtok(NULL, "\n");
      tk[0] = strtok(sp,",");
      for (int i=1; i<5; i++) tk[i] = strtok(NULL, ",");
      int pos = atoi(tk[0]);
      hpln[pos-1] = atoi(tk[1]);
      if (hpln[pos-1]==-2) {
        char *spr = strtok(tk[2], " ");
        char *sln = strtok(NULL, " \n");
        int px = SafeFind(Pl, spr, sln);
        hpid[pos-1] = px;
      } else {
        hpid[pos-1] = Pl->FindIndex(hpln[pos-1]);
      }
      ExtractInfo(pos, tk[3]);
    }
    fgets(s, 512, f);
  } while (s[0]!='C');
  hco = ExtractCoach(s);

  fgets(s, 512, f);
  if (s[0]=='-') fgets(s, 512, f);
  do {
    if (strstr(s, "Pos:")!=NULL) {
      char *si = strtok(s,":");
      char *sp = strtok(NULL, "\n");
      tk[0] = strtok(sp,",");
      for (int i=1; i<5; i++) tk[i] = strtok(NULL, ",");
      int pos = atoi(tk[0]);
      apln[pos-23] = atoi(tk[1]);
      if (apln[pos-23]==-2) {
        char *spr = strtok(tk[2], " ");
        char *sln = strtok(NULL, " \n");
        int px = SafeFind(Pl, spr, sln);
        apid[pos-23] = px;
      } else {
        apid[pos-23] = Pl->FindIndex(apln[pos-23]);
      }
      ExtractInfo(pos, tk[3]);
    }
    fgets(s, 512, f);
  } while (s[0]!='C');
  aco = ExtractCoach(s);
  fgets(s, 512, f);
}

//---------------------

int main(int argc, char **args) {
  if (argc < 2) {
    fprintf(stderr, "ERROR: missing args.");
  }
  season = atoi(args[1]);
  char *srep = new char[32767];
  Load();
  LoadSeason(season);
  LoadDB(season);
  LoadEvents(season);
  LoadCatalog(season);
  Pl->Load("catalogs/zplayers.dat");
  Rf->Load("catalogs/zreferees.dat");
  Co->Load("catalogs/zcoaches.dat");
  Loc->Load("catalogs/city.dat", "catalogs/stadium.dat");

  char filename[256];
  sprintf(filename, "%s", args[2]);
  FILE *f = fopen(filename, "rt");
  if (!f) {
    fprintf(stderr, "ERROR: file %s not found.", filename);
    return -1;
  }

  for (int i=0; i<ROSTER_SIZE; i++) {
    hpid[i] = apid[i] = hpln[i] = apln[i] = -1;
    hmin[i] = amin[i] = (i<11? 90 : 0);
    nev = nevy = 0;
  }

  fgets(srep, 512, f);
  ExtractRound(srep);
  fgets(srep, 512, f);
  ExtractDate(srep);
  fgets(srep, 512, f);
  ExtractVenue(srep);
  fgets(srep, 512, f);
  home = ExtractTid(srep);
  fgets(srep, 512, f);
  away = ExtractTid(srep);
  fgets(srep, 512, f);
  score = ExtractScore(srep);

  int mround = rnd[0][home][away]/1000;
  int mdate = rnd[0][home][away]%1000;
  int mday = mdate%50;
  int mmonth = mdate/50;
  hsc = res[0][home][away]/100;
  asc = res[0][home][away]%100;

  ExtractRosters();
  ExtractEvents();

  printf("--------------------------------------------------------------------------------\n");
  printf(" > R%d %02d/%02d @%02d:%02d (%02d/%02d)\n", round, day, month, hour, minu, mday, mmonth);
  printf(" > %s %d-%d %s  (%d-%d)\n", NickOf(L, id[home], season), hsc, asc, NickOf(L, id[away], season), score/100, score%100);
  printf(" > Attendance: %s\n", attend);
  printf("--------------------------------------------------------------------------------\n");

  char * shname = new char[30];
  char * saname = new char[30];
  for (int i=0; i<11; i++) {
    int phi = hpid[i];
    int pai = apid[i];
    if (phi>=0) sprintf(shname, "[%s] %s %s", Pl->P[phi].mnem, Pl->P[phi].pren, Pl->P[phi].name); else strcpy(shname, " ");
    if (pai>=0) sprintf(saname, "[%s] %s %s", Pl->P[pai].mnem, Pl->P[pai].pren, Pl->P[pai].name); else strcpy(saname, " ");
    printf("%2d.%-37s %3d %7d.%-37s %3d\n", i+1, shname, hmin[i], i+1, saname, amin[i]);
  }
  printf("--------------------------------------------------------------------------------\n");
  for (int i=0; i<11; i++) {
    int nsi = 0;
    if (hpid[11+i] >= 0) {
      nsi++;
      sprintf(shname, "[%s] %s %s", Pl->P[hpid[11+i]].mnem, Pl->P[hpid[11+i]].pren, Pl->P[hpid[11+i]].name);
    } else strcpy(shname, "");
    if (apid[11+i] >= 0) {
      nsi++;
      sprintf(saname, "[%s] %s %s", Pl->P[apid[11+i]].mnem, Pl->P[apid[11+i]].pren, Pl->P[apid[11+i]].name);
    } else strcpy(saname, "");
    if (nsi > 0) {
      printf("%2d.%-37s %3d %7d.%-37s %3d\n", 12+i, shname, hmin[11+i], 12+i, saname, amin[11+i]);
    }
  }
  printf("--------------------------------------------------------------------------------\n");
  if (hco >= 0) {
    sprintf(shname, "[%s] %s %s", Co->P[hco].mnem, Co->P[hco].pren, Co->P[hco].name);
  } else strcpy(shname, "");
  if (aco >= 0) {
    sprintf(saname, "[%s] %s %s", Co->P[aco].mnem, Co->P[aco].pren, Co->P[aco].name);
  } else strcpy(saname, "");
  printf("   %-37s    %-37s\n", shname, saname);
  printf("--------------------------------------------------------------------------------\n");
  for (int e=0; e<nev; e++) {
    printf("%s%c%d ", Pl->P[evp[e]].mnem, evsymb[evt[e]], evm[e]);
  }
  if (ref>=0) printf("\nRef: %s", Rf->P[ref].mnem);
  printf("\n--------------------------------------------------------------------------------\n");

  int mid = Mid(home, away, round, 50*month+day);
  sprintf(ldb[mid][DB_DATE], "%s", ldate);
  sprintf(ldb[mid][DB_ATTEND], "%6s", attend);
  if (hco>=0) sprintf(ldb[mid][DB_COACH1], "%s", Co->P[hco].mnem); else sprintf(ldb[mid][DB_COACH1], "      ");
  if (aco>=0) sprintf(ldb[mid][DB_COACH2], "%s", Co->P[aco].mnem); else sprintf(ldb[mid][DB_COACH2], "      ");
  if (ref>=0) sprintf(ldb[mid][DB_REF],    "%s", Rf->P[ref].mnem);

  for (int i=DB_ROSTER1; i<DB_ROSTER1+22; i++) {
    int p = hpid[i-DB_ROSTER1];
    sprintf(ldb[mid][i], "%6s:%-3d", (p>=0? Pl->P[p].mnem : " "), hmin[i-DB_ROSTER1]);
  }
  for (int i=DB_ROSTER2; i<DB_ROSTER2+22; i++) {
    int p = apid[i-DB_ROSTER2];
    sprintf(ldb[mid][i], "%6s:%-3d", (p>=0? Pl->P[p].mnem : " "), amin[i-DB_ROSTER2]);
  }

  for (int i=0; i<nev; i++) {
    sprintf(edb[mid][i], "%6s%c%-3d", Pl->P[evp[i]].mnem, evsymb[evt[i]], evm[i]);
  }

  printf("MID#: %d\n", mid);
  for (int i=0; i<DB_ROSTER1; i++) { printf("%s,", ldb[mid][i]); }  printf("\n");
  for (int i=DB_ROSTER1; i<DB_ROSTER1+11; i++) { printf("%s,", ldb[mid][i]); }  printf("\n");
  for (int i=DB_ROSTER1+11; i<DB_COACH1; i++) { printf("%s,", ldb[mid][i]); }  printf("\n");
  printf("%s,\n", ldb[mid][DB_COACH1]);
  for (int i=DB_ROSTER2; i<DB_ROSTER2+11; i++) { printf("%s,", ldb[mid][i]); }  printf("\n");
  for (int i=DB_ROSTER2+11; i<DB_COACH2; i++) { printf("%s,", ldb[mid][i]); }  printf("\n");
  printf("%s,\n", ldb[mid][DB_COACH2]);
  for (int i=DB_REF; i<DB_COLS; i++) { printf("%s,", ldb[mid][i]); }  printf("\n");

  for (int i=0; i<EV_COLS; i++) { printf("%s,", edb[mid][i]); }  printf("\n");

  SaveDB(season);
  SaveEvents(season);

  return 0;
}
