
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
** The byfreq() funnction is a comparator function for sorting the priority
** list by frequency.
*/
bool byfreq(WVit first, WVit second) { 
  if (first->count() > second->count()) return true; else return false; 
}

/*
******************************************************************************
********************* Dict CLASS DEFINITION BELOW HERE ***********************
******************************************************************************
*/

/*
** The default constructor uses the clear() function to initialize all data in
** the dictionary.
*/
Dict::Dict() { clear(); loadnix("nixlist.txt","standardlist.txt"); }
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
  prioritized = false;
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

  prioritized = false;     // makes any existing prioritization invalid
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
** The check() function checks to see if a wordvect iterator is valid.  It
** returns true if so, and false if not.
*/
bool Dict::check(WVit tocheck) {
  if (tocheck != words.end()) return true;
  return false;
} // end check()

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
  string   word, path;

  if (IS_PLATFORM(LINUX)) 
    path = "dict/dict.idx";
  else if (IS_PLATFORM(WINDOWS))
    path = "dict\\dict.idx";

  ofile.open(path, ios::out | ios::binary);
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
    cerr << "Error opening \"" << path << "\" index file for output." << endl;
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
  string   word, path;
  int      maxord = 0;

  if (IS_PLATFORM(LINUX)) 
    path = "dict/dict.idx";
  else if (IS_PLATFORM(WINDOWS))
    path = "dict\\dict.idx";

  ifile.open(path, ios::in | ios::binary);
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
    cerr << "Error opening \"" << path << "\" index file for output." << endl;
  } // end else (ofile)
} // end write()

/*
** The loadnix() function loads words that are specifically excluded from
** modeling.  Roman numerals (such as those used for preamble pages and in
** the table of contents) are examples.  It also loads in common short
** words that do not model well (like "a", "an" and "the"); these words
** always added to the list of candidates.
*/
void Dict::loadnix(string fname1, string fname2) {
  ifstream ifile;
  string   instring;

  nix.erase(nix.begin(),nix.end());
  ifile.open(fname1);
  if (ifile.is_open()) {
    while (!ifile.eof()) {
      ifile >> instring;
      nix.insert(instring);
    }
    ifile.close();
  } // end if (ifile)

  stand.erase(stand.begin(),stand.end());
  ifile.open(fname2);
  if (ifile.is_open()) {
    while (!ifile.eof()) {
      ifile >> instring;
      nix.insert(instring);
      stand.insert(instring);
    }
    ifile.close();
  } // end if (ifile)


} // end loadnix()

/*
** The prioritize() function creates a priority list of iterators sorted by word frequency
** that point directly to the corresponding word in the dictionary.
*/
void Dict::prioritize() {
  WVit     wit;
  string   fname, entry;
  ifstream ifile;
  bool     nixed;
  cout << "prioritizing..." << endl;
  // erasing any previous priority list
  prilist.erase(prilist.begin(),prilist.end());

  wit = words.begin();
  while (wit != words.end()) {
    entry = wit->str();
    // checking to see whether this word has been explicitly deprioritized
    if (nix.find(entry) != nix.end()) nixed = true; else nixed = false;
    if (!nixed) {
      if (IS_PLATFORM(LINUX)) 
        fname = "dict/" + entry + ".dat";
      else if (IS_PLATFORM(WINDOWS))
        fname = "dict\\" + entry + ".dat";
      ifile.open(fname);
      if (ifile.is_open()) ifile.close();
      else                 prilist.push_front(wit);
    } // end if (nixed)
    wit++;
  } // end while (wit)
  prilist.sort(byfreq);

  //cout << "Priority list:" << endl;
  //list<WVit>::iterator it;
  //it = prilist.begin();
  //while (it != prilist.end()) {
  //  cout << **it << endl;
  //  it++;
  //}

  prioritized = true;

} // end prioritize

/*
** The get_guesses() function returns which words are likely to follow a 
** specific word stream, which is given by the "svin" argument.  The
** argument follows the same pattern used in the probability calculation
** for the wordvect: svin[ordinal] = score.  The ordinal is the integer
** ordinal of the word, and the score is the proximity to the next word.
** if the word stream is "the quick brown fox", "the" would have a score
** of 1, "quick" would have a score of 2, "brown" would have a score of
** 3, and "fox" would have a score of 4.  Higher scores indicate closer
** proximity to the next word.
*/
struct prob_pair { int i; double d; };
bool   pcomp(prob_pair &first, prob_pair &second) { return (first.d > second.d); }

int*   Dict::get_guesses(Svect &svin) {
  list<prob_pair>            results;
  list<prob_pair>::iterator  lit;
  WVit                       wit;
  multiset<string>::iterator sit;
  prob_pair                  pp;
  int                        i, m;
  double                     thresh = 0.5;

  for (i=0; i<256; i++) guesses[i]=-1;

  // Figuring out what the probability of being the next word is for each
  // word in the dictionary.  If the weights are not populated, and there
  // is no data file saved, the probability will be zero.
  //cout << "Before sorting:" << endl;
  wit = words.begin();
  while (wit != words.end()) {
    pp.d = wit->find_prob(svin);
    pp.i = wit->getord();
    if (isnormal(pp.d)) if (pp.d > thresh) results.push_front(pp);
    //cout << "i: " << pp.i << ", d: " << pp.d << "  " << svin << endl;
    wit++;
  }

  i = 0;
  // adding the standard guesses to the results
  sit = stand.begin();
  while ((sit != stand.end()) && (i < 256)) {
    m = (*this)[*sit].getord();
    if (m != -1) guesses[i++] = m;
    sit++;
  }

  // adding the regressed guesses to the results
  results.sort(pcomp);
  lit = results.begin();
  while ((lit != results.end()) && (i < 256)) {
    guesses[i] = lit->i;
    //cout << "i: " << lit->i << ", d: " << lit->d << endl;
    i++;
    lit++;
  }
  nguesses = i;
  return guesses;
}

/*
** The num_guesses() function returns the number of valid guesses obtained 
** after running the get_guesses() function.
*/
int Dict::num_guesses(void) { return nguesses; }

/*
** The show_guesses() function prints the latest set of valid guesses for the
** next word out to stdout.
*/
void Dict::show_guesses(void) {
  cout << "{ ";
  for (int i = 0; i < nguesses; i++) {
    cout << (*this)[guesses[i]] << " ";
  } // end for (i)
  cout << "}" << endl;

} // end show_guesses()

/*
** The getnew() function gets the string component of the next word in the 
** priority list that has not been regressed.  The word that is returned is 
** popped off of the priority list.
*/
string Dict::getnew() {
  string retstring;
  if (!prioritized) prioritize(); // if there is not a valid prioritization, create one
  retstring = prilist.front()->str();
  prilist.erase(prilist.begin());
  return retstring;
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
