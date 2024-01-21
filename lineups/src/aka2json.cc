#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aliases.hh"

#define EOT 10000

int  NC;
Aliases **L;
char *league;

int Load() {
  FILE *f;
  char s[2000], *tok[20], *ystr, *name, *nick;
  f = fopen("teams.dat", "rt");
  if (f==NULL) return 0;
  fscanf(f, "%d\n", &NC);
  fclose(f);
  L = new Aliases*[NC];
  f = fopen("alias.dat", "rt");
  if (!f) return 0;
  for (int i=0; i<NC; i++) {
    L[i] = new Aliases;
    if (feof(f)) continue;
    fgets(s, 2000, f);
    if (!s) continue;
    if (strlen(s) < 3) continue;
    s[strlen(s)-1] = 0;
    tok[0] = strtok(s, "*");
    for (int j=1; j<20; j++)
      tok[j] = strtok(NULL, "*");
    int k=0;
    while(tok[k]) {
      ystr = strtok(tok[k], " ");
      name = strtok(NULL, "~");
      nick = strtok(NULL, "@");
      int y = atoi(ystr);
      alias *a = new alias(y, name, nick);
      L[i]->Append(a);
      k++;
    }
    s[0] = 0;
  }
  fclose(f);
  return 1;
}

void reverse(Aliases *L) {
  node *p = L->list;
  node *q = p->next;
  p->next = NULL;
  while (q!=NULL) {
    node *qnext = q->next;
    q->next = p;
    p = q;
    q = qnext;
  }
  L->list = p;
}

void extractJson() {
  for (int i=0; i<NC; i++) {
    node *head = L[i]->list;
    printf("{\"id\":%d", i);
    printf(",\"league\":\"%s\"", league);
    printf(",\"name\":\"%s\"", head->data->name);
    printf(",\"aka\":\"%s\"", head->data->nick);
//    printf(",\"euro\":\"%s\"", head->data->nick);
    printf(",\"aliases\":[");
    reverse(L[i]);
    node *p = L[i]->list;
    while (p!=NULL) {
      printf("{\"full\":\"%s\"", p->data->name);
      printf(",\"mnem\":\"%s\"", p->data->nick);
      int fy = p->data->year;
      int ty = (p->next!=NULL? p->next->data->year : EOT);
      printf(",\"interval\":{\"from\":\"%d-07-20\",\"to\":\"%d-07-20\"}}", fy-1, ty-1);
      if (p->next!=NULL) printf(",");
      p = p->next;
    }
    printf("]}\n");
  }
}

int main(int argc, char **argv) {
  league = new char[64];
  strcpy(league, "rom");
  if (argc>1) strcpy(league, argv[1]);
  if (!Load()) {
    printf("ERROR: called from invalid drectory.\n");
    return -1;
  }
  extractJson();
  return 0;
}
