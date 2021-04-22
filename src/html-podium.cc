#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#define MAX_TEAMS 2000
#define MAX_MED 20

#define MAX_LEVELS 4
#define MAX_SEASONS 250

int MED;
int FY, LY, FYA, LYA, MAXA;

int  NC, NE, ND, NR;
char **club;
char **mnem;
int  ***parta;
//int  **med;
char ***tab;
int  *maxc;
int  *rank;
char filename[128];
int count_last;
int *start_winter, *end_winter;
int med[MAX_TEAMS][MAX_MED];
FILE *of;

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

//--------------------------------------------------

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


void Load() {
  FILE *f;
  char s[2000], *tok[20], *ystr, *name, *nick;
  int dummy, n, p, t;
  char *dv, *pl, *yr, *suf;

  f = fopen("teams.dat", "rt");
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
  if (!f) return;
  for (int i=0; i<NC; i++) {
    fgets(s, 2000, f);
    if (!s) continue;
    s[strlen(s)-1] = 0;
    tok[0] = strtok(s, "*");
    for (int j=1; j<20; j++) tok[j] = strtok(NULL, "*");
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
  }
  fclose(f);

  MAXA = 1;
  FYA = 9999;
  LYA = 1800;
  ND = 0;
  DIR *dp;
  struct dirent *ep;
  dp = opendir("./");
  if (dp != NULL) {
      while (ep = readdir (dp)) {
        strcpy(s, ep->d_name);
        dv = strtok(s, ".");
        yr = strtok(NULL, ".");
        suf = strtok(NULL, ".");
        if (dv!=NULL && yr!=NULL && suf==NULL) {
          int l = strlen(dv);
          if (l==0 || l>3) continue;
          int d = ((int) s[0]) - 97;
          if (d<0 || d>=MAX_LEVELS) continue;
          if (d+1>ND) ND = d+1;
          if (l>1) p = atoi(dv+1); else p = 1;
          if (p<0 && p>12) continue;
          int y = atoi(yr);
          if (d==0 && y>1888 && y<2100) {
            if (y<FYA) FYA = y;
            if (y>LYA) LYA = y;
            if (p>MAXA) MAXA = p;
          }
        }
      }
      closedir(dp);
  }
  else
   fprintf(stderr, "ERROR: Couldn't open the directory.\n");

  int d=0;
    if (MAXA>0) {
      parta = new int**[LYA-FYA+1];
      for (int i=FYA; i<=LYA; i++) {
        parta[i-FYA] = new int*[MAXA+1];
        for (int j=0; j<=MAXA; j++) {
          parta[i-FYA][j] = new int[64];
        }
      }
    }

    sprintf(filename, "part.%c", (char) (d+97));
    f = fopen(filename, "rt");
    for (int y=0; y<=LYA-FYA; y++) {
      if (f==NULL) parta[y][0][1] = 0;
      else {
        fscanf(f, "%d %d", &dummy, &n);
        parta[y][0][0] = dummy;
        parta[y][0][1] = n;
        for (int i=0; i<n; i++) {
          fscanf(f, "%d", &t); parta[y][0][i+2] = t;
        }
        fgets(s, 200, f);
      }
    }
    if (f) fclose(f);


    for (int i=1; i<=MAXA; i++) {
      sprintf(filename, "part.%c%d", (char) (d+97), i);
      f = fopen(filename, "rt");
      for (int y=0; y<=LYA-FYA; y++) {
        if (f==NULL) parta[y][i][1] = 0;
        else {
          fscanf(f, "%d %d", &dummy, &n);
          parta[y][i][0] = dummy;
          parta[y][i][1] = n;
          for (int j=0; j<n; j++) {
            fscanf(f, "%d", &t);
            parta[y][i][j+2] = t;
          }
          fgets(s, 200, f);
        }
      }
      if (f) fclose(f);
    }

  tab = new char**[MAX_SEASONS];
  for (int y=0; y<MAX_SEASONS; y++) { tab[y] = new char*[20]; }
  maxc = new int[MAX_LEVELS];
  maxc[0] = 1;
  for (int d=1; d<MAX_LEVELS; d++) maxc[d] = 0;
  NR = 0;
  char *row = new char[1024];
  char **col = new char*[20];
  f = fopen("part.tab", "rt");
  if (!f) return;
  int y = 0;
  while (!feof(f)) {
    fgets(s, 2000, f);
    if (feof(f)) continue;
    if (!s) continue;
    s[strlen(s)-1] = 0;
    tok[0] = strtok(s, "| ");
    int lt = strlen(tok[0]);
    tab[y][0] = new char[lt+1];
    strncpy(tab[y][0], tok[0], lt+1);
    for (int j=1; j<5; j++) { tok[j] = strtok(NULL, "| "); }
    for (int j=1; j<5; j++) {
      if (tok[j]!=NULL) {
        lt = strlen(tok[j]);
        tab[y][j] = new char[lt+1];
        strncpy(tab[y][j], tok[j], lt+1);
        strncpy(row, tok[j], lt+1);
        col[0] = strtok(row, "; ");
        for (int k=1; k<5; k++) { col[k] = strtok(NULL, "; "); }
        for (int k=0; k<5; k++) {
          if (col[k]!=NULL) {
            if (k>maxc[j]-1) maxc[j] = k+1;
          }
        }
      }
      else {
        tab[y][j] = new char[3]; tab[y][j][0] = 0;
      }
    }
    y++;
    NR++;
  }
  fclose(f);
}


int sup(int x, int y) {
  for (int i=0; i<MED; i++) {
    if (med[x][i] > med[y][i]) return 1;
    if (med[x][i] < med[y][i]) return 0;
  }
  return x < y;
}

