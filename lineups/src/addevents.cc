#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "catalog.hh"
#include "aliases.hh"
#include "db.h"

#define MAX_LEVELS  12
#define MAX_RR	     4
#define MAX_N       64
#define MAX_TEAMS   2000
#define MAX_ROSTER	 100
#define ROSTER_SIZE		22

#define MAX_NAMES  20000
#define DB_ROWS     800
#define DB_COLS      60
#define DB_CELL      40
#define DB_KROWS     60
#define DB_KCOLS     60
#define DB_KCELL     40
#define EV_COLS		 60
#define PL_INITIAL	  0
#define PL_FULL_NAME  1

#define EV_GOAL		 0
#define EV_OWNGOAL	 1
#define EV_PKGOAL	 2
#define EV_PKMISS	 3
#define EV_YELLOW	 4
#define EV_RED	 	 5
#define EV_YELLOWRED	 6
#define OPT_NONE	-2
#define OPT_UNDO	-1
#define OPT_EVENT	 1
#define OPT_DONE	 0
#define OPT_SAME	 8
#define OPT_SAVE	 9
#define OPT_QUIT	10

#define NAT_ROM 39000

char **club;
char **mnem;
char **ctty;
char **dir;
int  NC, NM, NT, NN;
char ssn[32];
int  year, score, home, away;

const char *enormal  = "\033[0m";
const char *eyellow  = "\033[33m";
const char *ehicyan    = "\033[36;1m";

int  nev, evp[EV_COLS], evm[EV_COLS], evt[EV_COLS];
char roster[2*ROSTER_SIZE][DB_KCELL];
int  rmin[2*ROSTER_SIZE];
int  rid[2*ROSTER_SIZE];

/* *************************************** */

char  db[DB_ROWS][DB_COLS][DB_CELL];
char edb[DB_ROWS][EV_COLS][DB_CELL];
const char *evsymb = "'`\"/#!  ";

int NP;

Catalog Pl, Epl, Ant, Eant, Arb;
Locations Loc, Eloc;

FILE *of;
char dbfilename[256], eventsfilename[256];
char sf[128];
Aliases **L;

//--------------------------------------

