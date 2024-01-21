#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db.h"
#include "rhead.h"
#include "catalog.hh"

#define MAX_ROSTER 100
#define MNEM_LEN     7
#define FULL_LEN    64
#define MAX_ROUNDS  40
#define MAX_TEAMS	24
#define NUM_ORD	10
#define MAX_RR		 3
#define MAX_COL	50

#define DB_ROWS		400
#define DB_COLS		60
#define DB_CELL		20

#define DOB_DD_MM_YYYY  0
#define DOB_YYYYMMDD    1

extern const char *normal;
extern const char *black;
extern const char *red;
extern const char *green;
extern const char *yellow;
extern const char *blue;
extern const char *magenta;
extern const char *cyan;
extern const char *white;
extern const char *loblack;
extern const char *lored;
extern const char *logreen;
extern const char *loyellow;
extern const char *loblue;
extern const char *lomagenta;
extern const char *locyan;
extern const char *lowhite;
extern const char *hiblack;
extern const char *hired;
extern const char *higreen;
extern const char *hiyellow;
extern const char *hiblue;
extern const char *himagenta;
extern const char *hicyan;
extern const char *hiwhite;

int tm, NT, NR, NG, FC;
int tmid, oppid, tid;
char ofilename[FULL_LEN];
char rfilename[FULL_LEN];
char sfilename[FULL_LEN];
int borna[256];

char **club;
char **mnem;
int  NC;
int year;
int num_winter;
int *start_winter, *end_winter;
// --- schedule
char oppstr[MAX_ROUNDS][FULL_LEN];
int opp[MAX_ROUNDS];
int ha[MAX_ROUNDS];
int summ[MAX_ROUNDS];
int form[MAX_ROUNDS][23];
int oppfreq[MAX_TEAMS];
int hafreq[MAX_TEAMS];
char score[MAX_ROUNDS][7];
int nsub[MAX_ROUNDS], mid[MAX_ROUNDS], home[MAX_ROUNDS], post[MAX_ROUNDS];

char db[DB_ROWS][DB_COLS][DB_CELL];
char det[MAX_ROUNDS][DB_COLS][256];

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
Catalog P, Ant, Arb;
Locations W;

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

  f = fopen("teams.dat", "rt");
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
    club[i] = new char[32];
    memmove(mnem[i], s, 15); mnem[i][15] = 0;
    for (int j=0; j<30; j++) club[i][j] = 32;
    memmove(club[i], s+15, 30);
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
//      fprintf(stderr, "Alias (%d,%s~%s) for %d.%s...\n", y, name, (nick!=NULL?nick:""), i, club[i]);
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

/* *************************************** */

void trim(char *s) {
  if (s==NULL) return;
  int l = strlen(s);
  if (l<=1) return;
  int j = l-1;
  while ((s[j]==' ' || s[j]=='\t') && j>0) j--;
  int i = 0;
  while (s[i]==' ' && i<j) i++;
  if (i>0 || j<l-1) {
    s[j+1] = 0;
    memmove(s, s+i, j-i+1);
    s[j-i+1]=0;
  }
}

int isNumeric(char *s) {
  if (s==NULL) return 0;
  int l = strlen(s);
  if (l==0) return 0;
  for (int i=0; i<l; i++) if (s[i]<'0' || s[i]>'9') return 0;
  return 1;
}

int isNegative(char *s) {
  if (s==NULL) return 0;
  int l = strlen(s);
  if (l==0) return 0;
  if (s[0]!='-') return 0;
  for (int i=1; i<l; i++) if (s[i]<'0' || s[i]>'9') return 0;
  return 1;
}

void Mnem3(char *mn, char *m3) {
  int j = 0; 
  int k = 0;
  int l = strlen(mn);
  if (mn[0]=='F' && mn[1]=='C') { 
   while (j<l && mn[j]!=' ') j++; j++; 
  }
  while (k<3 && j<l) {
    if (mn[j]!='.' && mn[j]!=' ') { m3[k++] = mn[j]; } j++;
  }
  m3[k] = 0;
}

