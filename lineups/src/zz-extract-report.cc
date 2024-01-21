#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "zz-catalog.hh"

#define ROSTER_SIZE   24
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

const char *tnormal  = "\033[0m";
const char *thiyellow  = "\033[33;1m";

const char *months[] = {
"", "Janeiro", "Fevreiro", "Março", "Abril", "Maio", "Junho",
    "Julho", "Agosto", "Setembro", "Outibro", "Novembro", "Dezembro"};

const char* evsymb = "'`\"/#!*\\";
int round = -1;
int day = 0;
int month = 0;
int year = 0;
int season = 0;
int city = -1;
int stad = -1;
int ref = -1;
int home = -1;
int away = -1;
int hsc = -1;
int asc = -1;
int hid = -1;
int aid = -1;
int hx = -1;
int ax = -1;
int hpid[ROSTER_SIZE];
int apid[ROSTER_SIZE];
int hmin[ROSTER_SIZE];
int amin[ROSTER_SIZE];
int hco = -1;
int aco = -1;
int nev;
char evs[MAX_EVENTS][128];
int evp[MAX_EVENTS];
int evt[MAX_EVENTS];
int evm[MAX_EVENTS];
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
Locations *Cloc = new Locations();
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
  int n, ppv, tbr, pr1, pr2, rel1, rel2, r, z;
  fscanf(f, "%d %d %d %d %d %d %d\n", &n, &ppv, &tbr, &pr1, &pr2, &rel1, &rel2);
  int numr = tbr/NUM_ORD + 1;
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

int Mid(int h, int a) {
  return (NT-1)*h + a - (h<a? 1 : 0);
}

