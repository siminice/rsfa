#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "catalog.hh"
#include "aliases.hh"
#include "db.h"

#define MAX_ROSTER	 100
#define ROSTER_SIZE	  22
#define MAX_TEAMS     64
#define CAT_ROWS    2000
#define CAT_CELL     100

#define MAX_NAMES  20000
#define DB_ROWS     2048
#define DB_COLS      60
#define DB_CELL     128

#define COMP_LIGA	0
#define COMP_CUPA	1
#define COMP_EURO	2
#define COMP_NATL	3
#define ROM_NAT     39000
#define NUM_COMP	COMP_NATL+1

#define EV_COLS       60
#define EV_GOAL        0
#define EV_OWNGOAL     1
#define EV_PKGOAL      2
#define EV_PKMISS      3
#define EV_YELLOW      4
#define EV_RED         5
#define EV_YELLOWRED   6
#define PSO_TIME     200
#define SOME_TIME 999
const char *evsymb = "'`\"/#!*";

char **club;
char **mnem;
Aliases **L;
Catalog Pl;
int  NC, NM, NT, NP, ncat;
int  year, stm, comp;
int  no_fix = 0;

char  db[DB_ROWS][DB_COLS][DB_CELL];
char edb[DB_ROWS][EV_COLS][DB_CELL];
char oldcat[CAT_ROWS][CAT_COLS][CAT_CELL];
char newcat[CAT_ROWS][CAT_COLS][CAT_CELL];

typedef int Stats[MAX_TEAMS][MAX_ROSTER];

int  nev, evp[EV_COLS], evm[EV_COLS], evt[EV_COLS];
char roster[2*ROSTER_SIZE][DB_CELL];
int  rid[2*ROSTER_SIZE];
int  pmin[2*ROSTER_SIZE];
int  tid[MAX_TEAMS];
int  npl[MAX_TEAMS];
int  tord[MAX_TEAMS];
int  pord[MAX_ROSTER];
Stats pid, mec, tit, rez, gol, min;
Stats itg, ban, gre, pen, aut, rat;
Stats gal, ros;

//--------------------------------------

int Load() {
  FILE *f;
  char s[2000], *tok[20], *ystr, *name, *nick;

  f = fopen("data/webteams.dat", "rt");
  if (f==NULL) return 0;
  fscanf(f, "%d\n", &NC);
  club = new char*[NC];
  mnem = new char*[NC];
	L = new Aliases*[NC];
  for (int i=0; i<NC; i++) {
		L[i] = new Aliases;
    fgets(s, 60, f);
    s[strlen(s)-1] = 0;
    mnem[i] = new char[48];
    club[i] = new char[48];
    strncpy(mnem[i], strtok(s, ","), 32);
    strncpy(club[i], strtok(NULL, ",\n"), 32);
  }
  fclose(f);
	LoadAliases(L, NC, "data/alias.dat");
  return 1;
}

void LoadDB() {
  char filename[64], s[5000], *tk[DB_COLS];
  FILE *f;
	switch (comp) {
		case COMP_CUPA:
			sprintf(filename, "db/cup-lineups-%d.db", year);
			break;
		case COMP_EURO:
			sprintf(filename, "db/euro-lineups-%d.db", year);
			break;
		case COMP_NATL:
			sprintf(filename, "db/nat-lineups-%d.db", year);
			break;
		default:
		  sprintf(filename, "db/lineups-%d.db", year);
	}
  f = fopen(filename, "rt");
  if (f==NULL) { fprintf(stdout, "ERROR: database %s not found.\n", filename); return; }
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
		i++;
		s[0] = 0;
  }
  fclose(f);
	NM = i;
	fprintf(stdout, "NM = %d.\n", NM);
}

void LoadOldCatalog() {
	char filename[128];
	switch (comp) {
		case COMP_CUPA:
			sprintf(filename, "catalogs/cupa-%d.dat", year);
			break;
		case COMP_EURO:
			sprintf(filename, "catalogs/euro-%d.dat", year);
			break;
		case COMP_NATL:
			sprintf(filename, "catalogs/nat-%d.dat", year);
			break;
		default:
			sprintf(filename, "catalogs/catalog-%d.dat", year);
	}
  FILE *f = fopen(filename, "rt");
  if (f==NULL) {
    fprintf(stdout, "ERROR: Catalog %s not found.\n", filename);
    return;
  }
  ncat = 0;
  char s[1000], *tk[OLDCAT_COLS];
  while (!feof(f)) {
    fgets(s, 1000, f);
    if (strlen(s)<30) continue;
    tk[0]=strtok(s, ",\n");
    for (int j=1; j<OLDCAT_COLS; j++) tk[j] = strtok(NULL, ",\n");
    for (int j=0; j<OLDCAT_COLS; j++)
			if (tk[j]!=NULL) strcpy(oldcat[ncat][j], tk[j]); else strcpy(oldcat[ncat][j], " ");
    ncat++;
    s[0] = 0;
  }
  fclose(f);
}

