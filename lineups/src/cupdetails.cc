#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "catalog.hh"

#define MAX_LEVELS  12
#define MAX_RR	     4
#define MAX_N       64
#define MAX_TEAMS   2000
#define MAX_ROSTER	 100
#define CAT_ROWS    1000
#define CAT_COLS		  20
#define ROSTER_SIZE		22

#define MAX_NAMES  20000
#define DB_KROWS      60
#define DB_KCOLS      60
#define DB_KCELL      40
#define EV_COLS				30
#define PL_INITIAL	   0
#define PL_FULL_NAME 	 1

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

#define EV_GOAL		 0
#define EV_OWNGOAL	 1
#define EV_PKGOAL	 2
#define EV_PKMISS	 3
#define EV_YELLOW	 4
#define EV_RED	 	 5
#define EV_YELLOWRED	 6

//burdca,Carol,Burdan,00/00/1912,ROM,173, , ,3,3,0,-11
#define CAT_MNEM	 0
#define CAT_PREN	 1
#define CAT_NAME	 2
#define CAT_DOB		 3
#define CAT_CTTY	 4
#define CAT_TEAM	 5
#define CAT_NR		 6
#define CAT_POS		 7
#define CAT_CAPS	 8
#define CAT_TIT		 9
#define CAT_REZ		10
#define CAT_GSC		11
#define CAT_MIN	 	12
#define CAT_INT		13
#define CAT_BENCH	14
#define CAT_GREC	15

const char *month[] = {"", "Ian", "Feb", "Mar", "Apr", "Mai", "Iun",
                     "Iul", "Aug", "Sep", "Oct", "Noi", "Dec"};
const char *romonth[] = {"", "ianuarie", "februarie", "martie", "aprilie", "mai", "iunie",
                     "iulie", "august", "septembrie", "octombrie", "noiembrie", "decembrie"};

const char *evicon[] = {"g", "og", "pg", "pm", "cg" , "cr", "cgr"};
const char *cupround[] = {"Câºtigãtoare", "Finala", "Semifinale", "Sferturi", "Optimi", "ªaisprezecimi"};

const char *normal  = "\033[0m";
const char *yellow  = "\033[33m";
const char *hicyan    = "\033[36;1m";

char **club;
char **mnem;
int  NC, NM, NT;
char ssn[32];
int  year, score, home, guest;

int  num_winter;
int  *start_winter, *end_winter;
int  nev, evp[EV_COLS], evm[EV_COLS], evt[EV_COLS];

/* *************************************** */

char  db[DB_KROWS][DB_KCOLS][DB_KCELL];
char edb[DB_KROWS][EV_COLS][DB_KCELL];
char catalog[CAT_ROWS][128];
char lin[2*DB_KROWS][ROSTER_SIZE][DB_KCELL];
int  tid[MAX_N];
int  plid[MAX_N][MAX_ROSTER];
char rmnem[MAX_N][MAX_ROSTER][7];
int  npl[MAX_N];
int  processed[DB_KROWS];
int  msign[DB_KROWS];

int NP;

Catalog Pl, Ant, Arb;
Locations Loc;

FILE *of;
char sf[128];

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

//------------------------------------

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
  FILE *f;
  char s[2000], *tok[20], *ystr, *name, *nick;

  num_winter = 0;
  f = fopen("winter.dat", "rt");
  if (f!=NULL) {
    fscanf(f, "%d\n", &num_winter);
    start_winter = new int[num_winter];
    end_winter = new int[num_winter];
    for (int i=0; i<num_winter; i++) {
      fscanf(f, "%d %d\n", start_winter+i, end_winter+i);
    }
    fclose(f);
  }

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

int FindTid(int t) {
	for (int i=0; i<NT; i++) {
		if (tid[i]==t) return i;
	}
	return -1;
}


