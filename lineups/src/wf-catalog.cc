#include "wf-catalog.hh"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

#define CINS_LINK	0
#define CINS_LAST	1
#define CINS_FIRST	2
#define CINS_FULL	3
#define CINS_DOB	4
#define CINS_NAT	5
#define CINS_POB	6
#define CINS_COB	7

#define CCAT_MNEM	0
#define CCAT_LAST	1
#define CCAT_FIRST	2
#define CCAT_FULL	3
#define CCAT_DOB	4
#define CCAT_NAT	5
#define CCAT_POB	6
#define CCAT_COB	7
#define CCAT_LINK	8

const char* colmatch[] = {hiyellow, cyan, green};

int cMIN(int x, int y) {
  if (x<y) return x;
  else return y;
}

#define MAX_LEN 100
int d[MAX_LEN][MAX_LEN];

int cdist(char*a, char*b) {
  int i, j, modif, ins, del;
  int m = strlen(a);
  int n = strlen(b);
	if (m>=MAX_LEN || n>=MAX_LEN) return MAX_LEN;
  for (i=0; i<=m; i++) d[i][0] = i;
  for (i=0; i<=n; i++) d[0][i] = i;

  for (i=1; i<=m; i++)
    for (j=1; j<=n; j++) {
      if (a[i-1] == b[j-1]) modif = d[i-1][j-1];
        else modif = d[i-1][j-1] + 1;
      del = cMIN(d[i-1][j], d[i][j-1]) + 1;
      d[i][j] = cMIN(modif, del);
    }

  return d[m][n];
}

int GetYOB(char *s) {
	char w[32];
	if (!s) return -1;
	strncpy(w, s, 16);
	char *tkd = strtok(w, "/.-");
	char *tkm = strtok(NULL, "/.-");
	char *tky = strtok(NULL, "/.-");
	int y;
	if (tky == NULL) return 0;
    if (strlen(tky) == 4) {
		y = strtol(tky, NULL, 10);
		return y;
	}
	else {
		y = strtol(tkd, NULL, 10);
		return y;
	}
}

void copytk(char **pf, char *tk) {
	if (tk==NULL) {
		*pf = new char[2];
		strcpy(*pf, " ");
	}
	else {
		*pf = new char[strlen(tk)+1];
		strcpy(*pf, tk);
	}
}

void toLower(char *w) {
  if (!w) return;
  int l = strlen(w);
  int i=0;
  int j=0;
  for (int i=0; i<l; i++) {
    if (w[i]>='A' && w[i]<='Z') w[j++] = w[i] + 32;
    else if (w[i]>='a' && w[i] <= 'z') w[j++] = w[i];
  }
  if (j==0) w[j++] = 'x';
  if (j==1) w[j++] = 'x';
  w[j] = 0;
}

char low(char c) {
  if (c=='ª') return 'º';
  if (c=='Þ') return 'þ';
  if (c=='Î') return 'î';
  if ((c>='A' && c<='Z')) return c+32;
	if (c=='Á' || c=='É' || c=='Ó' || c=='Í' || c=='Ú' || c=='Ö' || c=='Ü') return c+32;
  return c;
}

int Person::Fits(char *sn, char *sp, char *sd) {
	int fit = 100;
	if (strcmp(name, sn)!=0) {
		if (strstr(sn, name)==NULL) {
			fit -= 5*cdist(name, sn);
		}
		else fit -= 25;
	}
	if (sp[0]!=' ' && strcmp(pren, sp)!=0) {
		if (strstr(sp, pren)==NULL) {
			fit -= 2*cdist(pren, sp);
		}
		else fit -= 15;
	}
	int y1 = GetYOB(dob);
	int y2 = GetYOB(sd);
	int diff = y2-y1;
	if (y2<y1) diff = y1-y2;
	if (diff<10) diff = diff/2;
	if (diff>20) diff = 100;
	fit -= diff;
	return fit;
}

Stat::Stat() {
	sez = champ = promo = releg = 0;
	win = drw = los = gsc = gre = 0;
}

void Stat::reset() {
	sez = champ = promo = releg = 0;
	win = drw = los = gsc = gre = 0;
}

int Stat::numg() {
	return win+drw+los;
}

double Stat::pct() {
	int ng = win+drw+los;
	if (ng==0) return 0.0;
	return (win+0.5*drw)/ng;
}