int FindOldcat(char *mnem, int tid) {
    int ot = -1;
	for (int i=0; i<ncat; ++i) {
        int ct = atoi(oldcat[i][CAT_CLUB]);
		if (strcmp(oldcat[i][CAT_MNEM], mnem)==0) {
          if (ct == tid || comp == COMP_NATL) {
            return i;
          } else {
//            ot = ct;
          }
        }
	}
	return ot;
}

int RosterID(char *s) {
	for (int i=0; i<2*ROSTER_SIZE; ++i) {
		if (strcmp(roster[i],s)==0) return i;
	}
	return -1;
}

int RosterID(int p) {
	for (int i=0; i<2*ROSTER_SIZE; ++i) {
		if (rid[i]==p) return i;
	}
	return -1;
}

void LoadEvents() {
  char filename[64], s[5000], *tk[DB_COLS];
  FILE *f;
	switch (comp) {
		case COMP_CUPA:
		  sprintf(filename, "db/cup-events-%d.db", year);
			break;
		case COMP_EURO:
		  sprintf(filename, "db/euro-events-%d.db", year);
			break;
		case COMP_NATL:
		  sprintf(filename, "db/nat-events-%d.db", year);
			break;
		default:
		  sprintf(filename, "db/events-%d.db", year);
	}
  f = fopen(filename, "rt");
  if (f==NULL) { fprintf(stdout, "ERROR: database %s not found.\n", filename); return; }
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
  char s[DB_CELL], *sp, *sm, cev;
  ResetEvents();
  for (int i=0; i<EV_COLS; i++) {
    strcpy(s, edb[r][i]);
    if (strlen(s) < 6) continue;
	cev = s[strlen(s)-4];
    sp = strtok(s, "'`\"/#!*,\n");
    sm = strtok(NULL, "'`\"/#!*,\n");
    if (sm!=NULL) em = atoi(sm); else em=-1;
//    if (sp!=NULL) ep = Pl.FindMnem(sp); else ep=-1;
    if (sp!=NULL) ep = RosterID(sp); else ep=-1;
    if (ep>=0 || em>=0) {
      if (em==0) em=999;
      evp[i] = ep; evm[i] = em;
                         evt[i] = EV_GOAL;
           if (cev=='`') evt[i] = EV_OWNGOAL;
      else if (cev=='"') evt[i] = EV_PKGOAL;
      else if (cev=='/') evt[i] = EV_PKMISS;
      else if (cev=='#') evt[i] = EV_YELLOW;
      else if (cev=='!') evt[i] = EV_RED;
      else if (cev=='*') evt[i] = EV_YELLOWRED;
      nev++;
    }
  }
}

int GKSub(int p, int tid) {
	int sub[11], xsub;
	sub[0] = 0;
	int nsubs = 0;
	for (int i=0; i<11; ++i) {
		if (pmin[p]+pmin[p+11+i]==90) {
			sub[nsubs++] = p+11+i;
		}
	}
	if (nsubs>1) {
		fprintf(stdout, "Multiple subs possible for %s '%d: ", (rid[p]>=0?Pl.P[rid[p]].name:roster[p]), pmin[p]);
		for (int j=0; j<nsubs; ++j) {
			if (rid[sub[j]]>=0) {
				fprintf(stdout, " %s ", Pl.P[rid[sub[j]]].name);
				int sidj = FindOldcat(Pl.P[rid[sub[j]]].mnem, tid);
				if (sidj>=0 && oldcat[sidj][CAT_POST][0]=='P') {
					fprintf(stdout, " -> GK\n");
					return sub[j];
				}
			}
			else {
				fprintf(stdout, " %s ", roster[sub[j]]);
			}
		}
		fprintf(stdout, "\033[33m GK not found !\033[0m\n");
        return sub[0];
	}
	return sub[0];
}

int GK(int m, int t, int tid) {
	if (pmin[t*ROSTER_SIZE]>=m || m>90) {
		return t*ROSTER_SIZE;
	}
	else {
		return GKSub(t*ROSTER_SIZE, tid);
	}
}

