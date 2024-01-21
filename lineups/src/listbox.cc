#include "listbox.hh"
#include <curses.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>

//=========================================================

ListItem::ListItem(char *s, ListItem *n, ListItem *p) {
  data = new char[strlen(s) + 1];
  strcpy(data, s);
  next = n;
  prev = p;
}

Listbox::Listbox(bool sort) {
  sorted   = sort;
  n_items  = 0;
  item     = NULL;
  maxlen   = 0;
  cur      = -1;
}

Listbox::~Listbox() {
  while (item != NULL) RemoveItem(0);
}

char* Listbox::Item(int pos) 
{
  if (!n_items) return NULL;
  int i = 0;
  ListItem *p = item;
  while (i<pos && p!=NULL)  { p = p->next; i++; }
  if (i==pos) return p->data;
  return NULL;      
}

void Listbox::SetCurrent(int c) {
  if (c>=0 && c<n_items) cur = c;
}

int Listbox::Find(char c) 
{
    ListItem *p = item;
    if (p == NULL) return NOT_FOUND;
    for (int i=0; i<cur; i++) p=p->next;
    int loc = (cur + 1) % n_items;
    if (cur == n_items - 1) p = item;
      else p = p->next;
    int found = 0;
    while (!found && loc != cur)
//      if (strchr(p->data, c) != NULL)  found = 1;
      if (p->data[0] ==c)  found = 1;
      else {
        if (p->next != NULL) p = p->next;
        else p = item;
        loc = (loc + 1) % n_items;
      }
    return loc;
}

int Listbox::Find(char *s) 
{ return 0; };

void  Listbox::AddItem(char* s, int pos) {
//  if (sorted)
  if (strlen(s) > maxlen) 
    maxlen = strlen(s);
  if (item == NULL) {
    item = new ListItem(s, item, NULL);
    n_items++;
    cur = 0;
    return;
  }
  if (pos == 0) {
    item = new ListItem(s, item, NULL);
    n_items++;
    cur++;
    return;
  }
  if (pos < 0) pos = n_items;
  ListItem *p = item;
  int i = 1;
  while (p->next != NULL && i < pos) { p = p->next; i++; }
  p->next = new ListItem(s, NULL, p);
  n_items++;
  if (cur >= pos) cur++;
}

void Listbox::SetItem(int pos, char *s) {
  if (pos < 0 || pos >= n_items) return;
  if (strlen(s) > maxlen) maxlen = strlen(s);
  ListItem *p = item;
  int i = 0;
  while (p->next != NULL && i < pos) { p = p->next; i++; }
  delete p->data;
  p->data = strdup(s);
}

void Listbox::ResetMaxlen() {
  maxlen = 0;
  ListItem *p = item;
  while (p!=NULL) {
    if (strlen(p->data) > maxlen)
      maxlen = strlen(p->data);
    p = p->next;
  }
}

int Listbox::RemoveItem(char *s) {
  return 0;
}

int Listbox::RemoveItem(int pos) {
  ListItem *p = item;
  if (p == NULL) return 0;
  if (pos == 0) {
    item = p->next;
    delete p->data;
    delete p;
    n_items--;
    if (cur > pos) cur--;
    ResetMaxlen();
    return 1;
  }
  ListItem *q = p;
  p = p->next;
  int i = 1;
  while (i < pos && p!=NULL) { p = p->next; q = q->next; i++; }
  if (i == pos) {
    q->next = p->next;
    if (p->next != NULL) p->next->prev = q;
    delete p->data;
    delete p;
    n_items--;
    if (cur >= pos) cur--;
    ResetMaxlen();
    return 1;
  }
  else return 0;
}

void Listbox::Clear() {
  while (n_items>0) RemoveItem(0);
}


/*************************************** 
              Visual part 
****************************************/

