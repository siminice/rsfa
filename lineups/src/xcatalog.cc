#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int verbosity;
int tm, pl, cr;
int FY, LY;

char **club;
char **mnem;
int  NC;
int year;

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
  FILE *f;
  char s[2000], *tok[20], *ystr, *name, *nick;
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

#define MAX_NAMES	20000
#define MAX_DB		800

#define TD_MNEM		 0
#define TD_PRENUME	 1
#define TD_NUME		 2
#define TD_DOB		 3
#define TD_NAT		 4
#define TD_CLUBID	 5
#define TD_NR		 6
#define TD_POST		 7
#define TD_MECIURI	 8
#define TD_TITULAR	 9
#define TD_REZERVA	10
#define TD_GOLURI	11
#define TD_SEZOANE	12
#define TD_FY		13
#define TD_LY		14
#define TD_NUM		TD_GOLURI+1
#define TD_XTRA		TD_LY+1

#define CR_M	0
#define	CR_G	1
#define CR_T	2
#define CR_R	3
#define CR_S	4

char ****db;

/* *************************************** */

int NP;
char **pmnem;
char **pname;
char **ppren;
char **pdob;
char **pcty;

int *psez;
int *pfy;
int *ply;
int *pmeci;
int *ptit;
int *prez;
int *pgol;
int *rank;

char ofilename[128];
FILE *of;

int FindMnem(char *mnem) {
  for (int i=0; i<NP; i++)
    if (strcmp(pmnem[i], mnem)==0) return i;
  return -1;
}

void LoadPlayers() {
  NP = 0;
  FILE *f = fopen("players.dat", "rt");
  if (f==NULL) {
    fprintf(stderr, "ERROR: player database not found!\n");
    exit(0);
  }
  char s[100], *t[10];
  fgets(s, 100, f);
  sscanf(s, "%d", &NP);
  pname = new char*[MAX_NAMES];
  pmnem = new char*[MAX_NAMES];
  ppren = new char*[MAX_NAMES];
  pdob  = new char*[MAX_NAMES];
  pcty  = new char*[MAX_NAMES];

  psez = new int[MAX_NAMES];
  pfy = new int[MAX_NAMES];
  ply = new int[MAX_NAMES];
  pmeci = new int[MAX_NAMES];
  ptit = new int[MAX_NAMES];
  prez = new int[MAX_NAMES];
  pgol = new int[MAX_NAMES];
  rank = new int[MAX_NAMES];

  for (int i=0; i<NP; i++) {
    fgets(s, 100, f);
    t[0] = strtok(s, ",\t\n");
     pmnem[i] = new char[7];
     strcpy(pmnem[i], t[0]);
    t[1] = strtok(NULL, ",\t\n");
     pname[i] = new char[strlen(t[1])+1];
     strcpy(pname[i], t[1]);
    t[2] = strtok(NULL, ",\t\n");
     ppren[i] = new char[strlen(t[2])+1];
     strcpy(ppren[i], t[2]);
    t[3] = strtok(NULL, ",\t\n");
     pdob[i] = new char[12];
     strcpy(pdob[i], t[3]);
    t[4] = strtok(NULL, ",\t\n");
     pcty[i] = new char[4];
     strcpy(pcty[i], t[4]);
     psez[i] = pfy[i] = ply[i] = pmeci[i] = ptit[i] = prez[i] = pgol[i] = 0;
     rank[i] = i;
  }
  fclose(f);
  fprintf(stderr, "Loaded %d names.\n", NP);
}

int ResetStats() {
  for (int i=0; i<NP; i++) {
     psez[i] = pfy[i] = ply[i] = pmeci[i] = ptit[i] = prez[i] = pgol[i] = 0;
     rank[i] = i;
  }
}

#define DOB_DD_MM_YYYY	0
#define DOB_YYYYMMDD	1

int NumericDOB(char *dob, int fmt) {
  char s[12];
  strcpy(s, dob);
  char *sd = strtok(s, "/.-");
  char *sm = strtok(NULL, "/.-");
  char *sy = strtok(NULL, "/.-");
  int xd = 0;
  if (sd!=NULL) xd = atoi(sd);
  int xm = 0;
  if (sm!=NULL) xm = atoi(sm);
  int xy = 0;
  if (sy!=NULL) xy = atoi(sy);
  if (xm>12 && xd<13) { /* inverseaza luna/ziua */
    int x = xm; xm = xd; xd = x;
  }
  if ((sd!=NULL && sm==NULL && sy==NULL) || (xd>0 && xm==0 && xy==0)) {
    xy = xd; xm = 0; xd = 0;
  }
  if (xy>0 && xy<100) xy = 1900+xy;
  return 10000*xy+100*xm+xd;
}