void GetRoster(int r) {
	char sloc[128], *smin;
	for (int i=0; i<ROSTER_SIZE; i++) {
		strcpy(sloc, db[r][DB_ROSTER1+i]);
		strtok(sloc, ":");
		smin = strtok(NULL, ":");
		strcpy(roster[i], sloc);
		pmin[i] = strtol(smin, NULL, 10);

		strcpy(sloc, db[r][DB_ROSTER2+i]);
		strtok(sloc, ":");
		smin = strtok(NULL, ":");
		strcpy(roster[i+ROSTER_SIZE], sloc);
		pmin[i+ROSTER_SIZE] = strtol(smin, NULL, 10);

		rid[i] = Pl.FindMnem(roster[i]);
		rid[i+ROSTER_SIZE] = Pl.FindMnem(roster[i+ROSTER_SIZE]);
	}
}

void ResetStats() {
	NT = 0;
	for (int t=0; t<MAX_TEAMS; ++t) {
		npl[t] = 0;
		for (int p=0; p<MAX_ROSTER; ++p) {
			mec[t][p] = 0;
			tit[t][p] = 0;
			rez[t][p] = 0;
			gol[t][p] = 0;
			min[t][p] = 0;
			itg[t][p] = 0;
			ban[t][p] = 0;
			gre[t][p] = 0;
			pen[t][p] = 0;
			rat[t][p] = 0;
			aut[t][p] = 0;
			gal[t][p] = 0;
			ros[t][p] = 0;
		}
	}
}

int FindTID(int t) {
	for (int i=0; i<NT; ++i) {
		if (tid[i]==t) return i;
	}
	tid[NT++]=t;
	return NT-1;
}

int FindPID(int t, int p) {
	if (p<0) return -1;
	for (int i=0; i<npl[t]; ++i) {
		if (pid[t][i]==p) return i;
	}
	pid[t][npl[t]++]=p;
	return npl[t]-1;
}

int GetPID(int t, int p) {
	if (p<0) return -1;
	for (int i=0; i<npl[t]; ++i) {
		if (pid[t][i]==p) return i;
	}
	return -1;
}

int GetRID(char *s) {
	for (int i=0; i<2*ROSTER_SIZE; ++i) {
		if (strcmp(roster[i], s)==0) return i;
	}
	return -1;
}

int GetMinutes(int r, int pos) {
	char spi[128];
	strcpy(spi, db[r][pos]);
	strtok(spi, ":");
	int m = strtol(spi, NULL, 10);
	return m;
}

int tComp(const void *p, const void *q) {
	int a = *(int*)p;
	int b = *(int*)q;
	if (tid[a]>1000 || tid[b]>1000) return tid[a]-tid[b];
	return strcmp(L[tid[a]]->GetNick(year), L[tid[b]]->GetNick(year));
}

int pComp(const void *p, const void *q) {
	int a = *(int*)p;
	int b = *(int*)q;
	return strcmp(Pl.P[pid[stm][a]].name, Pl.P[pid[stm][b]].name);
}

