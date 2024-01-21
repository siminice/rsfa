#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "catalog.hh"
#include "aliases.hh"
#include "db.h"

#define CTTY_ROM 39000

#define MAX_LEVELS  12
#define MAX_RR	     4
#define MAX_N       64
#define MAX_TEAMS   2000
#define MAX_ROSTER	 100
#define ROSTER_SIZE		22

#define MAX_NAMES  20000
#define DB_KROWS      60
#define DB_KCOLS      60
#define DB_KCELL      40
#define EV_COLS				60
#define PL_INITIAL	   0
#define PL_FULL_NAME 	 1

#define EV_GOAL		 0
#define EV_OWNGOAL	 1
#define EV_PKGOAL	 2
#define EV_PKMISS	 3
#define EV_YELLOW	 4
#define EV_RED	 	 5
#define EV_YELLOWRED	 6
#define OPT_UNDO	-1
#define OPT_EVENT	 1
#define OPT_DONE	 0
#define OPT_SAME	 8
#define OPT_SAVE	 9
#define OPT_QUIT	10

char **club;
char **mnem;
char **ctty;
char **dir;
int  NC, NM, NT, NN, NP;
char ssn[32];
int  year, score, home, away;

const char *enormal  = "\033[0m";
const char *eyellow  = "\033[33m";
const char *ehicyan    = "\033[36;1m";

int  nev, evp[EV_COLS], evm[EV_COLS], evt[EV_COLS];
char roster[2*ROSTER_SIZE][DB_KCELL];
int  rid[2*ROSTER_SIZE];

/* *************************************** */

char  db[DB_KROWS][DB_KCOLS][DB_KCELL];
char edb[DB_KROWS][EV_COLS][DB_KCELL];
const char *evsymb = "'`\"/   ";

Catalog Pl, Epl, Ant, Eant, Arb;
Locations Loc, Eloc;

FILE *of;
char sf[128];
Aliases **L;

//--------------------------------------

int Load() {
  FILE *f;
  char s[2000], *tok[20], *ystr, *name, *nick;

  f = fopen("country.dat", "rt");
  if (!f) {
    fprintf(stderr, "ERROR: file 'country.dat' not found.\n");
    return 0;
  }
  fscanf(f, "%d\n", &NN);
  L = new Aliases*[NN];
  for (int i=0; i<NN; i++) L[i] = new Aliases;
  ctty = new char*[NN];
  dir = new char*[NN];
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
      L[i]->Append(a);
      k++;
    }
    ctty[i] = strdup(name);
    s[0] = 0;
  }
  fclose(f);
	return 1;
}

char *NameOf(Aliases **L, int t, int y) {
  char *s = L[t]->GetName(y);
  return s;
}

char *NickOf(Aliases **L, int t, int y) {
  char *s = L[t]->GetNick(y);
  return s;
}

void LoadDB() {
  char filename[64], s[5000], *tk[DB_KCOLS];
  FILE *f;
  sprintf(filename, "nat-lineups-%d.db", year);
  f = fopen(filename, "rt");
  if (f==NULL) { fprintf(stderr, "ERROR: database %s not found.\n", filename); return; }
	int i = 0;
  while (!feof(f)) {
    fgets(s, 5000, f);
		if (strlen(s)<100) continue;
    tk[0] = strtok(s, ",\n");
    for (int j=1; j<DB_KCOLS; j++) tk[j]=strtok(NULL, ",\n");
    for (int j=0; j<DB_KCOLS; j++) {
      if (tk[j]!=NULL) strcpy(db[i][j], tk[j]);
      else strcpy(db[i][j], " ");
    }
    int a = atoi(db[i][DB_HOME]);
    int b = atoi(db[i][DB_AWAY]);
		i++;
		s[0] = 0;
  }
  fclose(f);
	NM = i;
}

void LoadEvents() {
  char filename[64], s[5000], *tk[DB_KCOLS];
  FILE *f;
  sprintf(filename, "nat-events-%d.db", year);
  f = fopen(filename, "rt");
  if (f==NULL) { fprintf(stderr, "ERROR: database %s not found.\n", filename); return; }
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
}

void ResetEvents() {
  nev = 0;
  for (int i=0; i<EV_COLS; i++) { evp[i] = -1; evm[i] = -1; evt[i] = -1; }
}

void GetEvents(int r) {
  int ep, em;
  char s[DB_KCELL], *sp, *sm;
  ResetEvents();
  for (int i=0; i<EV_COLS; i++) {
    strcpy(s, edb[r][i]);
    sp = strtok(s, "'`\"/,\n");
    sm = strtok(NULL, "'`\"/,\n");
    if (sm!=NULL) em = atoi(sm); else em=-1;
    if (sp!=NULL) ep = Pl.FindMnem(sp); else ep=-1;
    if (sp!=NULL && ep<0) ep = Epl.FindMnem(sp); else ep=-1;
    if (ep>=0 || em>=0) {
      if (em==0) em=999;
      evp[i] = ep; evm[i] = em; evt[i] = EV_GOAL;
           if (edb[r][i][6]=='`') evt[i] = EV_OWNGOAL;
      else if (edb[r][i][6]=='"') evt[i] = EV_PKGOAL;
      else if (edb[r][i][6]=='/') evt[i] = EV_PKMISS;
      nev++;
    }
  }
}

