
#include "../include/wordvect.h"
#include "../include/datamodule.h"

/*
******************************************************************************
************************** Friend Functions HERE *****************************
******************************************************************************
*/

/*
** Overloading the "<<" operator allows sending the string element to an 
** output stream.
*/
ostream& operator<<(ostream& os, const wordvect& w) { 
  os << w.ord << ":" << w.entry << "(" << w.wd->count() << ")"; 
  return os; 
} // end operator<<()

/*
******************************************************************************
****************** wordvect STRUCT DEFINITION BELOW HERE *********************
******************************************************************************
*/

/*
** The default constructor creates a new instance of wdata to hold the mutable
** word data and then uses the clear() function to initialize the remaining
** data.
*/
wordvect::wordvect(void) { 
  wd=new wdata; 
  clear(); 
} // end default constructor

/*
** The alternate constructor initializes the wordvect struct in the same way
** as the default constructor and then initializes the string data with a
** supplied argument.
*/
wordvect::wordvect(string str) 
  :wordvect()
{
  entry=str;
} // end alternate constructor

/*
** The copy constructor initializes the wordvect struct in the same way
** as the default constructor and then copies all data from another
** wordvect instance to this one using the copy() function.
*/
wordvect::wordvect(const wordvect &w) 
  :wordvect()
{ 
  copy(w); 
} // end copy constructor

/*
** The destructor deallocates the "wd" structure that was dynamically 
** allocated in the constructor.
*/
wordvect::~wordvect() { 
  if (wd != nullptr) delete wd; 
} // end destructor

/*
** This "=" (assignment) operator copies the data from the wordvect instance
** on the rhs of the operator to this instance.
*/
wordvect& wordvect::operator=(const wordvect &rhs) {
  copy(rhs);
  return *this;
} // end "=" (assignment) operator definition

/*
** This "=" (assignment) operator copies the string on the rhs of the 
** operator to the string data for this instance.
*/
wordvect& wordvect::operator=(const string &sw) {
  entry = sw;
  return *this;
} // end "=" (assignment) operator definition

/*
** This "==" (equivalence) operator compares the string data for the wordvect
** instance on the rhs of the  operator to the string data for this instance 
** and returns TRUE if they are identical.
*/
bool wordvect::operator==(const wordvect &w) { 
  return(entry == w.entry); 
} // end "==" (equivalence) operator

/*
** This "==" (equivalence) operator compares the string on the rhs of the 
** operator to the string data for this instance and returns TRUE if they
** are identical.
*/
bool wordvect::operator==(const string &sw) { 
  return(entry == sw); 
} // end "==" (equivalence) operator

/*
** This "<" (less-than) operator compares the string data for the wordvect
** instance on the rhs of the  operator to the string data for this instance 
** and returns TRUE if the lhs is less than the rhs string.
*/
bool wordvect::operator<(const wordvect &w) { 
  return(comp(w));
} // end "<" (less-than) operator

/*
** This "<" (less-than) operator compares the string on the rhs of the 
** operator to the string data for this instance and returns TRUE if the lhs 
** is less than the rhs string.
*/
bool wordvect::operator<(const string &sw) { 
  return(comp(sw));
} // end "<" (less-than) operator

/*
** This copy() function copies the contents of another wordvect instance into
** the data structure of this instance, including the subordinate wdata 
** structure.
*/
void wordvect::copy(const wordvect &w) { 
  entry = w.entry; 
  ord   = w.ord; 
  train = w.train;
  test  = w.test;
  wd->copy(*w.wd); 
} // end copy()

/*
** This comp() function compares the string data for the wordvect instance in
** the argument to the string data for this instance and returns TRUE if the 
** string for this instance is "less-than" the argument string. This function 
** is used in the less-than operator code.
*/
bool wordvect::comp(const wordvect &w) const { 
  return(entry < w.entry); 
} // end comp()

/*
** This comp() function compares the string in the argument to the string data
** for this instance and returns TRUE if the string for this instance is 
** "less-than" the argument string.  This function is used in the less-than
** operator code.
*/
bool wordvect::comp(const string &sw) const { 
  return(entry < sw); 
} // end comp()

