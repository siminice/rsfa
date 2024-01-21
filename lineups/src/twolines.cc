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
    else if (s[i1]==200 && s[i1+1]==154) {i1++; s[i2++]='Ş';}
    else if (s[i1]==196 && s[i1+1]==131) {i1++; s[i2++]='ã';}
    else if (s[i1]==195 && s[i1+1]==162) {i1++; s[i2++]='â';}
    else if (s[i1]==200 && s[i1+1]==153) {i1++; s[i2++]='º';}
    else if (s[i1]==200 && s[i1+1]==155) {i1++; s[i2++]='ş';}

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
    else if (s[i1]==195 && s[i1+1]==182) {i1++; s[i2++]='ö';}
    else if (s[i1]==195 && s[i1+1]==186) {i1++; s[i2++]='ú';}
    else if (s[i1]==195 && s[i1+1]==189) {i1++; s[i2++]='ı';}
    else if (s[i1]==200 && s[i1+1]==155) {i1++; s[i2++]='ş';}
    else s[i2++] = s[i1];
  }
  s[i2] = 0;
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

void purge(char *s) {
  delsub(s, " (Cap)");
  delsub(s, " ***");
  delsub(s, " **");
  delsub(s, " *");
  delsub(s, "*** ");
  delsub(s, "** ");
  delsub(s, "* ");
  int k=strlen(s)-1;
  while ((s[k]==' ' || s[k]=='\t') && k>0) k--;
  s[k+1]=0;
}

int minute(char *s) {
	if (!s) return 91;
	char *t = s;
	while (t && t[0]==' ') t++;
	if (t[0]=='-') return 91;
	strtok(t, "'");
	return atoi(t);
}

int main() {
  int nl = 0;
  char s[1000], w[100];
  lin1[0] = 0;
  lin2[0] = 0;
	int min = 91;
  while (!feof(stdin) && nl<56) {
    fgets(s, 1000, stdin);
    s[strlen(s)-1] = 0;
    if (nl%4==0) {
      if (nl>0 && nl!=44) strcat(lin1,",");
      if (nl==44) strcat(lin1,";");
      purge(s);
      utf2iso((unsigned char*)s);
      strcat(lin1,s);
    }
		else if (nl%4==1) {
			min = minute(s);
			sprintf(w, ":%d", min);
			strcat(lin1, w);
		}
		else if (nl%4==1 || nl%4==2) {
			min = minute(s);
			sprintf(w, ":%d", min);
		}
    else if (nl%4==3) {
      if (nl>3 && nl!=47) strcat(lin2,",");
      if (nl==47) strcat(lin2,";");
      purge(s);
      utf2iso((unsigned char*)s);
      strcat(lin2,s);
			strcat(lin2, w);
    }
    nl++;
  }

  fprintf(stdout, "%s\n%s\n", lin1, lin2);
  return 0;
}
