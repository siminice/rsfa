#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "catalog.hh"
#include "aliases.hh"

#define DB_KROWS      60
#define DB_KCOLS      60
#define DB_KCELL      40

#define DB_HOME		 0
#define DB_AWAY		 1
#define DB_SCORE	 2
#define DB_DATE		 3
#define DB_COMP		 4
#define DB_ROUND	 5
#define DB_VENUE	 6
#define DB_ATTEND	 7
#define DB_WEATHER	 8
#define DB_ROSTER1	 9
#define DB_COACH1	31
#define DB_ROSTER2	32
#define DB_COACH2	54
#define DB_REF		55
#define DB_ASIST1	56
#define DB_ASIST2	57
#define DB_OBSERV	58
#define DB_T1		 8
#define DB_T2		31

#define ROSTER_SIZE 22

const char *normal  = "\033[0m";
const char *yellow  = "\033[33m";
const char *cyan    = "\033[36m";
const char *hicyan    = "\033[36;1m";

static int count = 0;

char **club;
char **mnem;
int  NC, NM;
int  year, tm;

/* *************************************** */

char  db[DB_KROWS][DB_KCOLS][DB_KCELL];
char  roster[DB_KROWS][2*ROSTER_SIZE][DB_KCELL];
int   mph[ROSTER_SIZE];
int   mpa[ROSTER_SIZE];
int   xtra[DB_KROWS];
Catalog Pl;
FILE *of;

Aliases **L;

//--------------------------------------

int Load() {
  FILE *f;
  char s[2000], *tok[20], *ystr, *name, *nick;

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

void Minutes(char *tkm, int pos, int min, char *sm) {
	if (!tkm) { strcpy(sm, "90"); return; }
	int vm = atoi(tkm);
	if (pos<11) sprintf(sm, "%d", vm-1);
	else sprintf(sm, "%d", min-vm+1);
}

void LoadDB() {
  char filename[64], s[5000], *tk[DB_KCOLS];
  FILE *f;
  sprintf(filename, "cup-lineups-%d.db", year);
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
		i++;
		s[0] = 0;
  }
  fclose(f);
	NM = i;
}

void LoadExtra() {
  char filename[64], s[5000], *tk[DB_KCOLS];
  FILE *f;
  sprintf(filename, "cuparchive-%d.dat", year);
  f = fopen(filename, "rt");
  if (f==NULL) { fprintf(stderr, "ERROR: database %s not found.\n", filename); return; }
	for (int i=0; i<NM; ++i) {
    fgets(s, 5000, f);
		int len = strlen(s);
		s[len-1] = 0;
		xtra[i] = 0;
		if (len>14 && s[15]>='0' && s[16]<='9') xtra[i] = 1;
  }
  fclose(f);
}


void trim(char *s) {
  if (s[1]==' ' || s[1]=='.') { s[1] = s[2]; s[2] = s[3]; }
  if (s[2]==' ' || s[2]=='.') { s[2] = s[3]; }
  s[3] = 0;
}

int GetMinutes(int r, int pos) {
	char spi[128];
	strcpy(spi, db[r][pos]);
	char *tkn = strtok(spi, ":");
	char *tkm = strtok(NULL, ":");
	int min = 0;
	if (tkm) return strtol(tkm, NULL, 10);
}


int FindMnem(int r, int pos, char *pmnem) {
	int len = strlen(pmnem);
	if (pmnem[0]=='~') {
		if (pmnem[1]>='a' && pmnem[1]<='z') pmnem[1] -= 32;
	}
	for (int i=0; i<len; ++i) if (pmnem[i]=='_') pmnem[i]=' ';
	int found = 0;
	int where = -1;
	for (int i=1; i<=14; ++i) {
		if (strstr(db[r][pos+i], pmnem)==db[r][pos+i]) {
			found++;
			where = i;
		}
	}
	if (found==1) return where;
	return -1;
}

void GetFullName(int r, int j, char *sp) {
	strcpy(sp, db[r][j]);
	strtok(sp, ":");
	if (db[r][j][0]!='~') {
		int x = Pl.FindMnem(sp);
		if (x>=0) sprintf(sp, "%s %s", Pl.P[x].pren, Pl.P[x].name);
	}
}

void MakeSub(int r, int pos, int pin, int pout, int min) {
	char* m1 = strchr(db[r][pos+pout], ':');
	char* m2 = strchr(db[r][pos+pin], ':');
	int total = 90;
	if (min>90) total = 120;
	sprintf(m2+1, "%-3d", total+1-min);
	if (pout<=11) {
		sprintf(m1+1, "%-3d", min-1);
	}
	else {
		int min2 = GetMinutes(r, pos+pout);
		sprintf(m1+1, "%-3d", min2-(total+1-min));
	}
}

