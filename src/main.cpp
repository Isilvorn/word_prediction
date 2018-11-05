#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <list>

using namespace std;

#define NVEC 4 // the size of the word vector (# of priors)

/*
** The "wordvect" structure stores the word vector and associated data.
*/
struct wordvect {
public:
  wordvect(void)       { entry="";  for (int i=0; i<4; i++) prec[i]=""; }
  wordvect(string str) { entry=str; for (int i=0; i<4; i++) prec[i]=""; }
  string entry;
  string prec[NVEC];
};  

/*
** The compvect() function compares two vectors for sorting purposes.
*/
bool compvect(wordvect *first, wordvect *second) {
  if (first->entry > second->entry) return true; else return false;
}

/*
** The "Dict" class stores the dictionary that is used to score the vectors.
*/
class Dict {
public:
  Dict();
  ~Dict();

private:
  list<wordvect*> words; 
};

void   processfile(string);
string cleanword(string);

int main(int argv, char **argc) {
  string fname;

  if (argv == 2) {
    cout << "Executing with command line arguments: ";
    for (int i=0; i<argv; i++) cout << argc[i] << " ";
    cout << endl << endl;
    fname = argc[1];
    processfile(fname);
  } // end if (argv)
  else {
    cout << "Usage: ./words [Input File]" << endl;
  } // end else (argv)

  return 0;
} // end main()

/*
** The processfile() function reads in a file intended to be used for training.
*/
void processfile(string fname) {
  ifstream infile;
  string   wordstring, group[NVEC+1];
  int      n;


  infile.open(fname);
  if (infile.is_open()) {
    n = 0;
    while (!infile.eof()) {
      infile >> wordstring;
      wordstring = cleanword(wordstring);
      if (n < (NVEC+1)) group[n] = wordstring;
      else {
	for (int i=0; i<NVEC; i++) group[i]=group[i+1]; // shift every word to the left
	group[NVEC] = wordstring;
	for (int i=0; i<(NVEC+1); i++) cout   << group[i] << " ";
	cout << endl;
      } // end else (n)
      n++;
    } // end while (infile)
  } // end if (infile)
  else {
    cerr << "Bad input file name." << endl;
  }

  return;
}

/*
** The cleanword() function converts all characters to lower case and strips out punctuation,
** literal numbers, and other special characters.
*/
string cleanword(string inword) {
  char c;
  string outword;

  for (int i=0; i<inword.length(); i++) {
    c = inword[i];
    if (((int)c >= 97) && ((int)c <= 122)) outword += c;
    else {
      if (((int)c >= 65) && ((int)c <= 90)) { c += 32; outword += c; }
    } // end else (c)
  } // end for (i)

  return outword;
} // end cleanword()

Dict::Dict(void) { }

Dict::~Dict(void) { }