void CanonicDOB(char *dob, int fmt) {
  char s[12];
  strcpy(s, dob);
  char *sd = strtok(s, "/.-");
  char *sm = strtok(NULL, "/.-");
  char *sy = strtok(NULL, "/.-");
  int xd = 0;
  if (sd!=NULL) xd = atoi(sd);
  int xm = 0;
  if (sm!=NULL) xm = atoi(sm);
  int xy = 0;
  if (sy!=NULL) xy = atoi(sy);
  if (xm>12 && xd<13) {
    int x = xm; xm = xd; xd = x;
  }
  if ((sd!=NULL && sm==NULL && sy==NULL) || (xd>0 && xm==0 && xy==0)) {
    xy = xd; xm = 0; xd = 0;
  }
  if (xy>0 && xy<100) xy = 1900+xy;
  if (fmt==DOB_DD_MM_YYYY) {
    sprintf(dob, "%02d/%02d/%04d", xd, xm, xy);
  }
  else if (fmt==DOB_YYYYMMDD) {
    sprintf(dob, "%04d%02d%02d", xy, xm, xd);
  }
}

void Collect(int year) {
  char filename[128];
  char s[1000], *tok[100];
  int sm, st, sr, sg;

  sprintf(filename, "catalog-%d.dat", year);
  FILE *f = fopen(filename, "rt");
  if (f==NULL) {
    fprintf(stderr, "ERROR: Cannot open file %s.\n", filename);
    return;
  }
  
  int i = 0;
  int n = year - FY;
  do {
    fgets(s, 1000, f);
    if (strlen(s)<10) continue;
    tok[0] = strtok(s, ",\n");
    for (int k=1; k<TD_NUM; k++) tok[k] = strtok(NULL, ",\n");

    db[n][i] = new char*[TD_NUM];
    for (int k=0; k<TD_NUM; k++) {
      int l = 0;
      if (tok[k]!=NULL) {
        l = strlen(tok[k]);
        db[n][i][k] = new char[l+1];
        strcpy(db[n][i][k], tok[k]);
      }
    }    
    i++;
    db[n][i] = NULL;

    s[0] = 0;
  } while (!feof(f));
  fclose(f);
}

void Stats(int year, int t, int pl) {
  int sm, st, sr, sg, xt;
  int n = year - FY;
  int i = 0;

  while (i<MAX_DB && db[n][i]!=NULL) {
    int p = FindMnem(db[n][i][TD_MNEM]);
    if (p<0) {
      fprintf(stderr, "ERROR: mnemonic ID %s not found.\n", db[n][i][TD_MNEM]);
      i++;
      continue;
    }
    xt = -1;
    if (db[n][i][TD_CLUBID]) xt = atoi(db[n][i][TD_CLUBID]);
    if (t>=0 && xt!=t) {
      i++;
      continue;
    }
    sm = st = sr = sg = 0;
    if (db[n][i][TD_MECIURI]) sm = atoi(db[n][i][TD_MECIURI]);
    if (db[n][i][TD_TITULAR]) st = atoi(db[n][i][TD_TITULAR]);
    if (db[n][i][TD_REZERVA]) sr = atoi(db[n][i][TD_REZERVA]);
    if (db[n][i][TD_GOLURI])  sg = atoi(db[n][i][TD_GOLURI]);
  
    pmeci[p] += sm;
    ptit[p]  += st;
    prez[p]  += sr;
    pgol[p]  += sg;

    if (year!=pfy[p] && year!=ply[p]) psez[p]++;
    if (pfy[p]==0) pfy[p] = year;
    if (year<pfy[p]) pfy[p] = year;
    if (year>ply[p]) ply[p] = year;

    i++;
  }
}

void Ranking(int cr) {
  int sorted, last;
  for (int i=0; i<NP; i++) rank[i] = i;
  last = NP-1;
  do {
    sorted = 1;
    for (int i=0; i<NP-1; i++) { 
      if (pmeci[rank[i+1]] > pmeci[rank[i]]) {
        sorted = 0;
        int aux = rank[i]; rank[i] = rank[i+1]; rank[i+1] = aux;
      }
    }
  } while (sorted==0);
}

void HTMLHeader(int t) {  
  if (t<0)
    fprintf(of, "<HTML>\n<TITLE>Loturi Divizia A %d-%d</TITLE>\n", FY, LY);
  else 
    fprintf(of, "<HTML>\n<TITLE>Loturi %s în Divizia A %d-%d</TITLE>\n", NameOf(L, t, 3000), FY, LY);
  fprintf(of, "<HEAD>\n<link href=\"css/seasons.css\" rel=\"stylesheet\" type=\"text/css\"/>\n");
  fprintf(of, "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-2\">\n");
  fprintf(of, "</HEAD>\n<BODY>\n");
}

