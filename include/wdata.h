#include <list>
#include <iomanip>

#include "../include/vect.h"

#ifndef WDATA_H
#define WDATA_H

/*
** The "wdata" struct contains all of the mutable data pertaining to a word vector.
** any data contained in this structure is guaranteed not to affect the sort order
** in the multiset (binary search tree) used to contain the dictionary of words
** used in the data set.  It is dynamically allocated at the time of a wordvect
** instance creation.
*/
struct wdata {
public:
  wdata(void);                  // default constructor
  ~wdata(void);                 // destructor (does nothing)

  
  void clear(int=0);            // clear all data in the structure and specify Svect nominal size
  void clearf(void);            // clears the features vector collection only
  void copy(const wdata&);      // copy the data from one instance of the structure to another
  void add(Svect&);             // add an example to the precursor data
  Svect& front(void);           // access the front element in the precursor data
  Svect& next(void);            // access the next element in the precursor data
  int    size(void);            // access the size of the precursor data
  void incr(void);              // increment the usage count
  int  count(void) const;       // return the usage count
  bool is_populated(void);      // returns whether the weights vector is populated
  void init_weights(double);    // initializes the weights used for the logistic regression calculation
  void init_logr(double);       // initializes the features array used in the logistic regression
  void add_negs(list<Svect>&);  // adds negative observation data to the features and observations
  void pad_features();          // pads the features vector with empty vectors
  void disp_features(void);     // displays all of the data in the features vector
  void disp_obs(void);          // displays all of the data in the observations vector
  void disp_weights(void);      // displays the weights
  int  num_obs(void);           // displays the number of observations used (fmax)
  double find_prob(Svect&);     // given a vector of precursors, finding probability
                                // that the word in this instance is the next one

  list<Svect> prec;             // data set for precursors
  list<Svect>::iterator lit;    // the persistent list iterator
  int         ct;               // usage count
  int         sz;               // nominal size of the sparse vectors
  int         fmax;             // max size of the features vector (set to 10x prec size)
  int         fsize;            // current size of the features vector
  bool        populated;        // flag indicating whether the weights have been populated
  double      thr;              // threshold value calculated from ROC curve
  Svect       weights;          // the weights calculated via logistic regession for predicting
                                // this word based on its list of precursor words
  Svect       features[500];    // the features vector collection used in the logistic regression
  Svect       obs;              // the observations vector used in the logistic regression
};

#endif // WDATA_H