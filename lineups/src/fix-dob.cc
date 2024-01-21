#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "catalog.hh"

int complies(Person *p) {
  if (p->dob==NULL) { fprintf(stdout, "null dob\n"); return 0; }
//  fprintf(stdout, "*%s\n", p->dob);
  char *d = new char[32];
  strcpy(d, p->dob);
  char *tkd = strtok(d, "/\n");
  char *tkm = strtok(NULL, "/\n");
  char *tky = strtok(NULL, "/\n");
  if (tkm==NULL) { fprintf(stdout, ">missing month\n"); return 0; }
  if (tky==NULL) { fprintf(stdout, ">missing year\n"); return 0; }

  int z = atoi(tkd);
  if (z<0 || z>31)  { fprintf(stdout, ">dayrange=%d\n", z); }
  int m = atoi(tkm);
  if (m<0 || m>12)  { fprintf(stdout, ">monrange=%d\n", m); }
  int y = atoi(tky);
  if (y<1880 || y>2050) { fprintf(stdout, ">yearrange=%d\n", y); };

  if (strlen(tkd)!=2) { fprintf(stdout, ">daylen=%d\n", strlen(tkd)); return 0; }
  if (strlen(tkm)!=2) { fprintf(stdout, ">monlen=%d\n", strlen(tkm)); return 0; }
  if (strlen(tky)!=4)  { fprintf(stdout, ">yearlen=%d\n", strlen(tky)); return 0; }

  char *fd = new char[32];
  sprintf(fd, "%02d/%02d/%04d", z, m, y);
  if (m>12 && m<=31 && z<=12) {
   // swap z m
    sprintf(fd, "%02d/%02d/%04d", m, z, y);
  }

  strcpy(p->dob, fd);

  return 1;
}

int main(int argc, char **args) {
  Catalog *C = new Catalog();
  C->Load("players.dat");
  int n = C->Size();
  for (int i=0; i<n; i++) {
    Person p = C->P[i];
    if (!complies(&p)) {
      printf("%d. %s %s [%s]\n", i, p.pren, p.name, p.dob);
    }
  }
  C->Save("katalog.txt");
  return 0;
}
