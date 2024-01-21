#define _XOPEN_SOURCE_EXTENDED
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <locale.h>
#include <time.h>
#include <cursesw.h>
#include "listbox.hh"
#include "aliases.hh"
#include "db.h"
#include "catalog.hh"

#define UTF 0
#define ISO 1

#define EURO 1000
#define CTTY_ROM 39000
#define NUMROWS     40
#define ROSTER_SIZE   22

#define MAX_NAMES  20000
#define DB_ROWS     1000
#define DB_COLS      60
#define DB_CELL      60
#define DB_KCELL     40
#define EV_COLS      60

#define NCAT_NAME    2
#define NCAT_PREN    1

#define	COL_NR       1
#define	COL_DATE     2
#define COL_HOME     3
#define COL_AWAY     4
#define COL_RESULT   5
#define COL_COMP     6
#define COL_VENUE    7
#define COL_ATTEND   8
#define COL_ROSTER1  9
#define COL_COACH1  10
#define COL_ROSTER2 11
#define COL_COACH2  12
#define COL_REF     13
#define COL_SCORERS 14

#define CAT_ROWS    1000
//burdca,Carol,Burdan,00/00/1912,ROM,173, , ,3,3,0,-11

const int col_start[] = {0, 1,  5, 22, 38, 54, 59, 63, 70, 74, 81, 88, 95, 102, 109};
const int col_span[]  = {0, 3, 16, 15, 15,  4,  3,  6,  3,  6,  6,  6,  6,   6,   2};
const char blank100[] = "                                                                                                    ";
const char top100[]   = "+--------------------------------------------------------------------------------------------------+";
const char blank40[]  = "                                       ";
const char dot40[]    = ".......................................";
const char blank32[]  = "                               ";
const char dot32[]    = "...............................";
const char blank10[]  = "          ";
const char blank6[]   = "      ";

//const char *normal  = "\033[0m";
//const char *yellow  = "\033[33m";
//const char *hicyan    = "\033[36;1m";

const char*   month[] ={"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
char uchar[256][16];
char schar[256];
int	pos, tab, prev_pos, prev_tab;
int first, last, span;
int     row, col, field;
int     search_t, search_p;
int     search_t1, search_t2;
time_t  tt;
struct  tm* t;
std::wstring wstr;

Catalog *cat_pl;
Catalog *cat_co;
Catalog *cat_eco;
Catalog *cat_ref;
Catalog *cat_eref;
Catalog *cat_epl;
Locations *cat_ven;
Locations *cat_even;
Aliases **L;
Aliases **CL;

Listbox *lb_club;
Listbox *lb_country;
Listbox *lb_city;
Listbox *lb_venue;
Listbox *lb_eurocity;
Listbox *lb_eurovenue;
Listbox *lb_coach;
Listbox *lb_ref;
Listbox *lb_player;
Listbox *lb_event;
Listbox *lb_confirm;

char **club;
char **mnem;
char **ctty;
char **dir;
int  year;
int  NC, NT, NN;
int  NM, NKM, NEM, NNM;
int  euro, home, away;
char ssn[32];

int  nev, evp[EV_COLS], evm[EV_COLS], evt[EV_COLS];
char roster[2*ROSTER_SIZE][DB_CELL];
int  rid[2*ROSTER_SIZE];
int  mp[2*ROSTER_SIZE];

/* *************************************** */

char  db[DB_ROWS][DB_COLS][DB_CELL];
char edb[DB_ROWS][EV_COLS][DB_CELL];
char cat[CAT_ROWS][CAT_COLS][DB_CELL];
char cac[CAT_ROWS][CAT_COLS][DB_CELL];
const char *evsymb = "'`\"/   ";
int ncat, ncac;

char dbfilename[256], evfilename[256], cachename[256];
FILE *of;

//-----------------------------------
//  Global procedures
//-----------------------------------

void MapUtfChars() {
  for (int i=0; i<256; i++) uchar[i][0] = 0;
  strcpy(uchar['a'], "Äƒ");
  strcpy(uchar['q'], "Ã¢");
  strcpy(uchar['i'], "Ã®");
  strcpy(uchar['s'], "ÅŸ");
  strcpy(uchar['t'], "Å£");
  strcpy(uchar['S'], "Åž");
  strcpy(uchar['T'], "Å¢");
  strcpy(uchar['o'], "Ã¶");
  strcpy(uchar['u'], "Ã¼");
  strcpy(uchar['O'], "Ã–");
  strcpy(uchar['U'], "Ãœ");
  strcpy(uchar['c'], "Ä");
  strcpy(uchar['C'], "ÄŒ");
  strcpy(uchar['g'], "ÄŸ");
  strcpy(uchar['j'], "Ä±");
  strcpy(uchar['J'], "Ä°");
  strcpy(uchar['k'], "Ã§");
  strcpy(uchar['K'], "Ã‡");
  strcpy(uchar['v'], "Å¡");
  strcpy(uchar['V'], "Å ");
  strcpy(uchar['z'], "Å¾");
  strcpy(uchar['Z'], "Å½");

  for (int i=0; i<256; i++) schar[i] = 0;
  schar['a'] = 227;
  schar['q'] = 226;
  schar['i'] = 238;
  schar['s'] = 186;
  schar['t'] = 254;
  schar['A'] = 195;
  schar['Q'] = 194;
  schar['I'] = 206;
  schar['S'] = 170;
  schar['T'] = 222;
  schar['o'] = 246;
  schar['u'] = 252;
  schar['c'] = 232;
  schar['C'] = 200;
  schar['v'] = 185;
  schar['V'] = 169;
  schar['z'] = 190;
  schar['Z'] = 174;
  schar[39]  = 230;
}

const wchar_t *convert(char *s) {
  std::wstring ws;
  if (s!=NULL && strlen(s)>0) {
    for (int i=0; i<strlen(s); i++) {
      ws += wchar_t(s[i]);
    }
  }
  const wchar_t *wct = ws.c_str();
  return wct;
}

void Init() {
  int i;
  char s[100];
	/*
  t = new tm;
  time(&tt);
  t = localtime(&tt);

  int curr_year    = t->tm_year + 1900;
  int curr_month   = t->tm_mon  + 1;
  int curr_day     = t->tm_mday;
	*/

	cat_pl   = new Catalog();
	cat_epl  = new Catalog();
	cat_co   = new Catalog();
	cat_eco  = new Catalog();
	cat_ref  = new Catalog();
	cat_eref = new Catalog();
	cat_ven  = new Locations();
	cat_even = new Locations();

  lb_club = new Listbox();
  lb_country = new Listbox();
  lb_city = new Listbox();
  lb_venue = new Listbox();
  lb_eurocity = new Listbox();
  lb_eurovenue = new Listbox();
  lb_coach = new Listbox();
  lb_ref = new Listbox();
  lb_player = new Listbox();
  lb_event = new Listbox();
	lb_confirm = new Listbox();
	lb_confirm->AddItem("No");
	lb_confirm->AddItem("Yes");
	NM = NKM = NEM = NNM = 0;
	span = NUMROWS;
	last = NUMROWS;
	field = 0;
}

int Load() {
  FILE *f;
  char s[2000], *tok[20], *ystr, *name, *nick;

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
    mnem[i] = new char[48];
    club[i] = new char[48];
    strncpy(mnem[i], strtok(s, ","), 32);
    strncpy(club[i], strtok(NULL, ",\n"), 32);
  }
  fclose(f);

  f = fopen("catalogs/country.dat", "rt");
  if (!f) return 0;
	fscanf(f, "%d\n", &NN);
	ctty = new char*[NN];
	dir = new char*[NN];
  CL = new Aliases*[NN];
  for (int i=0; i<NN; i++) CL[i] = new Aliases;
  for (int i=0; i<NN; i++) {
    if (feof(f)) continue;
    fgets(s, 2000, f);
    s[strlen(s)-1] = 0;
    tok[0] = strtok(s, ":");
		dir[i] = strdup(tok[0]);
    tok[1] = strtok(NULL, "*");
    for  (int j=2; j<20; ++j) tok[j] = strtok(NULL, "*");
    int k = 2;
    while(tok[k]) {
      ystr = strtok(tok[k], " ");
      name = strtok(NULL, "~");
      nick = strtok(NULL, "@");
      int y = atoi(ystr);
      alias *a = new alias(y, name, name);
      CL[i]->Append(a);
      k++;
    }
    ctty[i] = strdup(name);
    s[0] = 0;
  }
  fclose(f);

	LoadAliases(L, NC, "data/alias.dat");
	cat_pl->Load("catalogs/players.dat");
	cat_epl->Load("catalogs/europlayers.dat");
	cat_co->Load("catalogs/coaches.dat");
	cat_eco->Load("catalogs/eurocoaches.dat");
	cat_ref->Load("catalogs/referees.dat");
	cat_eref->Load("catalogs/euroreferees.dat");
	cat_ven->Load("catalogs/city.dat", "catalogs/stadium.dat");
	cat_even->Load("catalogs/eurocity.dat", "catalogs/eurostadium.dat");
  return 1;
}

char *EuroName(int t, int y) {
	int ct = t/1000;
	int cl = t%1000;
	if (cl==0) return CL[ct]->GetName(y);
	char filename[128];
	char *name = new char[64];
	char s[128];
	sprintf(name, "%d/%d", ct, cl);
	sprintf(filename, "/home/radu/rsssf/sat/%s/euroteams.dat", dir[ct]);
	FILE *f = fopen(filename, "rt");
	if (!f) return name;
	for (int i=0; i<=cl; ++i) fgets(s, 128, f);
	strcpy(name, s);
	name[15] = 0;
	int k = 14; while (name[k]==' ') name[k--]=0;
	fclose(f);
	return name;
}

