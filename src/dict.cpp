
#include "../include/dict.h"

/*
** Default constructor.
*/
Dict::Dict() { clear(); }
/*
** Destructor (does nothing).
*/
Dict::~Dict() { }

/*
** Overloading the "<<" operator allows sending the string element to an output stream.
*/
ostream& operator<<(ostream& os, const wordvect& w) { 
  os << w.entry << "(" << w.wd->count() << ")"; 
  return os; 
} // end operator<<()
ostream& operator<<(ostream& os, const Dict& d) {
  WVit it = d.words.end();
  os << "[ ";
  do { it--; os << *it << " "; } while (it != d.words.begin());
  os << "] " << "<" << d.words.size() << ">";
  return os;
} // end operator<<()

void Dict::clear(void) {
  empty.setord(-1);
  empty = "@";
  nord = 0;
  words.erase(words.begin(),words.end());
}

bool Dict::addword(wordvect &w) {
  WVit      it;
  wdata    *wd;            // need a temp variable to avoid triggering -fpermissive violation

  it = words.find(w);
  if (it == words.end()) { // add a new record if an existing one was not found
    w.incr();
    w.setord(nord++);
    it = words.insert(w);
    return true;           // return true if a new record was added
  }
  else {                   // increment the record count if an existing one was found
    wd = (*it).word_data();
    wd->incr();
    return false;          // return false if a matching record was already present
  }

}

bool Dict::addword(string sw) {
  wordvect w(sw);
  if (sw == "") return false;
  else          return addword(w);
}

bool Dict::exist(string sw) {
  WVit     it;
  wordvect temp(sw);

  it = words.find(temp);
  if (it == words.end()) return false;
  else                   return true;
}
WVit Dict::find(wordvect &wfind) {
  return(words.find(wfind));
}

WVit Dict::find(string sw) {
  wordvect temp(sw);
  return(find(temp));
}

const wordvect& Dict::get(wordvect &wfind) {
  WVit it;
  it = find(wfind);
  if (it == words.end()) return empty;
  else                   return *it;
}

const wordvect& Dict::get(string sw) {
  WVit it;
  it = find(sw);
  if (it == words.end()) return empty;
  else                   return *it;
}

/*
** The bracket ("[]") operator allows accessing an individual element in the dict. This is
** equivalent to the "get" function (and, in fact, invokes it).
*/
const wordvect& Dict::operator[](wordvect &wfind) {
  return (get(wfind));
} // end "[]" (get) operator definition

const wordvect& Dict::operator[](string sw) {
  return (get(sw));
} // end "[]" (get) operator definition

const wordvect& Dict::operator[](int i) {
  WVit it;

  it = words.begin();
  while (it != words.end()) {
    if ((*it).getord() == i) return *it;
    it++;
  }
  return (empty);
} // end "[]" (get) operator definition