void Stat::addRes(int x, int y) {
	gsc += x; gre += y;
	if (x>y) ++win;
	else if (x==y) ++drw;
	else ++los;
}

void Stat::add(Stat *x) {
	sez += x->sez;
	champ += x->champ;
	promo += x->promo;
	releg += x->releg;
	win	+= x->win;
	drw += x->drw;
	los	+= x->los;
	gsc += x->gsc;
	gre += x->gre;
}

int Stat::sup(Stat *x, int rule) {
	int ng1 = win+drw+los;
	int ng2 = x->win+x->drw+x->los;
	if (rule==RULE_NUMG) {
		if (ng1>ng2) return -1;
		else if (ng1==ng2) return 0;
		else return 1;
	}
	if (rule==RULE_PCT) {
		double pc1 = pct();
		double pc2 = x->pct();
		if (pc1>pc2) return -1;
		if (pc1<pc2) return 1;
		if (ng1>ng2) return -1;
		else return 1;
	}
	int pt1 = 2*win+drw;
	int pt2 = 2*x->win+x->drw;
	if (pt1>pt2) return -1;
	if (pt1<pt2) return 1;
	if (pt1==0) {
		if (ng1>0  && ng2==0) return -1;
		if (ng1==0 && ng2>0)  return 1;
	}
	int gd1 = gsc-gre;
	int gd2 = x->gsc-x->gre;
	if (gd1>gd2) return -1;
	if (gd1<gd2) return 1;
	if (gsc>x->gsc) return -1;
	return 1;
}

Ranking::Ranking(int an) {
	n = an;
	S = new Stat[n];
	rank = new int[n];
	for (int i=0; i<n; ++i) {
		S[i].reset();
		rank[i] = i;
	}
}

int Ranking::compare0(const void *c1, const void *c2) {
	int* i1 = (int*)c1;
	int* i2 = (int*)c2;
	return(S[*i1].sup(&S[*i2], 0));
}

int Ranking::compare1(const void *c1, const void *c2) {
	int* i1 = (int*)c1;
	int* i2 = (int*)c2;
	return(S[*i1].sup(&S[*i2], 1));
}

void Ranking::rqsort(int rule) {
/*
	int (*comp0)(const void*, const void*);
	int (*comp1)(const void*, const void*);
	comp0 = &compare0;
	comp1 = &compare1;
	if (rule==0)
		qsort(rank, n, sizeof(int), *comp0);
	else if (rule==1)
    qsort(rank, n, sizeof(int), *comp1);
*/
}

void Ranking::bubbleSort(int rule) {
	int sorted;
	do {
		sorted = 1;
		for (int i=0; i<n-1; ++i) {
			if (S[rank[i+1]].sup(&S[rank[i]], rule) < 0)	{
				int aux = rank[i]; rank[i] = rank[i+1]; rank[i+1] = aux;
				sorted = 0;
			}
		}
	} while (!sorted);
}

int Catalog::Load(const char *filename) {
  n = 0;
  int c;
  for (c=0; c<257; c++) borna[c] = -1;
  FILE *f = fopen(filename, "rt");
  if (f==NULL) {
    fprintf(stderr, "ERROR: Catalog %s not found.\n", filename);
    return -1;
  }

  char s[1024], *t[10];
  fgets(s, 1024, f);
  sscanf(s, "%d", &n);
  P = new Person[MAX_CATALOG];
  for (int i=0; i<n; i++) {
    fgets(s, 1024, f);
    t[0] = strtok(s, ",\t\n");
    for (int j=1; j<10; j++) t[j] = strtok(NULL, ",\t\n");

    P[i].mnem = new char[7];
     strncpy(P[i].mnem, t[CCAT_MNEM], 7);
    P[i].name = new char[strlen(t[CCAT_LAST])+1];
     strcpy(P[i].name, t[CCAT_LAST]);
    P[i].pren = new char[strlen(t[CCAT_FIRST])+1];
     strcpy(P[i].pren, t[CCAT_FIRST]);
    P[i].fullname = new char[strlen(t[CCAT_FULL])+1];
     strcpy(P[i].fullname, t[CCAT_FULL]);

 	copytk(&(P[i].dob), t[CCAT_DOB]);
 	copytk(&(P[i].cty), t[CCAT_NAT]);
 	copytk(&(P[i].pob), t[CCAT_POB]);
 	copytk(&(P[i].jud), t[CCAT_COB]);

    P[i].link = new char[strlen(t[CCAT_LINK])+1];
     strcpy(P[i].link, t[CCAT_LINK]);

	if (i==0 || (i>0 && P[i-1].mnem[0]!=P[i].mnem[0])) {
		c = ((unsigned char) P[i].mnem[0]);
		borna[c] = i;
	}
  }
  fclose(f);

  c = ((unsigned char)P[n-1].mnem[0]);
  borna[c+1] = n;
  for (c=255; c>0; c--)
    if (borna[c]>=0 && borna[c-1]<0) borna[c-1] = borna[c];
	return 0;
}

