/*
** Created by: Jason Orender
** (c) 2018 all rights reserved
**
** The Datamodule class is an implementation of a logistic regression algorithm that uses a sparse 
** vector format. It uses the Svect class implementation for a sparse vector container.
*/

#ifndef DATAMODULE_H
#define DATAMODULE_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <random>
#include <math.h>
#include "../include/vect.h"

using namespace std;

#define TP 0 // true positive
#define FP 1 // false positive
#define FN 2 // false negative
#define TN 3 // true negative

class Datamodule {
public:
  Datamodule();                                                   // default constructor
  ~Datamodule();                                                  // destructor

  bool   read_input(char **, bool = true);                        // read_input reads input files
  void   LLcomp(Svect&, Svect&, Svect&, Svect*);                  // calc objective function components
  double LL(Svect&, Svect&, Svect*);                              // returns the objective function sum
  int    getsoln(double = 0.001, int = 100);                      // iterate to a solution: 
  void   pred(void);                                              // the predictive function
  void   apply_threshold(double = 0.999);                         // apply a threshold limiter to results
  void   calc_conf(double* = nullptr);                            // calculate confusion numbers
  void   outdata(string);                                         // writes TPR/FPR data to a file
  void   set_weights(Datamodule&);                                // copies the weights
  void   set_weights(Svect&);                                     // sets all of the weights
  void   set_features(Svect*);                                    // sets the features array pointer (does not deep copy)

  int    examples(void);                                          // returns the number of examples in the dataset
  void   display_weights(int = 4);                                // display the current weights
  void   display_observations(void);                              // display the observations vector
  void   display_features(int = 4);                               // display the features matrix
  void   display_results(void);                                   // display the results vector
  void   display_confusion(void);                                 // display the confusion matrix

  friend int xmat_split(Datamodule&, double, Datamodule&, Datamodule&);

private:
  Svect  wvec;   // weights vector
  Svect *xvec;   // features vector
  Svect  yvec;   // observations vector
  Svect  rvec;   // results vector
  Svect  lvec;   // objective function components
  Svect  tvec;   // threshold limited vector
  Svect  cvec;   // confusion matrix components
};

// randomly splits the matrices (weights, observed, features) into two subsets of the data
// xvec_split(input-data, fraction-to-out1, matrix-out1, matrix-out2)
int xmat_split(Datamodule&, double, Datamodule&, Datamodule&);

// calculates the components of the log-liklihood function
// LLcomp(output-vector, weights-vector, observed, features)
void LLcomp(Svect&, Svect&, Svect&, Svect*);

// returns the log-liklihood (scalar)
// LL(weights-vector, observed, features)
double LL(Svect&, Svect&, Svect*);

#endif // DATAMODULE_H
