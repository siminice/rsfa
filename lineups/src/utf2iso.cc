#include <stdio.h>
#include <string.h>
#include <stdlib.h>


void utf2iso(unsigned char *s) {
  if (s==NULL) return;
  int l = strlen((char *)s);
  int i2=0;
  for (int i1=0; i1<l; i1++) {
    printf("%d ", s[i1]);
         if (s[i1]==200 && s[i1+1]==152) {i1++; s[i2++]='ª';}
    else if (s[i1]==200 && s[i1+1]==154) {i1++; s[i2++]='Þ';}
    else if (s[i1]==196 && s[i1+1]==131) {i1++; s[i2++]='ã';}
    else if (s[i1]==226 && s[i1+1]==110) {i1++; s[i2++]='â';}
    else if (s[i1]==200 && s[i1+1]==153) {i1++; s[i2++]='º';}
    else if (s[i1]==200 && s[i1+1]==155) {i1++; s[i2++]='þ';}
    else s[i2++] = s[i1];
  }
  s[i2] = 0;
}

int main() {
  char s[200];
  fgets(s, 100, stdin);
  utf2iso((unsigned char*)s);
  fprintf(stdout, "%s", s);
  return 0;
}
