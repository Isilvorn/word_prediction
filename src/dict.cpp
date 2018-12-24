
#include "../include/dict.h"

/*
** These definitions of the random engine and distribution are used as
** components of the RAND macro to generate a random number in the
** semi-open range [0,1).
*/
default_random_engine             rand_gen; 
uniform_real_distribution<double> rand_distr(0.0,1.0);

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
ostream& operator<<(ostream& os, const Dict& d) {
  list<WVit>::const_iterator lit;
  WVit it = d.words.end();
  os << "dict: [ ";
  do { 
    it--; 
    if ((*it).count() > d.thr) os << *it << " "; 
  } while (it != d.words.begin());
  os << "] " << "<" << d.words.size() << ">" << endl << endl;

  os << "test: [ ";
  lit = d.test.begin();
  while (lit != d.test.end()) { os << **lit << " "; lit++; } 
  os << "] " << "<" << d.test.size() << ">" << endl << endl;

  os << "train: [ ";
  lit = d.train.begin();
  while (lit != d.train.end()) { os << **lit << " "; lit++; } 
  os << "] " << "<" << d.train.size() << ">" << endl << endl;

  return os;
} // end operator<<()

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
  ptrain = 0.18;
  ptest  = 0.18;
  words.erase(words.begin(),words.end());
  train.erase(train.begin(),train.end());
  test.erase(test.begin(),test.end());
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
bool Dict::addword(wordvect &w, bool neword) {
  WVit      it;
  double    d;
  wdata    *wd;            // temp variable to avoid triggering const violation

  it = words.find(w);
  if (it == words.end()) { // add a new record if an existing one was not found
    w.incr();
    if (neword) w.setord(nord++);
    w.set_train(&train);
    w.set_test(&test);
    it = words.insert(w);
    // there is a certain small chance that a newly created entry will also be
    // added to either the training or the testing data set (but not both)
    d = RAND;
    if      (d < ptrain)         train.push_front(it);
    else if (d < (ptrain+ptest)) test.push_front(it);
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
** The write() function writes the dictionary index to a file in the "dict"
** subdirectory.  This file can be later read in to reconstruct the multiset.
*/
void Dict::write(void) {
  wordvect wv;
  WVit     it;
  ofstream ofile;
  outint   iout_sz, iout_ord, iout_len;
  char     charout[50];
  string   word;

  ofile.open("dict\\dict.idx", ios::out | ios::binary);
  if (ofile.is_open()) {
    iout_sz.i = words.size();
    ofile.write(&iout_sz.c[0],4);
    it = words.begin();
    while (it != words.end()) {
      word = it->str();
      iout_ord.i = it->getord();
      strcpy(charout,word.c_str());
      //cout << charout << ": " << iout_ord.i << "(";
      iout_len.i = word.length();
      //cout << iout_len.i << ")" << endl;
      ofile.write(&iout_ord.c[0],4);
      ofile.write(&iout_len.c[0],4);
      ofile.write(&charout[0],iout_len.i);
      it++;
    } // end while (it)
    ofile.close();
  } // end if (ofile)
  else {
    cerr << "Error opening \"dict\\dict.idx\" index file for output." << endl;
  } // end else (ofile)
} // end write()

/*
** The read() function reads the dictionary index from a file in the "dict"
** subdirectory; this file was written earlier after training the model.
** Note that this function will destroy the current contents of this Dict
** instance.
*/
void Dict::read(void) {
  wordvect wv;
  WVit     it;
  ifstream ifile;
  outint   iout_sz, iout_ord, iout_len;
  char     charout[50];
  string   word;
  int      maxord = 0;

  ifile.open("dict\\dict.idx", ios::in | ios::binary);
  if (ifile.is_open()) {
    clear();
    ifile.read(&iout_sz.c[0],4);
    for (int i=0; i<iout_sz.i; i++) {
      ifile.read(&iout_ord.c[0],4);
      ifile.read(&iout_len.c[0],4);
      ifile.read(&charout[0],iout_len.i);
      charout[iout_len.i] = 0;
      if (iout_ord.i >= maxord) maxord = iout_ord.i + 1;
       word = charout;
      wv.clear();
      wv = word;
      wv.setord(iout_ord.i);
      //cout << wv << endl;
      addword(wv, false);
    } // end for (i)
    ifile.close();
    nord = maxord;
  } // end if (ifile)
  else {
    cerr << "Error opening \"dict\\dict.idx\" index file for output." << endl;
  } // end else (ofile)
} // end write()

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