void Listbox::SwitchTo(int k) {
  if (col==0) col++;
  int c = (cur - start) / col;
  int r = (cur - start) % col;
  mvprintw(top+r+1, left+c*(maxlen+1)+1, "%s", line[cur]);
  cur = k;
  c = (cur - start) / col;
  r = (cur - start) % col;
  attrset(A_STANDOUT);
  mvprintw(top+r+1, left+c*(maxlen+1)+1, "%s", line[cur]);
  attrset(A_NORMAL);
}

void Listbox::PrintRack(int start, int height) {
   /* make fixed strings */
  int size = n_cols*(maxlen+1);
  char *s = new char[size+2];
  s[0] = '|';
  s[size] = '|';
  s[size+1] = 0;
  for (int j=1; j<size; j++)
    s[j] = ' ';
  char *h = new char[size+2];
  h[0] = h[size] = '+';
  h[size+1] = 0;
  for (int j=1; j<size; j++)
    h[j] = '-';

  move(top, left);
  mvprintw(top, left, "%s", h);
  for (int i=0; i<height; i++)
    mvprintw(top+i+1, left, "%s", s);
  mvprintw(bottom, left, "%s", h);

  /* add arrows if necessary */
  if (start > 0) 
    mvprintw(top, (left + right)/2, "^");
  if (start + n_cols*height < n_items)
    mvprintw(bottom, (left+right)/2, "v");
  delete[] s;
  delete[] h;
}

void Listbox::PrintItems(int start, int height) {
  for (int i=0; i<height; i++) {
    int c = i / col;
    int r = i % col;
    mvprintw(top+r+1, left+c*(maxlen+1)+1, "%s", line[i+start]);
  }
}

int Listbox::Select(int valign, int halign) {
  if (!n_items) return -1;
 
  noecho();
  n_cols = (n_items - 1) / 30 + 1;
  col    = (n_items > 30 ? (n_items+n_cols-1)/n_cols : n_items);
  if (n_cols * maxlen > 80) // too many items to fit into screen;
    return Popup(valign, halign);

  left   = (halign > 0 ? halign : 20);
  right  = left + n_cols * (maxlen + 1);
  top    = (valign > 0 ? valign : 1);
  bottom = top + col + 1;
  start  = 0;
  
  line = new char*[n_items];
  ListItem *p = item;
  for (int i=0; i<n_items; i++) {
    line[i] = p->data;
    p = p->next;
  }

//  clrscr();
  PrintRack(0, col);
  PrintItems(0, n_items);
  SwitchTo(cur);
  int c;

  do {
    c = getch();
    if (c==27) {
      c=getch();
      c=getch();
      c += 1000;
      getch();
    }
    switch(c) {
      case 258 : // down
	SwitchTo((cur + 1) % n_items);
	break;
      case 259 : // up
	SwitchTo((cur + n_items - 1) % n_items);
	break;
      case 261 : // right
	SwitchTo((cur + col) % n_items);
	break;
      case 260 : // left
	SwitchTo((cur + n_items - col) % n_items);
	break;
      case 339 : case 1053 : // pgup
	SwitchTo(0);
	break;
      case 262 : case 1055 : // home
	SwitchTo(0);
	break;
      case 338 : case 1054 : // pgdn
	SwitchTo(n_items-1);
	break;
      case 360 : case 1056 : // end
	SwitchTo(n_items-1);
	break;
      default:
	SwitchTo(Find(toupper(c)));
    }	  
  } while (c != 13);
  delete[] line;
  echo();
  return cur;
}

