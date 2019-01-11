#include <cstdint>

#include "../include/wdata.h"

#ifndef WORDVECT_H
#define WORDVECT_H

/*
** The outdbl and outint union types provide a way to break apart integer and
** double types for writing to output files.
*/
union outdbl { char c[8]; double d;  };
union outint { char c[4]; int32_t i; };

/*
** The following definition allows the compiler to make some small changes
** based on which platform is being used.  Things like file name formatting
** are affected.
** 1 = Linux
** 2 = Windows
*/
#define LINUX    1
#define WINDOWS  2
// change the following definition to reflect the compilation platform
#define PLATFORM LINUX
#define IS_PLATFORM(A) (PLATFORM==A)

/*
** The "wordvect" structure stores the word vector and associated data.  Once the
** wordvect is written to the dictionary, the only data that is allowed to be
** changed is the data contained in the "wdata" substructure.  This guarantees
** that the sort order of the multiset is never affected by updates.
*/
struct wordvect {
public:
  wordvect(void);                         // default constructor (makes an empty instance)
  wordvect(string);                       // alternate constructor (initializes the string entry)
  wordvect(const wordvect&);              // copy constructor
  ~wordvect();                            // destructor

  wordvect& operator=(const wordvect&);   // assignment operator
  wordvect& operator=(const string&);     // assignment operator
  bool      operator==(const string&);    // equivalence operator
  bool      operator==(const wordvect&);  // equivalence operator
  bool      operator<(const wordvect&);   // less-than operator
  bool      operator<(const string&);     // less-than operator

  void   copy(const wordvect&);           // copies all of the data from one wordvect to another
  bool   comp(const wordvect&) const;     // compares two wordvect instances 
  bool   comp(const string&) const;       // compares a string to the string data of this instance
  void   clear(void);                     // clears all data in a wordvect instance
  string str(void) const;                 // returns the string entry
  void   incr(void);                      // increments the usage counter
  int    count(void) const;               // returns the usage count of the word
  int    num_obs(void) const;             // returns the number of observations expected
  void   setord(int);                     // sets the ordinal of the word
  int    getord(void) const;              // gets the ordinal of the word
  void   addprec(Svect&) const;           // adds a precursor vector to the word data
  void   set_train(list<multiset<wordvect>::iterator>*); // sets the pointer to the training set
  void   set_test(list<multiset<wordvect>::iterator>*);  // sets the pointer to the testing set
  void   solve(double, bool=false) const; // master function that solves for the weights
  bool   isvalid(void) const;             // checks to ensure that all of the weights are valid numbers
  double find_optimal(void) const;        // find the optimal threshold
  void   testsoln(void) const;            // benchmarks the solution against the test data
  double find_prob(Svect&) const;         // given a vector of precursors, finding probability
                                          // that the word in this instance is the next one
  bool   write(bool=true) const;          // writes the data to an eponymous file
  bool   read(bool=true) const;           // reads the data from an eponymous file

  // Once a wordvect instance is stored in the dictionary, the only
  // way to change the substructure data is to explicitly set the
  // pointer to that data to another variable and manipulate it that
  // way (since the structure is treated as a const from that point
  // forward). This function will be removed after testing.
  wdata* word_data(void) const { return wd; }

  // outputs string component to a stream
  friend ostream& operator<<(ostream&,const wordvect&);

private:
  list<multiset<wordvect>::iterator> *train; // pointer to the training set for a dictionary
  list<multiset<wordvect>::iterator> *test;  // pointer to the testing set for a dictionary
  string entry;      // the string data for this word
  Svect  empty_vec;  // an empty vector used to fill in
  int    ord;        // the ordinal number of a wordvect instance
  wdata  *wd;        // extra data on this word stored in a substructure
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

#endif // WORDVECT_H