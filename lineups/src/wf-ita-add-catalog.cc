#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "wf-catalog.hh"

#define CCAT_MNEM	0
#define CCAT_LAST	1
#define CCAT_FIRST	2
#define CCAT_FULL	3
#define CCAT_DOB	4
#define CCAT_NAT	5
#define CCAT_POB	6
#define CCAT_COB	7
#define CCAT_LINK	8

#define INS_LINK        0
#define INS_NAME        1
#define INS_FULL        2
#define INS_DOB 	3
#define INS_NAT 	4
#define INS_POB 	5
#define INS_COB 	6

#define NUM_PARTICLES 28

const char *particle[] = {
	"Van", "van", "Von", "von", "Al",    "El", "Della", "Dal", "Dalla", "Da",
    "De", "Dalle", "Di",  "Do",  "da",   "de",   "di",  "do",    "du", "Dos",
    "Mc", "La",    "Le",  "le",  "Lo",   "Ben", "Del", "del"};

char ctty[64];
int  year;

void Extract(char *s, char *sp, char *sn) {
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
		for (int i=0; i<NUM_PARTICLES; ++i) {
			if (strcmp(tkpi[n-2], particle[i])==0) {
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

void AddOne(char *w, Catalog *c) {
//ionel-danciulescu,dancio,Ionel Danciulescu,Danciulescu,Ionel Daniel,06.12.76,ROM,Slatina,
  char *tok[12];
	char slink[128], sname[128], spren[64], slnam[64], sfull[128], ins[256];
	char blank[3];
	char dob[20];
    char pob[64];
    char cob[4];
	if (!w) return;
	sprintf(blank, " ");
	sprintf(dob, "0000-00-00");
    sprintf(pob, " ");
    sprintf(cob, " ");

	tok[0] = strtok(w, ",\n");
	for (int j=1; j<10; ++j) tok[j] = strtok(NULL, ",\n");
	if (!tok[1]) return;

    strcpy(slink, tok[INS_LINK]);
	Extract(tok[INS_NAME], spren, slnam);
    strcpy(sfull, tok[INS_FULL]);
	if (!tok[INS_NAT]) tok[INS_NAT] = blank;
	if (!tok[INS_DOB]) tok[INS_DOB] = dob;
	if (!tok[INS_POB]) tok[INS_POB] = pob;
	if (!tok[INS_COB]) tok[INS_COB] = cob;
	sprintf(ins, "%s,%s,%s,%s,%s,%s,%s,%s", slink, slnam, spren, sfull, tok[INS_DOB], tok[INS_NAT], tok[INS_POB], tok[INS_COB]);
	c->Add(ins);
}

void AddLineup(char *w, Catalog *c) {
  char *tks, *tkt, *tkl, *tkc, *tkm, *tkp, *tkpi[10], *tkn, *tkf;
  char *tok[30], s[1024], spren[256], slnam[256], ins[512];
  int nr, ntr, npr, pos, min;
	strncpy(s, w, 1024);
  tkt = strtok(s, ":");
  tkl = strtok(NULL, "\n");
	if (!tkl) tkl = tkt;
  tkc = strrchr(tkl, '.');
	if (tkc) tkc[0] = 0;
  tok[0] = strtok(tkl, ",;\n");
  for (int i=1; i<22; ++i) tok[i] = strtok(NULL, ",;\n");
  nr = 0; while (tok[nr]) nr++;
  ntr = nr;
  for (int i=0; i<nr; ++i) {
    char *spar = strchr(tok[i], '(');
    tks = NULL;
    if (spar) {
      strtok(tok[i], "(");
      tks = strtok(spar+1, ")");
    }
	Extract(tok[i], spren, slnam);
	sprintf(ins, "%s,%s,%s,00/00/%d", slnam, spren, ctty, year);
	printf("\n#%d", i+1);
	c->AddSuggest(ins);

    if (tks) {
		if (strlen(tks)>5) {
	      tkm = strtok(tks, " .");
		  tkp = strtok(NULL, ")");
		  Extract(tkp, spren, slnam);
		  sprintf(ins, "%s,%s,%s,00/00/%d", slnam, spren, ctty, year);
		  c->AddSuggest(ins);
		  ntr++;
		}
    }
  }
}

void AddFromFile(char *filename, Catalog *cat) {
  FILE *f = fopen(filename, "rt");
  char s[512];
  if (!f) return;
  while (!feof(f)) {
    fgets(s, 512, f);
    if (feof(f) || s[0]==0) continue;
    s[strlen(s)-1] = 0;
    //AddOne(s, cat);
    cat->AddSuggest(s);
    s[0] = 0;
  }
  fclose(f);
}

int main(int argc, char **argv) {
	int opt;
	char sline[1024];
	char filename[128];
	char scriptfile[128]; scriptfile[0] = 0;
	strcpy(filename, "/home/radu/lineups/catalogs/players.dat");
	if (argc>1) { strcpy(filename, argv[1]); }
	if (argc>2) { strcpy(scriptfile, argv[2]); }

	Catalog *cat = new Catalog();
	cat->Load(filename);
    printf("Loaded %d entries.\n", cat->Size());
	strcpy(ctty, "ROM");
	year = 1956;
    if (scriptfile[0]!=0) AddFromFile(scriptfile, cat);
	do {
		printf("[1] Input line [2] Save [3] Year (%d) [4] Country (%s) [5] Add one [6] From file [0] Quit\n  ", year, ctty);
		scanf("%d", &opt);
		getc(stdin);
		switch(opt) {
			case 1:
				printf(">> Name: ");
				fgets(sline, 1024, stdin);
				AddLineup(sline, cat);
				break;
			case 2:
				cat->Save(filename);
				break;
			case 3:
				do {
					printf("Year: "); scanf("%d", &year);
				} while (year<1900 || year > 3000);
				break;
			case 4:
				do {
					printf("Country: "); scanf("%s", ctty);
				} while (strlen(ctty)!=3);
				break;
			case 5:
				printf("Info [Link,Prenume Nume,Nat,DOB,pob,cob]: \n  >>");
				fgets(sline, 1024, stdin);
				AddOne(sline, cat);
				break;
			case 6:
				printf("Filename: ");
				fgets(sline, 1024, stdin);
                sline[strlen(sline)-1] = 0;
				AddFromFile(sline, cat);
				break;
		}
	} while (opt);
	cat->Save(filename);
}