void HTMLTable(int t) {
  fprintf(of, "<script src=\"sorttable.js\"></script>\n");
  if (t<0)
    fprintf(of, "<H3>Jucãtori în campionatul României %d-%d</H3>\n", FY, LY);
  else 
    fprintf(of, "<H3>Jucãtori %s %d-%d</H3>\n", NameOf(L, t, 3000), FY, LY);
  fprintf(of, "<TABLE class=\"sortable\" cellpadding=\"2\" frame=\"box\">\n");  
  fprintf(of, "<THEAD><TR>\n");
  fprintf(of, "<TH>#</TH>");
  fprintf(of, "<TH>Prenume</TH>");
  fprintf(of, "<TH>Nume</TH>");
  fprintf(of, "<TH>Data naºterii</TH>");
  fprintf(of, "<TH>Naþionalitate</TH>");
  fprintf(of, "<TH>Sezoane</TH>");
  fprintf(of, "<TH>Primul</TH>");
  fprintf(of, "<TH>Ultimul</TH>");
  fprintf(of, "<TH>Meciuri</TH>");
  fprintf(of, "<TH>Titular</TH>");
  fprintf(of, "<TH>Rezervã</TH>");
  fprintf(of, "<TH>Goluri</TH>");
  fprintf(of, "</TR></THEAD>\n"); 

  for (int i=0; i<NP; i++) {
    int x = rank[i];
    if (pmeci[x]==0) continue;
    fprintf(of, "<TR");
    if (i%2==1) fprintf(of, " BGCOLOR=\"DDFFFF\"");
    fprintf(of, ">");
    fprintf(of, "<TD align=\"right\">%d.</TD>", i+1);
    fprintf(of, "<TD align=\"left\">%s</TD>", ppren[x]);
    fprintf(of, "<TD align=\"left\" sorttable_customkey=\"%s,%s\"><A HREF=\"jucatori/%03d/%03d.html\">%s</A></TD>",
        pname[x], ppren[x], x/1000, x%1000, pname[x]);
    CanonicDOB(pdob[x], DOB_DD_MM_YYYY);
    fprintf(of, "<TD align=\"right\" sorttable_customkey=\"%d\">%s</TD>", NumericDOB(pdob[x], DOB_YYYYMMDD), pdob[x]);
    fprintf(of, "<TD align=\"center\">%s<IMG SRC=\"../../thumbs/22/3/%s.png\"></IMG></TD>", pcty[x], pcty[x]);
    fprintf(of, "<TD align=\"right\">%d</TD>", psez[x]);
    fprintf(of, "<TD align=\"right\">%d</TD>", pfy[x]);
    fprintf(of, "<TD align=\"right\">%d</TD>", ply[x]);
    fprintf(of, "<TD align=\"right\">%d</TD>", pmeci[x]);
    fprintf(of, "<TD align=\"right\">%d</TD>", ptit[x]);
    fprintf(of, "<TD align=\"right\">%d</TD>", prez[x]);
    fprintf(of, "<TD align=\"right\">%d</TD>", pgol[x]);
    fprintf(of, "</TR>\n");
  }
  fprintf(of, "\n</TABLE>\n");
}

void HTMLFooter() {
  fprintf(of, "</BODY>\n</HTML>");
  fclose(of);
}

int main(int argc, char **argv) {

  verbosity = 2;
  FY = 1933;
  LY = 2013;
  tm = -1;
  pl = -1;
  cr = CR_M;

  for (int k=1; k<argc; k++) {
    if (strcmp(argv[k], "-q")==0) verbosity = 0;
    if (strcmp(argv[k], "-fy")==0 && k<argc-1)  {
      FY = atoi(argv[++k]);
    }
    else if (strcmp(argv[k], "-ly")==0 && k<argc-1)  {
      LY = atoi(argv[++k]);
    }
    else if (strcmp(argv[k], "-t")==0 && k<argc-1)  {
      tm = atoi(argv[++k]);
    }
    else if (strcmp(argv[k], "-rk")==0 && k<argc-1)  {
      cr = atoi(argv[++k]);
    }
  }

  Load();
  LoadPlayers();

  int nums = LY-FY+1;
  db = new char***[nums];
  for (int s=0; s<nums; s++) {
     db[s] = new char**[MAX_DB];
  }

  for (year=FY; year<=LY; year++) {
    Collect(year);
  }

  for (year=FY; year<=LY; year++) {
    Stats(year, tm, pl);
  }

  Ranking(cr);

  sprintf(ofilename, "html/catalog.html", year);
  of = fopen(ofilename, "wt");
  if (of==NULL) {
    fprintf(stderr, "ERROR: could not open file %s.\n", ofilename);
    return 1;
  }
  HTMLHeader(tm);
  HTMLTable(tm);
  HTMLFooter();
  
  fclose(of);

  if (tm<0) for (int t=0; t<NC; t++) {
    for (year=FY; year<=LY; year++) {
      Stats(year, tm, pl);
    } 

    Ranking(cr); 

    sprintf(ofilename, "html/lot-%d.html", t);
    of = fopen(ofilename, "wt");
    if (of==NULL) {
      fprintf(stderr, "ERROR: could not open file %s.\n", ofilename);
      continue;
    }
    HTMLHeader(t);
    HTMLTable(t);
    HTMLFooter();
    
    fclose(of);
  }
 
  return 0;
}
