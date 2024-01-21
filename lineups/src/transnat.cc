#include <stdio.h>
#include <string.h>

int isNumeric(char *s) {
  if (s==NULL) return 0;
  if (strlen(s)<1) return 0;
  if (s[0]>='0' && s[0]<='9') return 1;
  return 0;
}

void toLower(char *s) {
  if (s==NULL) return;
  int l = strlen(s);
  if (l<2) return;
  for (int i=1; i<l; i++) {
    if (s[i]>='A' && s[i]<='Z') s[i] += 32;
    else if (s[i]=='Ã') s[i]='ã';
    else if (s[i]=='Â') s[i]='â';
    else if (s[i]=='ª') s[i]='º';
    else if (s[i]=='Þ') s[i]='þ';
    else if (s[i]=='Î') s[i]='î';
    else if (s[i]=='Ö') s[i]='ö';
    else if (s[i]=='Ü') s[i]='ü';
    else if (s[i]=='Ó') s[i]='o';
/*
    else if (s[i]=='') s[i]='';
    else if (s[i]=='') s[i]='';
    else if (s[i]=='') s[i]='';
    else if (s[i]=='') s[i]='';
    else if (s[i]=='') s[i]='';
*/
  }
}

int main() {
  char s[5000], *tk[100];
  FILE *f = fopen("nationala.spc", "rt");
  while(!feof(f)) {
    fgets(s, 2000, f);
    if (strlen(s)<20) continue;
    tk[0] = strtok(s, "\t\n");
    for (int j=1; j<100; j++) tk[j] = strtok(NULL, "\t\n");
    if (!isNumeric(tk[0])) {
      fprintf(stdout, " \t");
      toLower(tk[1]);
    }
    else {
      toLower(tk[2]);
    }
    for (int j=0; j<100; j++) {
      if (tk[j]!=NULL) fprintf(stdout, "%s\t", tk[j]);
    }
    fprintf(stdout, "\n");
    s[0] = 0;
  }
  fclose(f);
}