/*
** The clear() function initializes all struct data, including for the wdata
** substructure.
*/
void wordvect::clear(void) { 
  entry = ""; 
  ord   = 0; 
  wd->clear(); 
} // end clear()

/*
** The str() function returns the value stored in the "entry" variable.
*/
string wordvect::str(void) const { return entry; }


/*
** The incr() function is a pass-through function that increments the usage
** count of the word by invoking the appropriate wdata substructure function.
*/
void wordvect::incr(void) { 
  wd->incr(); 
} // end incr()

/*
** The count() function is a pass-through function that returns the usage
** count of the word by invoking the appropriate wdata substructure function.
*/
int wordvect::count(void) const { 
  return wd->count(); 
} // end count()

/*
** The setord() function sets the ordinal (unique identifier) of the word.
** This cannot be changed after the word is added to the dictionary.
*/
void wordvect::setord(int o) { 
  ord=o; 
} // end setord()

/*
** The getord() function gets the ordinal (unique identifier) of the word.
*/
int wordvect::getord(void) const { 
  return ord; 
} // end getord()

/*
** The addprec() function is a pass-through function that adds a set of 
** precursor words in the form of a sparse vector (Svect) to the wdata
** substructure.
*/
void wordvect::addprec(Svect &prec) const { 
  wdata *w; 
  w = wd; 
  w->add(prec); 
} // end addprec()

/*
** The set_train() and set_test() functions set the pointers to the training
** and testing data sets (which are lists of iterators that point to words
** in the dictionary designated for these purposes).  These data sets should
** belong to the same dictionary that this wordvect does.
*/
void wordvect::set_train(list<WVit> *t) { train = t; }
void wordvect::set_test(list<WVit> *t)  { test = t;  }

/*
** The solve() function executes the correct series of initialization and
** iteration functions to solve for the weights vector, given a proper set
** of features and observations.
*/
void wordvect::solve(double d, bool verbose) const {
  Datamodule           dm;
  list<WVit>::iterator lit;
  wdata               *w;
  int                  niter;

  w = wd; 
  w->init_logr(d); 

  // For each word in the training set, adding the precursors that go with that
  // particular word to the features if the ordinal does not match the word
  // in this instance.  These are the negative observations, while the 
  // init_logr() function initialized the positive observation data.
  lit = train->begin();
  while (lit != train->end()) {
  	if (ord != (**lit).ord) {
	  w->add_negs((**lit).word_data()->prec);
	} // end if (ord)
  	lit++;
  } // end while (lit)

  dm.set_weights(w->weights);
  dm.set_features(w->features);
  dm.set_observations(w->obs);
  niter = dm.getsoln(0.01, 1000);
  dm.get_weights(w->weights);

  if (verbose) {
  	cout << "Calculated weights after (" << niter << ") iterations:" << endl;
  	dm.display_weights();
  	dm.pred();
  	dm.apply_threshold(0.5);
  	dm.display_results();
  	dm.display_confusion();
  } // end if (verbose)

} // end solve()

/*
** The testsoln() function benchmarks the solution against the test data.  This
** function can only be run after the "solve()" function is run.
*/
void wordvect::testsoln(double thr) const {
  Datamodule           dm;
  list<WVit>::iterator lit;
  wdata               *w;
  int                  niter;

  w = wd; 
  w->init_logr(0); 

  // For each word in the testing set, adding the precursors that go with that
  // particular word to the features if the ordinal does  not match the word in 
  // this instance.  These are the negative observations, while the init_logr() 
  // function initialized the positive observation data.
  lit = test->begin();
  while (lit != test->end()) {
  	if (ord != (**lit).ord) {
	  w->add_negs((**lit).word_data()->prec);
	} // end if (ord)
  	lit++;
  } // end while (lit)

  dm.set_weights(w->weights);
  dm.set_features(w->features);
  dm.set_observations(w->obs);

  cout << "Testing results:" << endl;
  dm.display_weights();
  dm.pred();
  dm.apply_threshold(thr);
  dm.display_results();
  dm.display_confusion();

} // end test()

