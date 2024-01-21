#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char buf[60][100];
char lin1[1000];
char lin2[1000];

void utf2iso(unsigned char *s) {
  if (s==NULL) return;
  int l = strlen((char *)s);
  int i2=0;
  for (int i1=0; i1<l; i1++) {
//    printf("%d ", s[i]);
         if (s[i1]==200 && s[i1+1]==152) {i1++; s[i2++]='ª';}
         if (s[i1]==197 && s[i1+1]==158) {i1++; s[i2++]='ª';}
    else if (s[i1]==200 && s[i1+1]==154) {i1++; s[i2++]='Þ';}
    else if (s[i1]==196 && s[i1+1]==131) {i1++; s[i2++]='ã';}
    else if (s[i1]==195 && s[i1+1]==162) {i1++; s[i2++]='â';}
    else if (s[i1]==200 && s[i1+1]==153) {i1++; s[i2++]='º';}
    else if (s[i1]==200 && s[i1+1]==155) {i1++; s[i2++]='þ';}

    else if (s[i1]==195 && s[i1+1]==129) {i1++; s[i2++]='Á';}
    else if (s[i1]==197 && s[i1+1]==160) {i1++; s[i2++]='©';}
    else if (s[i1]==197 && s[i1+1]==0) {i1++; s[i2++]='È';}
    else if (s[i1]==197 && s[i1+1]==189) {i1++; s[i2++]='®';}
    else if (s[i1]==196 && s[i1+1]==141) {i1++; s[i2++]='è';}
    else if (s[i1]==197 && s[i1+1]==161) {i1++; s[i2++]='¹';}
    else if (s[i1]==200 && s[i1+1]==155) {i1++; s[i2++]='¾';}
    else if (s[i1]==195 && s[i1+1]==161) {i1++; s[i2++]='á';}
    else if (s[i1]==196 && s[i1+1]==135) {i1++; s[i2++]='æ';}
    else if (s[i1]==195 && s[i1+1]==169) {i1++; s[i2++]='é';}
    else if (s[i1]==195 && s[i1+1]==173) {i1++; s[i2++]='í';}
    else if (s[i1]==195 && s[i1+1]==174) {i1++; s[i2++]='î';}
    else if (s[i1]==195 && s[i1+1]==179) {i1++; s[i2++]='ó';}
    else if (s[i1]==195 && s[i1+1]==180) {i1++; s[i2++]='ô';}
    else if (s[i1]==195 && s[i1+1]==186) {i1++; s[i2++]='ú';}
    else if (s[i1]==195 && s[i1+1]==189) {i1++; s[i2++]='ý';}
    else if (s[i1]==200 && s[i1+1]==155) {i1++; s[i2++]='þ';}
    else s[i2++] = s[i1];
  }
  s[i2] = 0;
}

void printName(char *s) {
  if (strchr(s, '.')!=NULL) {
    char *tkp = strtok(s, ".");
    char *tkn = strtok(NULL, ",");
    fprintf(stdout, "%s %s", tkp, tkn);
  }
//  else fprintf(stdout, "_ %s", s);
  else fprintf(stdout, "%s", s);
}

void delsub(char *s, char *ss) {
  int l = strlen(ss);
  char *cp=strstr(s, ss);
  if (cp!=NULL) {
    char *s2 = cp+l;
    cp[0] = 0;
    sprintf(cp, "%s", s2);
  }  
}

void trim(char *s) {
  if (s==NULL) return;
  int l = strlen(s);
  if (l<=1) return;
  int j = l-1;
  while ((s[j]==' ' || s[j]=='\t' || (s[j]>='0' && s[j]<='9')) && j>0) j--;
  int i = 0;
  while ((s[i]==' ' || (s[i]>='0' && s[i]<='9')) && i<j) i++;
  if (i>0 || j<l-1) {
    s[j+1] = 0;
    memmove(s, s+i, j-i+1);
    s[j-i+1]=0;
  }
}

int main(int argc, char **argv) {
  char s[1000], outs[2000];
  char tit[22][100];
  char sub[4][100];
  char *tok[100], tkp, tkn;
  int ntit = 0;
  int nsub = 0;
  int utf = 0;
   
  for (int i=1; i<argc; i++) {
    if (strcmp(argv[i], "-u")==0) utf = 1;
  }
 
  while(!feof(stdin)) {
    fgets(s, 1000, stdin);
    tok[0] = strtok(s, "\t\n");
    for (int i=1; i<30; i++) tok[i] = strtok(NULL, "\t\n");
    ntit = 0;
    nsub = 0;
    int j = 2;
    while (tok[j]!=NULL) {
      if (utf) utf2iso((unsigned char*)tok[j]);
      trim(tok[j]);
      if (ntit<11) {
        strcpy(tit[ntit], tok[j]);
        ntit++;
      }
      else {
        strcpy(sub[nsub], tok[j]);
        nsub++;
      }
      j++;
    }

    for (int i=0; i<ntit; i++) {
      printName(tit[i]);
      if (i<ntit-1) fprintf(stdout, ",");
    }
    fprintf(stdout, ";");
    for (int i=0; i<nsub; i++) {
      printName(sub[i]);
      if (i<nsub-1) fprintf(stdout, ",");
    }
    fprintf(stdout,"\n");

    fprintf(stderr, " %d,%d\n", ntit, nsub);
  }

  return 0;
}
