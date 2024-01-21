#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "catalog.hh"
#include "aliases.hh"
#include "db.h"

#define ROSTER_SIZE	22

#define DB_ROWS    400
#define DB_COLS     60
#define DB_KCELL    60
#define EV_COLS		60

#define EV_GOAL		 0
#define EV_OWNGOAL	 1
#define EV_PKGOAL	 2
#define EV_PKMISS	 3
#define EV_YELLOW	 4
#define EV_RED	 	 5
#define EV_YELLOWRED 6

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
int  sub[2*ROSTER_SIZE];
int  unmatched[ROSTER_SIZE], nun, nunsub;
int  fulltime;

/* *************************************** */

Catalog Pl, Epl, Ant, Eant, Arb;
char dbfilename[256], eventsfilename[256];
Aliases **L;

char  db[DB_ROWS][DB_COLS][DB_KCELL];
char edb[DB_ROWS][EV_COLS][DB_KCELL];
const char *evsymb = "'`\"/#!  ";


//--------------------------------------

int Load() {
  FILE *f;
  char s[2000], *tok[20], *ystr, *name, *nick;

  f = fopen("country.dat", "rt");
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

  f = fopen("webteams.dat", "rt");
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
  sprintf(filename, "../%s/euroteams.dat", dir[ct]);
  FILE *f = fopen(filename, "rt");
  if (!f) return name;
  for (int i=0; i<=cl; ++i) fgets(s, 128, f);
  strcpy(name, s);
  name[15] = 0;
  int k = 14; while (name[k]==' ') name[k--]=0;
  fclose(f);
  return name;
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
    fulltime = 90;
	char sloc[128];
	for (int i=0; i<ROSTER_SIZE; i++) {
		strcpy(sloc, db[r][DB_ROSTER1+i]);
		strtok(sloc, ":");
        char *sm = strtok(NULL, ":,");
		strcpy(roster[i], sloc);
        rmin[i] = atoi(sm);
        if (rmin[i]>fulltime) fulltime = rmin[i];
        sub[i] = -1;

		strcpy(sloc, db[r][DB_ROSTER2+i]);
		strtok(sloc, ":");
        sm = strtok(NULL, ":,");
		strcpy(roster[i+ROSTER_SIZE], sloc);
        rmin[i+ROSTER_SIZE] = atoi(sm);
        if (rmin[i+ROSTER_SIZE] > fulltime) fulltime = rmin[i+ROSTER_SIZE];
        sub[i+ROSTER_SIZE] = -1;

		if (home<1000) {
			rid[i] = Pl.FindMnem(roster[i]);
		}
		else {
			rid[i] = 100000 + Epl.FindMnem(roster[i]);
		}
		if (away<1000) {
			rid[i+ROSTER_SIZE] = Pl.FindMnem(roster[i+ROSTER_SIZE]);
		}
		else {
			rid[i+ROSTER_SIZE] = 100000 + Epl.FindMnem(roster[i+ROSTER_SIZE]);
		}
	}
}

int unambiguous(int h, int p1, int p2, int p3) {
  int lo = p1;
  int hi = p2;
  if (h>=p2) { lo = p2; hi = p3; }
  for (int j=lo; j<hi; j++) {
    if (h!=j && rmin[h]==rmin[j]) {
      return 0;
    }
  }
  return 1;
}

int subsubbed(int h, int p1, int p2, int p3) {
  if (rmin[h] + rmin[p2]   + rmin[p2+1] == fulltime) return 1;
  if (rmin[h] + rmin[p2]   + rmin[p2+2] == fulltime) return 1;
  if (rmin[h] + rmin[p2+1] + rmin[p2+2] == fulltime) return 1;
  return 0;
}

void revert(int h, int p1, int p2, int p3) {
  for (int j=p1; j<p2; j++) {
    if (h!=j && rmin[h] == rmin[j]) {
      sub[j] = -1;
      unmatched[nun++] = j;
    }
  }
}

