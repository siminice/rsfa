#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "db.h"

int m_year, m_month, m_day, m_div, m_pool, m_round, m_home, m_guest, m_score[8];
int year;

//--------------------------------------

void Decode(char *s) {
  int i;
  m_year     = 75*(s[0]-48) + (s[1]-48) + 1870;
  m_month    = s[2] - 48;
  m_day      = s[3] - 48;
  if (m_day<0) m_day = 0;
  m_div      = s[4];
  m_pool     = s[5] - 48;
  m_round    = (int) (s[6] - 48);
  m_home     = 75*((int)(s[7]-48)) + s[8] - 48;
  m_guest    = 75*((int)(s[9]-48)) + s[10] - 48;
  int len  = strlen((char *)s);
  for (i=0; i<8; i++)
    if (i+11 < len) m_score[i] = s[i+11] - 48;
    else m_score[i] = -1;
}

void AddLineupRow(FILE *of) {
		int k_year		 = m_year + (m_month/13 - 1);
		int k_month		 = (m_month-1)%12 + 1;
    fprintf(of, "%4d,%4d, %4d, %04d-%02d-%2d@     ,K,%2d", m_home, m_guest, 100*m_score[0]+m_score[1], k_year, k_month, m_day, m_round);
    fprintf(of, ", , , "); /* stad spect vreme */
    for (int k=0; k<22; k++) fprintf(of, ",      :0  ");
    fprintf(of, ",      ");
    for (int k=0; k<22; k++) fprintf(of, ",      :0  ");
    fprintf(of, ",      ");
    for (int k=0; k<4; k++) fprintf(of, ",      "); /* arbitri */
    fprintf(of, "\n");
}

//---------------------------------------------

int main(int argc, char* argv[]) {
	if (argc<2) perror("No year specified");
	int year = atoi(argv[1]);
	FILE *lf, *ef;
	char s[1000], ifilename[128], lfilename[128], efilename[128];
	sprintf(ifilename, "cuparchive-%d.dat", year);
	sprintf(lfilename, "cup-lineups-%d.db", year);
	sprintf(efilename, "cup-events-%d.db", year);
	FILE *inf = fopen(ifilename, "rt");
	lf = fopen(lfilename, "wt");
	ef = fopen(efilename, "wt");
	if (!inf) return EXIT_FAILURE;
	if (!lf) return EXIT_FAILURE;
	if (!ef) return EXIT_FAILURE;

	while (!feof(inf)) {
		fgets(s, 1000, inf);
		if (feof(inf)) continue;
		Decode(s);
//		if (m_year==year) {
			AddLineupRow(lf);
			for (int i=0; i<60; i++) fprintf(ef, " ,"); fprintf(ef, "\n");
//		}
		s[0] = 0;
	}
	fclose(lf);
	fclose(ef);
  fclose(inf);
  return EXIT_SUCCESS;
}