int Catalog::FindMnem(char *s) {
  int c = ((unsigned char)s[0]);
  for (int i=borna[c]; i<borna[c+1]; i++)
    if (strcmp(P[i].mnem, s)==0) return i;
  return -1;
}

void Catalog::MakeMnem(char *ins, char *ms, int opt) {
  char *tks[12];
  if (opt == 1) {
    tks[0] = strtok(ins, "-"); for (int i=1; i<12; i++) tks[i] = strtok(NULL, "-");
  } else {
    tks[0] = strtok(ins, " "); for (int i=1; i<12; i++) tks[i] = strtok(NULL, " ");
  }
  int ntks = 0; for (int i=0; i<12; i++) if (tks[i]) ntks++;
  toLower(tks[ntks-1]);
  char *ln = tks[ntks-1];
  toLower(tks[0]);
  char *fn = tks[0];
  ms[0] = ln[0];
  int d = 0; int k=1;
  while (k<4) {
    if (k>=strlen(ln)) ms[k]='x'; else ms[k] = ln[k];
    k++;
  }
  ms[4] = fn[0];
  if (ntks>2) ms[5] = tks[1][0];
  else if (strlen(fn)<2) ms[5] = '_'; else ms[5] = fn[1];
  ms[6] = 0;
  if (SearchMnem(ms)>=0) {
    fprintf(stderr, "%s.%s used%s.\n", green, ms, normal);
    int k=0;
    do {
      k++;
      if (k==10) { k=17; }; // skip over special characters
      if (k==43) {
        ms[4] = (ms[4]<'9' ? ms[4]+1 : '1');
        k=1;
      }
      ms[5] = (char) (k+48);
    } while (SearchMnem(ms)>=0);
  }
}

int Catalog::SearchMnem(char *s) {
  for (int i=0; i<n; i++)
    if (strcmp(P[i].mnem, s)==0) return i;
  return -1;
}

int Catalog::binFindMnem(char *s) {
  int c = ((unsigned char)s[0]);
	int lo = borna[c];
	int hi = borna[c+1]-1;
	int mid = (hi+lo)/2;
	while (lo<hi) {
		int comp = strcmp(P[mid].mnem, s);
    if (comp==0) return mid;
		if (comp<0) { hi = mid; } else { lo = mid+1; }
		mid = (lo+hi)/2;
	}
	if (strcmp(P[mid].mnem, s)==0) return mid;
  return -1;
}

int Catalog::FindNameUnique(char *sp, char *sn) {
	int exact = 0;
	int sub = 0;
	int loc = -1;
	for (int i=0; i<n; i++) {
		if (strcmp(P[i].name, sn)==0) {
			if (strcmp(P[i].pren, sp)==0) { exact++; loc = i; }
			else if (strstr(P[i].pren, sp)!=NULL) sub++;
		}
	}
	if (exact==1) return loc;
	return -1;
}

int Catalog::FindName(char *sp, char *sn) {
	int exact = 0;
	int sub = 0;
	int locx = -1;
	int locs = -1;
	for (int i=0; i<n; i++) {
		if (strcmp(P[i].name, sn)==0) {
			if (strcmp(P[i].pren, sp)==0) { exact++; locx = i; }
			else if (strstr(P[i].pren, sp)!=NULL) { sub++; locs = i; }
		}
	}
	if (exact==1) return locx;
	if (sub > 0) return locs;
    return -1;
}

int Catalog::FindLink(char *s) {
  for (int i=0; i<n; i++) {
    if (strcmp(P[i].link, s)==0) return i;
  }
  return -1;
}

