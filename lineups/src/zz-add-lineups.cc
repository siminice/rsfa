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
"", "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"};

const char* evsymb = "'`\"/#!*\\";
int round = -1;
int day = 0;
int month = 0;
int year = 0;
char clock[6];
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
  f = fopen("data/webteams.dat", "rt");
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

void ExtractRound() {
  seek("<title");
  strtok(s, ",");
  char *tr = strtok(NULL, ".");
  round = atoi(ltrim(tr));
}

void ExtractHome() {
  seek("Match details");
  seek("<table");
  seek("/teams/");
  char *sq = strstr(s, "<a href=\"/teams/")+16;
  char *st = strtok(sq, "/");
  home = FindWebTeam(st);
}

void ExtractDate() {
  strcpy(clock, "     ");
  seek("<th align=\"center\">");
  fgets(s, 1000, f);
  strtok(s, ",");
  char *tkd = ltrim(strtok(NULL, "."));
  char *tkm = ltrim(strtok(NULL, " "));
  char *tky = ltrim(strtok(NULL, "<\t\n"));
  day = atoi(tkd);
  month = FindMonth(tkm);
  year = atoi(tky);
  strtok(NULL, ">");
  char *tkc = strtok(NULL, " \r\n");
  if (tkc) strncpy(clock, tkc, 10);
}

void ExtractAway() {
  seek("/teams/");
  char *sq = strstr(s, "<a href=\"/teams/")+16;
  char *st = strtok(sq, "/");
  away = FindWebTeam(st);
}

void ExtractScore() {
  seek("resultat");
  fgets(s, 1024, f);
  char *tkx = strtok(ltrim(s), ":");
  char *tky = strtok(NULL, " \n");
  hsc = atoi(tkx);
  asc = atoi(tky);
}