void MakeNewCatalog() {
	int score, home, away, th, ta, p, m;
	int sch, sca;
	for (int r=0; r<NM; ++r) {
		GetRoster(r);
		home  = strtol(db[r][DB_HOME], NULL, 10);
		away  = strtol(db[r][DB_AWAY], NULL, 10);
		score = strtol(db[r][DB_SCORE], NULL, 10);
		sch   = score/100;
		sca   = score%100;
		int numg = sch + sca;
		th = FindTID(home);
		ta = FindTID(away);
		for (int i=0; i<ROSTER_SIZE; ++i) {
			p = FindPID(th, rid[i]);
			m = pmin[i];
			if (p>=0) {
				min[th][p] += m;
				if (i<11) {
					tit[th][p]++;
					mec[th][p]++;
					if (m==90) itg[th][p]++;
				}
				else {
					if (m>0) {
						rez[th][p]++;
						mec[th][p]++;
					}
					else ban[th][p]++;
				}
			}
			p = FindPID(ta, rid[i+ROSTER_SIZE]);
			if (p>=0) {
				m = pmin[i+ROSTER_SIZE];
				min[ta][p] += m;
				if (i<11) {
					tit[ta][p]++;
					mec[ta][p]++;
					if (m==90) itg[ta][p]++;
				}
				else {
					if (m>0) {
						rez[ta][p]++;
						mec[ta][p]++;
					}
					else ban[ta][p]++;
				}
			}
		}

		GetEvents(r);
		int ph, pa, gkh, gka;
		int schc = 0;
		int scac = 0;
		for (int e=0; e<nev; ++e) {
            if (evm[e] < SOME_TIME && evm[e] > PSO_TIME) continue; // pen shootout do not count for stats
			ph  = GetPID(th, rid[evp[e]]);
			pa  = GetPID(ta, rid[evp[e]]);
            if (comp!=COMP_NATL || tid[th]==ROM_NAT) {
  			  gkh = GetPID(th, rid[GK(evm[e], 0, th)]);
            } else {
              gkh = 0;
            }
            if (comp!=COMP_NATL || tid[ta]==ROM_NAT) {
  			  gka = GetPID(ta, rid[GK(evm[e], 1, ta)]);
            } else {
              gka = ROSTER_SIZE;
            }
			if (evt[e]==EV_GOAL || evt[e]==EV_PKGOAL) {
				if (evp[e]>=0 && evp[e]<ROSTER_SIZE) {
					schc++;
					if (ph>=0) {
						gol[th][ph]++;
						if (evt[e]==EV_PKGOAL) pen[th][ph]++;
					}
					if (gka>=0) gre[ta][gka]--;
				}
				else if (evp[e]>=ROSTER_SIZE && evp[e]<2*ROSTER_SIZE) {
					scac++;
					if (pa>=0) {
						gol[ta][pa]++;
						if (evt[e]==EV_PKGOAL) pen[ta][pa]++;
					}
					if (gkh>=0) gre[th][gkh]--;
				}
			}
			else if (evt[e]==EV_OWNGOAL) {
				if (evp[e]>=0 && evp[e]<ROSTER_SIZE) {
					scac++;
					if (ph>=0)  aut[th][ph]--;
					if (gkh>=0) gre[th][gkh]--;
				}
				else if (evp[e]>=ROSTER_SIZE && evp[e]<2*ROSTER_SIZE) {
					schc++;
					if (pa>=0) aut[ta][pa]--;
					if (gka>=0) gre[ta][gka]--;
				}
			}
            else if (evt[e]==EV_RED) {
              if (evp[e] >= 0) {
                if (evp[e]<ROSTER_SIZE) ros[th][ph]++; else ros[ta][pa]++;
              }
            }
            else if (evt[e]==EV_YELLOW) {
              if (evp[e] >= 0) {
                if (evp[e]<ROSTER_SIZE) gal[th][ph]++; else gal[ta][pa]++;
              }
            }
		}

		if (sch+sca != schc+scac && sca>=0) {
			fprintf(stdout, "[R%d] %s - %s [%d-%d -> %d-%d]\n", r+1,
				(home<1000?L[home]->GetNick(year):"XXX"),
				(away<1000?L[away]->GetNick(year):"XXX"), schc, scac, sch, sca);
			gkh = GetPID(th, rid[GK(1,0,th)]);
			gka = GetPID(ta, rid[GK(1,1,ta)]);
			int opt = 0;
			if ((sca!=scac && gkh>=0) || (sch!=schc && gka>=0)) {
				if (no_fix > 0 || comp==COMP_EURO || comp==COMP_NATL) opt = 1;
				else {
					fprintf(stdout, "Fix ? [0/1]: "); scanf("%d", &opt);
				}
				if (opt) {
					if (sca!=scac) {
						if (gkh>=0) gre[th][gkh] += (scac-sca);
					}
					if (sch!=schc) {
						if (gka>=0) gre[ta][gka] += (schc-sch);
					}
				}
			}
		}
	}

	for (int i=0; i<NT; ++i) tord[i] = i;
	qsort(tord, NT, sizeof(int), tComp);

	char filename[128];
	int nnft[1000];
	int nnfp[1000];
	int nnf = 0;
	switch (comp) {
		case COMP_CUPA:
			sprintf(filename, "catalogs/ncupa-%d.dat", year);
			break;
		case COMP_EURO:
			sprintf(filename, "catalogs/neuro-%d.dat", year);
			break;
		case COMP_NATL:
			sprintf(filename, "catalogs/nnat-%d.dat", year);
			break;
		default:
			sprintf(filename, "catalogs/ncatalog-%d.dat", year);
	}
	FILE *of = fopen(filename, "wt");
	if (!of) {
		fprintf(stdout, "ERROR: could not open file %s for writing.\n", filename);
		return;
	}
	for (int i=0; i<NT; ++i) {
		int t = tord[i];
		if (comp != COMP_NATL && tid[t]>999) continue;
        if (comp == COMP_NATL && tid[t] != ROM_NAT) continue;
		for (int i=0; i<npl[t]; ++i) pord[i] = i;
		stm = t;
		qsort(pord, npl[t], sizeof(int), pComp);
		for (int j=0; j<npl[t]; ++j) {
			int p = pord[j];
			char sn[128];
			Person *px = &(Pl.P[pid[t][p]]);
			sprintf(sn, "%s, %s", px->name, px->pren);
			sn[24] = 0;
			printf("%2d %-16s %-24s", j+1, (tid[t]<1000?L[tid[t]]->GetNick(year):"XXX"), sn);
			printf(" J:%2d T:%2d (%2d) R:%2d (%2d)  G:%2d (%d)  m:%4d  A:%-2d P:%-3d  Y:%d R:%d\n",
				mec[t][p], tit[t][p], itg[t][p], rez[t][p], ban[t][p],
				gol[t][p], pen[t][p],
				min[t][p],
                aut[t][p], gre[t][p], gal[t][p], ros[t][p]);
			int ci = FindOldcat(px->mnem, tid[t]);
			if (ci>=0) {
				fprintf(of, "%s,%s,%s,%s,%s,%s,%s,%s,",
					px->mnem, px->name, px->pren, oldcat[ci][CAT_DOB], oldcat[ci][CAT_NAT],
					oldcat[ci][CAT_CLUB], oldcat[ci][CAT_NR], oldcat[ci][CAT_POST]);
//					oldcat[ci][CAT_DOB], oldcat[ci][CAT_NAT], tid[t], oldcat[ci][CAT_NR], oldcat[CAT_POST]);
				fprintf(of, "%d,%d,%d,%d,%5d,%d,%d,%d,%d,%d,%d,%d,%d\n",
					mec[t][p], tit[t][p], rez[t][p], gol[t][p],
					min[t][p], itg[t][p], ban[t][p], gre[t][p], aut[t][p],
					pen[t][p], rat[t][p], gal[t][p], ros[t][p]);
			}
			else {
				nnft[nnf] = t;
				nnfp[nnf] = p;
				nnf++;
				fprintf(stdout, "  [%s] %s not found in old catalog (%s).\n", px->mnem, sn, 
					(tid[t]<1000?L[tid[t]]->GetNick(year):"XXX"));
				/*
				fprintf(stdout, "%s,%s,%s,%s,%s,%d, , ,%d,%d,%d,%d\n",
					px->mnem, px->pren, px->name, px->dob, px->cty, tid[t],
					mec[t][p], tit[t][p], rez[t][p], gol[t][p]);
				*/
			}
		}
	}
	for (int i=0; i<nnf; ++i) {
		int t = nnft[i];
		p = nnfp[i];
    Person *px = &(Pl.P[pid[t][p]]);
		char sn[128];
		sprintf(sn, "%s, %s", px->name, px->pren);
/*
		fprintf(stdout, "  [%s] %s not found in old catalog (%s).\n", px->mnem, sn, L[tid[t]]->GetNick(year));
*/
		fprintf(stderr, "%s,%s,%s,%s,%s,%d, , ,%d,%d,%d,%d\n",
			px->mnem, px->pren, px->name, px->dob, px->cty, tid[t],
			mec[t][p], tit[t][p], rez[t][p], gol[t][p]);
	}
	fclose(of);
}

//---------------------------------------------

int main(int argc, char* argv[]) {
  char filename[256];
	comp = COMP_LIGA;
  if (!Load()) {
    fprintf(stdout, "ERROR: called from invalid drectory.\n");
    return -1;
  }
  if (argc < 2) {
    fprintf(stdout, "ERROR: No edition specified.\n");
    fprintf(stdout, "Usage: new-catalog yyyy [0-3]\n");
    fprintf(stdout, "       0:liga, 1:cupa, 2:euro, 3:nat\n");
    return -1;
  }
	if (argc>2) {
        if (strcmp(argv[2], "-f")==0) no_fix = 1;
		comp = strtol(argv[2], NULL, 10);
		if (comp<COMP_LIGA || comp>=NUM_COMP) comp = COMP_LIGA;
	}

  char sarg1[128];
  strcpy(sarg1, argv[1]);
  year = atoi(sarg1);

  Pl.Load("catalogs/players.dat");
	NP = Pl.Size();

  LoadDB();
  LoadOldCatalog();
  LoadEvents();
  ResetStats();

  MakeNewCatalog();

  return 0;
}
