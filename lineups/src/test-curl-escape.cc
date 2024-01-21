#include <curl/curl.h>
#include <string.h>

const char *utfsq[] = {
"_c3_a1", "_c4_87", "_c3_a7", "_c3_a9", "_c3_ad", "_c3_b3", "_c3_ba", "_c5_be", "_c5_a1", "_c4_8d",
"_c3_b1", "_c3_81", "_c4_86", "_c3_89", "_c3_8d", "_c3_93", "_c3_9a", "_c5_bd", "_c5_a0", "_c4_8c",
"_c3_87", "_c3_a3", "_c3_b4", "_c3_aa"
};
const char *isoseq[] = {
"a", "c", "c", "e", "i", "o", "u", "z", "s", "c",
"n", "A", "C", "E", "I", "O", "U", "Z", "S", "C",
"C", "a", "o", "e"
};

void normalize(char *s) {
  int len = strlen(s);
  for (int i=0; i<len; i++) {
    if (s[i] == '%') s[i] = '_';
    if (s[i] >= 'A' && s[i] <= 'Z') s[i] += 32;
  }
  for (int i=0; i<24; i++) {
    char *us = strstr(s, utfsq[i]);
    if (us) {
       int l = strlen(us);
       for (int j=0; j<=l-5; j++) us[j] = us[j+5];
       us[0] = isoseq[i][0];
    }
  }
}

int main(int argc, char **argv) {
  CURL *curl= curl_easy_init();
  if (curl) {
  char *t = curl_easy_escape(curl, argv[1], strlen(argv[1]));
  if (t) {
    normalize(t);
    printf("Encoded: %s\n", t);
    curl_free(t);
  }
  }
  return 0;
}
