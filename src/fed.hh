#ifndef FED_HH
#define FED_HH

// Indices
#define UNKNOWN -1

// Load syntax
#define FIXED  0
#define VAR    1

struct Fed {
  int NC;
  char **club;
  char **mnem;
  int Add(char *mnem, char *name);
  int Load(const char* filename, int style = FIXED);
  int Save(const char* filename, int style = FIXED);
  void List();
  int Find(char*);
  int GetUnique(const char*);
};

#endif
