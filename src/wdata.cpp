
#include "../include/wdata.h"
#include "../include/dict.h"

/*
******************************************************************************
******************* wdata STRUCT DEFINITION BELOW HERE ***********************
******************************************************************************
*/

/*
** The default constructor uses the clear() function to initialize the mutable 
** word data.
*/
wdata::wdata(void)  { clear(); }

/*
** Destructor.
*/
wdata::~wdata(void) { }

/*
** The clear() function initializes all data in the wdata struct.  Any data 
** currently residing in the struct is lost.
*/
void wdata::clear(int s) { 
  ct  = 0;
  sz  = s;
  thr = 0.5;
  prec.erase(prec.begin(), prec.end());
  weights.resize(s);
  populated = false;
} // end clear()

/*
** The copy() function copies all data from another wdata struct to this
** instance.  Used in the assignment operator and the copy constructor.
*/
void wdata::copy(const wdata &wd) { 
  list<Svect>::const_iterator it;
  ct = wd.ct;
  sz = wd.sz;
  populated = wd.populated;
  it = wd.prec.cbegin(); 
  while (it != wd.prec.cend()) { 
    prec.push_front(*it); 
    it++; 
  } // end while (it)
} // end copy()

/*
** The add() function adds a vector to the precursor data.  This data will be
** used as a list of examples to run the logistic regression on.
*/
void wdata::add(Svect &v) { 
  prec.push_front(v); 
} // end add()

/*
** The front() function returns a reference to the first element of the
** precursor data.  The "lit" iterator is a persistent iterator that will
** be used in the next() function; it is reset to the beginning of the list
** when the front() function is called.
*/
Svect& wdata::front(void) {
  lit=prec.begin();
  return *lit;
} // end front()

/*
** The next() function returns a reference to the next element of the
** precursor data that is pointed to by the "lit" iterator after it is
** incremented.
*/
Svect& wdata::next(void) {
  lit++; 
  if (lit == prec.end()) lit=prec.begin(); 
  return *lit;
}

/*
** The size() function returns the size of the precursor list (# of elements).
*/
int wdata::size(void) { return prec.size(); }

/*
** The incr() function increments the usage count of the word.
*/
void wdata::incr(void) { ct++; }

/*
** The count() function returns the usage count of the word.
*/
int wdata::count(void) const { return ct; }

/*
** The is_populated() function returns whether the weights vector has valid
** entries and has been populated be the logistic regression or read in from
** a data file.
*/
bool wdata::is_populated(void) { return populated; }

/*
** The init_weights() function initializes the weights used in the logistic
** regression.  It first adds in all of the data from each precursor so that
** there is an explicit element in the weights vector for every explicit
** element used in the precursor list.  It then replaces the data for each
** explicit element with an initial weight (supplied in the argument).
*/
void wdata::init_weights(double w) { 
  list<Svect>::iterator it;

  // nothing to do if there is no precursor data
  if (prec.begin() == prec.end()) return; 

  it = prec.begin(); 
  weights = *it;
  it++; 
  while (it != prec.end()) { 
    weights += *it; 
    it++; 
  } // end while (lit);
  weights = w; // overwrite all of the explicit data with the initial weight supplied
  populated = false;
} // end init_weights()


/*
** The init_logr() function initializes the data used in the logistic 
** regression.
*/
void wdata::init_logr(double w, Svect *feat, Svect &obsv) {
  list<Svect>::iterator it;
  int i = 0; 

  if (w != 0) init_weights(w);
  fmax = 10*prec.size();             // setting max size of features vector
  if (fmax > 500) fmax = 500;
  //cout << "fmax = " << fmax << endl;
  obsv.resize(prec.size());           // clear and size the observations vector
  obsv.setall(1);                     // set all of these observations to 1

  it = prec.begin();
  while (it != prec.end()) {         // copy precursor data to features
    if (i >= 100) break;
    feat[i++] = *it; 
    it++; 
  } // end while (it)

  fsize = i;
} // end init_logr()

/*
** The add_negs() function adds the data for the negative observations to the
** features and observations data sets.  The negative observations are based on 
** a random sampling of the rest of the data; this needs to be appended to the 
** features and observations data so that an accurate model can be derived.  The 
** positive observations should be relatively small in size compared to the 
** negative observations in order to yield an accurate regression.
*/
void wdata::add_negs(list<Svect> &negs, Svect *feat, Svect &obsv) {
  list<Svect>::iterator it;
  int i = fsize;

  it = negs.begin();
  while (it != negs.end()) {
    if (i >= fmax) { fsize = fmax; return; }
    feat[i++] = *it;
    it++;
  } // end while (it)

  obsv.upsize(i);
  fsize = i;
} // end add_negs()

/*
** The disp_weights() function displays all of the data in the weights vector.
*/
void wdata::disp_weights(void) {
  cout << "Weights:" << endl;
  cout << setprecision(4) << fixed << weights << " (" << weights.size() << ")" << endl;
} // end disp_weights()

/*
** The num_obs() function simply returns the number of observations used in
** the calculation.  This also happens to be fmax.
*/
int wdata::num_obs(void) { return fmax; }

/*
** The find_prob() function finds the probability that the word in this
** instance is the next one.  The return value is the probability.
*/
double wdata::find_prob(Svect &p) {
  double               wTx;

  wTx  = (weights * p).sum();
  return exp(wTx)/(1 + exp(wTx));
}