#include "zz-catalog.hh"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>

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

#define CINS_INDEX	0
#define CINS_NAME	1
#define CINS_FULL	2
#define CINS_DOB	3
#define CINS_NAT	4
#define CINS_POB	5
#define CINS_COB	6

#define ZCAT_MNEM	0
#define ZCAT_LAST	1
#define ZCAT_FIRST	2
#define ZCAT_FULL	3
#define ZCAT_DOB	4
#define ZCAT_NAT	5
#define ZCAT_POB	6
#define ZCAT_COB	7
#define ZCAT_LINK	8
#define ZCAT_INDEX	9

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

#define ZNUM_PARTICLES 28

const char *zparticle[] = {
    "Van", "van", "Von", "von", "Al",    "El", "Della", "Dal", "Dalla", "Da",
    "De", "Dalle", "Di",  "Do",  "da",   "de",   "di",  "do",    "du", "Dos",
    "Mc", "La",    "Le",  "le",  "Lo",   "Ben", "Del", "del"};

void ZExtract(char *s, char *sp, char *sn) {
  char tok[512], *tkpi[10];
  int partic;

  strncpy(tok, s, 512);
    tkpi[0] = strtok(tok, " ");
    for (int j=1; j<10; ++j) tkpi[j] = strtok(NULL, " ");
    int n = 0; while (tkpi[n]) n++;

    if (n>0) strncpy(sn, tkpi[n-1], 64);
    strcpy(sp, " ");
    partic = 0;
    if (n>2) strcpy(sp, tkpi[0]);
    for (int j=1; j<n-2; ++j) {
        strcat(sp, " ");
        strcat(sp, tkpi[j]);
    }
    if (n>=2) {
        for (int i=0; i<ZNUM_PARTICLES; ++i) {
            if (strcmp(tkpi[n-2], zparticle[i])==0) {
                sprintf(sn, "%s %s", tkpi[n-2], tkpi[n-1]);
                partic = 1;
            }
        }
        if (partic==0 && n>1) {
            if (n==2) strcpy(sp, ""); else strcat(sp, " ");
            strcat(sp, tkpi[n-2]);
        }
    }
}

#define NUM_UTF 25
const char *utfsq[] = {
"_c3_a1", "_c4_87", "_c3_a7", "_c3_a9", "_c3_ad", "_c3_b3", "_c3_ba", "_c5_be", "_c5_a1", "_c4_8d",
"_c3_b1", "_c3_81", "_c4_86", "_c3_89", "_c3_8d", "_c3_93", "_c3_9a", "_c5_bd", "_c5_a0", "_c4_8c",
"_c3_87", "_c3_a3", "_c3_b4", "_c3_aa", "_20"
};
const char *isoseq[] = {
"a", "c", "c", "e", "i", "o", "u", "z", "s", "c",
"n", "a", "c", "e", "i", "o", "u", "z", "s", "c",
"c", "a", "o", "e", " "
};
void normalize(char *s) {
  int len = strlen(s);
  for (int i=0; i<len; i++) {
    if (s[i] == '%') s[i] = '_';
    if (s[i] >= 'A' && s[i] <= 'Z') s[i] += 32;
  }
  for (int i=0; i<NUM_UTF; i++) {
    char *us = strstr(s, utfsq[i]);
    while (us) {
       int l = strlen(us);
       int u = strlen(utfsq[i])-1;
       for (int j=0; j<=l-u; j++) us[j] = us[j+u];
       us[0] = isoseq[i][0];
       us = strstr(s, utfsq[i]);
    }
  }
}