int Catalog::Add(char *ins, int opt) {
  char s[1024], *tk[100], kol[64];
  int ch, choice[1000], nopt, pa;
	char slink[64], sfull[128], slast[64], sfirst[64], snat[64], sdob[64], spob[64], scob[64];
	int perfect_match;

	strcpy(s, ins);
	int len = strlen(s);
	tk[0] = strtok(s, ",\n");
	for (int j=1; j<10; ++j) tk[j] = strtok(NULL, ",");

	if (tk[CINS_LINK])  strcpy(slink,  tk[CINS_LINK]);  else strcpy(slink,  "___");
	if (tk[CINS_LAST])  strcpy(slast,  tk[CINS_LAST]);  else strcpy(slast,  "_____");
	if (tk[CINS_FIRST]) strcpy(sfirst, tk[CINS_FIRST]); else strcpy(sfirst, "__");
	if (tk[CINS_FULL])  strcpy(sfull,  tk[CINS_FULL]);  else strcpy(sfull,  "__ ____");
	if (tk[CINS_DOB])   strcpy(sdob,   tk[CINS_DOB]);   else strcpy(sdob,   "0000-00-00");
	if (tk[CINS_NAT])   strcpy(snat,   tk[CINS_NAT]);   else strcpy(snat,   " ");
	if (tk[CINS_POB])   strcpy(spob,   tk[CINS_POB]);   else strcpy(spob,   " ");
	if (tk[CINS_COB])   strcpy(scob,   tk[CINS_COB]);   else strcpy(scob,   " ");

  int usc = strcmp(slink, "___");

  for (int i=0; i<n; i++) {
    if (strcmp(P[i].link, slink)==0 && usc!=0) {
      fprintf(stdout, "   %s[%2d] %s: %-12s, %-20s%s (%3s) [%10s, @%s (%s)] %s {%s}%s\n",
        hiyellow, nopt+2, P[i].mnem, P[i].name, P[i].pren, normal, P[i].cty, P[i].dob, P[i].pob, P[i].jud, P[i].fullname, P[i].link, normal);
	  return i;
    }
  }

  if (usc==0) return AddSuggest(ins, opt);

        P[n].link = new char[strlen(slink)+1];
        strcpy(P[n].link, slink);
        P[n].fullname = new char[strlen(sfull)+1];
        strcpy(P[n].fullname, sfull);
        P[n].name = new char[strlen(slast)+1];
        strcpy(P[n].name, slast);
        P[n].pren = new char[strlen(sfirst)+1];
        strcpy(P[n].pren, sfirst);
        P[n].cty = new char[4];
        strcpy(P[n].cty, snat);
        P[n].dob = new char[12];
        strcpy(P[n].dob, sdob);
        P[n].pob = new char[strlen(spob)+1];
        strcpy(P[n].pob, spob);
        P[n].jud = new char[4];
        strcpy(P[n].jud, scob);

        P[n].mnem = new char[7];
        char srcx[64];
        if (opt==1) strncpy(srcx, P[n].link, 64);
        else sprintf(srcx, "%s %s", P[n].pren, P[n].name);
        char *ms = new char[7];
        MakeMnem(srcx, ms, opt);
        strncpy(P[n].mnem, ms, 7);
        fprintf(stdout, ".Added entry #%d, %s: %s%s %s%s (%s) [%s, @%s (%s)] %s {%s}\n", n+1,
			P[n].mnem, hicyan, P[n].pren, P[n].name, normal, P[n].cty, P[n].dob, P[n].pob, P[n].jud, P[n].fullname, P[n].link);
        n++;

	return n;
}

