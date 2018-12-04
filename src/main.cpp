

#include "../include/datamodule.h"
#include "../include/dict.h"

using namespace std;

void   processfile(string, Dict&);
string cleanword(string);

int main(int argv, char **argc) {
  Dict   words_used;
  string fname;

  if (argv == 2) {
    cout << "Executing with command line arguments: ";
    for (int i=0; i<argv; i++) cout << argc[i] << " ";
    cout << endl << endl;
    fname = argc[1];
    processfile(fname, words_used);
    cout << "Dictionary:" << endl << words_used << endl;
    cout << "word almost: " << words_used["almost"].getord() << endl;
    cout << "word hump: " << words_used["hump"].getord() << endl;
    cout << "word war: " << words_used["war"].getord() << endl;

  } // end if (argv)
  else {
    cout << "Usage: ./words [Input File]" << endl;
  } // end else (argv)

  return 0;
} // end main()

/*
** The processfile() function reads in a file intended to be used for training.
*/
void processfile(string fname, Dict &d) {
  ifstream infile;
  string   wordstring, group[NVEC+1];
  int      n;


  infile.open(fname);
  if (infile.is_open()) {
    n = 0;
    while (!infile.eof()) {
      infile >> wordstring;
      wordstring = cleanword(wordstring);
      if (wordstring != "") {
        d.addword(wordstring);
        if (n < (NVEC+1)) group[n] = wordstring;
        else {
          for (int i=0; i<NVEC; i++) group[i]=group[i+1]; // shift every word to the left
          group[NVEC] = wordstring;
          for (int i=0; i<(NVEC+1); i++) cout   << group[i] << " ";
            cout << endl;
        } // end else (n)
        n++;
        if (n > 100) break; // early termination for testing
      } // end if (wordstring)
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