char *NameOf(Aliases **L, int t, int y) {
	if (t>NC) return EuroName(t, y);
  char *s = L[t]->GetName(y);
  if (!s) return club[t];
  return s;
}

char *NickOf(Aliases **L, int t, int y) {
	if (t>NC) return EuroName(t, y);
  char *s = L[t]->GetNick(y);
  if (!s) return mnem[t];
  return s;
}

int LoadDB(char *filename) {
  char s[5000], *tk[DB_COLS];
  FILE *f;
  f = fopen(filename, "rt");
  if (f==NULL) { fprintf(stderr, "ERROR: database %s not found.\n", filename); return 0; }
  int i = 0;
  while (!feof(f)) {
    fgets(s, 5000, f);
    if (strlen(s)<5) continue;
    tk[0] = strtok(s, ",\n");
    for (int j=1; j<DB_COLS; j++) tk[j]=strtok(NULL, ",\n");
		if (tk[DB_DATE][0]==' ') tk[DB_DATE]++;
    for (int j=0; j<DB_COLS; j++) {
      if (tk[j]!=NULL) strcpy(db[i][j], tk[j]);
      else strcpy(db[i][j], " ");
    }
    i++;
    s[0] = 0;
  }
  fclose(f);
  NM = i;
  if (NM<span) span = NM;
  return 1;
}

int LoadEvents(char *filename) {
  char s[5000], *tk[DB_COLS];
  FILE *f;
  f = fopen(filename, "rt");
  if (f==NULL) { fprintf(stderr, "ERROR: database %s not found.\n", filename); return 0; }
  for (int i=0; i<NM; i++) {
    fgets(s, 5000, f);
    tk[0] = strtok(s, ",\n");
    for (int j=1; j<EV_COLS; j++) tk[j]=strtok(NULL, ",\n");
    for (int j=0; j<EV_COLS; j++) {
      if (tk[j]!=NULL) strcpy(edb[i][j], tk[j]);
      else strcpy(edb[i][j], " ");
    }
  }
  fclose(f);
	return 1;
}