int Load() {
  FILE *f;
  char s[2000], *tok[20], *ystr, *name, *nick;

  f = fopen("/home/radu/lineups/catalogs/country.dat", "rt");
  if (!f) return 0;
  fscanf(f, "%d\n", &NN);
  ctty = new char*[NN];
  dir = new char*[NN];
  for (int i=0; i<NN; i++) {
    if (feof(f)) continue;
    fgets(s, 2000, f);
    s[strlen(s)-1] = 0;
    tok[0] = strtok(s, ":");
    tok[1] = strtok(NULL, ",");
    dir[i] = strdup(tok[0]);
    ctty[i] = strdup(tok[1]+2);
    s[0] = 0;
  }
  fclose(f);

  f = fopen("/home/radu/lineups/data/webteams.dat", "rt");
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

char *EuroName(int t) {
  int ct = t/1000;
  int cl = t%1000;
  if (cl==0) return ctty[ct];
  char filename[128];
  char *name = new char[64];
  char s[128];
  sprintf(name, "%d/%d", ct, cl);
  sprintf(filename, "/var/www/html/%s/euroteams.dat", dir[ct]);
  FILE *f = fopen(filename, "rt");
  if (!f) return name;
  for (int i=0; i<=cl; ++i) fgets(s, 128, f);
  strcpy(name, s);
  name[15] = 0;
  int k = 14; while (name[k]==' ') name[k--]=0;
  fclose(f);
  return name;
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

void LoadDB(char *filename) {
  char s[5000], *tk[DB_COLS];
  FILE *f;
  f = fopen(filename, "rt");
  if (f==NULL) { fprintf(stderr, "ERROR: database %s not found.\n", filename); return; }
	int i = 0;
  while (!feof(f)) {
    fgets(s, 5000, f);
    if (strlen(s)<100) continue;
    tk[0] = strtok(s, ",\n");
    for (int j=1; j<DB_COLS; j++) tk[j]=strtok(NULL, ",\n");
    for (int j=0; j<DB_COLS; j++) {
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

void LoadEvents(char *filename) {
  char s[5000], *tk[DB_COLS];
  FILE *f;
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
    sp = strtok(s,    "'`\"/!#,\n");
    sm = strtok(NULL, "'`\"/!#,\n");
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
        char *sm = strtok(NULL, ":,");
		strcpy(roster[i], sloc);
        rmin[i] = atoi(sm);

		strcpy(sloc, db[r][DB_ROSTER2+i]);
		strtok(sloc, ":");
        sm = strtok(NULL, ":,");
		strcpy(roster[i+ROSTER_SIZE], sloc);
        rmin[i+ROSTER_SIZE] = atoi(sm);

		if (home<1000 || home==NAT_ROM) {
			rid[i] = Pl.FindMnem(roster[i]);
		}
		else {
			rid[i] = 100000 + Epl.FindMnem(roster[i]);
		}
		if (away<1000 || away==NAT_ROM) {
			rid[i+ROSTER_SIZE] = Pl.FindMnem(roster[i+ROSTER_SIZE]);
		}
		else {
			rid[i+ROSTER_SIZE] = 100000 + Epl.FindMnem(roster[i+ROSTER_SIZE]);
		}
	}
}

void SaveEvents(char *filename) {
  char oldfilename[64];
  char newfilename[64];
  char basename[64];
  strcpy(basename, filename);
  strtok(basename, ".");
  sprintf(oldfilename, "%s.old", basename);
  sprintf(newfilename, "%s.new", basename);
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

char uchar[256][16];

void MapUtfChars() {
  for (int i=0; i<256; i++) uchar[i][0] = 0;
  strcpy(uchar['a'], "ă");
  strcpy(uchar['q'], "â");
  strcpy(uchar['i'], "î");
  strcpy(uchar['s'], "ş");
  strcpy(uchar['t'], "ţ");
  strcpy(uchar['S'], "Ş");
  strcpy(uchar['T'], "Ţ");
  strcpy(uchar['o'], "ö");
  strcpy(uchar['u'], "ü");
  strcpy(uchar['O'], "Ö");
  strcpy(uchar['U'], "Ü");
  strcpy(uchar['c'], "č");
  strcpy(uchar['C'], "Č");
  strcpy(uchar['g'], "ğ");
  strcpy(uchar['j'], "ı");
  strcpy(uchar['J'], "İ");
  strcpy(uchar['k'], "ç");
  strcpy(uchar['K'], "Ç");
  strcpy(uchar['v'], "š");
  strcpy(uchar['V'], "Š");
  strcpy(uchar['z'], "ž");
  strcpy(uchar['Z'], "Ž");
}

void addUtfChars(char *s) {
  if (s==NULL) return;
  int l = strlen(s);
  int k = l-1;
  while (k>=0) {
    if (s[k] == 92) {
      unsigned char x = ((unsigned char) s[k+1]);
      if (uchar[x][0] != 0) {
        int ls = strlen(uchar[x]);
        for (int j=k+2; j<l; j++) s[j+ls-2] = s[j];
        for (int j=0; j<ls; j++) s[k+j] = uchar[x][j];
        l = strlen(s);
      }
    }
    k--;
  }
}

int findName(char *s) {
  if (s==NULL) return -1;
  addUtfChars(s);
  char *tkf, *tkl, *rfn, *rln;;
  int found = 0;
  int px = -1;
  char *tks = new char[strlen(s)];
  strcpy(tks, s);
  strtok(tks, ";:");
  if (strchr(tks, ',')!=NULL) {
    tkf = strtok(tks, ",");
    tkl = strtok(NULL, ",");
  } else {
    tkf = new char[2]; tkf[0] = 0;
    tkl = tks;
  }
  if (tkf[0] >= 'a' && tkf[0] <= 'z') tkf[0] -= 32;
  if (tkl[0] >= 'a' && tkl[0] <= 'z') tkl[0] -= 32;
  for (int i=0; i<2*ROSTER_SIZE; i++) {
    if (rid[i] >= 0 && rid[i] < 100000) {
      rln = Pl.P[rid[i]].name;
      rfn = Pl.P[rid[i]].pren;
      if (strstr(rln, tkl)==rln) {
        if (tkf[0]==0 || (tkf[0]!=0 && strstr(rfn,tkf)==rfn)) {
          found++; px = i+1;
        }
      }
    }
  }
  if (found == 1) return px;
  return -1;
}

int parse(char s[], int &epi, int &emin) {
	int len = strlen(s);
	char *stok = s;
	if (len<1) return -2;
	char *eptr;
	epi = -1;
	emin = -1;
	if (s[0]=='0') return 0;
	if (len<3 && (s[0]=='S' || s[0]=='s')) return OPT_SAVE;
	if (len<3 && (s[0]=='q' || s[0]=='Q')) return OPT_QUIT;
	if (s[0]=='.') {
		emin = strtol(stok+1, NULL, 10);
		return OPT_SAME;
	}
    if ((stok[0] >= 'a' && stok[0] <= 'z') || stok[0]==92 || stok[0]=='/') {
      epi = findName(stok);
      int k=0; while (k<strlen(stok) && stok[k]!=';' && stok[k]!=':' && stok[k]!='=') k++;
      eptr = stok+k;
    } else {
	  epi = strtol(stok, &eptr, 10);
    }
	if (epi==-1) return OPT_NONE;
	if (!eptr) { emin=0; return OPT_EVENT; }
	if (eptr[0]=='-'||eptr[0]==':'||eptr[0]==';'||eptr[0]=='=') {
		stok = eptr+1;
		emin = strtol(stok, &eptr, 10);
		if (!eptr[0]) return OPT_EVENT;
		if (eptr[0]=='p'||eptr[0]=='P') return OPT_EVENT+EV_PKGOAL;
		if (eptr[0]=='o'||eptr[0]=='a') return OPT_EVENT+EV_OWNGOAL;
		if (eptr[0]=='m'||eptr[0]=='/') return OPT_EVENT+EV_PKMISS;
		if (eptr[0]=='y'||eptr[0]=='#') return OPT_EVENT+EV_YELLOW;
		if (eptr[0]=='x'||eptr[0]=='r'||eptr[0]=='!') return OPT_EVENT+EV_RED;
	}
	return -2;
}

int AddEvents(int r) {
  char rfilename[128];
	home = atoi(db[r][DB_HOME]);
	away = atoi(db[r][DB_AWAY]);
	score = atoi(db[r][DB_SCORE]);
	printf("\n%s [L:#%d] R%s %s *** %s - %s %d-%d%s\n", eyellow, r+1, db[r][DB_ROUND], db[r][DB_DATE],
		(home<1000? NameOf(L, home, year): EuroName(home)),
		(away<1000? NameOf(L, away, year): EuroName(away)),
		score/100,  score%100, enormal);
	printf("---------------------\n");
	GetEvents(r);
	for (int i=0; i<nev; ++i) {
		if (strlen(edb[r][i])>5) printf("%s ", edb[r][i]);
	}
	printf("\n");
	printf("---------------------\n");
    GetRoster(r);

	char rhi[128], nhi[128],  rai[128], nai[128], smh[5], sma[5];
	for (int i=0; i<ROSTER_SIZE; i++) {
      sprintf(smh, "    ");
      sprintf(sma, "    ");
      if (rmin[i] > 0) sprintf(smh, "%3d'", rmin[i]);
      if (rmin[i+ROSTER_SIZE] > 0) sprintf(sma, "%3d'", rmin[i+ROSTER_SIZE]);
		int nvalid = 2;
		if (roster[i][0]==' ' || roster[i][0]==0) {
			sprintf(rhi, "%-36s %s", " ", smh);
			nvalid--;
		}
		else if (rid[i]>=0) {
			if (home<1000 || home==NAT_ROM) {
				sprintf(nhi, "%s %s", Pl.P[rid[i]].pren, Pl.P[rid[i]].name);
			}
			else {
				sprintf(nhi, "%s %s", Epl.P[rid[i]-100000].pren, Epl.P[rid[i]-100000].name);
			}
			sprintf(rhi, "%2d. %-32s %s", i+1, nhi, smh);
		}
		else sprintf(rhi, "%2d. %s%-32s %s%s", i+1, ehicyan, roster[i], enormal, smh);

		if (roster[i+ROSTER_SIZE][0]==' ' || roster[i+ROSTER_SIZE][0]==0) {
			sprintf(rai, "%-36s %s", " ", sma);
			nvalid--;
		}
		else if (rid[i+ROSTER_SIZE]>=0) {
			if (away<1000 || away==NAT_ROM) {
				sprintf(nai, "%s %s", Pl.P[rid[i+ROSTER_SIZE]].pren, Pl.P[rid[i+ROSTER_SIZE]].name);
			}
			else {
				sprintf(nai, "%s %s", Epl.P[rid[i+ROSTER_SIZE]-100000].pren, Epl.P[rid[i+ROSTER_SIZE]-100000].name);
			}
			sprintf(rai, "%2d. %-32s %s", i+ROSTER_SIZE+1, nai, sma);
		}
		else sprintf(rai, "%2d. %s%-32s %s%s", i+ROSTER_SIZE+1, ehicyan, roster[i+ROSTER_SIZE], enormal, sma);
		if (nvalid>0) printf("%s\t%s\n", rhi, rai);
	}

	printf("---------------------\n");
	char sopt[128];
	int opt, epi, etp, emin;
	int x = 0;
	int y = 0;
	int previ = -1;
    char evo = '#';
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
				SaveEvents(eventsfilename);
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
  MapUtfChars();
  if (!Load()) {
    printf("ERROR: called from invalid drectory.\n");
    return -1;
  }
  if (argc < 3) {
    printf("ERROR: No edition specified.\n");
    return -1;
  }
  strcpy(dbfilename, argv[1]);
  strcpy(eventsfilename, argv[2]);

  char sarg1[128];
  strcpy(sarg1, dbfilename);
  strtok(sarg1, ".");
  char *syear = sarg1+strlen(sarg1)-4;
  year = atoi(syear);

  Pl.Load("catalogs/players.dat");
  Epl.Load("catalogs/europlayers.dat");
  NP = Pl.Size();
  LoadDB(dbfilename);
  LoadEvents(eventsfilename);

	int mst = 0;
	if (argc > 3) {
        char *err;
        long number = strtol(argv[3], &err, 10);
        if (err == NULL || err[0] == 0) {
  		  mst = atoi(argv[3])-1;
        } else {
          mst = findTeams(argv[3]);
        }
        if (mst<0) mst=0;
	}

  for (int i=mst; i<NM; i++) {
    if (AddEvents(i)==10) i=NM;
  }

  SaveEvents(eventsfilename);

  return 0;
}
