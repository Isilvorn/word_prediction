#include <ctime>
#include <chrono>
#include <ratio>

#include "../include/datamodule.h"
#include "../include/dict.h"
#include "../include/menu.h"

using namespace std;
using namespace std::chrono;

void   processfile(string, Dict&, int=1000);
double predictCalcTime(int,double&,bool=false);
string cleanword(string);
int    parse(string, string[]);

int main(int argv, char **argc) {
  Menu           mainMenu;
  Dict           words_used;
  int            idx=0, N=1, maxwords = 1000;
  string         fname = "war_and_peace.txt", lastword="", nextword="";
  double         thr,f=1.0,ptime;
  bool           fileread=false;

  list<WVit>::iterator it;
  steady_clock::time_point t1, t2;
  duration<double> time_span;

  if (argv == 2) {
    cout << "Executing with command line arguments: ";
    for (int i=0; i<argv; i++) cout << argc[i] << " ";
    fname = argc[1];
    cout << endl << "Substituting \"" << fname << "\" for default data source." <<endl << endl;
  } // end if (argv)

  mainMenu.load("menu.txt");
  do {

    switch(idx) {
      case 1: // loading data source
        cout << "Processing data source...";
        fflush(stdout);
        t1 = steady_clock::now();
        processfile(fname, words_used, maxwords);
        t2 = steady_clock::now();
        cout << "Done." << endl;
        words_used.thresh(0);
        time_span = duration_cast<duration<double>>(t2 - t1);
        cout << endl << "Elapsed time: " << time_span.count() << " seconds." << endl;
        cout << "Writing dictionary index to disk...";
        fflush(stdout);
        words_used.write();
        cout << "Done." << endl << endl;
        nextword = words_used.getnew();
        fileread = true;
       break;
      case 2: // loading dictionary from disk
        cout << "Loading dictionary index from disk...";
        fflush(stdout);
        words_used.read();
        cout << "Done." << endl;
        break;
      case 3:
        cout << "Dictionary:" << endl << endl << words_used << endl << endl;
        break;
      case 4:
        cout << endl;
        words_used[lastword].word_data()->disp_features();
        cout << endl;
        break;
      case 5:
        cout << endl;
        words_used[lastword].word_data()->disp_obs();
        cout << endl;
        break;
      case 6:
        if (fileread) {
          words_used.thresh(0);
          for (int i = 0; i < N; i++) {
            ptime = predictCalcTime(words_used[nextword].num_obs(),f);
            cout << "Computing model for \"" << nextword << "\" (predicted time: "
                 << setprecision(1) << fixed << ptime << " seconds)..." << endl;
            fflush(stdout);
            t1 = steady_clock::now();
            words_used[nextword].solve(0.5, true);
            thr = words_used[nextword].find_optimal();
            t2 = steady_clock::now();
            cout << endl << "Done." << endl << endl;
            time_span = duration_cast<duration<double>>(t2 - t1);
            cout << endl << "Elapsed time  : " << time_span.count() << " seconds.";
            cout << endl << "Predicted time: " << ptime << " seconds." << endl;
            if (f == 1.0) f = time_span.count()/ptime;
            else          f = (time_span.count()/ptime + f)/2.0;
            cout <<"Optimal threshold for last regression: " << setprecision(4) << fixed << thr 
                 << endl << endl;
            cout << "Writing regression model for \"" << nextword << "\" to disk...";
            fflush(stdout);
            words_used[nextword].write();
            lastword = nextword;
            nextword = words_used.getnew();
          } // end for (i)
        } // end if (fileread)
        else {
          cout << "You must load the data source first!" << endl;
        } // end else (fileread)
        cout << "Done." << endl << endl;
        break;
      case 7:
        cout << "Testing solution for \"" << lastword << "\"..." << endl;
        fflush(stdout);
        words_used[lastword].testsoln(thr);
        cout << endl << "Done." << endl << endl;
        break;
      case 8:
        cout << "Current work queue size is " << N << "." << endl;
        cout << "Please enter a new size > ";
        cin  >> N;
        break;
      case 9:
        cout << "Current early termination for reading input file (# of words): " 
             << maxwords << endl;
        cout << "Please enter new value > ";
        cin  >> maxwords;
        fileread = false;
    }

    mainMenu.draw(0,50);
    mainMenu.addenda("Data Source: " + fname,false);
    mainMenu.addenda("Work queue : ",N,2,false);    
    mainMenu.addenda("Terminate  : ",maxwords,4,false);    
    mainMenu.addenda("Last word  : " + lastword,false);
    mainMenu.addenda("Next word  : " + nextword,true);

  } while ((idx=mainMenu.prompt()) != 0);

  return 0;
} // end main()

/*
** The processfile() function reads in a file intended to be used for training.
*/
void processfile(string fname, Dict &d, int maxwords) {
  ifstream   infile;
  string     wordstring, dropword, group[NVEC+1];
  int        n, m;
  WVit       wit;
  Svect      prec_example(MAXD);
  string     words[10];
  bool       dup;

  infile.open(fname);
  if (infile.is_open()) {
    n = 0;
    while (!infile.eof()) {
      infile >> wordstring;
      wordstring = cleanword(wordstring);
      
      m = parse(wordstring, words);
      //cout << endl;
      for (int j=0; j < m; j++) {
        if (words[j] != "") {
          d.addword(words[j]);
          wit = d.find(words[j]);
          if (n < NVEC) {
            group[n] = words[j];
            prec_example[wit->getord()] = n+1;
          }
          else {
            group[NVEC] = words[j];
            // checking for duplicates among the precursors - only want to drop if there are none
            // in order to keep the highest value precursor
            dup = false;
            for (int i=1; i < NVEC; i++) if (group[0] == group[i]) dup = true;
            if (!dup) dropword = group[0]; else dropword = "";
            // shift every word to the left and add to the precursor data
            for (int i = 0; i < NVEC; i++) {
              //cout << group[i] << " ";
              group[i] = group[i+1];
            }
            //cout << "<" << words[j] << ">" << endl;
            //cout << prec_example << endl;
            wit->addprec(prec_example);
            prec_example -= 1;                            // decrement the value of all precursor words by one
            prec_example.remove(d[dropword].getord());    // remove the oldest word from the precursors
            prec_example[d[words[j]].getord()] = NVEC;    // add the current word to the precursors
          } // end else (n)
          n++;
        } // end if (words)
      } // end for (j)
      if ((maxwords != 0) && (n > maxwords)) break; // early termination 
    } // end while (infile)
  } // end if (infile)
  else {
    cerr << "Bad input file name." << endl;
  }

  return;
}

/*
**
*/
double predictCalcTime(int nobs, double &f, bool init) {
  double ptime;
  if (nobs <= 130) ptime = 0.4353*exp(0.0262*nobs);
  else             ptime = 0.2636*nobs - 21.609;

  if (init) return ptime;
  else      return (f*ptime);
}

/*
** The cleanword() function converts all characters to lower case and strips out punctuation,
** numbers, and other special characters, and replaces them with spaces.
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

  words[k] = word;
  newword = false;
  word = "";
  word += c;

  return (k+1);
} // end parse()
