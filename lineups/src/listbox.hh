#define NOT_FOUND -1

struct ListItem {
  char     *data;
  ListItem *next;
  ListItem *prev;
  ListItem(char *s, ListItem *n, ListItem *p);
};

class Listbox {
  bool      sorted;
  int       n_items;
  ListItem *item;
  int       maxlen;
  int       n_cols, col;
  int       start;
  int       cur;
  int       top, bottom, left, right;
  char    **line;
public:
  Listbox(bool sort = false);
  ~Listbox();
  int   Length() { return n_items; };
  int   Current() { return cur; };
  void  SetCurrent(int);
  int   Maxlen() { return maxlen; };
  char* Item(int);
  int   Find(char*);
  int   Find(char);
  void  AddItem(char* s, int pos = -1);
  void  SetItem(int, char*);
  int   RemoveItem(char *);
  int   RemoveItem(int);
  void  Clear();
  void  ResetMaxlen();
  void  SwitchTo(int);
  void  PrintRack(int, int);
  void  PrintItems(int, int);
  int   Select(int, int);
  int   Popup(int, int);
  int   Width();
  int   Height();
};

void InitScreen();