int LoadCatalog() {
//burdca,Carol,Burdan,00/00/1912,ROM,173, , ,3,3,0,-11
  char filename[64], s[5000], *tk[CAT_COLS];
  FILE *f;
  NT = 0;
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

int LoadCache(char *filename) {
//burdca,Carol,Burdan,00/00/1912,ROM,173, , ,3,3,0,-11
  char s[5000], *tk[CAT_COLS];
  FILE *f;
  f = fopen(filename, "rt");
  if (f==NULL) { fprintf(stderr, "ERROR: catalog %s not found.\n", filename); return 0; }
  int i = 0;
  while (!feof(f)) {
    fgets(s, 5000, f);
    if (strlen(s)<20) continue;
    tk[0] = strtok(s, ",\n");
    for (int j=1; j<CAT_COLS; j++) tk[j]=strtok(NULL, ",\n");
		for (int j=0; j<CAT_COLS; ++j) {
			if (tk[j]) strcpy(cac[i][j], tk[j]); else strcpy(cac[i][j], " ");
		}
    s[0] = 0;
    i++;
  }
  fclose(f);
	ncac = i;
//	printf("Loaded %d cache entries.\n", ncac);
	return 1;
}

void ResetEvents() {
  nev = 0;
  for (int i=0; i<EV_COLS; i++) { evp[i] = -1; evm[i] = -1; evt[i] = -1; }
}

void SaveDB(char *filename) {
  char oldfilename[64];
  char newfilename[64];
  sprintf(oldfilename, "%s.old", filename);
  sprintf(newfilename, "%s.new", filename);
  FILE *f = fopen(newfilename, "wt");
  if (f==NULL) { fprintf(stderr, "ERROR: database %s not found.\n", newfilename); return; }
  for (int i=0; i<NM; i++) {
    for (int j=0; j<DB_COLS; j++) fprintf(f, "%s,", db[i][j]);
    fprintf(f, "\n");
  }
  fclose(f);
  rename(filename, oldfilename);
  rename(newfilename, filename);
}

void SaveEvents(char *filename) {
  char oldfilename[64];
  char newfilename[64];
  sprintf(oldfilename, "%s.old", filename);
  sprintf(newfilename, "%s.new", filename);
  FILE *f = fopen(newfilename, "wt");
  if (f==NULL) { fprintf(stderr, "ERROR: database %s not found.\n", newfilename); return; }
  for (int i=0; i<NM; i++) {
    for (int j=0; j<EV_COLS; j++) fprintf(f, "%s,", edb[i][j]);
    fprintf(f, "\n");
  }
  fclose(f);
  rename(filename, oldfilename);
  rename(newfilename, filename);
}

void addUtfChars(char *s) {
  if (s==NULL) return;
  int l = strlen(s);
  int k = l-1;
  while (k>=0) {
    if (s[k] == 92) {
      unsigned char x = ((unsigned char) s[k+1]);
      if (uchar[x][0] != 0) {
        // 0123456
        // ab\ac0
        // abUTFc0
        // abUTc0
        int ls = strlen(uchar[x]);
        for (int j=k+2; j<l; j++) s[j+ls-2] = s[j];
        for (int j=0; j<ls; j++) s[k+j] = uchar[x][j];
        l = strlen(s);
      }
    }
    k--;
  }
}

void addIsoChars(char *s) {
  if (s==NULL) return;
  if (s==NULL) return;
  int l = strlen(s);
  int k = l-1;
  while (k>=0) {
    if (s[k] == 92) {
      unsigned char x = ((unsigned char) s[k+1]);
      if (schar[x] != 0) {
        s[k] = schar[x];
        for (int j=k+1; j<=l; j++) s[j] = s[j+1];
      }
    }
    k--;
  }
}


/* *************************************** */
int GetMinutes(int r, int pos) {
  char spi[128];
  strcpy(spi, db[r][pos]);
  char *tkn = strtok(spi, ":;");
  char *tkm = strtok(NULL, ":;");
  int min = 0;
  if (tkm) return strtol(tkm, NULL, 10);
	else return 0;
}

void print_header(int r, int i) {
  char s[60], t[60];
  int ls;
  move(r,1); clrtoeol();
  mvprintw(r, 1, "%3d", i+1);
  mvprintw(r, 4, "|%-16s", db[i][DB_DATE]);
	int home = strtol(db[i][DB_HOME], NULL, 10);
	int away = strtol(db[i][DB_AWAY], NULL, 10);
	int sc   = strtol(db[i][DB_SCORE], NULL, 10);
	int att  = strtol(db[i][DB_ATTEND], NULL, 10);
	int rnd  = strtol(db[i][DB_ROUND], NULL, 10);
	mvprintw(r, 21, "|%-16s-%-16s", NickOf(L, home, year), NickOf(L, away, year));
  sprintf(t, "%d-%d", sc/100, sc%100);
	mvprintw(r, 53, " %4s", t);
	mvprintw(r, 58, "|%c%2s", db[i][DB_COMP][0], db[i][DB_ROUND]);
	strncpy(t, db[i][DB_VENUE], 6);
	mvprintw(r, 62, "|%-6s", t);
	mvprintw(r, 69, "|%3d", att/1000);
}

void print_report(int i) {
  char s[60], t[60], *pni, *pmi;
  int ls;
	int col = 1;
	int phi, min;

	attrset(A_NORMAL);
	home  = strtol(db[i][DB_HOME], NULL, 10);
	away  = strtol(db[i][DB_AWAY], NULL, 10);
	int sc    = strtol(db[i][DB_SCORE], NULL, 10);
	int att   = strtol(db[i][DB_ATTEND], NULL, 10);
	int rnd   = strtol(db[i][DB_ROUND], NULL, 10);
	int rstad = cat_ven->FindVenue(db[i][DB_VENUE]);
	int estad = cat_even->FindVenue(db[i][DB_VENUE]);
	int stad  = (home<EURO || home==CTTY_ROM || home==CTTY_ROM? rstad : estad);
	for (int j=0; j<ROSTER_SIZE; ++j) {
		mp[j] = GetMinutes(i, DB_ROSTER1+j);
		mp[j+ROSTER_SIZE] = GetMinutes(i, DB_ROSTER2+j);
	}
	strncpy(t, db[i][DB_VENUE], 4); t[4] = 0;
 	int rcity = cat_ven->FindCity(t);
 	int ecity = cat_even->FindCity(t);
	int city = (home<EURO || home==CTTY_ROM || home==CTTY_ROM ? rcity : ecity);
  move(1,col+1); clrtoeol();
  mvprintw(1, col, "%s", top100); clrtoeol();
	move(2, col); clrtoeol();
	mvprintw(2, col+96, "%3d", i+1);
	mvprintw(2, col, "| CompetiÅ£ia: %s %s", db[i][DB_COMP], db[i][DB_ROUND]);
	mvprintw(3, col, "| Data: %s", db[i][DB_DATE]); clrtoeol();
		mvprintw(4, col, "| Stadion: %s, %s",
		(estad>=0?cat_even->V[estad].getName(year):(rstad>=0?cat_ven->V[rstad].getName(year):"")),
		(ecity>=0?cat_even->C[ecity].name:(rcity>=0?cat_ven->C[rcity].name:"")));
		clrtoeol();
	mvprintw(5, col, "| Spectatori: %s", db[i][DB_ATTEND]); clrtoeol();
	mvprintw(6, col, "| %s - %s %d-%d",
		NameOf(L, home, year), NameOf(L, away, year), sc/100, sc%100);
	clrtoeol();
	if (euro) {
		int ref = cat_eref->FindMnem(db[i][DB_REF]);
		if (ref>=0) sprintf(t, "%s %s (%s)", cat_eref->P[ref].pren, cat_eref->P[ref].name, cat_eref->P[ref].cty); 
		else strcpy(t, "  ");
	}
	else {
		int ref = cat_ref->FindMnem(db[i][DB_REF]);
		if (ref>=0) sprintf(t, "%s %s (%s)", cat_ref->P[ref].pren, cat_ref->P[ref].name, cat_ref->P[ref].pob); 
		else strcpy(t, "  ");
	}
	mvprintw(7, col, "| Arbitru central: %s", t); clrtoeol();
	mvprintw(8, col, "| FormaÅ£ii"); clrtoeol();
	mvprintw(20, col, "| Rezerve"); clrtoeol();
	for (int j=0; j<22; j++) {
		strcpy(t, db[i][DB_ROSTER1+j]);
		pni = strtok(t, ":");
		pmi = strtok(NULL, ":");
		phi = (home<EURO || home==CTTY_ROM? cat_pl->FindMnem(pni): cat_epl->FindMnem(pni));
		min = 90;
		if (pmi) min = strtol(pmi, NULL, 10);
		if (phi>=0) {
			if (home<EURO || home==CTTY_ROM)
				sprintf(s, "%s %s", cat_pl->P[phi].pren, cat_pl->P[phi].name);
			else
				sprintf(s, "%s %s", cat_epl->P[phi].pren, cat_epl->P[phi].name);
		}
		else strcpy(s, t);
		s[35] = 0;	/* truncate */
        int row = 9+j+(j/11);
		mvprintw(row, col, "|%4d.%-35s ", j+1, s);
		if (j<11 && min>0 && min!=90) attron(COLOR_PAIR(2));
		mvprintw(row, col+41, "%d", min);
		attrset(A_NORMAL);
		clrtoeol();

		strcpy(t, db[i][DB_ROSTER2+j]);
		pni = strtok(t, ":");
		pmi = strtok(NULL, ":");
		phi = (away<EURO || away==CTTY_ROM || away==CTTY_ROM? cat_pl->FindMnem(pni) : cat_epl->FindMnem(pni));
		min = 90;
		if (pmi) min = strtol(pmi, NULL, 10);
		if (phi>=0) {
			if (away<EURO || away==CTTY_ROM)
				sprintf(s, "%s %s", cat_pl->P[phi].pren, cat_pl->P[phi].name);
			else
				sprintf(s, "%s %s", cat_epl->P[phi].pren, cat_epl->P[phi].name);
		}
		else strcpy(s, t);
		s[40] = 0;
		mvprintw(row, col+51, "%2d.%-40s ", j+1, s);
		if (j<11 && min>0 && min!=90) attron(COLOR_PAIR(2));
		mvprintw(row, col+92, "%d", min);
		attrset(A_NORMAL);
	}

	int co1, co2;
	if (home<EURO || home==CTTY_ROM) {
		co1 = cat_co->FindMnem(db[i][DB_COACH1]);
		if (co1>=0) sprintf(s, "%s %s", cat_co->P[co1].pren, cat_co->P[co1].name);
		else strcpy(s, t);
	}
	else {
		co1 = cat_eco->FindMnem(db[i][DB_COACH1]);
		if (co1>=0) sprintf(s, "%s %s", cat_eco->P[co1].pren, cat_eco->P[co1].name);
		else strcpy(s, t);
	}
	mvprintw(32, col, "| Antrenor: %s", s); clrtoeol();

	if (away<EURO || away==CTTY_ROM) {
		co2 = cat_co->FindMnem(db[i][DB_COACH2]);
		if (co2>=0) sprintf(s, "%s %s", cat_co->P[co2].pren, cat_co->P[co2].name);
		else strcpy(s, t);
	}
	else {
		co2 = cat_eco->FindMnem(db[i][DB_COACH2]);
		if (co2>=0) sprintf(s, "%s %s", cat_eco->P[co2].pren, cat_eco->P[co2].name);
		else strcpy(s, t);
	}
	mvprintw(32, col+50, "Antrenor: %s", s);

  mvprintw(33, col, "| "); clrtoeol();
  mvprintw(34, col, "| "); clrtoeol();
  mvprintw(35, col, "| "); clrtoeol();
  mvprintw(36, col, "%s", top100);
	for (int i=2; i<35; i++) mvprintw(i, col+99, "|");
}


//=====================================
//    Display functions
//=====================================

void Row() {
  attrset(A_NORMAL);
  print_header(pos-first+1, pos);
}

void Blank() {
  attrset(A_NORMAL);
  move(NUMROWS+1,1); clrtoeol();
  move(NUMROWS+2,1); clrtoeol();
  move(NUMROWS+3,1); clrtoeol();
  move(NUMROWS+4,1); clrtoeol();
}

void Cursor() {
  attrset(A_NORMAL);
  move(NUMROWS+4,1); clrtoeol();
  mvprintw(NUMROWS+3,2,"DATA:[%d,%d,%d,%d]", NM, NKM, NEM, NNM);
  mvprintw(NUMROWS+4,2,"POS:(%2d)", pos);
}

void Bar() {
  attrset(A_NORMAL);
  move(NUMROWS+1,1); clrtoeol();
  move(NUMROWS+2,1); clrtoeol();
  move(NUMROWS+3,1); clrtoeol();
  mvprintw(NUMROWS+2, 48, "%s", "[G]oto  [S]kip/[B]  [D]elete  [C]hange [0]Save");
  mvprintw(NUMROWS+3, 48, "%s", "[Y]ear  [F]ind/[V]  [N]ew     [P]layer [Q]uit");
}

void Message(char *msg) {
   attrset(A_STANDOUT);
   mvprintw(NUMROWS+1,2,"%s", msg);
   attrset(A_NORMAL);
   clrtoeol();
}

void Draw() {
  erase();
  attrset(A_NORMAL);
  for (int i=first; i<first+span; i++) {
    print_header(i-first+1, i);
  }
  Bar();
  Cursor();
}

void SavePos() {
  if (first+pos-1 != prev_pos) {
    prev_pos = first+pos-1;
    prev_tab = col;
  }
}

void JumpTo(int gtl) {
  SavePos();
  first = gtl - NUMROWS/2;
	pos = gtl;
  if (first<0) {
     first = 0;
  }
  if (first+span>NM) {
    first = NM-span;
  }
  Draw();
}

void Cell() {
  char s[100], t[100];
  int l1, l2, ls;
  attron(COLOR_PAIR(1));
  int x = pos-first+1;
	print_header(x, pos);
  attrset(A_NORMAL);
}

//=====================================
//    Interactive navigation
//=====================================

int RosterId(char *s) {
	for (int i=0; i<2*ROSTER_SIZE; ++i) {
		if (strcmp(roster[i], s)==0) return i;
	}
	return -1;
}

void GetRoster(int r) {
	for (int i=0; i<ROSTER_SIZE; ++i) {
		strcpy(roster[i], db[r][DB_ROSTER1+i]);
		strcpy(roster[i+ROSTER_SIZE], db[r][DB_ROSTER2+i]);
		strtok(roster[i], ":");
		strtok(roster[i+ROSTER_SIZE], ":");
	}
}

void ChangeMinutes(int i, int j, int m) {
	char t[256], *pni, *pmi;
	int len, unknown;
	strcpy(t, db[i][j]);
	pni = strtok(t, ":");
	pmi = strtok(NULL, ":");
	unknown = 0;
	if (pni) {
		len = strlen(pni);
		unknown = (pni[0]=='~');
	}
	sprintf(db[i][j]+(unknown?len+1:7), "%3d", m);
}

void ExtraMinutes(int i, int j, int sub) {
	char t[256], *pni, *pmi;
	int min, len, unknown;
	strcpy(t, db[i][j]);
	pni = strtok(t, ":");
	pmi = strtok(NULL, ":");
	unknown = 0;
	if (pni) {
		len = strlen(pni);
		unknown = (pni[0]=='~');
	}
	if (pmi) min = strtol(pmi, NULL, 10); else min = 0;
	int match120 = 0;
	int tm = (j>=DB_ROSTER2 ? 1 : 0);
	for (int k=0; k<11; ++k) {
		if (mp[k+tm*ROSTER_SIZE] + min == 120) match120 = 1;
	}
	if (min==90) min=120;
	else if (sub && min>0 && !match120) min += 30;
	if (min>0) sprintf(db[i][j]+(unknown?len+1:7), "%3d", min);
}

void ExtraTime(int i) {
	for (int j=0; j<22; j++) {
		ExtraMinutes(i, DB_ROSTER1+j, j>=11);
		ExtraMinutes(i, DB_ROSTER2+j, j>=11);
	}
}

void FromCatalog(char *s, int tid, int *opt, int &nopt) {
	char cap[128];
	strncpy(cap, s, 127);
	if (cap[0]=='~') {
		if (cap[1]>='a' && cap[1]<='z') cap[1] -= 32;
	}
	else if (cap[0]>='a' && cap[0]<='z') cap[0] -= 32;
	else if (cap[0]=='º') cap[0] = 'ª';
	else if (cap[0]=='þ') cap[0] = 'Þ';
	nopt = 0;
	for (int i=0; i<ncat; i++) {
		int t = strtol(cat[i][CAT_CLUB], NULL, 10);
		if (t==tid) {
			if (strstr(cat[i][NCAT_NAME], cap)!=NULL) {
				int x = cat_pl->FindMnem(cat[i][CAT_MNEM]);
				if (x>=0) opt[nopt++] = x;
			}
		}
	}
}

void FromCache(char *s, int tid, int *opt, int &nopt) {
	if (s[0]=='~') {
		if (s[1]>='a' && s[1]<='z') s[1] -= 32;
	}
	else if (s[0]>='a' && s[0]<='z') s[0] -= 32;
	else if (s[0]=='º') s[0] = 'ª';
	else if (s[0]=='þ') s[0] = 'Þ';
	nopt = 0;
	for (int i=0; i<ncac; i++) {
		if (strstr(cac[i][2], s)!=NULL) {
			int x = cat_pl->FindMnem(cac[i][CAT_MNEM]);
			if (x>=0) opt[nopt++] = x;
		}
	}
}

void FromForeign(char *s, int *opt, int &nopt) {
	if (s[0]>='a' && s[0]<='z') s[0] -= 32;
	nopt = 0;
	int nep = cat_epl->Size();
	for (int i=0; i<nep; ++i) {
			if (strstr(cat_epl->P[i].name, s)!=NULL) {
				opt[nopt++] = i;
			}
	}
}

int BestMatch(char *sp, char *sn, int tid) {
	for (int i=0; i<ncat; i++) {
		int t = strtol(cat[i][CAT_CLUB], NULL, 10);
		if (t==tid) {
            if (cat[i][NCAT_NAME][0] >= 'A' && cat[i][NCAT_NAME][0] <= 'Z' && sn[0] >= 'a' && sn[0] <= 'z') sn[0] -= 32;
            if (cat[i][NCAT_NAME][0] >= 'a' && cat[i][NCAT_NAME][0] <= 'z' && sn[0] >= 'A' && sn[0] <= 'Z') sn[0] += 32;
			if (strstr(cat[i][NCAT_NAME], sn)==cat[i][NCAT_NAME] &&	strstr(cat[i][NCAT_PREN], sp)!=NULL) {
				int x = cat_pl->FindMnem(cat[i][CAT_MNEM]);
				return x;
			}
		}
	}
	for (int i=0; i<ncat; i++) {
		int t = strtol(cat[i][CAT_CLUB], NULL, 10);
		if (t==tid) {
			if (strstr(cat[i][NCAT_NAME], sn)==cat[i][NCAT_NAME] &&	strstr(cat[i][NCAT_PREN], sp)!=NULL) {
				int x = cat_pl->FindMnem(cat[i][CAT_MNEM]);
				return x;
			}
		}
	}
	return -1;
}

int findTeams(char *stms) {
  for (int i=0; i<strlen(stms); i++) if (stms[i]==',') stms[i]=' ';
  char *st1 = strtok(stms, "-");
  char *st2 = strtok(NULL, "-");
  if (st1 == NULL) return -1;
  if (st1[0]>='a' && st1[0]<='z') st1[0] -= 32;
  if (st2!=NULL) { if (st2[0]>='a' && st2[0]<='z') st2[0] -= 32; }
  int m1 = -1;
  int m2 = -1;
  for (int m=0; m<NM; m++) {
	int home = strtol(db[m][DB_HOME], NULL, 10);
	int away = strtol(db[m][DB_AWAY], NULL, 10);
    if (strstr(NickOf(L, home, year), st1)!=NULL) {
      if (m1<0) m1 = m;
      if (st2!=NULL) {
        if (strstr(NickOf(L, away, year), st2)!=NULL) {
          if (m2<0) {
            m2 = m;
          }
        }
      }
    }
  }
  if (m2 >= 0) return m2;
  return m1;
}


#define NUM_PARTICLES 24

const char *particle[] = {
  "Van", "van", "Von", "von", "Al", "El", "Della", "Dal", "Da", "De",
  "Di",  "Do",  "da",  "de",  "di", "do", "du",    "Dos", "Mc", "Le",
  "le",  "Ben", "Del", "del"};

void modify(char *s, int encoding=UTF) {
	if (!s) return;
	int len = strlen(s);
	for (int i=0; i<len; ++i) {
		if (s[i]=='_') s[i] = ' ';
	}
    if (encoding==UTF) {
      addUtfChars(s);
    } else {
      addIsoChars(s);
    }
}

void tolower(char *s) {
	if (!s) return;
	int len = strlen(s);
	if (s[0]==34) {
		for (int i=0; i<len; ++i) s[i] = s[i+1];
		len--;
	}
	for (int i=1; i<len; ++i) {
		if (s[i]>='A' && s[i]<='Z') s[i] += 32;
		else if (s[i]=='Á' || s[i]=='É' || s[i]=='Í' || s[i]=='Õ' || s[i]=='Ú') s[i]+=32;
	}
	if (s[len-1]=='"') { s[len-1] = 0; len--; }
}

int is_capitalized(char *s) {
	if (!s) return 0;
	int len = strlen(s);
	int num_cap = 0;
	for (int i=1; i<len; ++i) {
		if (s[i]>='A' && s[i]<='Z') num_cap++;
	}
	if (num_cap>len/2) return 1;
	else return 0;
}

void Extract(char *s, char *sp, char *sn) {
  int is_cap[10], num_cap, n;
  char tok[512], *tkpi[10];
  int partic;

  strncpy(tok, s, 512);
  tkpi[0] = strtok(tok, " ");
//    if (tkpi[0]) {
//      while ((tkpi[0][0]>='0' && tkpi[0][0]<='9') || tkpi[0][0]=='.') tkpi[0] = tkpi[0]+1;
//    }
    for (int j=1; j<10; ++j) tkpi[j] = strtok(NULL, " ");
    num_cap = 0;
    n = 0;
    while (tkpi[n]) {
      if (is_capitalized(tkpi[n])) {
        is_cap[n] = 1;
        num_cap++;
      }
      else is_cap[n] = 0;
      tolower(tkpi[n]);
      n++;
    }

    if (n>0) strncpy(sn, tkpi[n-1], 64);
    strcpy(sp, " ");
    if (num_cap>0) {
      sn[0] = 0;
      sp[0] = 0;
      int nl = 0;
      int np = 0;
      for (int i=0; i<n; ++i) {
        if (is_cap[i]) {
          if (nl>0) strcat(sn, " ");
          strcat(sn, tkpi[i]);
          nl++;
        }
        else {
          if (np>0) strcat(sp, " ");
          strcat(sp, tkpi[i]);
          np++;
        }
      }
    }
    else {
      partic = 0;
      if (n>1) strcpy(sp, tkpi[0]);
      for (int j=1; j<n-2; ++j) {
        strcat(sp, " ");
        strcat(sp, tkpi[j]);
      }
      if (n>=2) {
        for (int i=0; i<NUM_PARTICLES; ++i) {
          if (strcmp(tkpi[n-2], particle[i])==0) {
            sprintf(sn, "%s %s", tkpi[n-2], tkpi[n-1]);
            partic = 1;
          }
        }
        if (partic==0 && n>2) {
          strcat(sp, " ");
          strcat(sp, tkpi[n-2]);
        }
      }
    }
}

int BestMatchForeign(char *sp, char *sn, char *sd) {
	int bm = -1;
	int maxf = -1;
	int nep = cat_epl->Size();
	for (int i=0; i<nep; ++i) {
		int f = cat_epl->P[i].Fits(sn, sp, sd);
		if (f==100) return i;
		if (f>70 && f>maxf) {
			maxf = f;
			bm = i;
		}
	}
	return bm;
}

void rtrim(char *s) {
  if (s==NULL) return;
  int l = strlen(s);
  if (l==0) return;
  int k = l-1;
  while (k>0 && s[k] == ' ') { s[k] = 0; k--; }
}

void GetLineup(int r, char *slin, int haw) {
	char s[512], *tok[50];
	char *tks, *tkt, *tkr, *tkf, *tkl, *tkc, *tkm, *tkp, *tkpi[10], *tkn, *tksp, *tkfn[5], *tkln[5];
	char buf[512], sp, sn;
	int nr, ntr, npr, pos, min, ntkln;
  int tid = strtol(db[r][DB_HOME], NULL, 10);
  if (!haw) tid = strtol(db[r][DB_AWAY], NULL, 10);
	strcpy(s, slin);
	tkt = strtok(s, ":â€“-");
	tkr = strtok(NULL, "\n");
    if (tkr==NULL) return;
    rtrim(tkr);
    if (tkr[strlen(tkr)-1] == '.') tkr[strlen(tkr)-1] = 0;
	tok[0] = strtok(tkr, ",;â€“-");
	for (int i=1; i<22; ++i) tok[i] = strtok(NULL, ",;-â€“");
	nr = 0; while (tok[nr]) nr++;
	ntr = nr;
	for (int i=0; i<nr; ++i) {
		char *spar = strchr(tok[i], '(');
		tks = NULL;
		if (spar) {
			strtok(tok[i], "(");
			tks = strtok(spar+1, ")");
		}
        if (strchr(tok[i], '.')) {
          char **tkfn = new char*[5];
          char **tkln = new char*[5];
          tkf = strtok(tok[i], ".");
          tkl = strtok(NULL, "\n");

          tkfn[0] = strtok(tkf, " ");
          for (int jj=1; jj<5; jj++) tkfn[jj] = strtok(NULL, " ");
          int nfn = 0; while (tkfn[nfn]!=NULL && nfn<5) nfn++;
          int fni = 0;
          while (tkfn[fni][0]>='0' && tkfn[fni][0]<='9' && fni<nfn-1) fni++;
          tkf = tkfn[fni];

          tkln[0] = strtok(tkl, " ");
          for (int jj=1; jj<5; jj++) tkln[jj] = strtok(NULL, " ");
          int nln = 0; while (tkln[nln]!=NULL && nln<5) nln++;
          int lni = nln - 1;
          while (tkln[lni][0]>='0' && tkln[lni][0]<='9' && lni>0) lni--;
          if (strstr(tkln[lni], "Substituted")!=NULL && lni>0) lni--;
          tkn = tkln[lni];
        }
        else {
          tkln[0] = strtok(tok[i], " ");
          for (int jj=1; jj<5; jj++) tkln[jj] = strtok(NULL, " ");
          ntkln = 0;
          for (int jj=0; jj<5; jj++) if (tkln[jj] != NULL) ntkln++;
          if (ntkln<0) continue;
          int lln = ntkln-1;
          tkn = tkln[lln];
          if (tkn==NULL) continue;
          while (tkn[0] == ' ') tkn++; rtrim(tkn);
          if (tkn[0]==39 && lln>0) { lln--; tkn = tkln[lln]; }
          if (strcmp(tkn, "I")==0 || strstr(tkn, "II")==tkn) {
            lln--;
            if (lln==0) {
              tkn = new char[128];
              sprintf(tkn, "%s %s", tkln[0], tkln[1]);
            } else {
              tkn = tkln[lln];
            }
          }
          while (tkln[lln][0]>='0' && tkln[lln][0]<='9' && lln>0) lln--;
          if (strstr(tkln[lln], "Substituted")!=NULL && lln>0) lln--;
          tkn = tkln[lln];
          tkf = new char[32]; tkf[0] = 0;
          if (ntkln>1 && lln>0) {
            int fni = 0;
            if (tkln[fni][0]>='0' && tkln[fni][0]<='9' && fni < lln-1) fni++;
            tkf = tkln[fni];
          }
        }
        while (tkf[0] == ' ') tkf++; rtrim(tkf);
        while (tkn[0] == ' ') tkn++; rtrim(tkn);
		int pid = BestMatch(tkf, tkn, tid);
		if (pid>=0) {
			pos = (haw?DB_ROSTER1+i:DB_ROSTER2+i);
			min = 91;
			if (tks) {
				strcpy(buf, tks);
				tkm = strtok(buf, " ");
				min = strtol(tkm, NULL, 10);
				if (min<=0) min = 46;
			}
			sprintf(db[r][pos], "%s:%-3d",cat_pl->P[pid].mnem, min-1);
		}
		if (tks && strlen(tks)>2) {
          min = 46;
          tksp = strtok(tks, ")");
          if (strchr(tksp, '.')) {
            tkf = strtok(tksp, ".");
            tkn = strtok(NULL, ".");
          } else {
            tkf = new char[2]; tkf[0] = 0;
            tkn = tksp;
          }
          while (tkf[0] == ' ') tkf++; rtrim(tkf);
          while (tkn[0] == ' ') tkn++; rtrim(tkn);
		  int sid = BestMatch(tkf, tkn, tid);
//		  if (sid<0) sid = BestMatch(tkpi[0], tkpi[npr-1], tid);
		  if (sid>=0) {
			pos = (haw?DB_ROSTER1+ntr:DB_ROSTER2+ntr);
			sprintf(db[r][pos], "%s:%-3d",cat_pl->P[sid].mnem, 91-min);
		  }
		  ntr++;
		}
	}
}

void GetForeignLineup(int r, char *slin, int haw) {
	char s[512], *tok[50];
	char *tks, *tkt, *tkl, *tkc, *tkm, *tkp, *tkpi[10], *tkn, *tkf;
	char buf[512], sp[128], sn[128], ty[12];
	int nr, ntr, npr, pos, min;
  int tid = strtol(db[r][DB_HOME], NULL, 10);
	sprintf(ty, "%d", year);
  if (!haw) tid = strtol(db[r][DB_AWAY], NULL, 10);
	strcpy(s, slin);
	tkt = strtok(s, ":");
	tkl = strtok(NULL, ".");
	tkc = strtok(NULL, ".\n");
	tok[0] = strtok(tkl, ",;.");
	for (int i=1; i<22; ++i) tok[i] = strtok(NULL, ",;.");
	nr = 0; while (tok[nr]) nr++;
	ntr = nr;
	for (int i=0; i<nr; ++i) {
		char *spar = strchr(tok[i], '(');
		tks = NULL;
		if (spar) {
			strtok(tok[i], "(");
			tks = strtok(spar+1, ")");
		}
		Extract(tok[i], sp, sn);
		tkpi[0] = strtok(tok[i], " ");
		for (int j=1; j<10; ++j) tkpi[j] = strtok(NULL, " ");
		int npr = 0; while (tkpi[npr]) npr++;
		int lni = npr-1;
		for (int j=0; j<npr; ++j) if (is_capitalized(tkpi[j])) lni=j;
		tkn = tkpi[lni];
		int fni = 0;
		if (fni==lni && fni<npr-1) fni++;
		tkf = tkpi[fni];
		tolower(tkn);
		tolower(tkf);
		//
		int pid = BestMatchForeign(sp, sn, ty);
		if (pid<0) pid = BestMatchForeign(tkpi[0], tkpi[npr-1], ty);
		if (pid>=0) {
			pos = (haw?DB_ROSTER1+i:DB_ROSTER2+i);
			min = 91;
			if (tks) {
				strcpy(buf, tks);
				tkm = strtok(buf, " ");
				min = strtol(tkm, NULL, 10);
				if (min<=0) min = 91;
			}
			sprintf(db[r][pos], "%s:%-3d",cat_epl->P[pid].mnem, min-1);
		}
		if (tks && strlen(tks)>10) {
			tkm = strtok(tks, " ");
			min = strtol(tkm, NULL, 10);
			tkn = strtok(NULL, ")");
			Extract(tkn, sp, sn);
			for (int j=0; j<10; ++j) tkpi[j] = strtok(NULL, " ");
			int npr = 0; while (tkpi[npr]) npr++;
			int lni = npr-1;
			for (int j=0; j<npr; ++j) if (is_capitalized(tkpi[j])) lni=j;
			tkn = tkpi[lni];
			int fni = 0;
			if (fni==lni && fni<npr-1) fni++;
			tkf = tkpi[fni];
			tolower(tkn);
			tolower(tkf);
			int sid = BestMatchForeign(sp, sn, ty);
			if (sid<0) sid = BestMatchForeign(tkpi[0], tkpi[npr-1], ty);
			if (sid>=0) {
				pos = (haw?DB_ROSTER1+ntr:DB_ROSTER2+ntr);
				sprintf(db[r][pos], "%s:%-3d",cat_epl->P[sid].mnem, 91-min);
			}
			ntr++;
		}
	}
}

void EditField(int fn, int i) {
	char s[256], u[256], ch, *pni, *pmi;
	int pick, sel, x, delta;
	int col = 1;
	int opt[10000], nopt, unknown, len;
	Listbox *Options = new Listbox();
  switch(fn) {
    case 0 :
			mvprintw(2, col+14, "   "); move(2, col+14);
            curs_set(1); scanw("%s", s); curs_set(0);
			strncpy(db[i][DB_COMP], s, DB_CELL);
      break;
		case 1:
			mvprintw(2, col+16, "   "); move(2, col+16);
            curs_set(1); scanw("%s", s); curs_set(0);
			strncpy(db[i][DB_ROUND], s, 3);
			break;
		case 2:
			mvprintw(3, col+8, "          "); move(3, col+8);
            curs_set(1); scanw("%s", s); curs_set(0);
			strncpy(db[i][DB_DATE], s, 10);
			break;
		case 3:
			mvprintw(3, col+19, "     "); move(3, col+19);
            curs_set(1); scanw("%s", s); curs_set(0);
			strncpy(db[i][DB_DATE]+11, s, 5);
			break;
		case 4:
			mvprintw(4, col+11, "      "); move(4, col+11);
            curs_set(1); scanw("%s", s); curs_set(0);
            modify(s, ISO);
			if (home>=EURO && home!=CTTY_ROM)
				cat_even->StartsWith(s, opt, nopt);
			else
				cat_ven->StartsWith(s, opt, nopt);
			Options->Clear();
			if (home>=EURO && home!=CTTY_ROM) {
				for (int j=0; j<nopt; j++) {
					sprintf(u, "%s - %s", cat_even->V[opt[j]].getName(year), 
						cat_even->C[cat_even->V[opt[j]].city].name);
					Options->AddItem(u);
				}
			}
			else {
				for (int j=0; j<nopt; j++) {
					sprintf(u, "%s - %s", cat_ven->V[opt[j]].getName(year), 
						cat_ven->C[cat_ven->V[opt[j]].city].name);
					Options->AddItem(u);
				}
			}
			x = Options->Select(4, col+11);
			if (home>=EURO && home!=CTTY_ROM)
				strcpy(db[i][DB_VENUE], cat_even->V[opt[x]].mnem);
			else
				strcpy(db[i][DB_VENUE], cat_ven->V[opt[x]].mnem);
			break;
		case 5:
			if (home>=EURO && home!=CTTY_ROM) {
				x = cat_even->FindVenue(db[i][DB_VENUE]);
				if (x>=0) delta = strlen(cat_even->V[x].getName(year));
				else delta = 0;
				mvprintw(4, col+delta+13, "    "); move(4, col+delta+13);
     	        curs_set(1); scanw("%s", s); curs_set(0);
                modify(s, ISO);
				x = cat_even->FindCity(s);
				if (x>=0) {
					sprintf(db[i][DB_VENUE], "%s  ", cat_even->C[x].mnem);
				}
			}
			else {
				x = cat_ven->FindVenue(db[i][DB_VENUE]);
				if (x>=0) delta = strlen(cat_ven->V[x].getName(year));
				else delta = 0;
				mvprintw(4, col+delta+13, "    "); move(4, col+delta+13);
       	        curs_set(1); scanw("%s", s); curs_set(0);
                modify(s, ISO);
				x = cat_ven->FindCity(s);
				if (x>=0) {
					sprintf(db[i][DB_VENUE], "%s  ", cat_ven->C[x].mnem);
				}
			}
			break;
		case 6:
			mvprintw(5, col+14, "      "); move(5, col+14);
            curs_set(1); scanw("%s", s); curs_set(0);
			strncpy(db[i][DB_ATTEND], s, 6);
			break;
/*
		case 7:
			mvprintw(6, col+2, "%s",	NameOf(L, home, year));
			break;
		case 8:
			delta = strlen(NameOf(L, home, year));
			mvprintw(6, col+delta+5, "%s",	NameOf(L, away, year));
			break;
*/
		case 9:
			delta = strlen(NameOf(L, home, year)) + strlen(NameOf(L, away, year));
			mvprintw(6, col+delta+6, "     "); move(6, col+delta+6);
            curs_set(1), scanw("%s", s); curs_set(0);
            strncpy(db[i][DB_SCORE], s, 6);
			break;
		case 10:
			mvprintw(7, col+19, "%30s", "   ");
			move(7, col+19);
            curs_set(1); scanw("%s", s); curs_set(0);
            modify(s);
			if (euro) {
				cat_eref->StartsWith(s, opt, nopt);
			}
			else {
				cat_ref->StartsWith(s, opt, nopt);
			}
			Options->Clear();
			for (int j=0; j<nopt; j++) {
				x = opt[j];
				if (euro)
					sprintf(u, "%s, %s (%s)", cat_eref->P[x].name, cat_eref->P[x].pren, cat_eref->P[x].cty);
				else
					sprintf(u, "%s, %s (%s)", cat_ref->P[x].name, cat_ref->P[x].pren, cat_ref->P[x].pob);
				Options->AddItem(u);
			}
			Options->AddItem(" *** ");;
			x = Options->Select(7, col+19);
			if (x<Options->Length()-1) {
				if (euro)
					strcpy(db[i][DB_REF], cat_eref->P[opt[x]].mnem);
				else
					strcpy(db[i][DB_REF], cat_ref->P[opt[x]].mnem);
			}
			else {
				strcpy(db[i][DB_REF], blank6);
			}
			break;
		case 33:
			mvprintw(32, col+12, "%30s", "   ");
			move(32, col+12);
            curs_set(1); scanw("%s", s); curs_set(0);
            modify(s, ISO);
			if (home<EURO || home==CTTY_ROM)
				cat_co->StartsWith(s, opt, nopt);
			else
				cat_eco->StartsWith(s, opt, nopt);
			Options->Clear();
			for (int j=0; j<nopt; j++) {
				x = opt[j];
				if (home<EURO || home==CTTY_ROM)
					sprintf(u, "%s, %s", cat_co->P[x].name, cat_co->P[x].pren);
				else
					sprintf(u, "%s, %s", cat_eco->P[x].name, cat_eco->P[x].pren);
				Options->AddItem(u);
			}
			Options->AddItem(" *** ");;
			x = Options->Select(20, col+12);

			if (x<Options->Length()-1) {
				if (home<EURO || home==CTTY_ROM)
					strcpy(db[i][DB_COACH1], cat_co->P[opt[x]].mnem);
				else
					strcpy(db[i][DB_COACH1], cat_eco->P[opt[x]].mnem);
			}
			else {
				strcpy(db[i][DB_COACH1], blank6);
			}
			break;
		case 56:
			mvprintw(32, col+60, "%30s", "   ");
			move(32, col+60);
            curs_set(1); scanw("%s", s); curs_set(0);
            modify(s, ISO);
			if (away<EURO || away==CTTY_ROM)
				cat_co->StartsWith(s, opt, nopt);
			else
				cat_eco->StartsWith(s, opt, nopt);
			Options->Clear();
			for (int j=0; j<nopt; j++) {
				x = opt[j];
				if (away<EURO || away==CTTY_ROM)
					sprintf(u, "%s, %s", cat_co->P[x].name, cat_co->P[x].pren);
				else
					sprintf(u, "%s, %s", cat_eco->P[x].name, cat_eco->P[x].pren);
				Options->AddItem(u);
			}
			Options->AddItem(" *** ");;
			x = Options->Select(20, col+55);
			if (x<Options->Length()-1) {
				if (away<EURO || away==CTTY_ROM)
					strcpy(db[i][DB_COACH2], cat_co->P[opt[x]].mnem);
				else
					strcpy(db[i][DB_COACH2], cat_eco->P[opt[x]].mnem);
			}
			else {
				strcpy(db[i][DB_COACH2], blank6);
			}
			break;
	}
	if (fn>=11 && fn<=32) {
		int j = fn-11;
		mvprintw(9+j+(j/11), col+6, "%35s", "   ");
		move(9+j+(j/11), col+6);
        curs_set(1); scanw("%s", s); curs_set(0);
		modify(s);
		pni = strtok(s, ":;");
		pmi = strtok(NULL, ":;");
		unknown = 0;
		if (pni) {
			if (strcmp(pni, "-")==0) {
				strcpy(u, db[i][DB_ROSTER1+j]);
				strtok(u, ":");
				len = strlen(u);
				unknown = (db[i][DB_ROSTER1+j][0]=='~');
			} else {
				len = strlen(pni);
				unknown = (pni[0]=='~');
			}
		}
		/* overwrite */
		if (strcmp(pni, "-")==0) {
			if (pmi) sprintf(db[i][DB_ROSTER1+j]+(unknown?len+1:7), "%s", pmi);
		}
		else {
		if (home>=EURO && home!=CTTY_ROM) {
				cat_epl->StartsWith(pni, opt, nopt);
				Options->Clear();
				for (int k=0; k<nopt; k++) {
					x = opt[k];
					sprintf(u, "%s, %s [%s]", cat_epl->P[x].name, cat_epl->P[x].pren, cat_epl->P[x].dob);
					Options->AddItem(u);
				}
				Options->AddItem("-");;
				x = Options->Select(9, col+6);
				if (x<Options->Length()-1) {
					sprintf(db[i][DB_ROSTER1+j], "%s:%s ",cat_epl->P[opt[x]].mnem, (pmi?pmi:(j<14?"90":"0 ")));
				}
				else {
					sprintf(db[i][DB_ROSTER1+j], "%s:%s ", pni, (pmi?pmi:(j<14?"90":"0 ")));
				}
		}
		else {
			if (home==CTTY_ROM)
				FromCache(pni, home, opt, nopt);
			else
				FromCatalog(pni, home, opt, nopt);
			Options->Clear();
			for (int k=0; k<nopt; k++) {
				x = opt[k];
				sprintf(u, "%s, %s [%s]", cat_pl->P[x].name, cat_pl->P[x].pren, cat_pl->P[x].dob);
				Options->AddItem(u);
			}
			Options->AddItem("--- Absent în catalog");
			Options->AddItem("-");;
			x = Options->Select(9, col+6);
			if (x<Options->Length()-2) {
				sprintf(db[i][DB_ROSTER1+j], "%s:%s ",cat_pl->P[opt[x]].mnem, (pmi?pmi:(j<14?"90":"0 ")));
			}
			else if (x==Options->Length()-1) {
				sprintf(db[i][DB_ROSTER1+j], "%s:%s ", pni, (pmi?pmi:"90"));
			}
			else {
				cat_pl->StartsWith(pni, opt, nopt);
				Options->Clear();
				for (int k=0; k<nopt; k++) {
					x = opt[k];
					sprintf(u, "%s, %s [%s]", cat_pl->P[x].name, cat_pl->P[x].pren, cat_pl->P[x].dob);
					Options->AddItem(u);
				}
				Options->AddItem("-");;
				x = Options->Select(9, col+6);
				if (x<Options->Length()-1) {
					sprintf(db[i][DB_ROSTER1+j], "%s:%s ",cat_pl->P[opt[x]].mnem, (pmi?pmi:(j<14?"90":"0 ")));
				}
				else {
					sprintf(db[i][DB_ROSTER1+j], "%s:%s ", pni, (pmi?pmi:(j<14?"90":"0 ")));
				}
			}
		}
		}
	}
	else if (fn>=34 && fn<=55) {
		int j = fn-34;
		mvprintw(9+j+(j/11), col+54, "%30s", "   ");
		move(9+j+(j/11), col+54);
        curs_set(1); scanw("%s", s); curs_set(0);
		modify(s);
		pni = strtok(s, ":;");
		pmi = strtok(NULL, ":;");
		unknown = 0;
		if (pni) {
			len = strlen(pni);
			unknown = (pni[0]=='~' || (pni[0]=='-' && db[i][DB_ROSTER2+j][0]=='~'));
		}
		if (pni && strcmp(pni, "-")==0) {
			strcpy(u, db[i][DB_ROSTER2+j]);
			strtok(u, ":");
			len = strlen(u);
			if (pmi) sprintf(db[i][DB_ROSTER2+j]+(unknown?len+1:7), "%s", pmi);
		}
		else {
			if (away>=EURO && away!=CTTY_ROM) {
				cat_epl->StartsWith(pni, opt, nopt);
				Options->Clear();
				for (int k=0; k<nopt; k++) {
					x = opt[k];
					sprintf(u, "%s, %s [%s]", cat_epl->P[x].name, cat_epl->P[x].pren, cat_epl->P[x].dob);
					Options->AddItem(u);
				}
				Options->AddItem("-");;
				x = Options->Select(9, col+40);
				if (x<Options->Length()-1) {
					sprintf(db[i][DB_ROSTER2+j], "%s:%s ",cat_epl->P[opt[x]].mnem, (pmi?pmi:(j<14?"90":"0 ")));
				}
				else {
					sprintf(db[i][DB_ROSTER2+j], "%s:%s ", pni, (pmi?pmi:(j<14?"90":"0 ")));
				}
		}
		else {
			if (away==CTTY_ROM)
				FromCache(pni, away, opt, nopt);
			else
				FromCatalog(pni, away, opt, nopt);
			Options->Clear();
			for (int k=0; k<nopt; k++) {
				x = opt[k];
				sprintf(u, "%s, %s [%s]", cat_pl->P[x].name, cat_pl->P[x].pren, cat_pl->P[x].dob);
				Options->AddItem(u);
			}
			Options->AddItem("--- Absent în catalog");
			Options->AddItem("-");;
			x = Options->Select(9, col+49);
			if (x<Options->Length()-2) {
				sprintf(db[i][DB_ROSTER2+j], "%s:%s ",cat_pl->P[opt[x]].mnem, (pmi?pmi:(j<14?"90":"0 ")));
			}
			else if (x==Options->Length()-1) {
				sprintf(db[i][DB_ROSTER2+j], "%s:%s ", pni, (pmi?pmi:(j<14?"90":"0 ")));
			}
			else {
				cat_pl->StartsWith(pni, opt, nopt);
				Options->Clear();
				for (int k=0; k<nopt; k++) {
					x = opt[k];
					sprintf(u, "%s, %s [%s]", cat_pl->P[x].name, cat_pl->P[x].pren, cat_pl->P[x].dob);
					Options->AddItem(u);
				}
				Options->AddItem("-");;
				x = Options->Select(9, col+49);
				if (x<Options->Length()-1) {
					sprintf(db[i][DB_ROSTER2+j], "%s:%s ",cat_pl->P[opt[x]].mnem, (pmi?pmi:(j<14?"90":"0 ")));
				}
				else {
					sprintf(db[i][DB_ROSTER2+j], "%s:%s ", pni, (pmi?pmi:"90"));
				}
			}
		}
		}
	}
}

void Field(int fn, int i) {
  char s[60], t[60], *pni, *pmi, *tkh;
  int ls;
	int col = 1;
	int ref, att;
	int stad, rstad, estad, city, rcity, ecity;
	int co1, co2, phi, min, delta;

	int sc = strtol(db[i][DB_SCORE], NULL, 10);

	switch(fn) {
		case 0:
			mvprintw(2, col+14, "%s", (strlen(db[i][DB_COMP])>0? db[i][DB_COMP] : "  "));
			break;
		case 1:
			mvprintw(2, col+16, "%s", (strlen(db[i][DB_ROUND])>0? db[i][DB_ROUND] : "  "));
			break;
		case 2:
			strcpy(t, db[i][DB_DATE]);
			strtok(t, "@");
			mvprintw(3, col+8, "%s", t);
			break;
		case 3:
			strcpy(t, db[i][DB_DATE]);
			strtok(t, "@");
			tkh = strtok(NULL, "@");
			mvprintw(3, col+19, "%s", (tkh?tkh:""));
			break;
		case 4:
			rstad = cat_ven->FindVenue(db[i][DB_VENUE]);
			estad = cat_even->FindVenue(db[i][DB_VENUE]);
			stad = (home<EURO || home==CTTY_ROM?rstad:estad);
			if (stad>=0) {
				if (home>=EURO && home!=CTTY_ROM)
					mvprintw(4, col+11, "%s,", cat_even->V[estad].getName(year));
				else
					mvprintw(4, col+11, "%s,", cat_ven->V[rstad].getName(year));
			}
			else mvprintw(4, col+11, "  ");
			break;
		case 5:
			rstad = cat_ven->FindVenue(db[i][DB_VENUE]);
			estad = cat_even->FindVenue(db[i][DB_VENUE]);
			stad = (home<EURO || home==CTTY_ROM?rstad:estad);
			if (stad>=0) {
				if (home>=EURO && home!=CTTY_ROM) delta = strlen(cat_even->V[estad].getName(year));
				else delta = strlen(cat_ven->V[rstad].getName(year));
			}
			else delta = 0;
			strncpy(t, db[i][DB_VENUE], 4); t[4] = 0;
			rcity = cat_ven->FindCity(t);
			ecity = cat_even->FindCity(t);
			city = (home>=EURO && home!=CTTY_ROM? ecity : rcity);
			if (city>=0) {
				if (home>=EURO && home!=CTTY_ROM) mvprintw(4, col+delta+13, "%s", cat_even->C[ecity].name);
				else mvprintw(4, col+delta+13, "%s", cat_ven->C[rcity].name);
			}
			else mvprintw(4, col+delta+13, "  ");
			break;
		case 6:
			mvprintw(5, col+14, "%s", (strlen(db[i][DB_ATTEND])>0? db[i][DB_ATTEND] : "  "));
			break;
		case 7:
			mvprintw(6, col+2, "%s", NameOf(L, home, year));
			break;
		case 8:
			delta = strlen(NameOf(L, home, year));
			mvprintw(6, col+delta+5, "%s", NameOf(L, away, year));
			break;
		case 9:
			delta = strlen(NameOf(L, home, year)) + strlen(NameOf(L, away, year));
			mvprintw(6, col+delta+6, "%d-%d",	sc/100, sc%100);
			break;
		case 10:
			if (euro) {
				ref = cat_eref->FindMnem(db[i][DB_REF]);
				if (ref>=0) sprintf(t, "%s %s (%s)", cat_eref->P[ref].pren, cat_eref->P[ref].name, cat_eref->P[ref].cty); 
				else strcpy(t, "  ");
			}
			else {
				ref = cat_ref->FindMnem(db[i][DB_REF]);
				if (ref>=0) sprintf(t, "%s %s (%s)", cat_ref->P[ref].pren, cat_ref->P[ref].name, cat_ref->P[ref].pob);
				else strcpy(t, "  ");
			}
			mvprintw(7, col+19, "%s", t);
			break;
		case 33:
			if (home<EURO || home==CTTY_ROM) {
				co1 = cat_co->FindMnem(db[i][DB_COACH1]);
				if (co1>=0) sprintf(s, "%s %s", cat_co->P[co1].pren, cat_co->P[co1].name);
				else strcpy(s, "  ");
			}
			else {
				co1 = cat_eco->FindMnem(db[i][DB_COACH1]);
				if (co1>=0) sprintf(s, "%s %s", cat_eco->P[co1].pren, cat_eco->P[co1].name);
				else strcpy(s, "  ");
			}
			mvprintw(32, col+12, "%s", s);
			break;
		case 56:
			if (away<EURO || away==CTTY_ROM) {
				co2 = cat_co->FindMnem(db[i][DB_COACH2]);
				if (co2>=0) sprintf(s, "%s %s", cat_co->P[co2].pren, cat_co->P[co2].name);
				else strcpy(s, t);
			}
			else {
				co2 = cat_eco->FindMnem(db[i][DB_COACH2]);
				if (co2>=0) sprintf(s, "%s %s", cat_eco->P[co2].pren, cat_eco->P[co2].name);
				else strcpy(s, t);
			}
			mvprintw(32, col+60, "%s", s);
			break;
	}
	if (fn>=11 && fn<=32) {
		int j = fn-11;
		strcpy(t, db[i][DB_ROSTER1+j]);
		pni = strtok(t, ":");
		pmi = strtok(NULL, ":");
		phi = (home<EURO || home==CTTY_ROM? cat_pl->FindMnem(pni): cat_epl->FindMnem(pni));
		min = 90;
		if (pmi) min = strtol(pmi, NULL, 10);
		if (phi>=0) {
			if (home<EURO || home==CTTY_ROM)
				sprintf(s, "%s %s", cat_pl->P[phi].pren, cat_pl->P[phi].name);
			else
				sprintf(s, "%s %s", cat_epl->P[phi].pren, cat_epl->P[phi].name);
		}
		else strcpy(s, t);
		mvprintw(9+j+(j/11), col+1, "%4d.%-35s %d", j+1, s, min);
	}
	else if (fn>=34 && fn<=55) {
		int j = fn-34;
		strcpy(t, db[i][DB_ROSTER2+j]);
		pni = strtok(t, ":");
		pmi = strtok(NULL, ":");
		phi = (away<EURO || away==CTTY_ROM? cat_pl->FindMnem(pni) : cat_epl->FindMnem(pni));
		min = 90;
		if (pmi) min = strtol(pmi, NULL, 10);
		if (phi>=0) {
			if (away<EURO || away==CTTY_ROM)
				sprintf(s, "%s %s", cat_pl->P[phi].pren, cat_pl->P[phi].name);
			else
				sprintf(s, "%s %s", cat_epl->P[phi].pren, cat_epl->P[phi].name);
		}
		else strcpy(s, t);
		s[40] = 0;
		mvprintw(9+j+(j/11), col+51, "%2d.%-40s %d", j+1, s, min);
	}
/*
  mvprintw(33, col, "%s", top100);
	for (int i=2; i<33; i++) mvprintw(i, col+99, "|");
*/
}

int EditReport(int i) {
	GetRoster(i);
	print_report(i);
	int quit = 0;
	int c, pout, pin, pmin, yes, haw, eurt;
	char slin[512], *slin1, *slin2;
  do {
    do {
			attron(COLOR_PAIR(1));
			Field(field, i);
      c = getch();
			print_report(i);
			if (c>='0' && c<='9') {
				field = c-48;
			}
      switch(c) {
				case 9: case '`':
					return c;
					break;
        case KEY_DOWN: // down
          if (field < 56) field++;
          break;
        case KEY_UP:
          if (field>0) field--;
          break;
        case KEY_HOME:
          field = 0;
          break;
        case KEY_END:
					field = 56;
          break;
        case KEY_RIGHT:
					if (field>10 && field<34) field += 23;
          break;
        case KEY_LEFT:
					if (field>33) field -= 23;
          break;
                case 'l': case 'L':
                    move(35, 0); clrtoeol();
                    move(36, 0); clrtoeol();
                    move(37, 0); clrtoeol();
                    move(38, 0); clrtoeol();
                    mvprintw(35,2, "Lineup: ");
                    getstr(slin);
                    slin1 = strtok(slin, "|\n");
                    slin2 = strtok(NULL, "|\n");
                    if (slin2 != NULL) {
                      GetLineup(i, slin1, 1);
                      GetLineup(i, slin2, 0);
                    } else {
                      haw = (field<=33);
                      eurt = 0;
                      if (haw) eurt = (home>=EURO && home!=CTTY_ROM); else eurt = (away>=EURO && away!=CTTY_ROM);
                      if (eurt) {
                          GetForeignLineup(i, slin, haw);
                      }
                      else {
                          GetLineup(i, slin, haw);
                      }
                    }
                    break;
				case 'r': case 'R':
					eurt = 0;
					move(35, 0); clrtoeol();
					move(36, 0); clrtoeol();
					move(37, 0); clrtoeol();
					move(38, 0); clrtoeol();
					mvprintw(35,2, "Lineups: ");
					getstr(slin);
					GetLineup(i, slin, 1);
					getstr(slin);
					GetLineup(i, slin, 0);
					break;
				case 'f': case 'F':
					if (field>=11 && field<=32) {
						ChangeMinutes(i, DB_ROSTER1+field-11, 90);
					}
					else if (field>=34 && field<=55) {
						ChangeMinutes(i, DB_ROSTER2+field-34, 90);
					}
					break;
				case 'k': case 'K':
					yes = lb_confirm->Select(20, 40);
					if (yes) {
						if (field>=11 && field<=32) {
							strcpy(db[i][DB_ROSTER1+field-11], "      :0  ");
						}
						else if (field>=34 && field<=55) {
							strcpy(db[i][DB_ROSTER2+field-34], "      :0  ");
						}
					}
					break;
				case 'h': case 'H':
					if (field>=11 && field<=32) {
						ChangeMinutes(i, DB_ROSTER1+field-11, 45);
					}
					else if (field>=34 && field<=55) {
						ChangeMinutes(i, DB_ROSTER2+field-34, 45);
					}
					break;
				case 'b': case 'B':
					mvprintw(34,2, "Schimbare [out] [in] [min]: "); scanw("%d %d %d", &pout, &pin, &pmin);
					if (pout>0 && pout<=11 && pin>11 && pin<=22) {
						ChangeMinutes(i, DB_T1+pout, pmin-1);
						ChangeMinutes(i, DB_T1+pin,  91-pmin);
					}
					if (pout>22 && pout<=33 && pin>33 && pin<=44) {
						ChangeMinutes(i, DB_T2+pout-22, pmin-1);
						ChangeMinutes(i, DB_T2+pin-22,  91-pmin);
					}
					move(34, 2); clrtoeol();
					refresh();
					break;
				case 'x': case 'X':
					ExtraTime(i);
					break;
        case 'c': case 'C':
					if (field<33) field = 33; else field = 56;
          break;
        case 'd': case 'D':
					field = 2;
          break;
        case 's': case 'S':
					field = 4;
          break;
        case 'o': case 'O':
					field = 5;
          break;
        case 'a': case 'A':
					field = 10;
          break;
        case '1':
					field = 11;
          break;
        case '2':
					field = 34;
          break;
				case 27: /* escape */
					quit = 1;
					break;
        default:
          break;
      }
    } while (c!=13 && !quit);
		if (quit) continue;
		EditField(field, i);
		if (field<56 && field!=10) field++;
		print_report(i);

	} while (!quit);
	return 0;
}

void Navigate() {
  first = 0;
  pos   = 0;
  int quit = 0;
  int c, gtl, rt;
  int i, j, w, t, p;
  char msg[128], s[128];

  Draw();
  Cell();
  do {
    do {
      c = getch();
      Row();
      switch(c) {
        case 's' : case 'S' : // save
          SaveDB(dbfilename);
          sprintf(msg, "Saved %d reports in %s", NM, dbfilename);
          Message(msg);
          break;
        case 'q': case 'Q': // quit
          //t = Confirm->Select(pos,60);
          //if (t) quit = 1;
					quit = 1;
          break;
        case 'g': case 'G': // goto
          int gtl;
          do {
            mvprintw(NUMROWS+2, 55, "Line: "); scanw("%d", &gtl);
          } while (gtl<0 || gtl>=NM);
          JumpTo(gtl-1);
          Draw();
        break;
        case 'f': case 'F': // find
        case 'm': case 'M': // match
          int gtt;
          mvprintw(NUMROWS+2, 55, "                                ");
          mvprintw(NUMROWS+2, 55, "Teams [t1-t2]: "); scanw("%s", s);
          gtt = findTeams(s);
          if (gtt > 0) JumpTo(gtt);
          Draw();
        break;
        // ---------------------------------
        // ---  navigation arrows
        // ---------------------------------
        case KEY_DOWN: // down
          if (pos-first+1 < span) pos++;
          else {
            if (first+span<NM) { JumpTo(first+span); Draw(); }
          }
          Cursor();
          break;
        case KEY_UP:
          if (pos>first) pos--;
          else {
            if (first>0) { first--; pos--; Draw(); }
          }
          Cursor();
          break;
        case KEY_PPAGE:
          rt = first - span;
					pos = pos - span;
          if (rt < 0) {
            first = 0;
						pos = 0;
          }
          else first = rt;
          Draw();
          break;
        case KEY_HOME:
          SavePos();
          first = 0;
          pos = 0;
          Draw();
          break;
        case KEY_NPAGE:
          rt = first + span;
          pos = pos + span;
          if (rt >= NM-span) {
            first = NM - span;
            if (pos >= NM - 1) pos = NM-1;
          }
          else first = rt;
          Draw();
          break;
        case KEY_END:
          SavePos();
          first = NM - span;
          pos = NM-1;
          Draw();
          break;
/*
        case KEY_F(12) : case '^' :
          first = pos;
          if (first+span>NM) first = NM-span;
          pos = 0;
          Draw();
          break;
        case KEY_BACKSPACE : case 9 :
          gtl = prev_pos;
          first = gtl - 20; pos = 21;
          if (first<0) {first = 0; pos = gtl+1;}
          if (first+span>NM) {first = NM-span; pos = gtl-first+1; }
          Draw();
          break;
*/
        default:
          break;
      }
      Cell();
    } while (c!=13 && !quit);


    if (quit) continue;
		int exitcode = 0;
		while (exitcode = EditReport(pos)) {
			if (exitcode==9) pos = (pos+1) % NM;
			if (exitcode=='`') pos = (pos+NM-1) % NM;
		}
		Draw();

    //=====================================
    //    Edit the selected field
    //=====================================

  } while (!quit);

}

void End() {
}

//--------------------------------------------

int main(int argc, char** argv) {
	euro = 0;
	if (argc<2) {
		fprintf(stderr, "ERROR: no input file specified.\n");
		return 1;
	}
	else {
		sprintf(dbfilename, "%s", argv[1]);
		char s[256];
		strcpy(s, argv[1]);
		char *fbase = strtok(s, ".");
		char *ystr = fbase+strlen(fbase)-4;
		year = strtol(ystr, NULL, 10);
   if (argc>2) {
		sprintf(evfilename, "%s", argv[2]);
   } else {
		sprintf(evfilename, "db/events-%d.db", year);
   }
		if (strstr(dbfilename, "cup-")!=NULL) {
			sprintf(evfilename, "db/cup-events-%d.db", year);
		}
		if (strstr(dbfilename, "euro-")!=NULL) {
			euro = 1;
			sprintf(evfilename, "db/euro-events-%d.db", year);
		}
		if (strstr(dbfilename, "nat-")!=NULL) {
			euro = 1;
			sprintf(evfilename, "db/nat-events-%d.db", year);
		}
	}
  MapUtfChars();

  setlocale(LC_ALL, "");
  setlocale(LC_CTYPE, "");

  Init();
  if (!Load()) { return 2; }
	if (!LoadDB(dbfilename)) { return 3; }
	if (!LoadEvents(evfilename)) { return 4; }
	/*if (!euro)*/ if (!LoadCatalog()) { /*return 5; */}
	sprintf(cachename, "catalogs/nat-%d.dat", year);
	if (!LoadCache(cachename)) { /*return 6; */}

  initscr();
  cbreak();
  nonl();
  echo();
  intrflush(stdscr, FALSE);
  keypad(stdscr, TRUE);
  curs_set(0);
  start_color();
  use_default_colors();

//  attrset(COLOR_PAIR(2));
  init_pair(1, COLOR_WHITE, COLOR_BLUE);
  init_pair(2, COLOR_CYAN, COLOR_BLACK);
  attron(COLOR_PAIR(1));

  Navigate();
  endwin();
  End();
  return 0;
}