void Ranking() {
  rank = new int[NC];
  for (int i=0; i<NC; i++) rank[i] = i;
  int sorted = 1;
  do {
    sorted = 1;
    for (int i=0; i<NC-1; i++)
      if (sup(rank[i+1], rank[i])) {
        sorted = 0;
        int aux = rank[i]; rank[i] = rank[i+1]; rank[i+1] = aux;
      }
  } while (!sorted);
}

void Listing() {

  fprintf(of, "<TABLE BORDER=\"1\" CELLPADDING=\"2\" RULES=\"rows\" FRAME=\"box\" WIDTH=50%%>\n");
  fprintf(of, "<TR BGCOLOR=\"DDDDDD\">");
  fprintf(of, "<TH WIDTH=\"5%%\" ALIGN=\"left\">#</TH>");
  fprintf(of, "<TH WIDTH=\"5%%\" ALIGN=\"right\"></TH>");
  fprintf(of, "<TH ALIGN=\"left\">Echipa</TH>");
  for (int k=1; k<=MED; k++)
    fprintf(of, "<TH WIDTH=\"30\" ALIGN=\"right\">%d</TH>", k);
  fprintf(of, "</TR>\n");


  for (int i=0; i<NC; i++) {
    int x = rank[i];
    int sum = 0;
    for (int j=0; j<MED; j++) sum += med[x][j];
    if (sum) {
      fprintf(of, "<TR BGCOLOR=\"%s\">", (i%2==1?"FFFFFF":"DDDDFF"));
      fprintf(of, "<TD>%d.</TD>", i+1);
      fprintf(of, "<TD ALIGN=\"right\"><IMG SRC=\"logo/logo-%d.png\" HEIGHT=\"20\"></TD>", x);
      fprintf(of, "<TD><A HREF=\"istoric-%d.html\">%s</A></TD>", x, NameOf(L, x, 3000));
      for (int j=0; j<MED; j++)
        fprintf(of, "<TD ALIGN=\"right\">%d</TD>",  med[x][j]);
      fprintf(of, "</TR>");
    }
  }

  fprintf(of, "</TABLE>\n</BODY>\n</HTML>\n");
}

int Medals() {
  int num;
  for (int t=0; t<NC; t++) {
    for (int j=0; j<MED; j++) med[t][j] = 0;
  }

  char outputfile[256];
  sprintf(outputfile, "html/podium.html");
  of = fopen(outputfile, "wt");
  if (!of) return 0;

  fprintf(of, "<HTML>\n<TITLE>Podium</TITLE>\n");
  fprintf(of, "<HEAD>\n<link href=\"css/seasons.css\" rel=\"stylesheet\" type=\"text/css\"/>\n");
  fprintf(of, "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n");
  fprintf(of, "</HEAD>\n<BODY>\n");

  fprintf(of, "<H2>Prezenţe pe podium</H2>\n");

  fprintf(of, "<TABLE BORDER=\"1\" CELLPADDING=\"3\" RULES=\"cols\" FRAME=\"box\" WIDTH=50%%>\n");
  fprintf(of, "<TR BGCOLOR=\"DDDDDD\">");
  fprintf(of, "<TH WIDTH=\"100\" ALIGN=\"left\">Ediţia</TH>");
  for (int k=1; k<=MED; k++)
    fprintf(of, "<TH ALIGN=\"center\">%d</TH>", k);
  fprintf(of, "</TR>\n");

  char *row = new char[512];
  for (int y=0; y<NR; y++) {
    if (count_last==0 && y==NR-1) continue;
    strncpy(row, tab[y][1], 512);
    char *fn = strtok(row, ",");
    char *champ = strtok(NULL, ",");
    int camp = atoi(champ);
    char *snr = strtok(NULL, ",;");
    int nr = atoi(snr);
    char *spl = new char[64];
    strncpy(spl, fn, 64);
    char *dvn = strtok(spl, ".");
    char *syr = strtok(NULL, ",; |");
    int year = atoi(syr);
    int g = 0;
    if (strlen(dvn)>1) g = atoi(dvn+1);
    num = parta[year-FYA][g][1];

    if (num>0 && camp>=0) {
      fprintf(of, "<TR BGCOLOR=\"%s\">", (y%2==1?"FFFFFF":"DDDDFF"));
      fprintf(of, "<TD><A HREF=\"%s-r%d.html\">%s </A></TD>", fn, nr, tab[y][0]);

      int delta = 0;
      for (int j=0; j<(MED>num?num:MED); j++)  {
        int tx = parta[year-FYA][g][j+2-delta];
        if (j==0 && camp != tx) { delta++; tx = camp; }
        if (j>0  && camp == tx) { delta--; tx = parta[year-FYA][g][j+2-delta]; }
        fprintf(of, "<TD>%s</TD>", NickOf(L, tx, year));
        med[tx][j]++;
      }
      for (int j=num; j<MED; j++) fprintf(of, "<TD></TD>");
      fprintf(of, "</TR>\n");
    }
  }
  fprintf(of, "</TABLE><BR><HR>\n");

  Ranking();
  Listing();
  fclose(of);

  return 0;
}


//---------------------------------------------

int main(int argc, char* argv[]) {
  int a, b;
  count_last = 0;
  MED = 3;
  for (int i=1; i<argc; i++) {
    if (strcmp(argv[i], "+")==0) count_last = 1;
    if (strcmp(argv[i], "-n")==0) {
      if (i+1<argc) MED = atoi(argv[i+1]);
      if (MED<1 || MED>MAX_MED) MED = 6;
    }
  }
  Load();
  Medals();
  return 0;
}