void ExtractGoals() {
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

void ExtractIncidents() {
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

void ExtractRosters() {
  seek("<table");
  int t = 0;
  int row = -1;
  int col = -1;
  do {
    fgets(s, 1024, f);
    if (strstr(s, "<tr")!=NULL) { row++, col=-1; }
    if (strstr(s, "<td")!=NULL) col++;
    if ((tk=strstr(s, "Substitutes"))!=NULL) {
       row--;
    }
    if ((tk=strstr(s, "rottext"))!=NULL) {
      char *tkm=strtok(tk+9, "'");
      if (tkm!=NULL) { suby[row][t] = atoi(tkm); }
    }
    if ((tk=strstr(s, "gruentext"))!=NULL) {
      char *tkm=strtok(tk+11, "'");
      if (tkm!=NULL) { subx[row][t] = atoi(tkm); }
    }
    if ((tk=strstr(s, "player_summary"))!=NULL) {
      strncpy(roster[row][t], tk+15, 128);
      strtok(roster[row][t], "/");
    }
    if (strstr(s, "/karten/")!=NULL) {
      char *tks = strstr(s, "kleine_schrift");
      strtok(tks, ">");
      char *tkm = strtok(NULL, "'");
      evm[nev] = atoi(tkm);
      if (strstr(s, "Red card")!=NULL || strstr(s, "Second yellow")!=NULL) {
        evt[nev] = EV_RED;
        card[row][t] = evm[nev];
      }
      else evt[nev] = EV_YELLOW;
      strcpy(evs[nev], roster[row][t]);
      nev++;
    }
    if (strstr(s, "</table>")!=NULL && t==0) { t++; row = -1; fgets(s, 2000, f); }
    // away
  } while (strstr(s, "</table>")==NULL);
}

void ExtractManagers() {
  char *tps;
  seek("Manager:");
  if (strstr(s, "???")!=NULL) {
    strcpy(coach[0], "???");
    hco = -1;
  } else {
    tps = strstr(s, "player_summary");
    strncpy(coach[0], strtok(tps+15, "/"), 64);
    hco = FindCoach(coach[0]);
    if (hco<0) ErrorMsg("Coach", coach[0]);
  }
  seek("Manager:");
  if (strstr(s, "???")!=NULL) {
    strcpy(coach[1], "???");
    aco = -1;
  } else {
    tps = strstr(s, "player_summary");
    strncpy(coach[1], strtok(tps+15, "/"), 64);
    aco = FindCoach(coach[1]);
    if (aco<0) ErrorMsg("Coach", coach[1]);
  }
}

void ExtractReferee() {
  if (strstr(s, "Referee")==NULL) seek("\"Referee\" ");
  if (strstr(s, "Referee")==NULL) return;
  fgets(s, 1000, f);
  char *tkr = strstr(s, "referee_summary/");
  strncpy(referee, strtok(tkr+16, "/"), 64);
  ref = FindReferee(referee);
  if (ref<0) ErrorMsg("Referee", referee);
}

void ExtractVenue() {
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
  s = new char[2048];
  Load();
  LoadSeason(season);
  LoadDB(season);
  LoadEvents(season);
  LoadCatalog(season);
  Pl->Load("catalogs/cplayers.dat");
  Rf->Load("catalogs/creferees.dat");
  Co->Load("catalogs/ccoaches.dat");
  Loc->Load("catalogs/city.dat", "catalogs/stadium.dat");
  Cloc->Load("catalogs/city.dat", "catalogs/cstadium.dat");

  char filename[256];
  sprintf(filename, "%s", args[2]);
  FILE *f = fopen(filename, "rt");
  if (!f) {
    fprintf(stderr, "ERROR: file %s not found.", filename);
  }

  for (int i=0; i<ROSTER_SIZE; i++) {
    hpid[i] = apid[i] = -1;
    hmin[i] = amin[i] = (i<11? 90 : 0);
  }

  ExtractRound();

  printf("--------------------------------------------------------------------------------\n");
  printf(" > Round %d\n", round);

  ExtractHome();
  ExtractDate();
  ExtractAway();
  ExtractScore();

  printf(" > %4d-%02d-%02d@%s\n", year, month, day, clock);
  printf(" > %s %d-%d %s\n", NickOf(L, id[home], year), hsc, asc, NickOf(L, id[away], year));

  ExtractGoals();
  ExtractIncidents();
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

  ExtractRosters();

  ExtractManagers();
  ExtractVenue();
  ExtractReferee();

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

  int mid = Mid(home, away);
  sprintf(ldb[mid][DB_DATE], "%04d-%02d-%02d@%5s", year, month, day, clock);
  if (stad>=0) sprintf(ldb[mid][DB_VENUE], "%s", Loc->V[stad].mnem);
  if (hsc+asc>=0) sprintf(ldb[mid][DB_SCORE], "%3d", 100*hsc+asc);
  sprintf(ldb[mid][DB_ATTEND], "%7s", attend);
  sprintf(ldb[mid][DB_ROUND], "%2d", round);
  if (hco>=0) sprintf(ldb[mid][DB_COACH1], "%s", Co->P[hco].mnem); else sprintf(ldb[mid][DB_COACH1], "      ");
  if (aco>=0) sprintf(ldb[mid][DB_COACH2], "%s", Co->P[aco].mnem); else sprintf(ldb[mid][DB_COACH2], "      ");
  if (ref>=0) sprintf(ldb[mid][DB_REF],    "%s", Rf->P[ref].mnem);
  for (int i=DB_ROSTER1; i<DB_ROSTER1+22; i++) {
    int p = hpid[i-DB_ROSTER1];
    sprintf(ldb[mid][i], "%6s:%-3d", (p>=0? cat[p][0] : " "), hmin[i-DB_ROSTER1]);
  }
  for (int i=DB_ROSTER2; i<DB_ROSTER2+22; i++) {
    int p = apid[i-DB_ROSTER2];
    sprintf(ldb[mid][i], "%6s:%-3d", (p>=0? cat[p][0] : " "), amin[i-DB_ROSTER2]);
  }

  for (int i=0; i<nev; i++) {
    sprintf(edb[mid][i], "%6s%c%-3d", cat[evp[i]][0], evsymb[evt[i]], evm[i]);
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