int Catalog::AddSuggest(char *ins, int opt) {
  char s[1024], *tk[100], kol[64];
  int ch, choice[1000], nopt, pa;
	char slink[64], sfull[128], slast[64], sfirst[64], snat[64], sdob[64], spob[64], scob[64];
	int perfect_match;

	strcpy(s, ins);
	int len = strlen(s);
	tk[0] = strtok(s, ",\n");
	for (int j=1; j<10; ++j) tk[j] = strtok(NULL, ",");

//antonio-donnarumma,Antonio Donnarumma, ,1990-07-07,ITA,Castellammare di Stabia,ITA
	if (tk[CINS_LINK])  strcpy(slink,  tk[CINS_LINK]);  else strcpy(slink,  "___");
	if (tk[CINS_LAST])  strcpy(slast,  tk[CINS_LAST]);  else strcpy(slast,  "_____");
	if (tk[CINS_FIRST]) strcpy(sfirst, tk[CINS_FIRST]); else strcpy(sfirst, "__");
	if (tk[CINS_FULL])  strcpy(sfull,  tk[CINS_FULL]);  else strcpy(sfull,  "__ ____");
	if (tk[CINS_DOB])   strcpy(sdob,   tk[CINS_DOB]);   else strcpy(sdob,   "0000-00-00");
	if (tk[CINS_NAT])   strcpy(snat,   tk[CINS_NAT]);   else strcpy(snat,   " ");
	if (tk[CINS_POB])   strcpy(spob,   tk[CINS_POB]);   else strcpy(spob,   " ");
	if (tk[CINS_COB])   strcpy(scob,   tk[CINS_COB]);   else strcpy(scob,   " ");

  fprintf(stdout, "\n");
  nopt = 0;
  perfect_match = 0;
  for (int i=0; i<n; i++) {
    if (strcmp(P[i].link, slink)==0 && strcmp(slink, "___")!=0) {
      choice[nopt] = i;
      fprintf(stdout, "   %s[%2d] %s: %-12s, %-20s%s (%3s) [%10s, @%s (%s)] %s {%s}\n",
        kol, nopt+2, P[i].mnem, P[i].name, P[i].pren, normal, P[i].cty, P[i].dob, P[i].pob, P[i].jud, P[i].fullname, P[i].link);
      nopt++;
	  perfect_match = i+1;
    }
    else if (strstr(P[i].name, slast)!=NULL) {
			strcpy(kol, hired);
      choice[nopt] = i;
      fprintf(stdout, "   %s[%2d] %s: %-12s, %-20s%s (%3s) [%10s, @%s (%s)] %s {%s}\n",
        kol, nopt+2, P[i].mnem, P[i].name, P[i].pren, normal, P[i].cty, P[i].dob, P[i].pob, P[i].jud, P[i].fullname, P[i].link);
      nopt++;
    }
    else {
      int delta = cdist(P[i].name, slast);
      if (delta<=2) {
        choice[nopt] = i;
        fprintf(stdout, "   %s[%2d] %s: %-12s, %-20s%s (%3s) [%10s, @%s (%s)] %s {%s}\n",
  	      colmatch[delta], nopt+2, P[i].mnem, P[i].name, P[i].pren, normal, P[i].cty, P[i].dob, P[i].pob, P[i].jud, P[i].fullname, P[i].link);
				nopt++;
			}
		}
  }

    int add = 0;
	if (perfect_match) return perfect_match-1;
	fprintf(stdout, "...%s           : %-12s, %-20s (%3s) [%10s, @%s (%s)] %s {%s} [0]=cancel [1]=add [2-%d]=use ",
		normal, slast, sfirst, snat, sdob, spob, scob, sfull, slink, nopt+2);
      scanf("%d", &add);
      if (add>1 && add<=nopt+2) {
		int k = choice[add-2];
        P[k].link = new char[strlen(slink)+1];
        strcpy(P[k].link, slink);
        P[k].fullname = new char[strlen(sfull)+1];
        strcpy(P[k].fullname, sfull);
        P[k].name = new char[strlen(slast)+1];
        strcpy(P[k].name, slast);
        P[k].pren = new char[strlen(sfirst)+1];
        strcpy(P[k].pren, sfirst);
        P[k].cty = new char[4];
        strncpy(P[k].cty, snat, 4);
        P[k].dob = new char[12];
        strncpy(P[k].dob, sdob, 4);
        P[k].pob = new char[strlen(spob)+1];
        strncpy(P[k].pob, spob, 4);
        P[k].jud = new char[4];
        strncpy(P[k].jud, scob, 4);
	  }
      else if (add==1) {
        P[n].link = new char[strlen(slink)+1];
        strcpy(P[n].link, slink);
        P[n].fullname = new char[strlen(sfull)+1];
        strcpy(P[n].fullname, sfull);
        P[n].name = new char[strlen(slast)+1];
        strcpy(P[n].name, slast);
        P[n].pren = new char[strlen(sfirst)+1];
        strcpy(P[n].pren, sfirst);
        P[n].cty = new char[4];
        strcpy(P[n].cty, snat);
        P[n].dob = new char[12];
        strcpy(P[n].dob, sdob);
        P[n].pob = new char[strlen(spob)+1];
        strcpy(P[n].pob, spob);
        P[n].jud = new char[4];
        strcpy(P[n].jud, scob);

        P[n].mnem = new char[7];
        char srcx[64];
        if (opt==1) strncpy(srcx, P[n].link, 64);
        else sprintf(srcx, "%s %s", P[n].pren, P[n].name);
        char *ms = new char[7];
        MakeMnem(srcx, ms, opt);
        strncpy(P[n].mnem, ms, 7);
        fprintf(stdout, ".Added entry #%d, %s: %s %s [%s] (%s, @%s %s) %s {%s}\n", n+1,
			P[n].mnem, P[n].pren, P[n].name, P[n].cty, P[n].dob, P[n].pob, P[n].jud, P[n].fullname, P[n].link);
        n++;

      }
	return n;
}

