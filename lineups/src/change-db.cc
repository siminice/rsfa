#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEVELS  12
#define MAX_RR	     4
#define MAX_N       64
#define MAX_TEAMS 2000

#define MAX_NAMES       20000
#define DB_ROWS         400
#define DB_COLS         60
#define DB_CELL         20
#define EV_COLS		30
#define PL_INITIAL	 0
#define PL_FULL_NAME	 1

#define DB_HOME		 0
#define DB_GUEST	 1
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

#define SPECIAL		50
#define LOSS_BOTH_0	50
#define LOSS_BOTH_9	59

const char *month[] = {"", "Ian", "Feb", "Mar", "Apr", "Mai", "Iun",
                     "Iul", "Aug", "Sep", "Oct", "Noi", "Dec"};
const char *romonth[] = {"", "ianuarie", "februarie", "martie", "aprilie", "mai", "iunie",
                     "iulie", "august", "septembrie", "octombrie", "noiembrie", "decembrie"};

const char *evicon[] = {"g", "og", "pg", "pm", "cg" , "cr", "cgr"};

char **club;
char **mnem;
int  NC;
int  id[MAX_N], win[MAX_N], drw[MAX_N], los[MAX_N], gsc[MAX_N], gre[MAX_N], pts[MAX_N], pen[MAX_N], pdt[MAX_N];
int  asez[MAX_TEAMS], awin[MAX_TEAMS], adrw[MAX_TEAMS], alos[MAX_TEAMS], agre[MAX_TEAMS], agsc[MAX_TEAMS], arank[MAX_TEAMS];
int  round[MAX_RR][MAX_N][MAX_N], res[MAX_RR][MAX_N][MAX_N], allres[MAX_RR][MAX_N][MAX_N];
int  rank[MAX_N];
int  pos[MAX_N][365];
char desc[MAX_N][32];
char ssn[32];

int  lid[MAX_N], lwin[MAX_N], ldrw[MAX_N], llos[MAX_N], lgsc[MAX_N], lgre[MAX_N], lpts[MAX_N], lpen[MAX_N], lpdt[MAX_N];
int  hwin[MAX_N], hdrw[MAX_N], hlos[MAX_N], hgsc[MAX_N], hgre[MAX_N], hpts[MAX_N];
int  gwin[MAX_N], gdrw[MAX_N], glos[MAX_N], ggsc[MAX_N], ggre[MAX_N], gpts[MAX_N];
int  prank[MAX_N], hrank[MAX_N], grank[MAX_N];
int  tbwin[MAX_N], tbdrw[MAX_N], tblos[MAX_N], tbgsc[MAX_N], tbgre[MAX_N], tbrk[MAX_N];
int  topsc[MAX_NAMES];
int  n, rr, r, lastr, lastrnd, tbr, pr1, pr2, rel1, rel2, ppv, year, numr;
int  NG;
bool winter;
int num_winter;
int *start_winter, *end_winter;
int roster[44];
int nev, evp[EV_COLS], evm[EV_COLS], evt[EV_COLS];

/* *************************************** */

char  db[DB_ROWS][DB_COLS][DB_CELL];
char edb[DB_ROWS][EV_COLS][DB_CELL];

int NP;
char **pmnem, **pname, **ppren, **pdob, **pcty, **ppob, **pjud;
int *psez, *pmeci, *ptit, *pint, *prez, *pban, *pmin, *pgol, *pgre, *prnk;
int *csez, *cmeci, *ctit, *cint, *crez, *cban, *cmin, *cgol, *cgre, *crnk;


int borna[256];

int  fd, ld, fr, frd, lr, lrd, decorate, calendar;
FILE *of;
char dvz;
int  seria;
char sf[128];
int  played[650];
int  sched[650][24];
int  num_sched, first_d, first_r, last_d, last_r;

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

