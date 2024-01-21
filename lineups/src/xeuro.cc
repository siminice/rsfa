#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *normal  = "\033[0m";
const char *black   = "\033[30m";
const char *red     = "\033[31m";
const char *green   = "\033[32m";
const char *yellow  = "\033[33m";
const char *blue    = "\033[34m";
const char *magenta = "\033[35m";
const char *cyan    = "\033[36m";
const char *white   = "\033[37m";
const char *loblack   = "\033[30;0m";
const char *lored     = "\033[31;0m";
const char *logreen   = "\033[32;0m";
const char *loyellow  = "\033[33;0m";
const char *loblue    = "\033[34;0m";
const char *lomagenta = "\033[35;0m";
const char *locyan    = "\033[36;0m";
const char *lowhite   = "\033[37;0m";
const char *hiblack   = "\033[30;1m";
const char *hired     = "\033[31;1m";
const char *higreen   = "\033[32;1m";
const char *hiyellow  = "\033[33;1m";
const char *hiblue    = "\033[34;1m";
const char *himagenta = "\033[35;1m";
const char *hicyan    = "\033[36;1m";
const char *hiwhite   = "\033[37;1m";

const char* colmatch[] = {hiyellow, cyan, green};

int verbosity;
int HFY, HLY, NT;

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


const int hashid[] = {
  0, 1, 3, 2, 6,
  4, 150, 48, 90, 28,
  210, 5, 5, 57, 10,
  86, 46, 7, 8, 11,
  53, 38, 45, 50, 47, 
  52, 144, 19, 91, 17
};