void Catalog::ForceAdd(Person p) {
  P[n].link = strdup(p.link);
  P[n].mnem = strdup(p.mnem);
  P[n].name = strdup(p.name);
  P[n].fullname = strdup(p.fullname);
  P[n].pren = strdup(p.pren);
  P[n].dob = strdup(p.dob);
  P[n].cty = strdup(p.cty);
  P[n].pob = strdup(p.pob);
  P[n].jud = strdup(p.jud);
  n++;
}

int Catalog::GetByLastname(char *s) {
  int choice[1000];
  int nopt = 0;
  int ch;
  for (int i=0; i<n; i++) {
    if (strcmp(s, P[i].name)==0) {
       choice[nopt++] = i;
    }
  }
  if (nopt==0) return -1;
  if (nopt==1) return choice[0];
  do {
    for (int i=0; i<nopt; i++) {
      fprintf(stderr, "\n... [%d]. %s %s \t[%10s] ", i+1,
       P[choice[i]].pren, P[choice[i]].name, P[choice[i]].dob);
    }
    scanf("%d", &ch);
  } while (ch<=0 || ch>nopt);
  return choice[ch-1];
}

void Catalog::StartsWith(char *patt, int *choice, int &n) {
	n = 0;
	if (!patt) return;
	char cappatt[128];
	strncpy(cappatt, patt, 128);
	unsigned char sc = (unsigned char) cappatt[0];
	if (sc>='A' && sc<='Z') sc += 32;
	if (cappatt[0]>='a' && cappatt[0]<='z') cappatt[0] -= 32;
	if (cappatt[0]=='º') cappatt[0] = 'ª';
	if (cappatt[0]=='þ') cappatt[0] = 'Þ';
	for (int i=borna[sc]; i<borna[sc+1]; i++) {
		if (strstr(P[i].name, cappatt)==P[i].name ||
				strstr(P[i].mnem, patt)==P[i].mnem) {
			choice[n++] = i;
		}
	}
}

int Catalog::BestMatch(char *sn, char *sp, int delta) {
  int choice[1000];
  int nopt = 0;
  int ch;
  for (int i=0; i<n; i++) {
    if (strcmp(sn, P[i].name)==0) {
       choice[nopt++] = i;
    }
  }
  if (nopt==1) return choice[0];

  for (int i=0; i<n; i++) {
    int d = cdist(P[i].name, sn);
    if (d>0 && d<=delta) {
       choice[nopt++] = i;
    }
  }

  if (nopt<1) return -1;
  do {
    for (int i=0; i<nopt; i++) {
      fprintf(stderr, "\n... [%d]. %s %s \t[%10s] ", i+1,
       P[choice[i]].pren, P[choice[i]].name, P[choice[i]].dob);
    }
    scanf("%d", &ch);
  } while (ch<=0 || ch>nopt);
  return choice[ch-1];
}

void Catalog::GetInitial(int i, char *pini) {
    pini[1] = '.'; pini[2] = 0;
    if (i<0 || P[i].pren==NULL || P[i].pren[0]==' ') pini[0] = 0;
    else if (strlen(P[i].pren)>0) pini[0] = P[i].pren[0];
    else pini[0] = 0;
}


int Catalog::Save(const char *filename) {
  char nfilename[128];
  char ofilename[128];
	sprintf(nfilename, "%s.new", filename);
	sprintf(ofilename, "%s.old", filename);
  FILE *f = fopen(nfilename, "wt");
	if (f==NULL) {
		fprintf(stderr, "ERROR: cannot open file %s.\n", nfilename);
		return -1;
	}
  fprintf(f, "%d\n", n);
  for (int i=0; i<n; i++) {
    fprintf(f, "%s,%s,%s,%s,%s,%s,%s,%s,%s\n",
      P[i].mnem, P[i].name, P[i].pren, P[i].fullname, P[i].dob, P[i].cty, P[i].pob, P[i].jud, P[i].link);
  }
  fclose(f);
  rename(filename, ofilename);
  rename(nfilename, filename);
	return 0;
}