void LoadDB(int year) {
  char filename[64], s[5000], *tk[DB_COLS];
  sprintf(filename, "db/lineups-%d.db", year);
  FILE *f = fopen(filename, "rt");
  if (f==NULL) { fprintf(stderr, "ERROR: database %s not found.\n", filename); return; }
  for (int i=0; i<NT*(NT-1); i++) {
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

void SaveDB(int year) {
  char oldfilename[64];
  char newfilename[64];
  char filename[64];
  sprintf(filename,    "db/lineups-%d.db",  year);
  sprintf(oldfilename, "db/lineups-%d.old", year);
  sprintf(newfilename, "db/lineups-%d.new", year);
  FILE *f = fopen(newfilename, "wt");
  if (f==NULL) { fprintf(stderr, "ERROR: database %s not found.\n", newfilename); return; }
  for (int i=0; i<NT*(NT-1); i++) {
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
  for (int i=0; i<NT*(NT-1); i++) {
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
  return Rf->FindLink(s);
}

int FindStadium(char *s) {
  return Cloc->FindStadium(city, s);
}

void seek(const char *pattern, const char *stop = "") {
  do { fgets(s, 1024, f); }
 while (!feof(f) &&
   (strstr(s, pattern)==NULL &&
   (stop[0]==0 || strstr(s, stop)==NULL))
 );
}

void ErrorMsg(const char *et, char *es) {
  fprintf(stderr, "%sError: %s %s not found%s.\n", thiyellow, et, es, tnormal);
}

void ExtractRound(char *s) {
  seek("<title");
  strtok(s, ",");
  char *tr = strtok(NULL, ".");
  round = atoi(ltrim(tr));
}

void ExtractHome(char *s) {
  char *st1 = strstr(s, "div class=\"team");
  char *sth = strstr(st1, "equipa.php");
  char *st2 = strstr(sth, "div class=\"team");
  char *sta = strstr(st2, "equipa.php");
  char *shome = new char[12];
  char *saway = new char[12];
  strncpy(shome, sth+14, 12);
  strncpy(saway, sta+14, 12);
  strtok(shome, "&");
  strtok(saway, "&");
  home = FindWebTeam(shome);
  away = FindWebTeam(saway);
  delete[] shome;
  delete[] saway;
}

void ExtractGoals(char *s) {
  seek("goals");
  nev = 0;
  do {
    fgets(s, 1024, f);
    if ((tk=strstr(s, "player_summary"))!=NULL) {
      strncpy(evs[nev], tk+15, 128);
      strtok(evs[nev], "/");
      char *tkm = strstr(tk, "</a>")+4;
      strtok(tkm, ".");
      evm[nev] = atoi(tkm);
      evt[nev] = EV_GOAL;
      char *tkt = strtok(NULL, "/");
      tkt = strtok(NULL, "\n");
      if (tkt!=NULL) {
        if (strstr(tkt, "penalty")!=NULL) evt[nev] = EV_PKGOAL;
        if (strstr(tkt, "own")!=NULL) evt[nev] = EV_OWNGOAL;
      }
      nev++;
    }
  } while (strstr(s, "</table>")==NULL);
}

void ExtractIncidents(char *s) {
  seek("<table");
  fgets(s, 1024, f); fgets(s, 1024, f);
  if (strstr(s, "Incidents")!=NULL) {
    char *tkpm;
    do {
      fgets(s, 1024, f);
      if ((tk=strstr(s, "player_summary"))!=NULL) {
      if ((tkpm=strstr(s, "misses a penalty"))!=NULL) {
        strncpy(evs[nev], tk+15, 128);
        strtok(evs[nev], "/");
        char *tkm = strchr(tk, '(');
        if (tkm != NULL) {
          tkm++;
          strtok(tkm, "'.)");
          evm[nev] = atoi(tkm);
        } else { evm[nev] = 999; }
        evt[nev] = EV_PKMISS;
        nev++;
      }
      }
    } while (strstr(s, "</table>")==NULL);
    seek("<table");
  }
}

void ExtractEvents(char *s, int i, bool h) {
  strncpy(seuff, s, 1024);
  char *spe = strstr(seuff+1, "player.php");
  char *spf = strstr(seuff+1, "column_300");
  char spe0 = 0;
  char spf0 = 0;
  if (spe) { spe0 = spe[0]; spe[0] = 0; }
  if (spf) { spf0 = spf[0]; spf[0] = 0; }
  // Q - goal, 51' (g.p.) - pen, 4' (p.b.) auto
  // <span title="Golos" class="icn_zerozero">Q</span><div>90+7' </div>
  // 8 - out
  // <span class="icn_zerozero grey">8</span><div>61</div>
  // 7 - in
  // <span title="Entrou" class="icn_zerozero grey">7</span><div>75</div>
  // R - yellow , red
  // <span title="Amarelos" class="icn_zerozero yellow">R</span><div>72'</div>
  // <span title="Vermelhos" class="icn_zerozero red">R</span><div>73' </div>
  // A - missed pen
  // <span title="Penaltis Falhados" class="icn_zerozero2" style="color:#e57f7f;">A</span><div>73' </div>
  char *sgl = strstr(seuff, "zerozero\">Q</span>");
  char *sso = strstr(seuff, "grey\">8</span>");
  char *ssi = strstr(seuff, "grey\">7</span>");
  char *syl = strstr(seuff, "yellow\">R</span>");
  char *srd = strstr(seuff, "red\">R</span>");
  char *spm = strstr(seuff, ";\">A</span>");
  int som = 0;
  int sim = 0;
  char *sds, *sde;
  if (sgl) {
    strncpy(spg, sgl, 256);
    sds = strstr(sgl, "<div>");
    sde = strstr(sds, "</div>"); sde[0] = 0;
    char *tk[15];
    tk[0] = strtok(sds+5, " )"); for (int k=1; k<15; k++) tk[k] = strtok(NULL, " )");
    for (int k=0; k<15; k++) {
      if (tk[k]) {
        bool ispk = strstr(tk[k], "(g.")!=NULL;
        bool isog = strstr(tk[k], "(p.")!=NULL;
        char *smr = strtok(tk[k], "+");
        char *sme = strtok(NULL, "+");
        if (ispk) { evt[nev-1] = EV_PKGOAL; }
        else if (isog) { evt[nev-1] = EV_OWNGOAL; }
        else {
          int gm = atoi(smr);
          int gmex = sme!=NULL? atoi(sme) : 0;
          evm[nev] = gm + gmex;
          evp[nev] = h? hpid[i] : apid[i];
          evt[nev] = EV_GOAL;
          nev++;
        }
      }
    }
  }
  if (ssi) {
    sds = strstr(ssi, "<div>");
    sde = strstr(sds, "</div>"); sde[0] = 0;
    sim = atoi(sds+5);
    if (h) hmin[i] = 91-sim;
      else amin[i] = 91-sim;
  }
  if (sso) {
    sds = strstr(sso, "<div>");
    sde = strstr(sds, "</div>"); sde[0] = 0;
    som = atoi(sds+5);
    if (h) hmin[i] = i<11? som-1: som-sim;
      else amin[i] = i<11? som-1: som-sim;
  }
  if (syl) {
    sds = strstr(syl, "<div>");
    sde = strstr(sds, "</div>"); sde[0] = 0;
    char *smr = strtok(sds+5, "+");
    char *sme = strtok(NULL, "+");
    int cm = atoi(smr);
    int cmex = sme!=NULL? atoi(sme) : 0;
    evt[nev] = EV_YELLOW;
    evm[nev] = cm + cmex;
    evp[nev] = h? hpid[i] : apid[i];
    nev++;
  }
  if (srd) {
    sds = strstr(srd, "<div>");
    sde = strstr(sds, "</div>"); sde[0] = 0;
    int cm = atoi(sds+5);
    evt[nev] = EV_RED;
    evm[nev] = cm;
    if (h) {
      evp[nev] = hpid[i]; hmin[i] -= (91-cm);
    } else {
      evp[nev] = apid[i]; amin[i] -= (91-cm);
    }
    nev++;
  }
  if (spm) {
    sds = strstr(spm, "<div>");
    sde = strstr(sds, "</div>"); sde[0] = 0;
    int cm = atoi(sds+5);
    evt[nev] = EV_PKMISS;
    evm[nev] = cm;
    evp[nev] = h? hpid[i] : apid[i];
    nev++;
  }
  if (spe0) spe[0] = spe0;
  if (spf0) spf[0] = spf0;
}

void ExtractManagers(char *sh, char *sa) {
  if (sh==NULL) return;
  char *shco = strstr(sh, "coach.php");
  if (shco) {
    strtok(shco, "\"");
    hco = Co->FindIndex(shco+13);
  }
  if (sa==NULL) return;
  char *saco = strstr(sa, "coach.php");
  if (saco) {
    strtok(saco, "\"");
    aco = Co->FindIndex(saco+13);
  }
}

void ExtractSubs(char *s, bool hsub) {
  if (!s) return;
  strncpy(sbuff, s, 4096);
  char *buffend = strstr(sbuff+1, "div class=\"column_300");
  if (buffend) buffend[0] = 0;
  char *sp = sbuff;
  char *spn  = new char[12];
  int x = 0;
  char *spi;
  do {
    spi = strstr(sp, "player.php");
    if (spi) {
      strncpy(spn, spi+14, 12);
      strtok(spn, "\"");
      int pix = Pl->FindIndex(spn);
      if (hsub) hpid[11+x] = pix; else apid[11+x] = pix;
      sp = spi+1;
      ExtractEvents(spi, 11+x, hsub);
      x++;
    }
  } while (spi != NULL);
}

void ExtractRosters(char *s) {
  char *s1 = strstr(s, "div class=\"column_300");
  char *s2 = strstr(s1+1, "div class=\"column_300");
  char *s3 = strstr(s2+1, "div class=\"column_300");
  char *s4 = strstr(s3+1, "div class=\"column_300");
  char *s5 = strstr(s4+1, "div class=\"column_300");
  char *s6 = s5!=NULL? strstr(s5+1, "div class=\"column_300") : NULL;
  char *s7 = s6!=NULL? strstr(s6+1, "Fotografias") : NULL;
  char *spn  = new char[12];
  char *sph = s1;
  for (int i=0; i<11; i++) {
    char *spi = strstr(sph, "player.php");
    if (spi) {
      strncpy(spn, spi+14, 12);
      strtok(spn, "\"");
      int pix = Pl->FindIndex(spn);
      hpid[i] = pix;
      ExtractEvents(spi, i, true);
      sph = spi+1;
    }
  }
  char *spa = s2;
  for (int i=0; i<11; i++) {
    char *spi = strstr(spa, "player.php");
    if (spi) {
      strncpy(spn, spi+14, 12);
      strtok(spn, "\"");
      int pix = Pl->FindIndex(spn);
      apid[i] = pix;
      ExtractEvents(spi, i, false);
      spa = spi+1;
    }
  }
  ExtractSubs(s3, true);
  ExtractSubs(s4, false);
  if (s6) s6[-1] = 0;
  if (s7) s7[-1] = 0;
  ExtractManagers(s5, s6);
}

void ExtractReferee(char *s) {
  if (strstr(s, "Referee")==NULL) seek("\"Referee\" ");
  if (strstr(s, "Referee")==NULL) return;
  fgets(s, 1000, f);
  char *tkr = strstr(s, "referee_summary/");
  strncpy(referee, strtok(tkr+16, "/"), 64);
  ref = FindReferee(referee);
  if (ref<0) ErrorMsg("Referee", referee);
}

void ExtractVenue(char *s) {
  seek("/venues/", "\"Referee\" ");
  if (feof(f) || strstr(s, "/venues/")==NULL) return;
  char *tkv = strstr(s, "/venues/")+8;
  strtok(tkv, "/");
  strncpy(venue, tkv, 128);
  stad = FindStadium(tkv);
  if (stad<0) ErrorMsg("Stadium", tkv);
  strcpy(attend, " ");
  seek("\"Attendance", "\"Referee\" ");
  if (feof(f) || strstr(s, "Attendance")==NULL) return;
  fgets(s, 1000, f); fgets(s, 1000, f); strtok(s, "<");
  rtrim(s); s = ltrim(s);
  strncpy(attend, s, 64);
}

//---------------------

int main(int argc, char **args) {
  if (argc < 2) {
    fprintf(stderr, "ERROR: missing args.");
  }
  int season = atoi(args[1]);
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
  Cloc->Load("catalogs/city.dat", "catalogs/cstadium.dat");

  char filename[256];
  sprintf(filename, "%s", args[2]);
  FILE *f = fopen(filename, "rt");
  if (!f) {
    fprintf(stderr, "ERROR: file %s not found.", filename);
    return -1;
  }

  for (int i=0; i<ROSTER_SIZE; i++) {
    hpid[i] = apid[i] = -1;
    hmin[i] = amin[i] = (i<11? 90 : 0);
  }

  fgets(srep, 32767, f);

  ExtractHome(srep);
  round = rnd[0][home][away]/1000;
  int mdate = rnd[0][home][away]%1000;
  day = mdate%50;
  month = mdate/50;
  hsc = res[0][home][away]/100;
  asc = res[0][home][away]%100;
  printf("--------------------------------------------------------------------------------\n");
  printf(" > R%d %02d/%02d\n", round, day, month);
  printf(" > %s %d-%d %s\n", NickOf(L, id[home], year), hsc, asc, NickOf(L, id[away], year));
  printf("--------------------------------------------------------------------------------\n");

  ExtractRosters(srep);

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
  printf("\n--------------------------------------------------------------------------------\n");

  /*
  ExtractGoals(srep);
  ExtractIncidents(srep);
  printf("--------------------------------------------------------------------------------\n");
  for (int e=0; e<nev; e++) {
    printf("%-30s %c%3d\n", evs[e], evsymb[evt[e]], evm[e]);
  }
  printf("--------------------------------------------------------------------------------\n");

  for (int i=0; i<ROSTER_SIZE; i++) {
    roster[i][0][0] = roster[i][1][0] = 0;
    smin[i][0][0] = smin[i][1][0] = 0;
    card[i][0] = card[i][1] = 0;
    subx[i][0] = subx[i][1] = 0;
    suby[i][0] = suby[i][1] = 0;
  }

  ExtractVenue(srep);
  ExtractReferee(srep);

  for (int i=0; i<ROSTER_SIZE; i++) {
    if (subx[i][0] > 0) {
      if (suby[i][0] > 0)
           sprintf(smin[i][0], "<%d' >%d'", subx[i][0], suby[i][0]);
      else sprintf(smin[i][0], "<%d'", subx[i][0]);
    } else if (suby[i][0] > 0) {
           sprintf(smin[i][0], ">%d'", suby[i][0]);
    }
    if (subx[i][1] > 0) {
      if (suby[i][1] > 0)
           sprintf(smin[i][1], "<%d' >%d'", subx[i][1], suby[i][1]);
      else sprintf(smin[i][1], "<%d'", subx[i][1]);
    } else if (suby[i][1] > 0) {
           sprintf(smin[i][1], ">%d'", suby[i][1]);
    }
  }

  for (int i=0; i<ROSTER_SIZE; i++) {
    if (i==11)   printf("--------------------------------------------------------------------------------\n");
    if (roster[i][0][0]!=0 || roster[i][1][0]!=0) {
      printf("%-30s %-7s  %-30s %-7s\n", roster[i][0], smin[i][0], roster[i][1], smin[i][1]);
    }
  }
  printf("--------------------------------------------------------------------------------\n");
  for (int i=0; i<nev; i++) {
    printf("%s%c%d ", evs[i], evsymb[evt[i]], evm[i]);
  }

  for (int i=0; i<11; i++) {
    hpid[i] = FindInCatalog(roster[i][0], id[home]);
    apid[i] = FindInCatalog(roster[i][1], id[away]);
    if (suby[i][0]>0) {
      hmin[i] = suby[i][0] -1;
    }
    if (card[i][0]>0) {
      hmin[i] = card[i][0] - 1;
    }
    if (suby[i][1]>0) {
      amin[i] = suby[i][1] -1;
    }
    if (card[i][1]) {
      amin[i] = card[i][1] - 1;
    }
  }

  printf("\n--------------------------------------------------------------------------------\n");

  for (int i=0; i<11; i++) { printf("%3d:%2d ", hpid[i], hmin[i]);}; printf("\n");
  for (int i=0; i<11; i++) { printf("%3d:%2d ", apid[i], amin[i]);}; printf("\n");
  for (int i=0; i<11; i++) {
    if (hpid[i]<0) ErrorMsg("Player", roster[i][0]);
    if (apid[i]<0) ErrorMsg("Player", roster[i][1]);
  };

  for (int i=11; i<ROSTER_SIZE-1; i++) {
    hpid[i] = FindInCatalog(roster[i][0], id[home]);
    apid[i] = FindInCatalog(roster[i][1], id[away]);
    if (subx[i][0]>0) {
      if (card[i][0]>0) {
        hmin[i] = card[i][0] - subx[i][0];
      } else {
        hmin[i] = 91 - subx[i][0];
      }
    }
    if (subx[i][1]>0) {
      if (card[i][1]>0) {
        amin[i] = card[i][1] - subx[i][1];
      } else {
        amin[i] = 91 - subx[i][1];
      }
    }
  }

  for (int i=11; i<22; i++) { printf("%3d:%2d ", hpid[i], hmin[i]); }; printf("\n");
  for (int i=11; i<22; i++) { printf("%3d:%2d ", apid[i], amin[i]); }; printf("\n");
  printf("--------------------------------------------------------------------------------\n");
  for (int e=0; e<nev; e++) {
    evp[e] = FindInCatalog(evs[e], -1);
    printf("%s%c%d ", cat[evp[e]][0], evsymb[evt[e]], evm[e]);
  }
  printf("\n--------------------------------------------------------------------------------\n");
  printf("Managers: %s (%d), %s (%d)\n", coach[0], hco, coach[1], aco);
  printf("Referee: %s (%d)\n", referee, ref);
  printf("Venue: %s (%d)\n", venue, stad);
  printf("Attendance: %s\n", attend);
  printf("--------------------------------------------------------------------------------\n");

  sprintf(ldb[mid][DB_DATE], "%04d-%02d-%02d@%5s", year, month, day, mdate);
  if (stad>=0) sprintf(ldb[mid][DB_VENUE], "%s", Loc->V[stad].mnem);
  if (hsc+asc>=0) sprintf(ldb[mid][DB_SCORE], "%3d", 100*hsc+asc);
  sprintf(ldb[mid][DB_ATTEND], "%7s", attend);
  sprintf(ldb[mid][DB_ROUND], "%2d", round);
  */

  int mid = Mid(home, away);
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
