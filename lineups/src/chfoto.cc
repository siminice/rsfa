#include <stdio.h>

int main(int argc, char **argv) {

  char oldfn[128];
  char newfn[128];
  
  if (argc<2) {
    sprintf(oldfn, "html/jucatori/foto/pituvi.jpg", argv[1]);
    sprintf(newfn, "html/jucatori/foto/piþuvi.jpg", argv[2]);
  }
  else {  
    sprintf(oldfn, "html/jucatori/foto/%s.jpg", argv[1]);
    sprintf(newfn, "html/jucatori/foto/%s.jpg", argv[2]);
    fprintf(stderr, "Renaming %s to %s...\n", argv[1], argv[2]);
  }

  rename(oldfn, newfn);
  return 0;
}