int idof(int x) {
  int j=0;
  while (j<n && lid[j]!=x) j++;
  if (j>=n) {
    printf("Error: Could not match %s in preloaded file.\n", mnem[x]);
    return -1;
  }
  return j;
}

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
  for (int i=0; i<NC; i++) {
    asez[i] = awin[i] = alos[i] = agsc[i] = agre[i] = 0;
    arank[i] = i;
  }
  return 1;
}

void LoadPlayers() {
  NP = 0;
  int c;
  for (c=0; c<256; c++) borna[c] = -1;
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
  ppob  = new char*[MAX_NAMES];
  pjud  = new char*[MAX_NAMES];

  for (int i=0; i<NP; i++) {
    fgets(s, 100, f);
//    fprintf(stderr, "... parsing %s", s);
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
    t[5] = strtok(NULL, ",\t\n");
     if (t[5]!=NULL) {
       ppob[i] = new char[strlen(t[5])+1];
       strcpy(ppob[i], t[5]);
     }
     else {
       ppob[i] = new char[2];
       strcpy(ppob[i], "");
     }
    t[6] = strtok(NULL, ",\t\n");
     if (t[6]!=NULL) {
       pjud[i] = new char[strlen(t[6])+1];
       strcpy(pjud[i], t[6]);
     }
     else {
       pjud[i] = new char[2];
       strcpy(pjud[i], "");
     }

     if (i==0 || (i>0 && pmnem[i-1][0]!=pmnem[i][0])) {
       c = ((unsigned char) pmnem[i][0]);
       borna[c] = i;
     }
  }
  fclose(f);

  c = ((unsigned char)pmnem[NP-1][0]);
  borna[c+1] = NP;
  for (c=255; c>0; c--)
    if (borna[c]>=0 && borna[c-1]<0) borna[c-1] = borna[c];
}

int FindMnem(char *mnem) {
  int c = ((unsigned char)mnem[0]);
  for (int i=borna[c]; i<borna[c+1]; i++)
    if (strcmp(pmnem[i], mnem)==0) return i;
  return -1;
}

void GetInitial(int px, char *pini) {
    pini[1] = '.'; pini[2] = 0;
    if (px<0 || ppren[px]==NULL || ppren[px][0]==' ') pini[0] = 0;
    else if (strlen(ppren[px])>0) pini[0] = ppren[px][0];
    else pini[0] = 0;
}