void Person::makeHandle(CURL *curl, char *s) {
  char *slong = curl_easy_escape(curl, fullname, strlen(fullname));
  normalize(slong);
        char *tkl[12];
        tkl[0] = strtok(slong, " "); for (int i=1; i<12; i++) tkl[i] = strtok(NULL, " ");
        int ntkl = 0; for (int i=0; i<12; i++) if (tkl[i]) ntkl++;
        char *ln = tkl[ntkl-1];
        char *fn = tkl[0];
        if (ntkl<2) {
          char fullc[128];
          strncpy(fullc, fullname, 128);
          char *tkf[12];
          tkf[0] = strtok(fullc, " "); for (int i=1; i<12; i++) tkf[i] = strtok(NULL, " ");
          int ntkf = 0;
          for (int i=0; i<12; i++) {
            if (tkf[i]) { toLower(tkf[i]); ntkf++; };
          }
          if (ntkf>0) fn = tkf[0];
        }

        s[0] = ln[0];
        int d = 0; int k=1;
        while (k<4) {
          if (k>=strlen(ln)) s[k]='x';
          else s[k] = ln[k];
          k++;
        }
        s[4] = fn[0];
        if (ntkl>2) s[5] = tkl[1][0];
        else if (strlen(fn)<2) s[5] = '_';
        else if (strlen(fn)>1) s[5] = fn[1];
        else s[5] = 'x';
        s[6] = 0;
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
     strncpy(P[i].mnem, t[ZCAT_MNEM], 7);
    P[i].name = new char[strlen(t[ZCAT_LAST])+1];
     strcpy(P[i].name, t[ZCAT_LAST]);
    P[i].pren = new char[strlen(t[ZCAT_FIRST])+1];
     strcpy(P[i].pren, t[ZCAT_FIRST]);
    P[i].fullname = new char[strlen(t[ZCAT_FULL])+1];
     strcpy(P[i].fullname, t[ZCAT_FULL]);

 	copytk(&(P[i].dob), t[ZCAT_DOB]);
 	copytk(&(P[i].cty), t[ZCAT_NAT]);
 	copytk(&(P[i].pob), t[ZCAT_POB]);
 	copytk(&(P[i].jud), t[ZCAT_COB]);

    P[i].link = new char[strlen(t[ZCAT_LINK])+1];
     strcpy(P[i].link, t[ZCAT_LINK]);

    P[i].index = new char[strlen(t[ZCAT_INDEX])+1];
     strcpy(P[i].index, t[ZCAT_INDEX]);

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

void Catalog::MakeMnem(char *ins, char *ms, int opt) {
  char *tks[12];
  strtok(ins, "/");
  if (opt == 1) {
    tks[0] = strtok(ins, "-"); for (int i=1; i<12; i++) tks[i] = strtok(NULL, "-");
  } else {
    tks[0] = strtok(ins, " "); for (int i=1; i<12; i++) tks[i] = strtok(NULL, " ");
  }
  int ntks = 0; for (int i=0; i<12; i++) if (tks[i]) ntks++;
  toLower(tks[ntks-1]);
  char *ln = tks[ntks-1];
  char *fn = new char[12];
  strncpy(fn, tks[0], 10);
  toLower(fn);
  ms[0] = ln[0];
  int d = 0; int k=1;
  while (k<4) {
    if (k>=strlen(ln)) ms[k]='x'; else ms[k] = ln[k];
    k++;
  }
  ms[4] = fn[0];
  if (ntks>2) {
    char *mn = new char[10];
    strncpy(mn, tks[1], 8);
    toLower(mn);
    ms[5] = mn[0];
  }
  else if (strlen(fn)<2) ms[5] = '_';
  else if (strlen(fn)>1) ms[5] = fn[1];
  else ms[5] = 'x';
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

int Catalog::FindMnem(char *s) {
  int c = ((unsigned char)s[0]);
  for (int i=borna[c]; i<borna[c+1]; i++)
    if (strcmp(P[i].mnem, s)==0) return i;
  return -1;
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

int Catalog::FindIndex(char *s) {
  for (int i=0; i<n; i++) {
    if (strcmp(P[i].index, s)==0) return i;
  }
  return -1;
}

int Catalog::FindIndexPrefix(char *s) {
  for (int i=0; i<n; i++) {
    if (strstr(P[i].index, s)!=NULL) return i;
  }
  return -1;
}

int Catalog::Add(/*CURL *curl, */char *ins, int opt) {
//    sprintf(ins, "%s,%s,%s,%s,%s,%s,%s,%s", slink, slnam, spren, sfull, tok[INS_DOB], tok[INS_NAT], tok[INS_POB], tok[INS_COB]);
  char s[1024], *tk[100], kol[64];
  int ch, choice[1000], nopt, pa;
	char slink[64], sfull[128], slast[64], sfirst[64], snat[64], sdob[64], spob[64], scob[64], sindex[64];
	int perfect_match;

	strcpy(s, ins);
	int len = strlen(s);
	tk[0] = strtok(s, ",\n");
	for (int j=1; j<10; ++j) tk[j] = strtok(NULL, ",");

	strcpy(slink,  "___");
	if (tk[0]) strcpy(sindex, tk[0]); else strcpy(sindex,   "-1");
	if (tk[1]) strcpy(slast,  tk[1]); else strcpy(slast,  "_____");
	if (tk[2]) strcpy(sfirst, tk[2]); else strcpy(sfirst,  "__");
	if (tk[3]) strcpy(sfull,  tk[3]); else strcpy(sfull,  "__ ____");
	if (tk[4]) strcpy(sdob,   tk[4]); else strcpy(sdob,   "0000-00-00");
	if (tk[5]) strcpy(snat,   tk[5]); else strcpy(snat,   " ");
	if (tk[6]) strcpy(spob,   tk[6]); else strcpy(spob,   " ");
	if (tk[7]) strcpy(scob,   tk[7]); else strcpy(scob,   " ");

    for (int i=0; i<n; i++) {
      if (strcmp(P[i].index, sindex)==0) {
        fprintf(stdout, "   %s[%2d] %s: %-12s, %-20s%s (%3s) [%10s, @%s (%s)] %s {%s, %s}%s\n",
          hiyellow, nopt+2, P[i].mnem, P[i].name, P[i].pren, normal, P[i].cty, P[i].dob, P[i].pob, P[i].jud, P[i].fullname,
            P[i].link, P[i].index, normal);
 	    return i;
      }
    }

        P[n].link = new char[strlen(slink)+1];
        strcpy(P[n].link, slink);
        P[n].fullname = new char[strlen(sfull)+1];
        strcpy(P[n].fullname, sfull);
        P[n].name = new char[strlen(slast)+1];
        strcpy(P[n].name, slast);
        P[n].pren = new char[strlen(sfirst)+1];
        strcpy(P[n].pren, sfirst);
        P[n].cty = new char[strlen(snat)+1];
        strcpy(P[n].cty, snat);
        P[n].dob = new char[12];
        strcpy(P[n].dob, sdob);
        P[n].pob = new char[strlen(spob)+1];
        strcpy(P[n].pob, spob);
        P[n].jud = new char[strlen(spob)+1];
        strcpy(P[n].jud, scob);
        P[n].index = new char[strlen(sindex)+1];
        strcpy(P[n].index, sindex);
/*
        char *pmnem = new char[15];
        P[n].makeHandle(curl, pmnem);

        P[n].mnem = new char[8];
        strncpy(P[n].mnem, pmnem, 6);

        if (SearchMnem(P[n].mnem)>=0) {
          fprintf(stderr, "%s.%s used%s.\n", green, P[n].mnem, normal);
          int k=0;
          do {
            k++;
            if (k==10) { k=17; }; // skip over special characters
            if (k==43) {
              P[n].mnem[4] = (P[n].mnem[4]<'9' ? P[n].mnem[4]+1 : '1');
              k=1;
            }
            P[n].mnem[5] = (char) (k+48);
          } while (SearchMnem(P[n].mnem)>=0);
        }
        fprintf(stdout, ".Added entry #%d, %s: %s%s %s%s (%s) [%s, @%s (%s)] %s {%s, %s}\n", n+1,
			P[n].mnem, hicyan, P[n].pren, P[n].name, normal, P[n].cty, P[n].dob, P[n].pob, P[n].jud, P[n].fullname, P[n].link, P[n].index);
        n++;
*/
        P[n].mnem = new char[7];
        char srcx[64];
        if (opt==1) strncpy(srcx, P[n].index, 64);
        else sprintf(srcx, "%s %s", P[n].pren, P[n].name);
        char *ms = new char[7];
        MakeMnem(srcx, ms, opt);
        strncpy(P[n].mnem, ms, 7);

        fprintf(stdout, ".Added entry #%d, %s: %s%s %s%s (%s) [%s, @%s (%s)] %s {%s}\n", n+1,
            P[n].mnem, hicyan, P[n].pren, P[n].name, normal, P[n].cty, P[n].dob, P[n].pob, P[n].jud, P[n].fullname, P[n].link);
        n++;

	return n;
}

int Catalog::AddSuggest(/*CURL *curl, */char *ins, int opt) {
  char s[1024], *tk[100], kol[64];
  int ch, choice[2000], nopt, pa;
	char slink[64], sfull[128], slast[64], sfirst[64], snat[64], sdob[64], spob[64], scob[64], sindex[64];
	int perfect_match;

	strcpy(s, ins);
	int len = strlen(s);
	tk[0] = strtok(s, ",\n");
	for (int j=1; j<10; ++j) tk[j] = strtok(NULL, ",");

//12345,Antonio Donnarumma, ,1990-07-07,ITA,Castellammare di Stabia,ITA
	strcpy(slink,  "___");
	if (tk[CINS_INDEX]) strcpy(sindex, tk[CINS_INDEX]); else strcpy(sindex, "-1");
	if (tk[CINS_NAME])  {
      ZExtract(tk[CINS_NAME], sfirst, slast);
    } else strcpy(slast,  "_____");
	if (tk[CINS_FULL])  strcpy(sfull,  tk[CINS_FULL]);  else strcpy(sfull,  "__ ____");
	if (tk[CINS_DOB])   strcpy(sdob,   tk[CINS_DOB]);   else strcpy(sdob,   "0000-00-00");
	if (tk[CINS_NAT])   strcpy(snat,   tk[CINS_NAT]);   else strcpy(snat,   " ");
	if (tk[CINS_POB])   strcpy(spob,   tk[CINS_POB]);   else strcpy(spob,   " ");
	if (tk[CINS_COB])   strcpy(scob,   tk[CINS_COB]);   else strcpy(scob,   " ");

  fprintf(stdout, "\n");
  nopt = 0;
  perfect_match = 0;
  for (int i=0; i<n; i++) {
    if (strcmp(P[i].index, sindex)==0) {
	  strcpy(kol, hiyellow);
      choice[nopt] = i;
      fprintf(stdout, "   %s[%2d] %s: %-12s, %-20s%s (%3s) [%10s, @%s (%s)] %s {%s, %s}\n",
        kol, nopt+2, P[i].mnem, P[i].name, P[i].pren, normal, P[i].cty, P[i].dob, P[i].pob, P[i].jud, P[i].fullname, P[i].link, P[i].index);
      nopt++;
	  perfect_match = i+1;
    }
    else if (strcmp(P[i].fullname, sfull)==0 && strcmp(P[i].dob, sdob)==0 && strlen(sfull)>1 && strcmp(sdob,"0000-00-00")!=0) {
	  strcpy(kol, yellow);
      choice[nopt] = i;
      fprintf(stdout, "   %s[%2d] %s: %-12s, %-20s%s (%3s) [%10s, @%s (%s)] %s {%s, %s}\n",
        kol, nopt+2, P[i].mnem, P[i].name, P[i].pren, normal, P[i].cty, P[i].dob, P[i].pob, P[i].jud, P[i].fullname, P[i].link, P[i].index);
      nopt++;
	  perfect_match = i+1;
    }
    else if (strcmp(P[i].fullname, sfull)==0 && strlen(sfull)>1) {
	  strcpy(kol, hicyan);
      choice[nopt] = i;
      fprintf(stdout, "   %s[%2d] %s: %-12s, %-20s%s (%3s) [%10s, @%s (%s)] %s {%s, %s}\n",
        kol, nopt+2, P[i].mnem, P[i].name, P[i].pren, normal, P[i].cty, P[i].dob, P[i].pob, P[i].jud, P[i].fullname, P[i].link, P[i].index);
      nopt++;
    }
    else if (strcmp(P[i].dob, sdob)==0 && strlen(sdob)>1 && sdob[0]!=' ' && strcmp(sdob,"0000-00-00")!=0) {
	  strcpy(kol, hicyan);
      choice[nopt] = i;
      fprintf(stdout, "   %s[%2d] %s: %-12s, %-20s%s (%3s) [%10s, @%s (%s)] %s {%s, %s}\n",
      kol, nopt+2, P[i].mnem, P[i].name, P[i].pren, normal, P[i].cty, P[i].dob, P[i].pob, P[i].jud, P[i].fullname, P[i].link, P[i].index);
      nopt++;
    }
    else if (strstr(P[i].name, slast)!=NULL) {
	  strcpy(kol, green);
      choice[nopt] = i;
      fprintf(stdout, "   %s[%2d] %s: %-12s, %-20s%s (%3s) [%10s, @%s (%s)] %s {%s, %s}\n",
      kol, nopt+2, P[i].mnem, P[i].name, P[i].pren, normal, P[i].cty, P[i].dob, P[i].pob, P[i].jud, P[i].fullname, P[i].link, P[i].index);
      nopt++;
    }
    else if (strlen(sfull)>5 && cdist(P[i].fullname, sfull)<5) {
	  strcpy(kol, hired);
      choice[nopt] = i;
      fprintf(stdout, "   %s[%2d] %s: %-12s, %-20s%s (%3s) [%10s, @%s (%s)] %s {%s, %s}\n",
      kol, nopt+2, P[i].mnem, P[i].name, P[i].pren, normal, P[i].cty, P[i].dob, P[i].pob, P[i].jud, P[i].fullname, P[i].link, P[i].index);
      nopt++;
    }
    else {
      int delta = cdist(P[i].name, slast);
      if (delta<=2) {
  	    strcpy(kol, red);
        choice[nopt] = i;
        fprintf(stdout, "   %s[%2d] %s: %-12s, %-20s%s (%3s) [%10s, @%s (%s)] %s {%s, %s}\n",
  	       kol, nopt+2, P[i].mnem, P[i].name, P[i].pren, normal, P[i].cty, P[i].dob, P[i].pob, P[i].jud, P[i].fullname, P[i].link, P[i].index);
				nopt++;
			}
		}
  }

    int add = 0;
	if (perfect_match) {
      P[perfect_match-1].index = new char[strlen(sindex)+1];
      strcpy(P[perfect_match-1].index, sindex);
      if (P[perfect_match-1].dob==NULL || strlen(P[perfect_match-1].dob)<4) {
        P[perfect_match-1].dob = new char[12];
        strncpy(P[perfect_match-1].dob, sdob, 12);
      }
      return perfect_match-1;
    }

	fprintf(stdout, "...%s           : %-12s, %-20s (%3s) [%10s, @%s (%s)] %s {%s, %s} [0]=cancel [1]=add [2-%d]=use ",
		normal, slast, sfirst, snat, sdob, spob, scob, sfull, slink, sindex, nopt+2);
      scanf("%d", &add);
//      add=1;
      if (add>1 && add<=nopt+2) {
		int k = choice[add-2];
        P[k].index = new char[strlen(sindex)+1];
        strcpy(P[k].index, sindex);
        P[k].fullname = new char[strlen(sfull)+1];
        strcpy(P[k].fullname, sfull);
        if (P[k].dob==NULL || strlen(P[k].dob)<2 || strcmp(P[k].dob, "0000-00-00")==0) {
          P[k].dob = new char[12];
          strncpy(P[k].dob, sdob, 12);
        }
        if (P[k].pob==NULL || strlen(P[k].pob)<2) {
          P[k].pob = new char[strlen(spob)+1];
          strcpy(P[k].pob, spob);
        }
        if (P[k].jud==NULL || strlen(P[k].jud)<2) {
          P[k].jud = new char[strlen(scob)+1];
          strcpy(P[k].jud, scob);
        }
        return k;
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
        P[n].cty = new char[strlen(snat)+1];
        strcpy(P[n].cty, snat);
        P[n].dob = new char[12];
        strcpy(P[n].dob, sdob);
        P[n].pob = new char[strlen(spob)+1];
        strcpy(P[n].pob, spob);
        P[n].jud = new char[strlen(scob)+1];
        strcpy(P[n].jud, scob);
        P[n].index = new char[strlen(sindex)+1];
        strcpy(P[n].index, sindex);

/*
        char *pmnem = new char[15];
        P[n].makeHandle(curl, pmnem);
        P[n].mnem = new char[8];
        strncpy(P[n].mnem, pmnem, 6);

        if (SearchMnem(P[n].mnem)>=0) {
          fprintf(stderr, "...Mnemonic %s already in use.\n", P[n].mnem);
          int k=0;
          do {
            k++;
            if (k==10) { k=17; }; // skip over special characters
            P[n].mnem[5] = (char) (k+48);
          } while (SearchMnem(P[n].mnem)>=0);
        }
        fprintf(stdout, ".Added entry #%d, %s: %s %s [%s] (%s, @%s %s) %s {%s, %s}\n", n+1,
			P[n].mnem, P[n].pren, P[n].name, P[n].cty, P[n].dob, P[n].pob, P[n].jud, P[n].fullname, P[n].link, P[n].index);

        n++;
*/
        P[n].mnem = new char[7];
        char srcx[64];
        if (opt==1) strncpy(srcx, P[n].index, 64);
        else sprintf(srcx, "%sxx %sxxxx", P[n].pren, P[n].name);
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
  P[n].index = strdup(p.index);
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

void Catalog::StartsWith(char *patt, int *opt, int &n) {
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
			opt[n++] = i;
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
    fprintf(f, "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",
      P[i].mnem, P[i].name, P[i].pren, P[i].fullname, P[i].dob, P[i].cty, P[i].pob, P[i].jud, P[i].link, P[i].index);
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

void Locations::StartsWith(char *patt, int *opt, int &n) {
	n = 0;
	if (!patt) return;
	for (int i=0; i<nv; i++) {
		if (strstr(V[i].mnem, patt)==V[i].mnem) opt[n++] = i;
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