void Mnem4(char *mn, char *m3) {
  int j = 0; 
  int k = 0;
  int l = strlen(mn);
  if (mn[0]=='F' && mn[1]=='C') { 
   while (j<l && mn[j]!=' ') j++; j++; 
  }
  while (k<4 && j<l) {
    if (mn[j]!='.' && mn[j]!=' ') { m3[k++] = mn[j]; } j++;
  }
  m3[k] = 0;
}

int MIN(int x, int y) {
  if (x<y) return x;  
  else return y;
}

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


int isWinterSeason(int y) {
  for (int  i=0; i<num_winter; i++) {
    if (y>=start_winter[i] && y<=end_winter[i]) return 1;
  }
  return 0;
}

int getYear(int y, int r, int z) {
  if (isWinterSeason(y)) return y;
  if (z/50>7) return y-1;
  if (z/50==7) {
    if (r<NR/2) return y-1;
  }
  return y;
}

int id[MAX_TEAMS], res[MAX_RR][MAX_TEAMS][MAX_TEAMS], rnd[MAX_RR][MAX_TEAMS][MAX_TEAMS];

int FindTid(int t) {
  for (int i=0; i<NT; i++) {
    if (id[i]==t) return i;
  }
  return -1;
}

void LoadSeason() {
  char filename[64];
  sprintf(filename, "a.%d", year);
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

int FindID(int t) {
  for (int i=0; i<NT; i++) 
    if (id[i]==t) return i;
  return -1;
}

void LoadDB() {
  char filename[64], s[5000], *tk[DB_COLS];
  sprintf(filename, "lineups-%d.db", year);
  FILE *f = fopen(filename, "rt");
  if (f==NULL) { fprintf(stderr, "ERROR: database %s not found.\n", filename); return; }
  for (int i=0; i<NT*(NT-1); i++) {
    fgets(s, 5000, f);
    tk[0] = strtok(s, ",\n");
    for (int j=1; j<DB_COLS; j++) tk[j]=strtok(NULL, ",\n");
    for (int j=0; j<DB_COLS; j++) {
      if (tk[j]!=NULL) strcpy(db[i][j], tk[j]);
      else strcpy(db[i][j], " ");
    }
  }
  fclose(f);
}

void SaveDB() {
  char oldfilename[64];
  char newfilename[64];
  char filename[64];
  sprintf(filename,    "lineups-%d.db",  year);
  sprintf(oldfilename, "lineups-%d.old", year);
  sprintf(newfilename, "lineups-%d.new", year);
  FILE *f = fopen(newfilename, "wt");
  if (f==NULL) { fprintf(stderr, "ERROR: database %s not found.\n", newfilename); return; }
  for (int i=0; i<NT*(NT-1); i++) {
    for (int j=0; j<DB_COLS; j++) fprintf(f, "%s,", db[i][j]);
    fprintf(f, "\n");
  }
  fclose(f);
  rename(filename, oldfilename);
  rename(newfilename, filename);
}

#define MAX_DIST	1000

int MatchOpponent(char *sopp) {
  int d[MAX_TEAMS];
  int min_dist = MAX_DIST;
  int best_match = -1;
  for (int i=0; i<NT; i++) {
    d[i] = dist(sopp, NameOf(L, id[i], year));
//    d[i] = dist(sopp, NickOf(L, id[i], year));
    if (d[i] < min_dist) {
      min_dist = d[i];
      best_match = i;
    }
  }
  return best_match;
}

int FindHA(int a, int b, char *scr, int frq) {
  char s1[10], s2[10];
  sprintf(s1, "%d-%d", res[0][a][b]/100, res[0][a][b]%100);
  sprintf(s2, "%d-%d", res[0][b][a]%100, res[0][b][a]/100);
  if (strcmp(s1, s2)!=0) {
    if (strcmp(s1, scr)==0) return 0;
    if (strcmp(s2, scr)==0) return 1;
  }
  int r1 = rnd[0][a][b]/1000;
  int r2 = rnd[0][b][a]/1000;
  if (frq==1) {
    if (r1<r2) return 0;
    if (r2<r1) return 1;
  }
  else {
    if (r1<r2) return 1;
    if (r2<r1) return 0;
  }
  fprintf(stderr, "ERROR: Could not determine round for %s-%s [%s].\n", NickOf(L, id[a], year), NickOf(L, id[b], year), scr);
  int haw;
  fprintf(stderr, "Choose home/away for match #%d [0=home, 1=away]: ", frq);
  scanf("%d", &haw);
  return (haw!=0);
}

int FindOpp(int tmid, int r) {
   for (int i=0; i<NT; i++) {
      int r1 = rnd[0][tmid][i]/1000;
      int r2 = rnd[0][i][tmid]/1000;
      if (r1==r)  return i;
      if (r2==r)  return NT+i;
   }
   return -1;
}

void Extract(char *filename) {
  char s[5000], w[5000], u[5000], *tk[100], m3[5], smin[10];
  int pa, pb, sa[5], sb[5];
  int first, last, nopps;
  int postfreq[MAX_ROUNDS][22];

  FILE *f = fopen(filename, "rt");
  if (f==NULL) {
    fprintf(stderr, "ERROR: Cannot open file %s.\n", filename);
    return;
  }

  if (strchr(filename, '/')) {
    strcpy(s, filename);
    char *sy = strtok(s, "/");
    if (sy!=NULL) year = atoi(sy);
  }

  for (int i=0; i<MAX_ROUNDS; i++) {
    nsub[i] = 0;
    mid[i] = -1;
    home[i] = 0;
    post[i] = 0;
    summ[i] = 0;
    for (int j=0; j<22; j++) { 
      postfreq[i][j] = 0;
      form[i][j] = -1;
    }
  }

    fgets(w, 5000, f);
    if (w[0]=='#') {
      fgets(s, 5000, f);
      tk[0]= strtok(s, "\t");
      tm = atoi(tk[0]);
      tmid = FindID(tm);
      for (int i=1; i<50; i++) tk[i] = strtok(NULL, "\t\n");
      first = FC; 
      while (tk[first]!=NULL && (tk[first][0]==' ' || tk[first][0]=='_') && first < 50) first++;
      if (first>=50 || tk[first]==NULL) first = 4; /* default */
      for (int i=0; i<NT; i++) { oppfreq[i] = 0; hafreq[i] = 0; }
      nopps = 0;
      for (int i=0; i<NR; i++) {
        if (tk[first+i]) strcpy(oppstr[i], tk[first+i]); else strcpy(oppstr[i], " ");
        if (strcmp(oppstr[i], " ")!=0) nopps++;
        opp[i] = MatchOpponent(oppstr[i]);
      }

      fgets(s, 5000, f);
      tk[0]= strtok(s, "\t");
      for (int i=1; i<50; i++) tk[i] = strtok(NULL, "\t\n");
      first = FC; 
      while (tk[first]!=NULL && (tk[first][0]==' ' || tk[first][0]=='_') && first < 50) first++;
      if (first>=50 || tk[first]==NULL) first = 4;
      for (int i=0; i<NR; i++) { strcpy(score[i], tk[first+i]); }
      fprintf(stderr, "first column: %d\n", first);

      if (nopps==0) {
        fprintf(stderr, "Matching opponents by round #...\n");
        for (int i=0; i<NR; i++) {
          int xop = FindOpp(tmid, i+1);
          if (xop<0 || xop>=2*NT) { fprintf(stderr, "Could not find opponent in round %d.\n", i+1); }
          else if (xop<NT) {
            home[i] = 0;
            opp[i] = xop; oppid = xop;
            oppfreq[opp[i]]++;
            hafreq[oppid]++;
            strcpy(oppstr[i], NameOf(L, opp[i], year));
            mid[i] = tmid*(NT-1)+oppid-(oppid>tmid?1:0);
          }
          else {
            home[i] = 1;
            opp[i] = xop-NT; oppid = xop-NT;
            oppfreq[opp[i]]++;
            strcpy(oppstr[i], NameOf(L, opp[i], year));             
            mid[i] = oppid*(NT-1)+tmid-(tmid>oppid?1:0);
          }
        }
      }
      else {
        for (int i=0; i<NR; i++) {
          if (opp[i]>=0 && opp[i]<NT) {
            oppfreq[opp[i]]++;
            oppid = opp[i];
            if (tmid>=0 && oppid>=0) {
              int r1 = rnd[0][tmid][oppid]/1000;
              int r2 = rnd[0][oppid][tmid]/1000;
              int haw = FindHA(tmid, oppid, score[i], oppfreq[opp[i]]);
              if (haw==0) { mid[i] = tmid*(NT-1)+oppid-(oppid>tmid?1:0); home[i] = 0; hafreq[oppid]++;}
              if (haw==1) { mid[i] = oppid*(NT-1)+tmid-(tmid>oppid?1:0); home[i] = 1; }
            }
          }
        }
      }


      // --- Header info
      fprintf(stderr, "%s %d.\n", NameOf(L, tm, year), year);

      fprintf(stderr, "Opponents: \n");
      for (int i=0; i<NR; i++) {
        Mnem4(NickOf(L, id[opp[i]], year), m3);
        fprintf(stderr, "%2d%c%4s %s, ", i+1, (home[i]?'-':'+'), m3, score[i]);
        if (i==NR/2-1) fprintf(stderr, "\n");
      }
      fprintf(stderr, "\n");
      for (int i=0; i<NT; i++)  {
        if (oppfreq[i]!=2 && id[i]!=tm) {
          fprintf(stderr, "%sWARNING: oppfreq[%s] = %d.\n", normal, NickOf(L, id[i], year), oppfreq[i]);
        }
      }
      for (int i=0; i<NT; i++)  {
        if (hafreq[i]!=1 && id[i]!=tm) {
          fprintf(stderr, "%sWARNING: hafreq[%s] = %d.\n", normal, NickOf(L, id[i], year), hafreq[i]);
        }
      }
      fprintf(stderr, "%s", yellow);
    }

  fclose(f);
}

void AddName(Catalog C, char *xn) {
  char s[128], *tk[5];
	char sp[64], sn[64];
  int pix, cix, aix;

  if (strcmp(xn, "_")==0) return;
	strcpy(s, xn);
	tk[0] = strtok(s, " ");
	for (int j=1; j<5; j++) tk[j] = strtok(NULL, " ");
	int ntk = 0;
	for (int j=0; j<5; j++) if (tk[j]) ntk++;
  if (ntk==1) {
    strcpy(sp, " ");
		strcpy(sn, tk[0]);
		ntk++;
  }
	else if (ntk==2) {
		strcpy(sp, tk[0]);
		strcpy(sn, tk[1]);
	}
	else if (ntk==3) {
	  sprintf(sp, "%s %s", tk[0], tk[1]);
		sprintf(sn, "%s", tk[2]);
    ntk--;
  }
	if (ntk==2) {
		cix = C.GetByLastname(sn);
		if (cix>=0) {
			return;
		}
		pix = P.GetByLastname(sn);
		if (pix>=0) {
			fprintf(stderr, "%s: %s %s [%s]\n", P.P[pix].mnem, P.P[pix].pren, P.P[pix].name, P.P[pix].dob);
//			C.ForceAdd(P.P[pix]);
		}
		else {
//			C.Add(xn);
		}
	}
}


int FindName(Catalog C, char *xn) {
  char s[128], *tk[5];
  char sp[64], sn[64];
  int cix, aix;

  if (xn==NULL) return -1;
  if (strcmp(xn, "_")==0) return -1;
	strcpy(s, xn);
	tk[0] = strtok(s, " ");
	for (int j=1; j<5; j++) tk[j] = strtok(NULL, " ");
	int ntk = 0;
	for (int j=0; j<5; j++) if (tk[j]) ntk++;
  if (ntk==1) {
    strcpy(sp, " ");
		strcpy(sn, tk[0]);
		ntk++;
  }
	else if (ntk==2) {
		strcpy(sp, tk[0]);
		strcpy(sn, tk[1]);
	}
	else if (ntk==3) {
	  sprintf(sp, "%s %s", tk[0], tk[1]);
		sprintf(sn, "%s", tk[2]);
    ntk--;
  }
	if (ntk==2) {
		cix = C.BestMatch(sn, sp, 2);
		if (cix>=0) {
			return cix;
		}
	}
  return -1;
}


void Scream(char *s) {
	if (strchr(s, ',')) fprintf(stderr, "%sString %s contains \",\" !!!\n", hired, s);
}

void GetDetails(char *filename) {
  char s[5000], *tk[100], m4[5], pini[5];
  char lastm[128], sdb[128];
	int mn, x, y, q, z, loc, cix, lastx;


  FILE *f = fopen(filename, "rt");
  if (f==NULL) {
    fprintf(stderr, "ERROR: Cannot open file %s.\n", filename);
    return;
  }

  strcpy(lastm, " ");
  lastx = -1;
  do {
      fgets(s, 5000, f);
      tk[0] = strtok(s, "\t\n");
      for (int j=1; j<100; j++) tk[j] = strtok(NULL, "\t\n");
      if (tk[0]!=NULL && isNumeric(tk[0])) {
        mn = atoi(tk[0]);
        if (mn>=1 && mn<=NR) {
          if (home[mn-1]) {
            x = res[0][opp[mn-1]][tmid]%100;
            y = res[0][opp[mn-1]][tmid]/100;
			      q = rnd[0][opp[mn-1]][tmid]/1000;
            z = rnd[0][opp[mn-1]][tmid]%1000;
            loc = y;
          }
          else {
            x = res[0][tmid][opp[mn-1]]/100;
            y = res[0][tmid][opp[mn-1]]%100;
            q = rnd[0][tmid][opp[mn-1]]/1000;
            z = rnd[0][tmid][opp[mn-1]]%1000;
            loc = 0;
          }
          Mnem4(NickOf(L, id[opp[mn-1]], year), m4);
          char scol[32];
          strcpy(scol, yellow);
          if (x>y) strcpy(scol, green);
          if (x<y) strcpy(scol, white);
          fprintf(stderr, "%s#%2d %c%4s %s%d-%d%s: ", yellow, mn, (home[mn-1]?'-':'+'), m4, scol, x, y, normal);

          fprintf(stderr, "H:@%5s   sp:%6s   Ant:%-24s   Arb:%s\n", tk[RH_HOUR], tk[RH_SPECT], tk[RH_COACH], tk[RH_REF]);

          Scream(tk[RH_HOUR]);
          Scream(tk[RH_SPECT]);

          sprintf(db[mid[mn-1]][DB_ATTEND], "%6s", (tk[RH_SPECT]!=NULL?tk[RH_SPECT]:"      "));

          int sy = getYear(year, q, z);
          sprintf(db[mid[mn-1]][DB_DATE], "%02d-%02d-%d@%s", z%50, z/50, sy, (tk[RH_HOUR]!=NULL?tk[RH_HOUR]:"00:00"));

//          AddName(Ant, tk[RH_COACH]);
          if (tk[RH_COACH]!=NULL && strcmp(tk[RH_COACH], lastm)==0) {
            cix = lastx;
          }
          else {
             cix = FindName(Ant, tk[RH_COACH]);
          }

          if (cix>=0) {
            strcpy(lastm, tk[RH_COACH]);
            lastx = cix;
            if (home[mn-1])
              sprintf(db[mid[mn-1]][DB_COACH2], "%s", Ant.P[cix].mnem);
            else
              sprintf(db[mid[mn-1]][DB_COACH1], "%s", Ant.P[cix].mnem);
          }
			}
      s[0] = 0;
    }

  } while (!feof(f));
  fclose(f);
}

void trimParens(char *s) {
  char *p = strstr(s, " (");
  if (p!=NULL) p[0] = 0;
}

void LoadDetails(char *filename) {
  char s[5000], *tk[DB_COLS];
  FILE *f = fopen(filename, "rt");
  if (f==NULL) { fprintf(stderr, "ERROR: file %s not found.\n", filename); return; }
  while (!feof(f)) {
    fgets(s, 5000, f);
    tk[0] = strtok(s, "\t\n");
    if (tk[0]==NULL) continue;
    int r = atoi(tk[0]);
    for (int j=1; j<DB_COLS; j++) tk[j]=strtok(NULL, "\t\n");

    if (r<=0 || r>NR) continue;
    for (int j=0; j<DB_COLS; j++) {
      if (tk[j]!=NULL) strcpy(det[r][j], tk[j]);
      else strcpy(det[r][j], " ");
    }
  }
  fclose(f);
}

void AddDetails(char *filename) {
  int r, cix;

  for (int i=0; i<NT*(NT-1); i++) {
    int home = atoi(db[i][DB_HOME]);
    int away = atoi(db[i][DB_AWAY]);
    int th = FindTid(home);
    int ta = FindTid(away);
    r = rnd[0][th][ta]/1000;
    if (home==tid || away==tid) {


/*
//          int sy = getYear(year, q, z);
//          sprintf(db[mid[mn-1]][DB_DATE], "%02d-%02d-%d@%s", z%50, z/50, sy, (det[r][RH_HOUR]!=NULL?det[r][RH_HOUR]:"00:00"));
          Scream(det[r][RH_HOUR]);
          Scream(det[r][RH_SPECT]);

          sprintf(db[i][DB_ATTEND], "%6s", (det[r][RH_SPECT]!=NULL?det[r][RH_SPECT]:"      "));

          cix = FindName(Ant, det[r][RH_COACH]);
          if (cix>=0) {
            fprintf(stderr, "%s - %s: %s%s %s%s.\n", mnem[home], mnem[away],
              yellow, Ant.P[cix].pren, Ant.P[cix].name, normal);
            fprintf(stderr, "   H:@%5s   sp:%6s   Ant:%-24s   Arb:%s\n",
            det[r][RH_HOUR], det[r][RH_SPECT], det[r][RH_COACH], det[r][RH_REF]);
            if (home == tid) {
              if (strcmp(db[i][DB_COACH1], Ant.P[cix].mnem)!=0) {
                fprintf(stderr, "%s %s != %s %s\n", hired, db[i][DB_COACH1], Ant.P[cix].mnem, normal);
                sprintf(db[i][DB_COACH1], "%s", Ant.P[cix].mnem);
              }
            }
            else {
              if (strcmp(db[i][DB_COACH2], Ant.P[cix].mnem)!=0) {
                fprintf(stderr, "%s %s != %s %\n", hired, db[i][DB_COACH2], Ant.P[cix].mnem, normal);
                sprintf(db[i][DB_COACH2], "%s", Ant.P[cix].mnem);
              }
            }
          }
*/
          fprintf(stderr, "   H:@%5s   sp:%6s   Ant:%-24s   Arb:%s\n",
            det[r][RH_HOUR], det[r][RH_SPECT], det[r][RH_COACH], det[r][RH_REF]);
          trimParens(det[r][RH_REF]);

          int rix = FindName(Arb, det[r][RH_REF]);
          if (rix>=0) {
            fprintf(stderr, "%s - %s: %s%s %s%s.\n", mnem[home], mnem[away],
                yellow, Arb.P[rix].pren, Arb.P[rix].name, normal);
            if (strcmp(db[i][DB_REF], Arb.P[rix].mnem)!=0) {
              sprintf(db[i][DB_REF], "%s", Arb.P[rix].mnem);
            }
          }
    }
  }
}

int main(int argc, char **argv) {
  strcpy(rfilename, "");
  strcpy(sfilename, "");
  NT = 18; NR = 34; FC = 4;
  for (int k=1; k<argc; k++) {
         if (strcmp(argv[k], "-f")==0 && k<argc-1) strcpy(rfilename, argv[++k]);
    else if (strcmp(argv[k], "-s")==0 && k<argc-1) strcpy(sfilename, argv[++k]);
    else if (strcmp(argv[k], "-n")==0 && k<argc-1) { NT = atoi(argv[++k]);   NR = 2*(NT-1); }
    else if (strcmp(argv[k], "-m")==0 && k<argc-1) { NR = atoi(argv[++k]); }
    else if (strcmp(argv[k], "-y")==0 && k<argc-1) year = atoi(argv[++k]);
    else if (strcmp(argv[k], "-t")==0 && k<argc-1) tid  = atoi(argv[++k]);
  }
  if (strlen(rfilename)==0) {
    fprintf(stderr, "ERROR: input file not specified.\n");
    return -1;
  }
  if (NT < 12 || NT > 20) {
    fprintf(stderr, "ERROR: invalid number of teams %d.\n", NT);
    return -2;
  }

  Load();
  LoadSeason();
  LoadDB();
  P.Load("players.dat");
  Ant.Load("coaches.dat");
  Arb.Load("referees.dat");
  W.Load("city.dat", "stadium.dat");

//  Extract(rfilename);
//  GetDetails(sfilename);
  LoadDetails(sfilename);
  AddDetails(sfilename);

  SaveDB();
//	Ant.Save("coaches.dat");

  return 0;
}
