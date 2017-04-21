#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {

  if (argc<3) {
    printf("Error: no input file specified.\n");
    return 1;
  }
  char cname[64], cmnem[5], season[15];
  strncpy(cmnem, argv[1], 3);
  strcpy(season, argv[2]);
  char y[15];
  strcpy(y, season);  
  char *y1 = strtok(y, "-");
  char *year = strtok(NULL, "\n");
  if (year==NULL) year = y1;
 
  FILE *inf, *outf;
  char infn[64], outfn[64];
  sprintf(infn, "%s/ln.dat", cmnem);
  inf = fopen(infn, "rt");
  char s[256];
  fgets(s, 250, inf);
  if (strlen(s)<1) {
    printf("Error: empty file %s.\n", infn);
    fclose(inf);
    return 1;
  }
  s[strlen(s)-1] = 0;
  strcpy(cname, s);
  char lcode[20][5];
  char lname[20][64];
  char ldiv[20][64];
  int numlg = 0;
  while (!feof(inf)) {
    fgets(s, 250 , inf);
    if (!feof(inf)) {
      char *tkc = strtok(s, ":");
      char *tkn = strtok(NULL, "/:\n");
      char *tkd = strtok(NULL, "\n");
      strcpy(lcode[numlg], tkc);
      strcpy(lname[numlg], tkn);
      if (tkd!=NULL) { strcpy(ldiv[numlg], tkd); } else strcpy(ldiv[numlg], ""); 
//      printf("%s : %s : %s\n", lcode[numlg], lname[numlg], ldiv[numlg]);
      numlg++;
    }
  }
  fclose(inf);

  fprintf(stdout, "---------------- RES file ----------------\n"); 
  fprintf(stdout, "#!/bin/csh\n\n");
  fprintf(stdout, "cd /Users/radu/Programs/httrack-3.40.3/src/\n");
  for (int i=0; i<numlg; i++) {
    char c = lcode[i][strlen(lcode[i])-1];
    if (c > '0' && c <= '9') {    
      fprintf(stdout, "./httrack \"http://soccerway.com/national/%s/%s/%s/%s//results\" -O", cname, lname[i], season, ldiv[i]);
    }
    else {
      fprintf(stdout, "./httrack \"http://soccerway.com/national/%s/%s/%s/regular-season//results\" -O", cname, lname[i], season);
    }
    fprintf(stdout, " \"/Users/radu/rsssf/sat/web/%s/\" --get -%%v\n", cmnem);
    if (c > '0' && c <= '9') {    
      fprintf(stdout, "mv -f /Users/radu/rsssf/sat/web/%s/results.html /Users/radu/rsssf/sat/web/%s/%s.html\n", cmnem, cmnem, ldiv[i]);
    }
    else {
      fprintf(stdout, "mv -f /Users/radu/rsssf/sat/web/%s/results.html /Users/radu/rsssf/sat/web/%s/%s.html\n", cmnem, cmnem, lname[i]);
    }
  }
  fprintf(stdout, "\ncd /Users/radu/rsssf/sat/%s\n", cmnem);

  fprintf(stdout, "---------------- GET file ----------------\n"); 
  fprintf(stdout, "#!/bin/csh\n\n");
  fprintf(stdout, "cd /Users/radu/Programs/httrack-3.40.3/src/\n");
  for (int i=0; i<numlg; i++) {
    char c = lcode[i][strlen(lcode[i])-1];
    if (c > '0' && c <= '9') {
      fprintf(stdout, "./httrack \"http://soccerway.com/national/%s/%s/%s/%s\" -O", cname, lname[i], season, ldiv[i]);
    }
    else {
      fprintf(stdout, "./httrack \"http://soccerway.com/national/%s/%s\" -O", cname, lname[i]);
    }
    fprintf(stdout, " \"/Users/radu/rsssf/sat/web/%s/\" --get -%%v\n", cmnem);
  }
  fprintf(stdout, "cd /Users/radu/rsssf/sat/%s\n", cmnem);

  fprintf(stdout, "---------------- UPD file ----------------\n"); 
  fprintf(stdout, "#!/bin/csh\n\n");
  fprintf(stdout, "cd /Users/radu/rsssf/sat/%s\n", cmnem);
  for (int i=0; i<numlg; i++) {
    char c = lcode[i][strlen(lcode[i])-1];
    if (c > '0' && c <= '9') {
      fprintf(stdout, "../update %s.%s ../web/%s/%s.html\n", lcode[i], year, cmnem, ldiv[i]);
    }
    else {
      fprintf(stdout, "../update %s.%s ../web/%s/%s.html\n", lcode[i], year, cmnem, lname[i]);
    }
  }

}
