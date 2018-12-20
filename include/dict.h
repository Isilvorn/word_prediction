#include <iostream>
#include <utility>
#include <fstream>
#include <iomanip>
#include <string>
#include <list>
#include <set>
#include <random>

#include "../include/vect.h"
#include "../include/datamodule.h"

#ifndef DICT_H
#define DICT_H

using namespace std;
using namespace std::rel_ops;

#define NVEC 4     // the size of the word vector predecessor array (# of priors)
#define MAXD 50000 // the nominal size of the dictionary for use in Svect operations

/*
** Defining the RAND macro and component parts globally so that a psuedorandom number 
** can be supplied in any necessary function with no danger of repeating the same
** sequence in multiple function calls, but retain the dertministic nature of the
** pseudorandom sequence application-wide if desired.
*/
#define RAND rand_distr(rand_gen)
/*
** The "wdata" struct contains all of the mutable data pertaining to a word vector.
** any data contained in this structure is guaranteed not to affect the sort order
** in the multiset (binary search tree) used to contain the dictionary of words
** used in the data set.  It is dynamically allocated at the time of a wordvect
** instance creation.
*/
struct wdata {
public:
  wdata(void);                 // default constructor
  ~wdata(void);                // destructor (does nothing)

  
  void clear(int=0);           // clear all data in the structure and specify Svect nominal size
  void clearf(void);           // clears the features vector collection only
  void copy(const wdata&);     // copy the data from one instance of the structure to another
  void add(Svect&);            // add an example to the precursor data
  Svect& front(void);          // access the front element in the precursor data
  Svect& next(void);           // access the next element in the precursor data
  int    size(void);           // access the size of the precursor data
  void incr(void);             // increment the usage count
  int  count(void) const;      // return the usage count
  void init_weights(double);   // initializes the weights used for the logistic regression calculation
  void init_logr(double);      // initializes the features array used in the logistic regression

  list<Svect> prec;            // data set for precursors
  list<Svect>::iterator lit;   // the persistent list iterator
  int         ct;              // usage count
  int         sz;              // nominal size of the sparse vectors
  Svect       weights;         // the weights calculated via logistic regession for predicting
                               // this word based on its list of precursor vectors
  Svect      *features;        // the features vector collection used in the logistic regression
  Svect       obs;             // the observations vector used in the logistic regression
};

/*
** The "wordvect" structure stores the word vector and associated data.  Once the
** wordvect is written to the dictionary, the only data that is allowed to be
** changed is the data contained in the "wdata" substructure.  This guarantees
** that the sort order of the multiset is never affected by updates.
*/
struct wordvect {
public:
  wordvect(void);                        // default constructor (makes an empty instance)
  wordvect(string);                      // alternate constructor (initializes the string entry)
  wordvect(const wordvect&);             // copy constructor
  ~wordvect();                           // destructor

  wordvect& operator=(const wordvect&);  // assignment operator
  wordvect& operator=(const string&);    // assignment operator
  bool      operator==(const string&);   // equivalence operator
  bool      operator==(const wordvect&); // equivalence operator
  bool      operator<(const wordvect&);  // less-than operator
  bool      operator<(const string&);    // less-than operator

  void copy(const wordvect&);            // copies all of the data from one wordvect to another
  bool comp(const wordvect&) const;      // compares two wordvect instances 
  bool comp(const string&) const;        // compares a string to the string data of this instance
  void clear(void);                      // clears all data in a wordvect instance
  void incr(void);                       // increments the usage counter
  int  count(void) const;                // returns the usage count of the word
  void setord(int);                      // sets the ordinal of the word
  int  getord(void) const;               // gets the ordinal of the word
  void addprec(Svect&) const;            // adds a precursor vector to the word data
  void solve(double) const;              // master function that solves for the weights

  // Once a wordvect instance is stored in the dictionary, the only
  // way to change the substructure data is to explicitly set the
  // pointer to that data to another variable and manipulate it that
  // way (since the structure is treated as a const from that point
  // forward). This function will be removed after testing.
  wdata* word_data(void) const { return wd; }

  // outputs string component to a stream
  friend ostream& operator<<(ostream&,const wordvect&);

private:
  string entry;   // the string data for this word
  int    ord;     // the ordinal number of a wordvect instance
  wdata  *wd;     // extra data on this word stored in a substructure
};  

/*
** The classcompv structure compares two wordvectors for sorting purposes.  
** This is the format that must be used for the multiset container declaration.
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
  void      thresh(int=0);        // sets the threshold for group operations
  int       thresh(void);         // gets the current threshold
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
  multiset<wordvect,classcompv> words; // the set of words that make up the dictionary
  multiset<wordvect,classcompv> train; // the words in the training set (random subset of words)
  multiset<wordvect,classcompv> test;  // the words in the testing set (random subset of words)
  wordvect empty;  // an empty wordvect to return in cases where the requested entry does not exist
  int      nord;   // the next ordinal number
  int      thr;    // count threshold for group operations (like display)
  double   ptrain; // the probability of being copied into the train wordvect
  double   ptest;  // the probability of being copied into the test wordvect
};

#endif // DICT_H