void GetRoster(int r) {
	char sloc[128];
	for (int i=0; i<ROSTER_SIZE; i++) {
		strcpy(sloc, db[r][DB_ROSTER1+i]);
		strtok(sloc, ":");
		strcpy(roster[i], sloc);

		strcpy(sloc, db[r][DB_ROSTER2+i]);
		strtok(sloc, ":");
		strcpy(roster[i+ROSTER_SIZE], sloc);
	
		if (home==CTTY_ROM) {
			rid[i] = Pl.FindMnem(roster[i]);
		}
		else {
			rid[i] = 100000 + Epl.FindMnem(roster[i]);
		}
		if (away==CTTY_ROM) {
			rid[i+ROSTER_SIZE] = Pl.FindMnem(roster[i+ROSTER_SIZE]);
		}
		else {
			rid[i+ROSTER_SIZE] = 100000 + Epl.FindMnem(roster[i+ROSTER_SIZE]);
		}
	}
}

void SaveDB() {
  char oldfilename[64];
  char newfilename[64];
  char filename[64];
  sprintf(filename,    "nat-lineups-%d.db",  year);
  sprintf(oldfilename, "nat-lineups-%d.old", year);
  sprintf(newfilename, "nat-lineups-%d.new", year);
  FILE *f = fopen(newfilename, "wt");
  if (f==NULL) { fprintf(stderr, "ERROR: database %s not found.\n", newfilename); return; }
  for (int i=0; i<NM; i++) {
    for (int j=0; j<DB_KCOLS; j++) fprintf(f, "%s,", db[i][j]);
    fprintf(f, "\n");
  }
  fclose(f);
  rename(filename, oldfilename);
  rename(newfilename, filename);
}

