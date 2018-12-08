

#include "../include/datamodule.h"
#include "../include/dict.h"

using namespace std;

void   processfile(string, Dict&);
string cleanword(string);
int    parse(string, string[]);

int main(int argv, char **argc) {
  Dict   words_used;
  string fname;

  if (argv == 2) {
    cout << "Executing with command line arguments: ";
    for (int i=0; i<argv; i++) cout << argc[i] << " ";
    cout << endl << endl;
    fname = argc[1];
    processfile(fname, words_used);
    words_used.thresh(0);
    cout << "Dictionary:" << endl << words_used << endl;
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
  int      n, m;
  WVit     wit;
  wdata   *wd;
  Svect    prec_example;
  string   words[10];

  infile.open(fname);
  if (infile.is_open()) {
    n = 0;
    while (!infile.eof()) {
      infile >> wordstring;
      wordstring = cleanword(wordstring);
      
      m = parse(wordstring, words);
      cout << endl;
      for (int j=0; j < m; j++) {
        if (words[j] != "") {
          d.addword(words[j]);
          wit = d.find(words[j]);
          wd = wit->word_data();
          if (n < (NVEC+1)) group[n] = words[j];
          else {
            prec_example.resize(MAXD); // clears the existing data
            // shift every word to the left and add to the precursor data
            for (int i=0; i<=NVEC; i++) {
              prec_example[d[group[i]].getord()] = (double)(i+1);
              cout << group[i] << " ";
              if (i < NVEC) group[i]=group[i+1];
            }
            cout << "<" << words[j] << ">" << endl;
            cout << prec_example << endl;
            wd->add(prec_example);
            group[NVEC] = words[j];
          } // end else (n)
          n++;
        } // end if (wordstring)
      } // end for (j)
      //if (n > 100) break; // early termination for testing
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
      else if (outword[outword.length()-1] != ' ')     outword += ' ';
    } // end else (c)
  } // end for (i)

  return outword;
} // end cleanword()

/*
** The parse() strips out spaces that are generated when special characters are replaced
** by spaces in the cleanword() function and then splits up those components into different
** words.
*/
int parse(string instr, string words[]) {
  char   c;
  string word;
  bool   newword = false;
  int    k;

  for (int i=0; i<10; i++) words[i]="";
  word = "";
  k = 0;
  for (int i=0; i<instr.length(); i++) {
    c = instr[i];
    if ((int)c == 32) {
      newword = true;
    } // end if (c, newword)
    else if (newword == true) {
      words[k] = word;
      k++;
      newword = false;
      word = "";
      word += c;
    } // end else if (c, newword)
    else {
      word += c;
    } // end else (c, newword)

  } // end for (i)

  //if (newword == false) {
    words[k] = word;
    newword = false;
    word = "";
    word += c;
  //} // end if (newword)

  return (k+1);
} // end parse()