const char *hashkey[] = {
  "Steaua", "Dinamo", "Rapid", "Craiova", "Piteºti",
  "Petrolul", "CFR", "Politehnica Timiºoara", "FC Timiºoara", "Sportul", 
  "Victoria", "Progresul", "Naþional", "Vaslui", "Oþelul", 
  "Urziceni", "UTA", "Bacãu", "Braºov", "Bistriþa", 
  "Universitatea Cluj", "Corvinul", "Mediaº", "ASA", "Jiul", 
  "Chimia", "Baia Mare", "CSU Galaþi", "Moreni", "Electroputere"
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
#define TD_POST		 0
#define TD_PRENUME	 1
#define TD_NUME		 2
#define TD_DOB		 3
#define TD_NAT		 4
#define TD_NR		 5
#define TD_SEZON	 6
#define TD_CUPA		 7
#define TD_MECIURI	 8
#define TD_TITULAR	 9
#define TD_REZERVA	10
#define TD_GOLURI	11
#define TD_CLUB		12
#define TD_CLUBNAT	13
#define TD_CLUBID	14
#define TD_MNEM		15
#define TD_NUM		TD_CLUBNAT+1
#define TD_XTRA		TD_MNEM+1

int NP;
char **pmnem;
char **pname;
char **ppren;
char **pdob;
char **pcty;
char **ppob;
char **pjud;
int phist[MAX_NAMES][30];

int ntd;
char ***td;
char ofilename[128];
FILE *of;
int opt[MAX_NAMES];
int nopt;

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

void Mnem3(char *mn, char *m3) {
  int j = 0; 
  int k = 0;
  int l = strlen(mn);
  if (mn[0]=='F' && mn[1]=='C') {
    while (j<l && mn[j]!=' ') j++;
    j++;
  }
  while (k<3 && j<l) {
    if (mn[j]!='.' && mn[j]!=' ') {
      m3[k++] = mn[j];
    }
    j++;
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

#define DOB_DD_MM_YYYY	0
#define DOB_YYYYMMDD	1

int Swapped(char *pn1, char *pn2) {
  char s1[100], s2[100];
  char *tk1[3], *tk2[3];
  strcpy(s1, pn1);
  strcpy(s2, pn2);
  tk1[0] = strtok(s1, " ");
  tk1[1] = strtok(NULL, " ");
  tk1[2] = strtok(NULL, " ");
  tk2[0] = strtok(s2, " ");
  tk2[1] = strtok(NULL, " ");
  tk2[2] = strtok(NULL, " ");
  int np1 = 0; 
  for (int i=0; i<3; i++) if (tk1[i]!=NULL) np1++;
  int np2 = 0; 
  for (int i=0; i<3; i++) if (tk2[i]!=NULL) np2++;
  if (np1==2 && np2==2 && strcmp(tk1[0],tk2[1])==0 && strcmp(tk1[1], tk2[0])==0)
    return 1;
  else 
    return 0;
}

int GetYear(char *dob, int fmt) {
  if (dob==NULL) return -1;
  char s[12];
  strcpy(s, dob);
  char *sd = strtok(s, "/.-");
  char *sm = strtok(NULL, "/.-");
  char *sy = strtok(NULL, "/.-");
  int xy = 0;
  if (sd!=NULL && sm==NULL && sy==NULL) xy = atoi(sd);
  if (sy!=NULL) xy = atoi(sy);
  if (xy>0 && xy<100) xy += 1900;
  return xy;
}

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

void PrintHistory(int p) {
  char m3[12];
  int fy = phist[p][0];
  if (fy<1900 || fy>3000) return;
  int j=1;
  while (j<30) {
    int c = phist[p][j];
    if (c >= 0) {
      fprintf(stderr, "%d", (fy+j-1));
      if (c>=1000) {
        fprintf(stderr, ":");
        while (c >= 1000) {
          Mnem3(NickOf(L,c%1000,fy+j-1), m3);
          fprintf(stderr, "%s%s%s,", yellow, m3, normal);
          c = c/1000;
        }
        Mnem3(NickOf(L,c%1000,fy+j-1), m3);
        fprintf(stderr, "%s%s%s ", yellow, m3, normal);
      }
      else{
        if (j==29 || phist[p][j+1]!=c) {
          Mnem3(NickOf(L,c%1000,fy+j-1), m3);
          fprintf(stderr, ":%s%s%s ", yellow, m3, normal);
        }
        else {
          j++;
          while (j<29 && phist[p][j+1]==c) j++;
          Mnem3(NickOf(L,c%1000,fy+j-1), m3);
          fprintf(stderr, "-%d:%s%s%s ", (fy+j-1), yellow, m3, normal);
        }
      }
    }
    j++;
  }
}

int FindMnem(char *mnem) {
  for (int i=0; i<NP; i++)
    if (strcmp(pmnem[i], mnem)==0) return i;
  return -1;
}

int FindClub(char *s) {
  for (int i=0; i<30; i++) {
    if (strstr(s, hashkey[i])!=NULL) return hashid[i];
  }
  return -1;
}

int FindEcup(char *s) {
  if (strcmp(s, "CL")==0) return 1;
  if (strcmp(s, "CW")==0) return 2;
  if (strcmp(s, "EL")==0) return 3;
  if (strcmp(s, "SC")==0) return 0;
  return -1;
}

int FindName(char *name, char *pren, char *dob, char *cty) {
  char kol[64];
  int dob1, dob2;

  for (int i=0; i<NP; i++) {
    dob1 = GetYear(dob, DOB_DD_MM_YYYY);
    dob2 = GetYear(pdob[i], DOB_DD_MM_YYYY);
    if ((strcmp(pname[i], name)==0)) {
      if (strcmp(ppren[i],pren)==0 || Swapped(pren, ppren[i])) {
        if ((dob1==dob2) && (dob1>0)) {
           return i;
        }
      }
      if ((dob1==dob2) && (dob1>0) && (strstr(ppren[i], pren)!=NULL)) {
           return i;
      }
    }
  }

  nopt = 0;
  for (int i=0; i<NP; i++) {
    dob1 = GetYear(dob, DOB_DD_MM_YYYY);
    dob2 = GetYear(pdob[i], DOB_DD_MM_YYYY);
    if (strcmp(pname[i], name)==0) {
      opt[nopt] = i;
      strcpy(kol, hiwhite);
      if (strcmp(pren, ppren[i])==0) {
        if (dob1==dob2) strcpy(kol,hiyellow); else strcpy(kol,himagenta);
      }
      if (verbosity > 1) {
        fprintf(stderr, "   %s[%2d] %s: %-12s, %-20s%s [%10s]\t", kol, nopt+2, pmnem[i], pname[i], ppren[i], normal, pdob[i]);
        PrintHistory(i);
        fprintf(stderr, "\n");
      }
      nopt++;
    }
    else if (strstr(pname[i], name)!=NULL) {
      opt[nopt] = i;
      if (verbosity > 1) {
        fprintf(stderr, "   %s[%2d] %s: %-12s, %-20s%s [%10s]\t", hired, nopt+2, pmnem[i], pname[i], ppren[i], normal, pdob[i]);
        PrintHistory(i);
        fprintf(stderr, "\n");
      }
      nopt++;
    }
    else {
      int delta = dist(pname[i], name);
      if (delta<=2) {
        opt[nopt] = i;
        if (verbosity > 1) {
          fprintf(stderr, "   %s[%2d] %s: %-12s, %-20s%s [%10s]\t", colmatch[delta], nopt+2, pmnem[i], pname[i], ppren[i], normal, pdob[i]);
          PrintHistory(i);
          fprintf(stderr, "\n");
        }
        nopt++;        
      }
    }
  }

  return -1;
}

char low(char c) {
  if (c=='ª') return 'º';
  if (c=='Þ') return 'þ';
  if (c=='Î') return 'î';
  if (c>='A' && c<='Z') return c+32;
  return c;
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

//     if (verbosity > 1) {
//       fprintf(stderr, "Loaded %s: %s %s [%s] (%s)\n", pmnem[i], ppren[i], pname[i], pdob[i], pcty[i]);
//     }
  }
  fclose(f);
  fprintf(stderr, "Loaded %d names.\n", NP);
}

void SavePlayers() {
  FILE *f = fopen("players.new", "wt");
  fprintf(f, "%d\n", NP);
  for (int i=0; i<NP; i++) {
    fprintf(f, "%s,%s,%s,%s,%s,%s,%s\n", pmnem[i], pname[i], ppren[i], pdob[i], pcty[i], ppob[i], pjud[i]);
  }
  fclose(f);
  rename("players.dat", "players.old");
  rename("players.new", "players.dat");
}


void LoadHistory() {
  char histfile[128];
  char s[512], *tk[20];
  int px, tx;
  FILE *hf;

  for (int p=0; p<MAX_NAMES; p++) 
    for (int j=0; j<30; j++) phist[p][j] = -1;
  for (int y=HFY; y<=HLY; y++) {
     sprintf(histfile, "catalog-%d.dat", y);
     hf = fopen(histfile, "rt");
     if (hf==NULL) continue;
     while (!feof(hf)) {
       fgets(s, 500, hf);
       if (strlen(s)>10) {
         tk[0] = strtok(s, ",\n");
         for (int k=1; k<=5; k++) tk[k] = strtok(NULL, ",\n");
         px = -1; tx = -1;
         if (tk[0]!=NULL) px = FindMnem(tk[0]);
         if (tk[5]!=NULL) tx = atoi(tk[5]);
         if (px>=0 && px<MAX_NAMES && tx>=0 && tx<NC) {
           int fy = phist[px][0];
           if (fy<0) {
             phist[px][0] = y;
             phist[px][1] = tx;
           }
           else {
             int dy = y-fy;
             if (phist[px][dy+1] >=0) {
               phist[px][dy+1] = phist[px][dy+1]*1000 + tx;
             }
             else {
               phist[px][dy+1] = tx;
             }
           }
         }
       }
       s[0] = 0;
     }
     fclose(hf);
  }
}

void Extract(char *filename) {
  char s[5000], w[5000], *u, *t[10], *tk[100];
  char r;
  int tm, sez, ecup;
  int da, db, cont;
  int pa, pb, sa[5], sb[5];
  int e_m, e_t, e_r, e_g;

  FILE *f = fopen(filename, "rt");
  if (f==NULL) {
    fprintf(stderr, "ERROR: Cannot open file %s.\n", filename);
    return;
  }
  
  strtok(filename, ".");
  char *sy = strtok(NULL, ".\n");
  if (sy!=NULL) year = atoi(sy);

  ntd = 0;
  td = new char**[MAX_NAMES];
  for (int j=0; j<MAX_NAMES; j++) td[j] = new char*[TD_XTRA];

  do {
    fgets(s, 5000, f);
    if (strlen(s)<20) continue;
    
    tk[0] = strtok(s, "\t\n");
    for (int i=1; i<20; i++) tk[i] = strtok(NULL, "\t");

    e_m = e_t = e_r = e_g = 0;
    if (tk[TD_MECIURI]!=NULL) e_m = atoi(tk[TD_MECIURI]);
    if (tk[TD_TITULAR]!=NULL) e_t = atoi(tk[TD_TITULAR]);
    if (tk[TD_REZERVA]!=NULL) e_r = atoi(tk[TD_REZERVA]);
    if (tk[TD_GOLURI]!=NULL)  e_g = atoi(tk[TD_GOLURI]);

    trim(tk[1]);
    trim(tk[2]);

    pa = FindName(tk[2], tk[1], tk[3], t[4]);

    if (pa>=0) {
      if (verbosity > 1) {
        fprintf(stderr, ">Found  %s: %-12s, %-20s [%10s] (%s)\n", pmnem[pa], pname[pa], ppren[pa], pdob[pa], pcty[pa]);
      }
    }
    else if (pa<0) {
      fprintf(stderr, "...Name ......: %-12s, %-20s [%10s] (%s) not found. [0]=cancel [1]=add [2-%d]=use ", tk[2], tk[1], tk[3], tk[TD_CLUB], nopt+1);
      int add = 0;
      scanf("%d", &add);
      if (add==1) {
        pname[NP] = new char[strlen(tk[2])+1];
        strcpy(pname[NP], tk[2]);
        ppren[NP] = new char[strlen(tk[1])+1];
        strcpy(ppren[NP], tk[1]);
        pdob[NP] = new char[12];
        if (tk[3]!=NULL) strcpy(pdob[NP], tk[3]); else strcpy(pdob[NP], "00/00/0000");
        pcty[NP] = new char[4];
        if (tk[4]!=NULL) strcpy(pcty[NP], tk[4]); else strcpy(pcty[NP], "ROM");
        ppob[NP] = new char[2];
        strcpy(ppob[NP], "");
        pjud[NP] = new char[2];
        strcpy(pjud[NP], "");

        pmnem[NP] = new char[7];
        pmnem[NP][0] = low(pname[NP][0]);
        int d = 0; int k=1;
        while (k<4) {
          if (k>=strlen(tk[2])) pmnem[NP][k]='x'; 
          else if (pname[NP][k+d]==' ') {d++; k--;}
          else pmnem[NP][k] = pname[NP][k+d];
          k++;
        }
        if (tk[1]!=NULL) {
          if (tk[1][0]==' ') pmnem[NP][4] = '_';
          else pmnem[NP][4] = low(ppren[NP][0]); 
        }
        else pmnem[NP][4] = '_';
        if (tk[1]!=NULL) {
          if (strlen(tk[1])<2) pmnem[NP][5] = '_';
          else if (tk[1][1]==' ') pmnem[NP][5] = '_';
          else pmnem[NP][5] = low(ppren[NP][1]);
        }
        pmnem[NP][6] = 0;
        if (FindMnem(pmnem[NP])>=0) {
          fprintf(stderr, "...Mnemonic %s already in use.\n", pmnem[NP]);
          int k=0;
          do {
            k++;
            pmnem[NP][5] = (char) (k+48);
          } while (FindMnem(pmnem[NP])>=0);
        }
        pa = NP;
        fprintf(stderr, ".Added player #%d, %s%s%s: %s %s [%s] (%s)\n", NP+1, hiyellow, pmnem[NP], normal, ppren[NP], pname[NP], pdob[NP], pcty[NP]);
        NP++;
      }
      else if (add>=2 && add<=nopt+1) {
        pa = opt[add-2];
        fprintf(stderr, " Use #%d %s: %s, %s.\n", pa, pmnem[pa], pname[pa], ppren[pa]);

        int dob1 = GetYear(pdob[pa], 0);
        int dob2 = GetYear(tk[3], 0);
        if ((strcmp(pname[pa],tk[2])==0) && (strcmp(ppren[pa],tk[1])==0) && dob1==0 && dob2>0) {
          strcpy(pdob[pa], tk[3]);
        }

      }
    }
//    else fprintf(stderr, "Identified: %s %s (%s)\n", ppren[pa], pname[pa], pdob[pa]);
    
    if (pa<0) continue;

    td[ntd][TD_POST] = new char[3];
      strcpy(td[ntd][TD_POST], tk[TD_POST]);
    td[ntd][TD_MNEM] = new char[7];
      strcpy(td[ntd][TD_MNEM], pmnem[pa]);
    td[ntd][TD_PRENUME] = new char[strlen(ppren[pa])+1];
      strcpy(td[ntd][TD_PRENUME], ppren[pa]);
    td[ntd][TD_NUME] = new char[strlen(pname[pa])+1];
      strcpy(td[ntd][TD_NUME], pname[pa]);
    td[ntd][TD_DOB] = new char[strlen(pdob[pa])+1];
      strcpy(td[ntd][TD_DOB], pdob[pa]);
    td[ntd][TD_NAT] = new char[4];
      strcpy(td[ntd][TD_NAT], pcty[pa]);
    td[ntd][TD_SEZON] = new char[10];
      strcpy(td[ntd][TD_SEZON], tk[TD_SEZON]);
    td[ntd][TD_CUPA] = new char[4];
    ecup = FindEcup(tk[TD_CUPA]);
      sprintf(td[ntd][TD_CUPA], "%d", ecup);
    td[ntd][TD_CLUB] = new char[strlen(tk[TD_CLUB])+1];
      strcpy(td[ntd][TD_CLUB], tk[TD_CLUB]);
    tm = FindClub(tk[TD_CLUB]);
    td[ntd][TD_CLUBID] = new char[5];
      sprintf(td[ntd][TD_CLUBID], "%d", tm);

    td[ntd][TD_MECIURI] = new char[3];
      sprintf(td[ntd][TD_MECIURI], "%d", e_m);
    td[ntd][TD_TITULAR] = new char[3];
      sprintf(td[ntd][TD_TITULAR], "%d", e_t);
    td[ntd][TD_REZERVA] = new char[3];
      sprintf(td[ntd][TD_REZERVA], "%d", e_r);
    td[ntd][TD_GOLURI] = new char[3];
      sprintf(td[ntd][TD_GOLURI], "%d", e_g);

    fprintf(stderr, "%s %s [%s]: %s %s -- %d %d %d %d\n", 
     tk[TD_PRENUME], tk[TD_NUME], tk[TD_DOB], tk[TD_CLUB], tk[TD_SEZON],
     e_m, e_t, e_r, e_g);

    ntd++;

    s[0] = 0;

  } while (!feof(f));
  fclose(f);
}

void TextTable() {
  for (int i=0; i<ntd; i++) {
      fprintf(of, "%s,", td[i][TD_MNEM]);
      fprintf(of, "%s,", td[i][TD_PRENUME]);
      fprintf(of, "%s,", td[i][TD_NUME]);
      CanonicDOB(td[i][TD_DOB], DOB_DD_MM_YYYY);
      fprintf(of, "%s,", td[i][TD_DOB]);
      fprintf(of, "%s,", td[i][TD_NAT]);
      fprintf(of, "%s,", td[i][TD_CLUBID]);
      fprintf(of, "%s,", td[i][TD_CUPA]);
      fprintf(of, "%s,", td[i][TD_POST]);
      fprintf(of, "%s,", td[i][TD_MECIURI]);
      fprintf(of, "%s,", td[i][TD_TITULAR]);
      fprintf(of, "%s,", td[i][TD_REZERVA]);
      fprintf(of, "%s\n", td[i][TD_GOLURI]);
    }
}


int main(int argc, char **argv) {
  char filename[64];
  sprintf(filename, "euro-%s.tab", argv[1]);
  verbosity = 2;
  for (int k=1; k<argc; k++) {
    if (strcmp(argv[k], "-q")==0) verbosity = 0;
    else if (strcmp(argv[k], "-f")==0 && k<argc-1) strcpy(filename, argv[++k]);
  }
  if (strlen(filename)==0) {
    fprintf(stderr, "ERROR: input file not specified.\n");
    return -1;
  }

  HFY = 1948; HLY = 2013;

  Load();
  LoadPlayers();
  LoadHistory();

  Extract(filename);
  SavePlayers();

  sprintf(ofilename, "euro-%s.dat", argv[1]);
  of = fopen(ofilename, "wt");
  if (of==NULL) {
    fprintf(stderr, "ERROR: could not open file %s.\n", ofilename);
    return 1;
  }
  
  TextTable();
  fclose(of);

  return 0;
}