void MatchSubs(int p1, int p2, int p3) {
  nun = nunsub = 0;
    printf("---------------------\n  ");
    for (int i=p1; i<p3; i++) {
      if (i%ROSTER_SIZE == ROSTER_SIZE/2) printf(" | ");
      if (rmin[i]>0 && rmin[i]!=fulltime) printf("%s:%-3d ", roster[i], rmin[i]);
    }
    printf("\n---------------------\n");
  for (int i=p1; i<p2; i++) {
    if (rmin[i]==fulltime) sub[i] = i;
    else /*if (unambiguous(i, p1, p2, p3)) */{
      if (sub[i]<0 && rmin[i]>0 && rmin[i]<fulltime) {
        for (int j=p2; j<p3; j++) {
          if (sub[i]<0 && rmin[i] + rmin[j] == fulltime && sub[j]<0) {
            sub[i] = j; sub[j] = i;
          }
        }
      }
    }
  }
  for (int i=p1; i<p2; i++) {
    if (rmin[i]>0 && sub[i]<0) {
      unmatched[nun++] = i;
      if (!unambiguous(i, p1, p2, p3)) {
        revert(i, p1, p2, p3);
      }
    }
  }
  for (int i=p2; i<p3; i++) {
    if (rmin[i]>0 && sub[i]<0) {
      unmatched[nun+nunsub] = i;
      nunsub++;
    }
  }
}

void FindRedInterval(int r, int p1, int p2, int p3) {
    int opt;
    MatchSubs(p1, p2, p3);
    for (int i=p1; i<p3; i++) {
      if (rmin[i]>0 && sub[i]<0) {
        int rcmin =  rmin[i]+1;
        if (i%ROSTER_SIZE>=ROSTER_SIZE/2) {
          rcmin = rmin[unmatched[0]]+rmin[i]+1;
        }
        printf("%s(%d/%d,%s,%s)%s Confirm #%d. %s!%-3d ?  [0/1]  ",
          eyellow, nun, nunsub,
          (unambiguous(i, p1,p2,p3))? "-" : "!!!",
          (subsubbed(i, p1,p2,p3))? "S" : "-", enormal,
          i+1, roster[i], rcmin);
        scanf("%d", &opt);
        if (opt==1) {
          sprintf(edb[r][nev++], "%s!%-3d", roster[i], rcmin);
        }
      }
    }
}

void FindRed() {
  for (int r=0; r<NM; r++) {
    home = atoi(db[r][DB_HOME]);
    away = atoi(db[r][DB_AWAY]);
    score = atoi(db[r][DB_SCORE]);
    printf("\n%s [#%d] R%s *** %s - %s %d-%d%s\n", eyellow, r+1, db[r][DB_ROUND],
        (home<1000? NameOf(L, home, year): EuroName(home)),
        (away<1000? NameOf(L, away, year): EuroName(away)),
        score/100,  score%100, enormal);
    GetEvents(r);
    GetRoster(r);
    FindRedInterval(r, 0, ROSTER_SIZE/2, ROSTER_SIZE);
    FindRedInterval(r, ROSTER_SIZE, 3*ROSTER_SIZE/2, 2*ROSTER_SIZE);
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

//---------------------------------------------

int main(int argc, char* argv[]) {
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
	int mst = 0;
	if (argc > 3) {
		mst = atoi(argv[3])-1;
		if (mst<0) mst=0;
	}

  char sarg1[128];
  strcpy(sarg1, dbfilename);
  strtok(sarg1, ".");
  char *syear = sarg1+strlen(sarg1)-4;
  year = atoi(syear);

  Pl.Load("players.dat");
  Epl.Load("europlayers.dat");
  LoadDB(dbfilename);
  LoadEvents(eventsfilename);

  FindRed();
  SaveEvents(eventsfilename);

  return 0;
}
