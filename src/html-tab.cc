#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEVELS 10
#define MAX_SEASONS 250

int  NC, ND, NR;
char **club;
char **mnem;
char ***tab;
int  *maxc;
int fd, ld, final;
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


int Load() {
  FILE *f;
  char s[2000], *tok[20], *ystr, *name, *nick;
  int dummy, n, p, t, d;
  char *dv, *pl, *yr, *suf;

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
    fgets(s, 2000, f);
    if (!s) continue;
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
  }
  fclose(f);

  tab = new char**[MAX_SEASONS];
  for (int y=0; y<MAX_SEASONS; y++) { tab[y] = new char*[20]; }
  maxc = new int[MAX_LEVELS];
  maxc[0] = 1;
  for (int d=1; d<MAX_LEVELS; d++) maxc[d] = 0;
  NR = 0;
  char *row = new char[1024];
  char **col = new char*[20];
  f = fopen("part.tab", "rt");
  if (!f) return 0;
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
    for (int j=1; j<20; j++) { tok[j] = strtok(NULL, "| "); }
    for (int j=1; j<20; j++) {
      if (tok[j]!=NULL) {
        lt = strlen(tok[j]);
        tab[y][j] = new char[lt+1];
        strncpy(tab[y][j], tok[j], lt+1);
        strncpy(row, tok[j], lt+1);
        col[0] = strtok(row, "; ");
        for (int k=1; k<20; k++) { col[k] = strtok(NULL, "; "); }
        for (int k=0; k<20; k++) {
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
  return 1;
}

//---------------------------------------------

int main(int argc, char* argv[]) {
  int n, maxr;
  int nw[2000];
  char *row = new char[1024];
  char **col = new char*[20];
  char *dvn = new char[64];
  for (int c=0; c<20; c++) col[c] = new char[1024];
  final = 0;
  if (argc>1 && strcmp(argv[1], "+")==0) final = 1;

  if (!Load()) {
    printf("ERROR: called from invalid directory.\n");
    return -1;
  }

  for (int t=0; t<NC; t++) nw[t] = 0;

  FILE *f = fopen("index.html", "wt");
  if (f==NULL) return 1;
  fprintf(f, "<HTML>\n");
  fprintf(f, "<TITLE></TITLE>\n<HEAD>\n");
  fprintf(f, "<link href=\"css/seasons.css\" rel=\"stylesheet\" type=\"text/css\"/>\n");
  fprintf(f, "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n");
  fprintf(f, "</HEAD>\n<BODY BGCOLOR=\"333333\">");
  fprintf(f, "<center>\n");
//  fprintf(f, "<H2>%s</H2>\n", liga);
  fprintf(f, "<table cellpadding=\"1\" border=\"1\" bgcolor=\"999999\" width=\"65%%\" align=\"center\">\n");
  fprintf(f, "<thead>\n");
  fprintf(f, "<TH>Sezon</TH>");
  fprintf(f, "<TH>Campioană</TH>");
  for (int d=1; d<MAX_LEVELS; d++) {
    if (maxc[d]>0) {
      fprintf(f, "<th colspan=\"%d\">%c</th>", maxc[d], (char)(d+64));
    }
  }
  fprintf(f, "</thead>");
  fprintf(f, "<tbody>\n");
  for (int y=0; y<NR; y++) {
    int champ = -1;
    fprintf(f, "<TR><TD ALIGN=\"center\"><B>%s</B></TD>", tab[y][0]);
    strncpy(row, tab[y][1], 1024);
    col[0] = strtok(row, ","); for (int c=1; c<20; c++) col[c] = strtok(NULL, ",");
    strtok(col[0], ".");
    char *sy = strtok(NULL, ".");
    int year = atoi(sy);
    champ = atoi(col[1]);
    if (champ>=0 && champ<NC) {
      if (y<NR-1 || final>0) {
        nw[champ]++;
        fprintf(f, "<TD> <IMG SRC=\"logo/logo-%d.png\" HEIGHT=\"20\"> <A HREF=\"istoric-%d.html\"><FONT COLOR=\"000000\">%s</FONT></A> (%d)</TD>", champ, champ, NameOf(L, champ, year), nw[champ]);
      }
      else {
        fprintf(f, "<TD> -- <A HREF=\"istoric-%d.html\"><FONT COLOR=\"000000\"><I>%s? (%d)</I></FONT></A></TD>", champ, NameOf(L, champ, year), nw[champ]+1);
      }
    }
    else {
      fprintf(f, "<TD></TD>");
    }
    for (int d=1; d<MAX_LEVELS; d++) {
      if (maxc[d]<=0) continue;
      strncpy(row, tab[y][d], 1024);
      col[0] = strtok(row, "; "); for (int c=1; c<20; c++) col[c] = strtok(NULL, "; ");
      int nc = 0;
      for (int c=0; c<20; c++) {
        if (col[c]==NULL) continue;
        char *tkf = strtok(col[c], ",");
        char *tkc = strtok(NULL, ",");
        char *tkr = strtok(NULL, ",;");
        strncpy(dvn, tkf, 64);
        strtok(dvn, ".");
        dvn[0] = dvn[0] - 32;
        fprintf(f, "<TD><A HREF=\"%s-r%s.html\">%s</A></TD>", tkf, tkr, dvn);
        nc++;
      }
      if (nc < maxc[d]) {
        fprintf(f, "<TD COLSPAN=\"%d\"></TD>", maxc[d] - nc);
      }
    }
    fprintf(f, "</TR>\n");
  }
  fprintf(f, "</tbody>");
  fprintf(f, "</table>\n</body>\n");
  fprintf(f, "</center>\n");
  fprintf(f, "<HR>\n");
  fprintf(f, "<UL>\n");

  fprintf(f, "<LI><A HREF=\"podium.html\"><FONT COLOR=\"999999\">Podium</FONT></A></LI>\n");
  fprintf(f, "<LI><A HREF=\"alltime-a.html\"><FONT COLOR=\"999999\">Clasament <I>all-time</I> Liga 1</FONT></A></LI>\n");
  fprintf(f, "<LI><A HREF=\"alltime-b.html\"><FONT COLOR=\"999999\">Clasament <I>all-time</I> Liga 2</FONT></A></LI>\n");
  fprintf(f, "<LI><A HREF=\"alltime-c.html\"><FONT COLOR=\"999999\">Clasament <I>all-time</I> Liga 3</FONT></A></LI>\n");
  fprintf(f, "<LI><A HREF=\"catalog.html\"><FONT COLOR=\"AAAAAA\">Jucători în prima divizie</FONT></A></LI>\n");
  fprintf(f, "<LI><A HREF=\"antrenori.html\"><FONT COLOR=\"AAAAAA\">Antrenori în prima divizie</FONT></A></LI>\n");
  fprintf(f, "<LI><A HREF=\"arbitri.html\"><FONT COLOR=\"AAAAAA\">Arbitri în prima divizie</FONT></A></LI>\n");

  fprintf(f, "<LI><A HREF=\"consecutive.html\"><FONT COLOR=\"999999\">Serii consecutive</FONT></A></LI>\n");
  fprintf(f, "<LI><A HREF=\"consecutive-a.html\"><FONT COLOR=\"999999\">Serii consecutive - Liga 1</FONT></A></LI>\n");
  fprintf(f, "</UL>\n");
  fprintf(f, "</body>");
  fprintf(f, "</html>");
  fclose(f);

  return 0;
}