int Listbox::Popup(int valign, int halign) {
  if (!n_items) return -1;

  noecho();
  left   = (halign > 0 ? halign : 20);
  right  = left + maxlen + 1;
  top    = (valign > 0 ? valign : 1);
  bottom = (n_items > 20 ? top + 21 : top + n_items + 1);
  n_cols = 1;
  col    = bottom - top - 1;
  if (cur < col) start = 0;
    else if (cur + col - 1 < n_items) start = cur;
      else start = n_items - col;
  
  line = new char*[n_items];
  ListItem *p = item;
  for (int i=0; i<n_items; i++) {
    line[i] = p->data;
    p = p->next;
  }

  PrintRack(cur, col);
  PrintItems(start, col);
  SwitchTo(cur);
  int c;

  do {
    c = getch();
    if (c==27) {
      c=getch();
      c=getch();
      c += 1000;
      getch();
    }
    switch(c) {
      case 258 : // down
        mvprintw(top+cur-start+1, left+1, "%s", line[cur]);
        if (cur < n_items - 1) cur++;
        if (cur-start >= col) {
          start++; 
          PrintRack(start, col);
          PrintItems(start, col);
        }
        SwitchTo(cur);
        break;
      case 259 : // up
        mvprintw(top+cur-start+1, left+1, "%s", line[cur]);
        if (cur > 0) cur--;
        if (cur < start) {
          start--; 
          PrintRack(start, col);
          PrintItems(start, col);
        }
        SwitchTo(cur);
        break;
      case 260  : // left
        mvprintw(top+cur-start+1, left+1, "%s", line[cur]);
        cur = start = 0;
        PrintRack(start, col);
        PrintItems(start, col);
        SwitchTo(start);
        break;
      case 262 : case 1049 : // home
        mvprintw(top+cur-start+1, left+1, "%s", line[cur]);
        cur = start = 0;
        PrintRack(start, col);
        PrintItems(start, col);
        SwitchTo(start);
        break;
      case 261  : // right
        mvprintw(top+cur-start+1, left+1, "%s", line[cur]);
        cur = n_items-1;
        start = (n_items > 20 ? n_items - 20 : 0);
        PrintRack(start, col);
        PrintItems(start, col);
        SwitchTo(cur);
        break;
      case 360 : case 1052 : // end
        mvprintw(top+cur-start+1, left+1, "%s", line[cur]);
        cur = n_items-1;
        start = (n_items > 20 ? n_items - 20 : 0);
        PrintRack(start, col);
        PrintItems(start, col);
        SwitchTo(cur);
        break;
      case 339 : case 1053 : // pgup
        mvprintw(top+cur-start+1, left+1, "%s", line[cur]);
        cur = (cur > 20 ? cur - 20 : 0);
        start = cur;
        PrintRack(start, col);
        PrintItems(start, col);
        SwitchTo(cur);
        break;
      case 338 : case 1054 : // pgdn
        mvprintw(top+cur-start+1, left+1, "%s", line[cur]);
        cur = (cur + 20 < n_items ? cur + 20 : n_items - 1);
        start = (cur + col -1 < n_items ? cur : n_items - col);
        if (start < 0) start = 0; // special case when n_items < 20
        PrintRack(start, col);
        PrintItems(start, col);
        SwitchTo(cur);
        break;
      default:
        mvprintw(top+cur-start+1, left+1, "%s", line[cur]);
        cur = Find(toupper(c));
        if (cur < start) start = cur;
        if (cur > start+col-1) 
          start = (cur+col-1 < n_items ? cur : n_items - col);
        if (start < 0) start = 0; // special case when n_items < 20
        PrintRack(start, col);
        PrintItems(start, col);
        SwitchTo(cur);
    }
  } while (c != 13);
  delete[] line;
  echo();
  return cur;
}

int Listbox::Width() {
  n_cols = (n_items - 1) / 30 + 1;
  col    = (n_items > 30 ? (n_items+n_cols-1)/n_cols : n_items);
  int size = n_cols*(maxlen+1);
  if (n_cols * maxlen > 80) {
    return maxlen+3;
  }
  return size+2;
}

int Listbox::Height() {
  n_cols = (n_items - 1) / 30 + 1;
  col    = (n_items > 30 ? (n_items+n_cols-1)/n_cols : n_items);
  int size = n_cols*(maxlen+1);
  if (n_cols * maxlen > 80) {
    return 22;
  }
  return col+2;
}