/*
** The find_optimal() function finds the optimal threshold by stepping
** through and noting the threshold with the closest proximity to the
** optimal point.
*/
double wordvect::find_optimal(void) const {
  Datamodule           dm;
  list<WVit>::iterator lit;
  wdata               *w;
  int                  niter;
  double               thr, tpr, fpr, dist, optDIST, optTPR, optFPR, optTHR, 
					   res[4];

  w = wd; 
  w->init_logr(0); 

  // For each word in the testing set, adding the precursors that go with that
  // particular word to the features if the ordinal does  not match the word in 
  // this instance.  These are the negative observations, while the init_logr() 
  // function initialized the positive observation data.
  lit = test->begin();
  while (lit != test->end()) {
  	if (ord != (**lit).ord) {
	  w->add_negs((**lit).word_data()->prec);
	} // end if (ord)
  	lit++;
  } // end while (lit)

  dm.set_weights(w->weights);
  dm.set_features(w->features);
  dm.set_observations(w->obs);
  dm.pred();

  thr = 0.0;
  optDIST = 1.0;
  for (int i=0; i<50; i++) {
	thr += 0.01998;
	dm.apply_threshold(thr);
    dm.calc_conf(res);
	tpr = (res[TP]/(res[TP]+res[FN]));
	fpr = (res[FP]/(res[FP]+res[TN]));
	dist = (tpr - 1.0)*(tpr - 1.0) + fpr*fpr;
	if (dist < optDIST) 
		{ optDIST = dist; optTPR = tpr; optFPR = fpr; optTHR = thr; }
  } // end for (i)
  return optTHR;
}

/*
** The find_prob() function is a pass-through function that finds the 
** probability that the word in this instance is the next one.  The 
** return value is the probability.
*/
double wordvect::find_prob(Svect &p) {
  wdata *w = wd;
  return w->find_prob(p);
}

/*
** The write() function creates an eponymous file in the "dict" subdirectory
** to store the weights that were calculated using logistic regression.
*/
void wordvect::write(void) const {
  ofstream ofile;
  wdata *w = wd;
  outdbl dout;
  outint iout;
  int    expl, sz, position, index, composite;

  string   fname = "dict\\" + entry + ".dat";
  ofile.open(fname, ios::out | ios::binary);
  if (ofile.is_open()) {
  	expl = w->weights.count_explicit();
  	sz   = w->weights.size();
  	iout.i = expl;
  	ofile.write(&iout.c[0],4);
  	iout.i = sz;
  	ofile.write(&iout.c[0],4);
  	for (int i=0; i<sz; i++) {
  		if (w->weights.is_explicit(i)) {
  			iout.i = i;
 			dout.d = w->weights[i];
 			ofile.write(&iout.c[0],4);
 			ofile.write(&dout.c[0],8);
  		} // end if (weights) 
  	} // end for (i)
  	ofile.close();
  } // end if (ofile)
  else {
  	cerr << "Could not open file \"dict\\" << entry << "\" for output." << endl;
  } // end else (ofile)
} // end write()

/*
** The read() function reads the weights from an eponymous file in the "dict" 
** subdirectory that were calculated using logistic regression and stored in 
** the file at an earlier time.
*/
void wordvect::read(void) const {
  ifstream ifile;
  wdata *w = wd;
  outdbl dout;
  outint iout;
  int expl, sz, position, index, composite;

  string   fname = "dict\\" + entry + ".dat";
  ifile.open(fname, ios::in | ios::binary);
  if (ifile.is_open()) {
  	ifile.read(&iout.c[0],4);
  	expl = iout.i;
  	ifile.read(&iout.c[0],4);
  	sz = iout.i;
  	cout << "size = " << sz << endl;
  	w->weights.resize(sz);
  	cout << "explicit = " << expl << endl;
  	for (int i=0; i<expl; i++) {
  		iout.i = 0;
  		dout.d = 0.0;
  		ifile.read(&iout.c[0],4);
  		ifile.read(&dout.c[0],8);
	  	w->weights[iout.i] = dout.d;
  	} // end for (i)
  	ifile.close();
  } // end if (ofile)
  else {
  	cerr << "Could not open file \"dict\\" << entry << "\" for input." << endl;
  } // end else (ofile)
} // end write()