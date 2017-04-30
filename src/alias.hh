#ifndef __ALIAS_HH
#define __ALIAS_HH

struct alias {
  int   year;
  char *name;
  char *nick;
  alias(int, char*, char*);
  ~alias();
};

struct node {
  alias *data;
  node  *next;
  node(alias*, node*);
  ~node();
};

struct Aliases {
  node *list;
  Aliases();
  ~Aliases();
  void Append(alias *a);
  char* GetName(int y);
  char* GetNick(int y);
  void List();
};

struct AliasCollection {
  int NC;
  Aliases **L;
  AliasCollection();
  ~AliasCollection();
  int Load(int n, const char *filename);
  int Save(const char *filename);
  int Add(alias *a);
  void List();
};

#endif