void SaveEvents() {
  char oldfilename[64];
  char newfilename[64];
  char filename[64];
  sprintf(filename,    "nat-events-%d.db",  year);
  sprintf(oldfilename, "nat-events-%d.old", year);
  sprintf(newfilename, "nat-events-%d.new", year);
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

int parse(char s[], int &epi, int &emin) {
	int len = strlen(s);
	char *stok = s;
	if (len<1) return -2;
	char *eptr;
	epi = -1;
	emin = -1;
	if (s[0]=='0') return 0;
	if (s[0]=='S' || s[0]=='s') return OPT_SAVE;
	if (s[0]=='q' || s[0]=='Q') return OPT_QUIT;
	if (s[0]=='.') {
		emin = strtol(stok+1, NULL, 10);
		return OPT_SAME;
	}
	epi = strtol(stok, &eptr, 10);
	if (epi==-1) return OPT_UNDO;
	if (!eptr) { emin=0; return OPT_EVENT; }
	if (eptr[0]=='-'||eptr[0]==':'||eptr[0]==';'||eptr[0]=='=') {
		stok = eptr+1;
		emin = strtol(stok, &eptr, 10);
		if (!eptr[0]) return OPT_EVENT;
		if (eptr[0]=='p'||eptr[0]=='P') return OPT_EVENT+EV_PKGOAL;
		if (eptr[0]=='o'||eptr[0]=='a') return OPT_EVENT+EV_OWNGOAL;
		if (eptr[0]=='m'||eptr[0]=='a') return OPT_EVENT+EV_PKMISS;
	}
	return -2;
}

int AddScorers(int r) {
  char rfilename[128];
	home = atoi(db[r][DB_HOME]);
	away = atoi(db[r][DB_AWAY]);
	score = atoi(db[r][DB_SCORE]);
	printf("\n%s [#%d] *** %s - %s %d-%d%s\n", eyellow, r+1,
		NameOf(L, home/1000, year),
		NameOf(L, away/1000, year),
		score/100,  score%100, enormal);
	printf("---------------------\n");
	GetEvents(r);
	for (int i=0; i<nev; ++i) {
		if (strlen(edb[r][i])>5) printf("%s ", edb[r][i]);
	}
	printf("\n");
	printf("---------------------\n");
  GetRoster(r);

	char rhi[128], nhi[128],  rai[128], nai[128];
	for (int i=0; i<ROSTER_SIZE; i++) {
		int nvalid = 2;
		if (roster[i][0]==' ' || roster[i][0]==0) {
			sprintf(rhi, "%-36s", " ");
			nvalid--;
		}
		else if (rid[i]>=0) {
			if (home==CTTY_ROM) {
				sprintf(nhi, "%s %s", Pl.P[rid[i]].pren, Pl.P[rid[i]].name);
			}
			else {
				sprintf(nhi, "%s %s", Epl.P[rid[i]-100000].pren, Epl.P[rid[i]-100000].name);
			}
			sprintf(rhi, "%2d. %-32s", i+1, nhi);
		}
		else sprintf(rhi, "%2d. %s%-32s%s", i+1, ehicyan, roster[i], enormal);

		if (roster[i+ROSTER_SIZE][0]==' ' || roster[i+ROSTER_SIZE][0]==0) {
			sprintf(rai, "%-36s", " ");
			nvalid--;
		}
		else if (rid[i+ROSTER_SIZE]>=0) {
			if (away==CTTY_ROM) {
				sprintf(nai, "%s %s", Pl.P[rid[i+ROSTER_SIZE]].pren, Pl.P[rid[i+ROSTER_SIZE]].name);
			}
			else {
				sprintf(nai, "%s %s", Epl.P[rid[i+ROSTER_SIZE]-100000].pren, Epl.P[rid[i+ROSTER_SIZE]-100000].name);
			}
			sprintf(rai, "%2d. %-32s", i+ROSTER_SIZE+1, nai);
		}
		else sprintf(rai, "%2d. %s%-32s%s", i+ROSTER_SIZE+1, ehicyan, roster[i+ROSTER_SIZE], enormal);
		if (nvalid>0) printf("%s\t%s\n", rhi, rai);
	}

	printf("---------------------\n");
	char sopt[128];
	int opt, epi, etp, emin;
	int x = 0;
	int y = 0;
	int previ = -1;
	do {
		printf("[0] Done [num:<min_>] Event [.] Repeat [S] Save [Q] Quit [-1] Undo\n");
		scanf("%s", sopt);
		opt = parse(sopt, epi, emin);
		switch(opt) {
			case OPT_UNDO:
				if (nev>0) nev--;
				break;
			case OPT_EVENT:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
			case OPT_SAME:
				if (opt==OPT_SAME && previ>=0) {
					epi = previ;
					opt = OPT_EVENT;
				} else {
					while (epi<0 || epi>=2*ROSTER_SIZE) {
						printf("PID: ");
						scanf("%d", &epi);
					}
				}

				if (opt==OPT_EVENT || opt==OPT_EVENT+EV_PKGOAL) {
					if (epi<=ROSTER_SIZE) x++; else y++;
				}
				if (opt==OPT_EVENT+EV_OWNGOAL) {
					if (epi<=ROSTER_SIZE) y++; else x++;
				}

				if (rid[epi-1]<0) {
					printf("[%d-%d] %s %c", x, y, roster[epi-1], evsymb[opt-1]);
				}	else if (rid[epi-1]<100000) {
					printf("[%d-%d] %s %s %c", x, y, Pl.P[rid[epi-1]].pren, Pl.P[rid[epi-1]].name, evsymb[opt-1]);
				}
				else {
					printf("[%d-%d] %s %s %c", x, y, Epl.P[rid[epi-1]-100000].pren, Epl.P[rid[epi-1]-100000].name, evsymb[opt-1]);
				}

				if (emin<0) scanf("%d", &emin); else printf("%d\n", emin);
				if (emin==0) emin=999;
				evp[nev] = epi-1;
				evt[nev] = opt-1;
				evm[nev] = emin;
				sprintf(edb[r][nev], "%s%c%-3d", roster[epi-1], evsymb[opt-1], emin);
				nev++;
				break;
			case OPT_SAVE:
				SaveEvents();
				opt = OPT_DONE;
				break;
			case OPT_QUIT:
				return OPT_QUIT;
				break;
			default: break;
		}
		previ = epi;
	} while (opt!=OPT_DONE);
	for (int i=0; i<nev; i++) {
		printf("%s%c%-3d ", roster[evp[i]], evsymb[evt[i]], evm[i]);
	}
	return opt;
}

//---------------------------------------------

int main(int argc, char* argv[]) {
  char filename[256];
  if (!Load()) {
    printf("ERROR: called from invalid drectory.\n");
    return -1;
  }
  if (argc < 2) {
    printf("ERROR: No edition specified.\n");
    return -1;
  }
	int mst = 0;
	if (argc > 2) {
		mst = atoi(argv[2])-1;
		if (mst<0) mst=0;
	}

  char sarg1[128];
  strcpy(sarg1, argv[1]);
  year = atoi(sarg1);

  Pl.Load("players.dat");
  Epl.Load("europlayers.dat");
	NP = Pl.Size();
  LoadDB();
  LoadEvents();

	for (int i=mst; i<NM; i++) {
		if (AddScorers(i)==10) i=NM;
	}

	SaveEvents();

  return 0;
}