int NextMid(int a, int b) {
  for (int i=0; i<NM; i++) {
    if (!processed[i] && msign[i]==1000*a+b) return i;
  }
  return -1;
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

int isWinterSeason(int y) {
  for (int  i=0; i<num_winter; i++) {
    if (y>=start_winter[i] && y<=end_winter[i]) return 1;
  }
  return 0;
}

void SeasonName(int y, char *ss) {
  int winter = 0;
  for (int  i=0; i<num_winter; i++) {
    if (y>=start_winter[i] && y<=end_winter[i]) winter = 1;
  }
  if (winter) sprintf(ss, "%d", y);
  else sprintf(ss, "%d/%02d", y-1, y%100);
}

int isNumeric(char *s) {
	if (s==NULL) return 0;
	int l = strlen(s);
	for (int i=0; i<l; i++) {
		if (s[i]<'0' || s[i]>'9') return 0;
	}
	return 1;
}


void LoadCatalog() {
//burdca,Carol,Burdan,00/00/1912,ROM,173, , ,3,3,0,-11
  char filename[64], s[5000], *tk[CAT_COLS];
  FILE *f;
	NT = 0;
  sprintf(filename, "cupa-%d.dat", year);
  f = fopen(filename, "rt");
  if (f==NULL) { fprintf(stderr, "ERROR: database %s not found.\n", filename); return; }
	int i = 0;
  while (!feof(f)) {
    fgets(s, 5000, f);
		if (strlen(s)<20) continue;
		strncpy(catalog[i], s, 127);
    tk[0] = strtok(s, ",\n");
    for (int j=1; j<CAT_COLS; j++) tk[j]=strtok(NULL, ",\n");
		int tm = atoi(tk[CAT_TEAM]);
		int tix = FindTid(tm);
		if (tix<0) { tix = NT; tid[NT] = tm; npl[tix] = 0; NT++; }
		int pix = Pl.FindMnem(tk[CAT_MNEM]);
		plid[tix][npl[tix]] = pix;
		strncpy(rmnem[tix][npl[tix]], tk[CAT_MNEM], 6);
		npl[tix]++;
		s[0] = 0;
		i++;
  }
  fclose(f);
}

#define MIN(a,b) ((a)<(b)?(a):(b))

int dist(char*a, char*b) {
  int i, j, modif, ins, del;
  int m = strlen(a);
  int n = strlen(b);
  int **d = new int*[m+1];
  for (i=0; i<=m; i++) {
    d[i] = new int[n+1];
    d[i][0] = i;
  }
  for (i=0; i<=n; i++)
    d[0][i] = i;
  for (i=1; i<=m; i++)
    for (j=1; j<=n; j++) {
      if (a[i-1] == b[j-1]) modif = d[i-1][j-1];
        else modif = d[i-1][j-1] + 1;
      del = MIN(d[i-1][j], d[i][j-1]) + 1;
      d[i][j] = MIN(modif, del);
    }
  int res = d[m][n];
  // cleanup;
  for (int i=0; i<=m; i++)
    delete[] d[i];
  delete[] d;
  return res;
}

int MatchMnem(char *s, int tix) {
	if (tix<0) return -1;
	if (!s) return -1;
	if (s[0]==' ' || s[0]=='?') return -1;
	char ls[100];
	int delta[100];
	strncpy(ls, s, 96);
	char *tknm = strtok(ls, "+");
	char *tkg = strtok(NULL, "+");
	char *tkn = strtok(tknm, ":");
	char *tkm = strtok(NULL, ":");
	char *sep = strchr(tkn, ' ');
	char tkpr[128], *tkp[4];
	if (sep) {
		tkp[0] = strtok(tkn, " ");
		for (int j=1; j<4; j++) tkp[j] = strtok(NULL, " ");
		int npr = 3; while (tkp[npr]==NULL && npr>0) npr--;
		if (npr==2) sprintf(tkpr, "%s %s", tkp[0], tkp[1]);
		else if (npr==1) sprintf(tkpr, "%s", tkp[0]);
		tkn = tkp[npr];
	}
	for (int i=0; i<npl[tix]; ++i) {
		int pix = plid[tix][i];
		delta[i] = 2*dist(Pl.P[pix].name, tkn);
		if (tkpr!=NULL) {
			if (!strstr(Pl.P[pix].pren, tkpr)) delta[i]++;
		}
	}
	int best = -1;
	int anotherbest = -1;
	int numbest = 0;
	int mind = 4;
	for (int i=0; i<npl[tix]; ++i) {
			int pix = plid[tix][i];
			if (delta[i]<mind) {
				numbest = 0; best = i; mind = delta[i];
				anotherbest = -1;
			}
			else if (delta[i]==mind) {
				numbest++;
				if (numbest==2) anotherbest = i;
			}
	}
	if (numbest>1) {
		int pix = plid[tix][anotherbest];
		fprintf(stderr, "Another best match: %s %s.\n", Pl.P[pix].pren, Pl.P[pix].name);
	}
	if (best>=0) return plid[tix][best];
	return -1;
}

void Minutes(char *tkm, int pos, int min, char *sm) {
	if (!tkm) { strcpy(sm, "90"); return; }
	int vm = atoi(tkm);
	if (pos<11) sprintf(sm, "%d", vm-1);
	else sprintf(sm, "%d", min-vm+1);
}

void LoadRosters() {
//burdca,Carol,Burdan,00/00/1912,ROM,173, , ,3,3,0,-11
  char filename[64], s[5000], *tk[DB_KCOLS];
  FILE *f;
  sprintf(filename, "data/cupa-%d.lin", year);
  f = fopen(filename, "rt");
  if (f==NULL) { fprintf(stderr, "ERROR: database %s not found.\n", filename); return; }
	int i = 0, k;
	int tix[2], tm[2];
	char rpix[2*ROSTER_SIZE][DB_KCELL];
	char smin[16];
	int minpld;
  while (!feof(f)) {
    fgets(s, 5000, f);
		if (strlen(s)<2) continue;
		minpld = 90;
    char *tkt = strtok(s, ";\n");
		char *tkl = strtok(NULL, ";\n");
		char *tks = strtok(NULL, ";\n");
		for (int j=0; j<22; j++) tk[j]=NULL;
		tk[0] = strtok(tkl, ",");
    for (int j=1; j<11; j++) tk[j]=strtok(NULL, ",\n");
		if (tks) {
			tk[11] = strtok(tks, ",");
			for (int j=12; j<22; j++) tk[j] = strtok(NULL, ",");
		}
		k = i%2;
		tm[k] = -1;
		if (tkt) tm[k] = atoi(tkt);
		tix[k] = FindTid(tm[k]);
		if (tkt) strcpy(lin[i][0], tkt); else strcpy(lin[i][0], "-1");
		if (tm[k]>=0) fprintf(stderr, "#%d %s:\n", (i/2)+1, NameOf(L, tm[k], year));
		for (int j=0; j<ROSTER_SIZE; j++) strcpy(rpix[j+k*ROSTER_SIZE], "      :0  ");
		for (int j=0; j<ROSTER_SIZE; j++) {
			if (tk[j]) strcpy(lin[i][j+1], tk[j]); else strcpy(lin[i][j+1], " ");
			if (!tk[j]) continue;
			char *tknm = strtok(tk[j], "+");
			char *tkg  = strtok(NULL, "+");
			char *tkn  = strtok(tknm, ":");
			char *tkm  = strtok(NULL, ":");
			int pix = MatchMnem(lin[i][j+1], tix[k]);
			int mpldj = atoi(smin);
			if (mpldj>minpld) minpld = mpldj;
			Minutes(tkm, j, minpld, smin);
			if (pix>=0) {
				char sni[100];
				sprintf(sni, "%s %s", Pl.P[pix].pren, Pl.P[pix].name);
				fprintf(stderr, "%s%3d. %3s' [%s] %-30s [%10s] %s%s\n",
					yellow, j+1, smin, Pl.P[pix].mnem, sni, Pl.P[pix].dob, (tkg?tkg:""), normal);
				sprintf(rpix[j+k*ROSTER_SIZE], "%s:%-3s", Pl.P[pix].mnem, smin);
			}	else {
				if (tk[j]) if (tk[j][0]!='?') {
					fprintf(stderr, "%3d. %3s' [      ] %s\n", j+1, smin, tk[j]);
					sprintf(rpix[j+k*ROSTER_SIZE], "~%s:%-3s", tk[j], smin);
				}
			}
		}
    if (k==1) {
			int m = NextMid(tm[0], tm[1]);
			if (m<0) {
				fprintf(stderr, "%sError %d: no fixture for m#%d: %s-%s%s.\n",
					hicyan, year, (i+1)/2, NameOf(L, tm[0], year), NameOf(L, tm[1], year), normal);
			} else {
				processed[m] = 1;
				for (int j=0; j<2*ROSTER_SIZE; j++) {
					sprintf(db[m][DB_ROSTER1+j+(j/ROSTER_SIZE)], "%s", rpix[j]);
				}
			}
		}
		s[0] = 0;
		i++;
		if (k) fprintf(stderr, "\n");
  }
  fclose(f);
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
    processed[i] = 0;
    int a = atoi(db[i][DB_HOME]);
    int b = atoi(db[i][DB_AWAY]);
    msign[i] = 1000*a+b;
		i++;
		s[0] = 0;
  }
  fclose(f);
	NM = i;
}

