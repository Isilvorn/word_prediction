
#include "../include/dict.h"

using namespace std;

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
ostream& operator<<(ostream& os, const Dict& d) {
  WVit it = d.words.end();
  os << "[ ";
  do { 
    it--; 
    if ((*it).count() > d.thr) os << *it << " "; 
  } while (it != d.words.begin());
  os << "] " << "<" << d.words.size() << ">";
  return os;
} // end operator<<()

/*
******************************************************************************
******************* wdata STRUCT DEFINITION BELOW HERE ***********************
******************************************************************************
*/

/*
** The default constructor uses the clear() function to initialize the mutable 
** word data.
*/
wdata::wdata(void)  { 
  features = nullptr; 
  clear(); 
} // end default constructor

/*
** Destructor (does nothing).
*/
wdata::~wdata(void) { }

/*
** The clear() function initializes all data in the wdata struct.  Any data 
** currently residing in the struct is lost.
*/
void wdata::clear(void) { 
  ct=0;
  prec.erase(prec.begin(), prec.end());
  clearf();
} // end clear()

/*
** The clearf() function initializes the features data.  This array is only
** ever meant to be a temporary container to hold the features used in the
** logistic regression calculation.  Once the weights are obtained, this
** data may be discarded.
*/
void wdata::clearf(void) {
  if (features != nullptr) delete features;
  features = nullptr;
} // end clearf()

/*
** The copy() function copies all data from another wdata struct to this
** instance.  Used in the assignment operator and the copy constructor.
*/
void wdata::copy(const wdata &wd) { 
  list<Svect>::const_iterator it;
  ct = wd.ct;
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
** The init_weights() function initializes the weights used in the logistic
** regression.  It first adds in all of the data from each precursor so that
** there is an explicit element in the weights vector for every explicit
** element used in the precursor list.  It then replaces the data for each
** explicit element with an initial weight (supplied in the argument).
*/
void wdata::init_weights(double w) { 
  list<Svect>::iterator it;

  it = prec.begin(); 
  weights = *it;
  it++; 
  while (it != prec.end()) { 
    weights += *it; 
    it++; 
  } // end while (lit);
  weights = w; // overwrite all of the data with the initial weight supplied
} // end init_weights()


/*
** The init_logr() function initializes the data used in the logistic 
** regression.
*/
void wdata::init_logr(double w) {
  list<Svect>::iterator it;
  int i = 0; 

  init_weights(w);

  clearf();                          // clear any features that already exist
  features = new Svect[prec.size()]; // allocate a new array of vectors
  obs.resize(prec.size());           // clear and size the observations vector
  obs.setall(1);                     // set all of these observations to 1

  it = prec.begin();
  while (it != prec.end()) {         // copy precursor data to features
    features[i++] = *it; 
    it++; 
  } // end while (it)
} // end init_logr()

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
** The solve() function executes the correct series of initialization and
** iteration functions to solve for the weights vector, given a proper set
** of features and observations.
*/
void wordvect::solve(double d) const { 
  wdata *w; 
  w = wd; 
  w->init_logr(d); 
} // end solve()

/*
******************************************************************************
********************* Dict CLASS DEFINITION BELOW HERE ***********************
******************************************************************************
*/

/*
** The default constructor uses the clear() function to initialize all data in
** the dictionary.
*/
Dict::Dict() { clear(); }
/*
** Destructor (does nothing - there is no dynamic data other than the multiset
** which has its own destructor).
*/
Dict::~Dict() { }

/*
** The clear() function initializes all data in the dictionary.  Any data 
** currently residing in the dictionary is lost.
*/
void Dict::clear(void) {
  empty.setord(-1);
  empty = "@";
  nord = 0;
  thr = 0;
  ptrain = 0.1;
  ptest  = 0.1;
  words.erase(words.begin(),words.end());
}

/*
** The thresh() function sets the threshold for performing group operations 
** (like display). Any element that falls below the number of counts specified 
** is ignored.  The default threshold is zero.
*/
void Dict::thresh(int t) { thr = t; }

/*
** The addword() functions add a wordvect to the dictionary if it does not 
** already exist or increments the usage counter if it does already exist.  
** Returns TRUE if the wordvect was required to be added to the dictionary, 
** and FALSE if it was already in the dictionary.
*/
bool Dict::addword(wordvect &w) {
  WVit      it;
  wdata    *wd;            // temp variable to avoid triggering const violation

  it = words.find(w);
  if (it == words.end()) { // add a new record if an existing one was not found
    w.incr();
    w.setord(nord++);
    it = words.insert(w);
    return true;           // return true if a new record was added
  }
  else {                   // increment record count if an existing one was found
    wd = (*it).word_data();
    wd->incr();
    return false;          // return false if matching record already present
  }

}
// create a a wordvect from a supplied string, and then add it to the dictionary
bool Dict::addword(string sw) {
  wordvect w(sw);
  if (sw == "") return false;
  else          return addword(w);
}

/*
** The exist() function returns TRUE if there is an entry matching a string 
** search template in the dictionary, and FALSE otherwise.
*/
bool Dict::exist(string sw) {
  WVit     it;
  wordvect temp(sw);

  it = words.find(temp);
  if (it == words.end()) return false;
  else                   return true;
}

/*
** The find() functions return an iterator that points to a wordvect in the 
** dictionary that matches a search template.  If there is not an entry that 
** matches the search template, it returns an iterator set equal to words.end().
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
** The get() functions return a wordvect from the dictionary that matches a 
** search template. The first version uses another wordvect as a search template, 
** and the second uses a string.
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
** The bracket ("[]") operator allows accessing an individual element in the 
** dict. This is equivalent to the "get" function (and, in fact, invokes it in 
** the first two cases).  The last case finds the wordvect that matches a 
** specific ordinal; this one is slower, since it runs in O(n) time; the others 
** run on O(log n) time.
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
