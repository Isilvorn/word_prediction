
#include "../include/dict.h"

/*
** The default constructor uses the clear() function to initialize all data in the dictionary.
*/
Dict::Dict() { clear(); }
/*
** Destructor (does nothing - there is no dynamic data other than the multiset which has its
** own destructor).
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

/*
** The clear() function initializes all data in the dictionary.  Any data currently residing
** in the dictionary is lost.
*/
void Dict::clear(void) {
  empty.setord(-1);
  empty = "@";
  nord = 0;
  words.erase(words.begin(),words.end());
}

/*
** The addword() functions add a wordvect to the dictionary if it does not already exist or
** increments the usage counter if it does already exist.  Returns TRUE if the wordvect
** was required to be added to the dictionary, and FALSE if it was already in the dictionary.
*/
// add a supplied wordvect to the dictionary
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
// create a a wordvect from a supplied string, and then add it to the dictionary
bool Dict::addword(string sw) {
  wordvect w(sw);
  if (sw == "") return false;
  else          return addword(w);
}

/*
** The exist() function returns TRUE if there is an entry matching a string search template
** in the dictionary, and FALSE otherwise.
*/
bool Dict::exist(string sw) {
  WVit     it;
  wordvect temp(sw);

  it = words.find(temp);
  if (it == words.end()) return false;
  else                   return true;
}

/*
** The find() functions return an iterator that points to a wordvect in the dictionary that
** matches a search template.  If there is not an entry that matches the search template, it
** returns an iterator set equal to words.end().
*/
// use a wordvect as a search template
WVit Dict::find(wordvect &wfind) {
  return(words.find(wfind));
}
// use a string as a search template
WVit Dict::find(string sw) {
  wordvect temp(sw);
  return(find(temp));
}

/*
** The get() functions return a wordvect from the dictionary that matches a search template.
** the first version uses another wordvect as a search template, and the second uses a string.
*/
// use a wordvect as a search template
const wordvect& Dict::get(wordvect &wfind) {
  WVit it;
  it = find(wfind);
  if (it == words.end()) return empty;
  else                   return *it;
}
// use a string as a search template
const wordvect& Dict::get(string sw) {
  WVit it;
  it = find(sw);
  if (it == words.end()) return empty;
  else                   return *it;
}

/*
** The bracket ("[]") operator allows accessing an individual element in the dict. This is
** equivalent to the "get" function (and, in fact, invokes it in the first two cases).  The
** last case finds the wordvect that matches a specific ordinal; this one is slower, since it
** runs in O(n) time; the others run on O(log n) time.
*/
// use another wordvect as a search template
const wordvect& Dict::operator[](wordvect &wfind) {
  return (get(wfind));
} // end "[]" (get) operator definition
// use a string as a search template
const wordvect& Dict::operator[](string sw) {
  return (get(sw));
} // end "[]" (get) operator definition
// find a wordvect by looking for a specific ordinal
const wordvect& Dict::operator[](int i) {
  WVit it;
  it = words.begin();
  while (it != words.end()) { if ((*it).getord() == i) return *it; it++; }
  return(empty);
} // end "[]" (get) operator definition
