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
#define MAX_TEAMS   32
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
#define EV_COLS     40

#define DB_ROWS      1000
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

const char* evsymb = "'`\"/#!*\\";
int force_round = 0;
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

//--------------------------------------

int Load() {
  f = fopen("data/zteams.dat", "rt");
  if (f==NULL) return 0;
  fscanf(f, "%d\n", &NC);
  club = new char*[NC];
  mnem = new char*[NC];
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
  return 1;
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
  char t[128];
  strncpy(t, s, 128);
  char *td = strchr(t, '-');
  if (td!=NULL) td[1] = 0;
  for (int i=0; i<NT; i++) {
    if (strstr(club[id[i]], t)!=NULL) return i;
  }
  return -1;
}

int Mid(int h, int a, int r, int d) {
  int m0 = (NT-1)*h + a - (h<a? 1 : 0);
  int m1 = m0 + NT*(NT-1);
  int m2 = m1 + NT*(NT-1);
  if (numr < 2 || d==rnd[0][h][a]%1000) return m0;
  if (numr > 1 && d==rnd[1][h][a]%1000) return m1;
  if (numr > 2 && d==rnd[2][h][a]%1000) return m2;
  return m0;
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

int FindPlayer(char *s) {
  char t[128];
  strncpy(t, s, 128);
  char *td = strchr(t, '-');
  if (td != NULL) td[1] = 0;
  return Pl->FindIndexPrefix(t);
}

int FindCoach(char *s) {
  char t[128];
  strncpy(t, s, 128);
  char *td = strchr(t, '-');
  if (td != NULL) td[1] = 0;
  return Co->FindIndexPrefix(t);
}

int FindReferee(char *s) {
  char t[128];
  strncpy(t, s, 128);
  char *td = strchr(t, '-');
  if (td != NULL) td[1] = 0;
  return Rf->FindIndexPrefix(t);
}

int FindCity(char *s) {
  return Loc->FindCityName(s);
}

int FindStadium(char *s) {
  if (s==NULL) return -1;
  if (s == NULL) return -1;
  return Loc->FindStadium(-1, s);
}

//---------------------

int main(int argc, char **args) {
  if (argc < 2) {
    fprintf(stderr, "ERROR: missing args.");
  }
  season = atoi(args[1]);
  if (argc > 3) {
    if (strcmp(args[3], "-r")==0) force_round = 1;
  }
  char *srep = new char[32767];
  Load();
  LoadSeason(season);
  LoadDB(season);
  LoadEvents(season);
//  LoadCatalog(season);
  Pl->Load("catalogs/zplayers.dat");
  Rf->Load("catalogs/zreferees.dat");
  Co->Load("catalogs/zcoaches.dat");
  Loc->Load("catalogs/city.dat", "catalogs/zstadium.dat");

  char filename[256];
  sprintf(filename, "%s", args[2]);
  FILE *f = fopen(filename, "rt");
  if (!f) {
    fprintf(stderr, "ERROR: file %s not found.", filename);
    return -1;
  }

  char report[2048];
  char events[2048];
  for (int i=0; i<21; i++) fgets(report, 2048, f);
  fgets(events, 2048, f);
  fclose(f);
  char *rtk[80];
  rtk[0] = strtok(report + 9, ",");
  for (int i=1; i<DB_COLS; i++) rtk[i] = strtok(NULL, ",\n");

  char *etk[80];
  events[strlen(events)-1] = 0;
  etk[0] = strtok(events + 8, ",");
  for (int i=1; i<EV_COLS; i++) etk[i] = strtok(NULL, ",\n");

  home = FindWebTeam(rtk[DB_HOME]);
  away = FindWebTeam(rtk[DB_AWAY]);
  round = atoi(rtk[DB_ROUND]);
  char *sscore = new char[12];
  strncpy(sscore, rtk[DB_SCORE], 8);
  char *sx = strtok(sscore, ":-");
  char *sy = strtok(NULL, ":-");
  score = 100*atoi(sx) + atoi(sy);
  char *rdate = new char[15];
  strncpy(rdate, rtk[DB_DATE], 12);
  char *ryear = strtok(rdate, "-");
  char *rmonth = strtok(NULL, "-");
  char *rday = strtok(NULL, "@");
  month = atoi(rmonth);
  day = atoi(rday);

  int mround = rnd[0][home][away]/1000;
  int iround = atoi(rtk[DB_ROUND]);
  int mdate = rnd[0][home][away]%1000;
  int mday = mdate%50;
  int mmonth = mdate/50;
  hsc = res[0][home][away]/100;
  asc = res[0][home][away]%100;

  int mid = Mid(home, away, round, 50*month+day);

  sprintf(ldb[mid][DB_DATE], "%s", rtk[DB_DATE]);
  if (mround < 1 && round > 0) sprintf(ldb[mid][DB_ROUND], "%2d", round);
  if (force_round > 0) {
    sprintf(ldb[mid][DB_ROUND], "%2d", iround);
    sprintf(ldb[mid][DB_SCORE], "%2d", score);
  }
  stad = FindStadium(rtk[DB_VENUE]);
  if (stad<0) {
    if (strlen(rtk[DB_VENUE])>0 && rtk[DB_VENUE][0]!=' ') fprintf(stderr, "%sStadium %s not found.%s\n", thiyellow, rtk[DB_VENUE], tnormal);
  } else {
    sprintf(ldb[mid][DB_VENUE], "%6s", Loc->V[stad].mnem);
  }
  sprintf(ldb[mid][DB_ATTEND], "%6s", rtk[DB_ATTEND]);
  hco = FindCoach(rtk[DB_COACH1]);
  aco = FindCoach(rtk[DB_COACH2]);
  ref = FindReferee(rtk[DB_REF]);

  if (hco>=0) sprintf(ldb[mid][DB_COACH1], "%s", Co->P[hco].mnem); else sprintf(ldb[mid][DB_COACH1], "      ");
  if (aco>=0) sprintf(ldb[mid][DB_COACH2], "%s", Co->P[aco].mnem); else sprintf(ldb[mid][DB_COACH2], "      ");
  if (ref>=0) sprintf(ldb[mid][DB_REF],    "%s", Rf->P[ref].mnem);

  char *rpl = new char[256];
  char *rpn = new char[8];
  char *rpm = new char[4];
  for (int i=DB_ROSTER1; i<DB_ROSTER1+22; i++) {
    strncpy(rpl, rtk[i], 64);
    rpn = strtok(rpl, ":");
    rpm = strtok(NULL, ":");
    int p = FindPlayer(rpn);
    int pm = atoi(rpm);
    if (rpn[0]!=' ' && p<0) fprintf(stderr, "%sLineup name %s not found.%s\n", thiyellow, rpn, tnormal);
    sprintf(ldb[mid][i], "%6s:%-3d", (p>=0? Pl->P[p].mnem : " "), pm);
  }
  for (int i=DB_ROSTER2; i<DB_ROSTER2+22; i++) {
    strncpy(rpl, rtk[i], 64);
    rpn = strtok(rpl, ":");
    rpm = strtok(NULL, ":");
    int p = FindPlayer(rpn);
    int pm = atoi(rpm);
    if (rpn[0]!=' ' && p<0) fprintf(stderr, "%sLineup name %s not found.%s\n", thiyellow, rpn, tnormal);
    sprintf(ldb[mid][i], "%6s:%-3d", (p>=0? Pl->P[p].mnem : " "), pm);
  }

  nev = 0;
  while (etk[nev]!=NULL) nev++;
  for (int i=0; i<nev; i++) {
    int sk = strlen(etk[i])-1;
    while (etk[i][sk]>='0' && etk[i][sk]<='9') sk--;
    strncpy(rpl, etk[i], sk+2); rpl[sk] = 0;
    int p = FindPlayer(rpl);
    if (p<0) fprintf(stderr, "%sEvent name %s not found.%s\n", thiyellow, rpn, tnormal);
    else {
      char es = etk[i][sk];
      strncpy(rpm, etk[i] + sk + 1, 3); rpm[3] = 0;
      int evm = atoi(rpm);
      sprintf(edb[mid][i], "%6s%c%-3d", Pl->P[p].mnem, es, evm);
    }
  }

  printf("MID#: %d\n", mid);

  SaveDB(season);
  SaveEvents(season);

  return 0;
}
