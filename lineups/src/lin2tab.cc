#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX 1000
#define LEN 100

int NE;

int ech[MAX];
char pren[MAX][LEN];
char name[MAX][LEN];
int mec[MAX];
int tit[MAX];
int rez[MAX];
int gol[MAX];

int Find(int xt, char *pr, char *nm) {
  int j, num, fj, plen;
  plen = 0;
  if (pr!=NULL) plen = strlen(pr);
  if (plen>0 && pr[0]!='_') {
    j = 0;
    while (j<NE) {
       if (ech[j]==xt && strcmp(name[j], nm)==0) {
         if (strcmp(pren[j], pr)==0) {
           return j;
         }
       }
       j++;
    }
  }
  if (pr[0]=='_') {
    j = 0;
    num = 0;
    fj = 0;
    while (j<NE) {
       if (ech[j]==xt && strcmp(name[j], nm)==0) {
         fj = j;
         num++;
       }
       j++;
    }
    if (num==1) return fj;
  }
  else if (plen>0) {
    j = 0;
    num = 0;
    fj = 0;
    while (j<NE) {
       if (ech[j]==xt && strcmp(name[j], nm)==0) {
         if (strstr(pren[j],pr)==pren[j]) {
           fj = j;
           num++;
         }
         else if (strcmp(pren[j],"_")==0) {
           fj = j;
           num++;
         }
         else if (strstr(pr,pren[j])!=NULL) {
           fj = j;
           num++;
         }
       }
       j++;
    }
    if (num==1 && strcmp(pren[fj],"_")==0) strcpy(pren[fj], pr);
    if (num==1 && strstr(pr,pren[fj])!=NULL) strcpy(pren[fj], pr);
    if (num==1) return fj;
  }
  return NE;
}

int main(int argc, char **argv) {
  char filename[64];
  if (argc < 2) {
    fprintf(stderr, "ERROR: no input file.\n");
    return -1;
  }

  sprintf(filename, "%s", argv[1]);
  FILE *f = fopen(filename, "rt"); 
  if (f==NULL) {
    fprintf(stderr, "ERROR: file %s not found.\n", filename);
    return -2;
  }

  NE = 0;
  for (int i=0; i<MAX; i++) {
    ech[i] = -1;
    pren[i][0] = 0;
    name[i][0] = 0;
    mec[i] = tit[i] = rez[i] = gol[i] = 0;
  }

  char s[5000], *tk[22], *tke, *tkt, *tkr;
  char defp[3];
  char *tkp, *tkp2, *tkn, *tkg, *tnm;
  int tm, g;
  int s1[2], s2[2], ntt[2], nrz[2];

  strcpy(defp, "_");
  int k = 0;
  int h = k%2;
  while (!feof(f)) {
    fgets(s, 5000, f);
    if (strlen(s)<10) continue;
    tke = strtok(s, ";");
    tkt = strtok(NULL, ";\n");
    tkr = strtok(NULL, ";\n");
    if (tke) tm  = atoi(tke); else tm = -1;
    h = k%2;
    s1[h] = s2[h] = ntt[h] = nrz[h] = 0;

    tk[0] = strtok(tkt, ",;\n");
    for (int j=1; j<22; j++) tk[j] = strtok(NULL, ",;\n");
    for (int j=0; j<22; j++) if (tk[j]!=NULL) {
      tnm  = strtok(tk[j], "+");
      tkg  = strtok(NULL, ",\n");
      tkp  = strtok(tnm, " ");
      tkp2 = strtok(NULL, " ");
      tkn  = strtok(NULL, " ");
      if (tkn==NULL) { tkn = tkp2; tkp2 = NULL; }
      if (tkn==NULL) { tkn = tkp; tkp = defp; }
      if (tkn!=NULL) {
        if (tkg!=NULL) g = atoi(tkg); else g=0;
        int p = Find(tm, tkp, tkn); 
        if (p>=NE) {
          ech[NE] = tm;
          if (tkp2==NULL) strcpy(pren[NE], tkp);
          else sprintf(pren[NE], "%s %s", tkp, tkp2);
          strcpy(name[NE], tkn);
          NE++;
        }
        mec[p]++;
        tit[p]++;
        gol[p] += g;
        ntt[h]++;
        if (j==0) s1[h] -= g; else s2[h] += g;
        fprintf(stderr, "Added %2d.[%3d] %-20s %-20s: %2d %2d %2d %2d\n",
           j+1, tm, pren[p], name[p], mec[p], tit[p], rez[p], gol[p]);
      }
    }

    fprintf(stderr, "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - \n");
    tk[0] = strtok(tkr, ",;\n");
    for (int j=1; j<5; j++) tk[j] = strtok(NULL, ",;\n");
    for (int j=0; j<5; j++) if (tk[j]!=NULL) {
      tnm  = strtok(tk[j], "+");
      tkg  = strtok(NULL, ",\n");
      tkp  = strtok(tnm, " ");
      tkp2 = strtok(NULL, " ");
      tkn  = strtok(NULL, " ");
      if (tkn==NULL) { tkn = tkp2; tkp2 = NULL; }
      if (tkn==NULL) { tkn = tkp; tkp = defp; }
      if (tkn!=NULL) {
        if (tkg!=NULL) g = atoi(tkg); else g=0;
        int p = Find(tm, tkp, tkn); 
        if (p>=NE) {
          ech[NE] = tm;
          if (tkp2==NULL) strcpy(pren[NE], tkp);
          else sprintf(pren[NE], "%s %s", tkp, tkp2);
          strcpy(name[NE], tkn);
          NE++;
        }
        mec[p]++;
        rez[p]++;
        gol[p] += g;
        fprintf(stderr, "Added %2d.[%3d] %-20s %-20s: %2d %2d %2d %2d\n",
           j+12, tm, pren[p], name[p], mec[p], tit[p], rez[p], gol[p]);
        nrz[h]++;
        s2[h] += g;
      }
    }

    if (h==1) {
      fprintf(stderr, "======================================================================\n");
      fprintf(stderr, "*** #%2d: scor %d-%d / %d-%d   (%d;%d / %d;%d) ***\n", (k+1)/2,
              s1[1], s1[0], s2[0], s2[1], ntt[0], nrz[0], ntt[1], nrz[1]);
    }    
    fprintf(stderr, "======================================================================\n");
    s[0] = 0;
    k++;
  }

  fclose(f);
  fprintf(stderr, "%d entries.\n", NE);

  for (int j=0; j<NE; j++) {
    fprintf(stdout, "%s\t%s\t%d\t%d\t%d\t%d\t%d\n",
      name[j], pren[j], ech[j], mec[j], tit[j], rez[j], gol[j]);
  }

  return 0;
}