void CheckSubs() {
	for (int r=0; r<NM; ++r) {
		int home = atoi(db[r][DB_HOME]);
		int away = atoi(db[r][DB_AWAY]);
		int sco = strtol(db[r][DB_SCORE], NULL, 10);
		int sc1 = sco/100;
		int sc2 = sco%100;

		int total_start_h = 0;
		int total_subst_h = 0;
		int total_start_a = 0;
		int total_subst_a = 0;
		for (int i=0; i<22; ++i) {
			mph[i] = GetMinutes(r, DB_ROSTER1+i);
			if (i<11) total_start_h += mph[i];
			else total_subst_h += mph[i];
			mpa[i] = GetMinutes(r, DB_ROSTER2+i);
			if (i<11) total_start_a += mpa[i];
			else total_subst_a += mpa[i];
		}

		int ok = 1;
		if (!xtra[r] && total_start_h + total_subst_h != 990) ok = 0;
		if (!xtra[r] && total_start_a + total_subst_a != 990) ok = 0;
		if (xtra[r] && total_start_h + total_subst_h != 1320) ok = 0;
		if (xtra[r] && total_start_a + total_subst_a != 1320) ok = 0;

		if (!ok) {
			count++;
			printf("*** [%3d] %s %s - %s [%d-%d]\n", count, db[r][DB_DATE],
				NickOf(L, home, year), NickOf(L, away, year), sc1, sc2);
			printf(" %5d + %5d = %5d; %5d + %5d = %5d\n\n",
				total_start_h, total_subst_h, total_start_h + total_subst_h,
				total_start_a, total_subst_a, total_start_a + total_subst_a);
		}
	}
}

void AddSubs() {
	for (int i=0; i<NM; ++i) {
		int home = atoi(db[i][DB_HOME]);
		int away = atoi(db[i][DB_AWAY]);
		int sco = strtol(db[i][DB_SCORE], NULL, 10);
		int sc1 = sco/100;
		int sc2 = sco%100;
		int pos = -1;
		int min = 0;
		int minsub = 46;
		int pout = 0;
		int changed = 0;
		char pmnem[DB_KCELL], spout[DB_KCELL], spin[DB_KCELL];
		if (home==tm || tm==-1) pos = DB_T1;
//		if (away==tm || tm==-1) pos = DB_T2;
		if (away==tm) pos = DB_T2;
		if (pos >= 0) {
			printf("\n\n%s*** %s %s - %s [%d-%d]%s\n", yellow, db[i][DB_DATE], 
				NickOf(L, home, year), NickOf(L, away, year), sc1, sc2, normal);
			for (int k=1; k<=14; k++) printf("%s%c", db[i][pos+k],
				((k==1 || k==6 || k==11 || k==14)?'\n':','));
			changed = 0;
			for (int j=12; j<=14; ++j) {
				min = GetMinutes(i, pos+j);
				minsub = 46;
				pout = 0;
				if (min==90 || min==120) {
					GetFullName(i, pos+j, spin);
					do {
						printf("   %d %s <= ", j, spin); scanf("%d", &minsub);
					} while (minsub<0 || minsub>120);
					do  {
						printf("      => "); scanf("%s", pmnem);
						pout = FindMnem(i, pos, pmnem);
					} while (pout<1 || pout > 14);
					GetFullName(i, pos+pout, spout);
					printf("      => %s%s%s\n", cyan, spout, normal);
					MakeSub(i, pos, j, pout, minsub);
					changed = 1;
				}
			}
			if (changed) {
				for (int k=1; k<=14; k++) printf("%s%c", db[i][pos+k], 
				((k==1 || k==6 || k==11 || k==14)?'\n':','));
			}
		}
	}
}

void SaveDB() {
  char oldfilename[64];
  char newfilename[64];
  char filename[64];
  sprintf(filename,    "cup-lineups-%d.db",  year);
  sprintf(oldfilename, "cup-lineups-%d.old", year);
  sprintf(newfilename, "cup-lineups-%d.new", year);
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

//---------------------------------------------

int main(int argc, char* argv[]) {
  char filename[256];
  if (!Load()) {
    printf("ERROR: called from invalid drectory.\n");
    return -1;
  }

  char sarg1[128];
	tm = -1;
	int FY = 1934;
	int LY = 2013;

	if (argc>1) {
	  strcpy(sarg1, argv[1]);
 		tm = atoi(sarg1);
		FY = tm;
		LY = FY;
	}

  Pl.Load("players.dat");

	for (year = FY; year<=LY; ++year) {
	  LoadDB();
		LoadExtra();
//		AddSubs();
		CheckSubs();
//		SaveDB();
	}

  return 0;
}