void LoadDB() {
  char filename[64], s[5000], *tk[DB_COLS];
  FILE *f;
  sprintf(filename, "lineups-%d.db", year);
  f = fopen(filename, "rt");
  if (f==NULL) { fprintf(stderr, "ERROR: database %s not found.\n", filename); return; }
  for (int i=0; i<n*(n-1); i++) {
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

void LoadEvents() {
  char filename[64], s[5000], *tk[DB_COLS];
  FILE *f;
  sprintf(filename, "events-%d.db", year);
  f = fopen(filename, "rt");
  if (f==NULL) { fprintf(stderr, "ERROR: database %s not found.\n", filename); return; }
  for (int i=0; i<n*(n-1); i++) {
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

int after(int dt, int r) {
  if (winter) return (dt<r);
  if (dt==0) return 1;
  if (dt>=fd) {
    if (r<fd) return 1;
    else return (dt<r);
  }
  if (r>=fd) return 0;
  else return (dt<r);
}


void FindFirstRound() {
  first_r = 2000;
  for (int i=0; i<n; i++) {
    for (int j=0; j<n; j++) {
      if (allres[0][i][j]>=0 && round[0][i][j]>0) {
        int r = round[0][i][j]/1000;
        int z = round[0][i][j]%1000;
        if (r==1 && z<first_r) first_r = z;
      }
    }
  }
  fprintf(stderr, "First round date: %d\n", first_r);
}

int FindPos(int *rk, int n, int a) {
  for (int i=0; i<n; i++)
    if (rk[i]==a) return i+1;
  return 0;
}

void CollectSched(int calendar) {
  int k;
  num_sched = 0;
  last_r = 0;
  for (int h=0; h<rr; h++) {
  for (int i=0; i<n; i++) {
    for (int j=0; j<n; j++) {
      if (i!=j && allres[h][i][j]>=0) {
        int r = round[h][i][j]/1000;
        int z = round[h][i][j]%1000;
        if (h>0 && r<numr) r += h*numr;
        played[z] = 1;
        if (r>last_r) last_r = r;
        if (round[h][i][j] > 0) {
        if (calendar) {
          if (sched[z][0]==0) num_sched++;
          if (sched[z][0]>=19) {
             fprintf(stderr, "ERROR: too many matches on %d.\n", z);
             return;
          }
          k = ++sched[z][0];
          sched[z][k] = 100*i+j;
        }
        else {
          if (sched[r][0]==0) num_sched++;
          if (sched[r][0]>=19) {
             fprintf(stderr, "ERROR: too many matches in round %d.\n", r);
             return;
          }
          k = ++sched[r][0];
          sched[r][k] = 100*i+j;
        }
        }
      }
    }
  }
  }

    last_d = ld%650;
    while (last_d > 0 && played[last_d]==0) last_d--;
    if (last_d==0) {
      last_d = 631;
      while (last_d > 0 && played[last_d]==0) last_d--;
    }
    first_d = fd;
    while (first_d < 631 && played[first_d]==0) first_d++;
}

//---------------------------------------------

int main(int argc, char* argv[]) {
  char filename[256];
  if (!Load()) {
    printf("ERROR: called from invalid drectory.\n"); 
    return -1;     
  }
  if (argc < 2) { 
    printf("ERROR: No input file specified.\n"); 
    return -1; 
  }
  sprintf(sf, "%s", argv[1]);
  dvz = sf[0] - 32;
  char spl[3];
  if (sf[1]=='.') seria = 0;
  else {
    spl[0] = sf[1]; spl[1] = 0;
    if (sf[2]!='.') {
      spl[1] = sf[2]; spl[2] = 0;
    }
    seria = atoi(spl);
  }

  char sarg1[128];
  strcpy(sarg1, argv[1]);
  char *ystr = strtok(sarg1, ".");
  ystr = strtok(NULL, " ");
  if (ystr) year = atoi(ystr); else year = 0;
  SeasonName(year, ssn);
  bool full = true;
  winter = false;
  fd = -1;
  ld = -1;
  NG = 0;

  decorate = 1;
  calendar = 1;
  for (int i=0; i<650; i++) played[i] = 0;
  for (int i=0; i<650; i++) for (int j=0; j<20; j++) sched[i][j] = 0;
  if (argc > 2) {
    int i=2; 
    do {
      if (strcmp(argv[i],"-")==0) full = false;
      else if (argv[i][0] == '-') {
        if (argv[i][1] == 'w') {
          winter = true;
          fd = 1;
          ld = 650;
        }
        else if (strcmp(argv[i],"-fd")==0) {
          fd = atoi(argv[++i]);
//          ld = fd + 649;
        }
        else if (strcmp(argv[i],"-ld")==0) {
          ld = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "-g")==0) {
          NG = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "-d")==0) {
          decorate = 0;
        }
        else
          printf("Unknown option: %s\n", argv[i]);
      }
      else printf("Illegal command line argument: %s\n", argv[i]);
      i++;
    } while (i<argc);
  }

  CollectSched(1);
  LoadPlayers();
  LoadPlayerStats();
  LoadDB();
  LoadEvents();

  if (fd < 0) {
    FindFirstRound();
    fd = first_r;
  }
  if (fd<0) fd = 415;
  if (ld<0) ld = fd+649;

  int nd = 0;
  for (int i=fd; i<=ld; i++) {
    int r = i%650;
    if (played[r]) { PrintDate(r); }
  }

  SaveAccumulatedStats();
  return 0;
}