/* ******************************************************* */

Alias::Alias(char *an, int afy, int aly) {
	int len = strlen(an);
	name = new char[len+1];
	strcpy(name, an);
	fy = afy;
	ly = aly;
	next = NULL;
}

char * Venue::getName(int year) {
	Alias *a = alias;
	while (a!=NULL) {
		if (a->fy<=year && year<=a->ly) return a->name;
		a = a->next;
	}
	return name;
}

int Locations::Load(const char *cfilename, const char *vfilename) {
  char s[1024], *tk[100];
  nc = 0;
  FILE *f = fopen(cfilename, "rt");
  if (f==NULL) {
    fprintf(stderr, "ERROR: file %s not found.\n", cfilename);
    return -1;
  }
  fgets(s, 100, f);
  sscanf(s, "%d", &nc);
  C = new City[nc];
  for (int i=0; i<nc; i++) {
    fgets(s, 100, f);
    tk[0] = strtok(s, ",\t\n");
    for (int j=1; j<4; j++) tk[j] = strtok(NULL, ",\t\n");
 		copytk(&(C[i].mnem), tk[0]);
 		copytk(&(C[i].name), tk[1]);
 		copytk(&(C[i].jud), tk[2]);
	}
  fclose(f);

	char sc[5];
  nv = 0;
  f = fopen(vfilename, "rt");
  if (f==NULL) {
    fprintf(stderr, "ERROR: file %s not found.\n", vfilename);
    return -1;
  }
  fgets(s, 10, f);
  sscanf(s, "%d", &nv);
	int y;
	Alias *a, *b;
  V = new Venue[nv];
  for (int i=0; i<nv; i++) {
    fgets(s, 1000, f);
    tk[0] = strtok(s, ",\t\n");
    for (int j=1; j<16; j++) tk[j] = strtok(NULL, ",\t\n");
 		copytk(&(V[i].mnem), tk[0]);
 		copytk(&(V[i].name), tk[1]);
    strncpy(sc, V[i].mnem, 4); sc[4] = 0;
		int ci = FindCity(sc);
	  V[i].city = ci;
 		if (tk[2]) V[i].capacity = atoi(tk[2]);
		if (tk[3]) V[i].built = atoi(tk[3]);
		a = new Alias(tk[1], V[i].built, 9999);
		V[i].alias = a;
		for (int j=4; tk[j]!=NULL; j+=2) {
			if (tk[j+1]) y = atoi(tk[j+1]); else y = 0;
			b = new Alias(tk[j], 0, y);
			a->fy = y+1;
			b->next = a;
			V[i].alias = b;
			a = b;
		}
/*
		if (ci>=0) fprintf(stderr, "Located venue %s in %s.\n", V[i].name, C[ci].name);
		else fprintf(stderr, "Cannot locate venue %s.\n", V[i].mnem);
*/
	}
  fclose(f);

	return 0;
}

void Locations::StartsWith(char *patt, int *choice, int &n) {
	n = 0;
	if (!patt) return;
	for (int i=0; i<nv; i++) {
		if (strstr(V[i].mnem, patt)==V[i].mnem) choice[n++] = i;
	}
}

int Locations::FindCity(char *s) {
  for (int i=0; i<nc; i++) {
    if (strcmp(C[i].mnem, s)==0) return i;
  }
  return -1;
}

int Locations::FindCityName(char *s) {
  for (int i=0; i<nc; i++) {
    if (strcmp(C[i].name, s)==0) return i;
  }
  return -1;
}

int Locations::FindVenue(char *s) {
  for (int i=0; i<nv; i++) {
    if (strcmp(V[i].mnem, s)==0) return i;
  }
  return -1;
}

int Locations::FindStadium(int city, char *s) {
  for (int i=0; i<nv; i++) {
    if (strcmp(V[i].name, s)==0) {
      if (city<0) {
        return i;
      }
      if (strstr(V[i].mnem, C[city].mnem)==V[i].mnem) {
        return i;
      }
    }
  }
  return -1;
}