void LoadEvents() {
  char filename[64], s[5000], *tk[DB_KCOLS];
  FILE *f;
  sprintf(filename, "cup-events-%d.db", year);
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

void GetRoster(int r, int a, int b) {
/*
*/
}

void GetEvents(int r, int a, int b) {
}

void trim(char *s) {
  if (s[1]==' ' || s[1]=='.') { s[1] = s[2]; s[2] = s[3]; }
  if (s[2]==' ' || s[2]=='.') { s[2] = s[3]; }
  s[3] = 0;
}

void AddDetails(int r) {
  char rfilename[128];
	int a = atoi(db[r][DB_HOME]);
	int b = atoi(db[r][DB_AWAY]);
	home = a;
	guest = b;
	score = atoi(db[r][DB_SCORE]);

  GetRoster(r, a, b);
  GetEvents(r, a, b);
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

void ShowProcessed() {
	printf("\n%d:", NM);
	for (int i=0; i<NM; i++) printf("%c", (processed[i]?'_':'X'));
	printf("\n");
	for (int i=0; i<NM; i++) if (!processed[i]) {
		printf(" Unprocessed #%d: %s-%s.\n", i+1, NameOf(L, msign[i]/1000, year), NameOf(L, msign[i]%1000, year));
	};
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

  char sarg1[128];
  strcpy(sarg1, argv[1]);
  year = atoi(sarg1);
  SeasonName(year, ssn);

  Pl.Load("players.dat");
	NP = Pl.Size();
  LoadDB();
  LoadEvents();
	LoadCatalog();
/*
  Ant.Load("coaches.dat");
  Arb.Load("referees.dat");
	Loc.Load("city.dat", "stadium.dat");
*/
	LoadRosters();

/*
	for (int i=0; i<NM; i++)
		AddDetails(i);
*/

	SaveDB();
	ShowProcessed();

  return 0;
}
