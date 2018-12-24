#include <iostream>
#include <utility>
#include <fstream>
#include <iomanip>
#include <string>
#include <list>
#include <set>
#include <random>

#include "../include/vect.h"
#include "../include/wdata.h"
#include "../include/wordvect.h"
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
** The "Dict" class stores the dictionary that is used to score the vectors.  A tree structure
** is used, which greatly enhances lookup and retrieval speeds O(log n) vice O(n).
*/
class Dict {
public:
  Dict();                                 // default constructor
  ~Dict();                                // destructor (does nothing)

  void      clear(void);                  // clears all data in the dict
  void      thresh(int=0);                // sets the threshold for group operations
  int       thresh(void);                 // gets the current threshold
  bool      addword(wordvect&,bool=true); // adds a word to the dictionary
  bool      addword(string);
  bool      exist(string);                // returns true if the word is already added
  WVit      find(wordvect&);              // finds an entry in the dictionary and returns an iterator
  WVit      find(string);
  void      write(void);                  // writes the dictionary index file to the dict directory
  void      read(void);                   // reads the dictionary index file from the dict directory
  const wordvect& get(wordvect&);         // finds an entry in the dictionary and returns a reference to that item
  const wordvect& get(string);

  // allows accessing an individual element via brackets
  const wordvect& operator[](wordvect&);
  const wordvect& operator[](string);
  const wordvect& operator[](int);

  friend ostream& operator<<(ostream&,const Dict&); // outputs all elements to a stream

  list<WVit>                    train;    // the words in the training set (random subset of words)
  list<WVit>                    test;     // the words in the testing set (random subset of words)
private:
  multiset<wordvect,classcompv> words;    // the set of words that make up the dictionary
  wordvect empty;  // an empty wordvect to return in cases where the requested entry does not exist
  int      nord;   // the next ordinal number
  int      thr;    // count threshold for group operations (like display)
  double   ptrain; // the probability of being copied into the train wordvect
  double   ptest;  // the probability of being copied into the test wordvect
};

#endif // DICT_H