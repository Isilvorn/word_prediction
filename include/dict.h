#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <list>
#include <set>

#ifndef DICT_H
#define DICT_H

using namespace std;

#define NVEC 4 // the size of the word vector predecessor array (# of priors)

/*
** The "wdata" struct contains all of the mutable data pertaining to a word vector.
** any data contained in this structure is guaranteed not to affect the sort order
** in the multiset (binary search tree) used to contain the dictionary of words
** used in the data set.
*/
struct wdata {
public:
  wdata(void)  { clear(); } // default constructor
  ~wdata(void) { }          // destructor (does nothing)

  void clear(void)
    { ct=0; for (int i=0; i<NVEC; i++) prec[i]=""; }
  void copy(const wdata &wd)
    { ct=wd.ct; for (int i=0; i<NVEC; i++) prec[i]=wd.prec[i]; }
  void incr(void)        { ct++; }
  int  count(void) const { return ct; }

private:
  string prec[NVEC];
  int    ct;
};

/*
** The "wordvect" structure stores the word vector and associated data.  Once the
** wordvect is written to the dictionary, the only data that is allowed to be
** changed is the data contained in the "wdata" substructure.  This guarantees
** that the sort order of the multiset is never affected by updates.
*/
struct wordvect {
public:
  wordvect(void)              { wd=new wdata; clear(); }            // default constructor (makes an empty instance)
  wordvect(string str)        { wd=new wdata; clear(); entry=str; } // alternate constructor (initializes the string entry)
  wordvect(const wordvect &w) { wd=new wdata; clear(); copy(w); }   // copy constructor
  ~wordvect()                 { if (wd != nullptr) delete wd; }     // destructor

  // assignment operators
  wordvect& operator=(const wordvect &w)  { copy(w); return *this;  }
  wordvect& operator=(const string &sw)   { entry=sw; return *this; }
  // equivalence operators
  bool      operator==(const string &sw)  { return(entry == sw);                     }
  bool      operator==(const wordvect &w) { return(entry == w.entry);                }
  bool      operator!=(const wordvect &w) { return(!(*this == w));                   }
  bool      operator> (const wordvect &w) { return(comp(w));                         }
  bool      operator< (const wordvect &w) { return(!((*this ) > w) && (*this != w)); }
  bool      operator<=(const wordvect &w) { return(!(*this > w));                    }
  bool      operator>=(const wordvect &w) { return(!(*this < w));                    }

  // copies all of the data from one wordvect to another
  void copy(const wordvect &w) 
    { entry=w.entry; ord=w.ord; wd->copy(*w.wd); }
  // compares two wordvect instances
  bool comp(const wordvect &w) const
    { return(entry > w.entry); }
   // clears all data in a wordvect instance
  void clear(void) 
    { entry=""; ord=0; wd->clear(); }
  // increments the usage counter by invoking the substructure
  void incr(void)              { wd->incr(); }
  // returns the usage count stored in the substructure
  int  count(void) const       { return wd->count(); }
  // sets and gets the ordinal of the word
  void setord(int o)           { ord=o; }
  int  getord(void) const      { return ord; }
  // Once a wordvect instance is stored in the dictionary, the only
  // way to change the substructure data is to explicitly set the
  // pointer to that data to another variable and manipulate it that
  // way (since the structure is treated as a const from that point
  // forward).
  wdata* word_data(void) const { return wd; }

  // outputs string component to a stream
  friend ostream& operator<<(ostream&,const wordvect&);

private:
  string entry;
  int    ord;    // the ordinal number of a wordvect instance
  wdata  *wd;    // extra data on this word stored in a substructure
};  

/*
** The classcompv structure compares two wordvectors for sorting purposes.  
** The is the format that must be used for the multiset container declaration.
*/
struct classcompv {
  bool operator() (const wordvect &lhs, const wordvect &rhs) const
  { return (lhs.comp(rhs)); }
};

/*
** The WVit type definition saves considerable typing and reduces apparent complexity.
** It must be kept in mind that this iterator is treated as const.  Any attempt to
** access a member function of a wordvect that is not const will cause a compile-time
** error.  For this reason, the "wdata" substruct is created as a dynamic pointer
** within the wordvect.  The pointer to the wdata must be retrieved and set to a temp
** variable that is not subject to the same restrictions.
*/
typedef multiset<wordvect>::iterator WVit;

/*
** The "Dict" class stores the dictionary that is used to score the vectors.  A tree structure
** is used, which greatly enhances lookup and retrieval speeds O(log n) vice O(n).
*/
class Dict {
public:
  Dict();                         // default constructor
  ~Dict();                        // destructor (does nothing)

  void      clear(void);          // clears all data in the dict
  bool      addword(wordvect&);   // adds a word to the dictionary
  bool      addword(string);
  bool      exist(string);        // returns true if the word is already added
  WVit      find(wordvect&);      // finds an entry in the dictionary and returns an iterator
  WVit      find(string);
  const wordvect& get(wordvect&); // finds an entry in the dictionary and returns a reference to that item
  const wordvect& get(string);

  // allows accessing an individual element via brackets
  const wordvect& operator[](wordvect&);
  const wordvect& operator[](string);
  const wordvect& operator[](int);

  friend ostream& operator<<(ostream&,const Dict&); // outputs all elements to a stream

private:
  multiset<wordvect,classcompv> words;
  wordvect empty; // an empty wordvect to return in cases where the requested entry does not exist
  int      nord;  // the next ordinal number
};

#endif // DICT_H