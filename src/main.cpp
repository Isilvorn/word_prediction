#include <ctime>
#include <chrono>
#include <ratio>

#include "../include/datamodule.h"
#include "../include/dict.h"
#include "../include/menu.h"

using namespace std;
using namespace std::chrono;

void   processfile(string, Dict&, int=1000);
void   evalmodel(string, Dict&, int=0);
double predictCalcTime(int,double=1.0,bool=false);
string cleanword(string);
int    parse(string, string[]);

int main(int argv, char **argc) {
  Menu           mainMenu;
  Dict           words_used;
  int            n, m, idx=0, N=1, maxwords = 5000, nobs, nobsmin;
  string         strtime, altfname, fname = "sherlock_holmes.txt", lastword="", nextword="", inword;
  double         thr,f=1.0,ptime;
  bool           incpool=true,fileread=false;
  Svect          testvector(MAXD);
  string         words[10], teststring = "you are no longer";
  int            *ret;
  ofstream       logfile;
  time_t         curtime;

  // creating default test vector
  //testvector[24]  = 1; // you
  //testvector[129] = 2; // are
  //testvector[18]  = 3; // no
  //testvector[160] = 4; // longer

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
        if (fileread) {
          cout << "Dictionary:" << endl << endl << words_used << endl << endl;
        } // end if (fileread)
        else {
          cerr << "You must load the data source first!" << endl;
        } // end else (fileread)
        break;
      case 4:
        nobsmin = 300;
        words_used.thresh(0);
        logfile.open("log.txt", std::ios_base::app);
        for (int i = 0; i < N; i++) {  
          if (fileread) nobs = words_used[nextword].num_obs(); else nobs=0;
          if (nobs > nobsmin) {
            incpool = true;
            ptime = predictCalcTime(nobs,f);
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
            cout << endl << "Projected time: " << ptime << " seconds." << endl;
            if (f == 1.0) f = time_span.count()/ptime;
            else          f = (f*time_span.count()/ptime + f)/2.0;
            cout <<"Optimal threshold for last regression: " << setprecision(4) << fixed << thr 
                 << endl << endl;
            cout << "Writing regression model for \"" << nextword << "\" to disk...";
            fflush(stdout);
            words_used[nextword].write();
            lastword = nextword;
            nextword = words_used.getnew();
            cout << "Done." << endl;
            if (logfile.is_open()) {
              time(&curtime);
              strtime = ctime(&curtime);
              strtime = strtime.substr(0,strtime.length()-1);
              logfile << setprecision(1) << fixed;
              logfile << "#" << setw(4) << i << " of " << setw(4) << N << " / " << strtime 
                      << " : " << setw(15) << lastword << " : " << setw(5) 
                      << time_span.count() << " sec " 
                      << (words_used[lastword].isvalid()?"(success)":"(FAILURE)") << endl;
              } // end if (logfile) 
          } // end if (nobs)
          else {
            if (incpool) {
              cout << "Increasing data set size to obtain more examples..." << endl;
              maxwords += 500;
              processfile(fname, words_used, maxwords);
              words_used.thresh(0);
              words_used.write();
              nextword = words_used.getnew();
              fileread = true;
              incpool = false;
              if (logfile.is_open()) {
                logfile << "*** Increasing data set size to " << maxwords << " words ***" << endl;
              } // end if (logfile)
            } // end if (incpool)
            else {
              cout << "Reducing required data set size...";
              nobsmin *= 0.95;
              incpool = true;
              if (logfile.is_open()) {
                logfile << "*** Reducing required data set size to " << nobsmin << " features ***" << endl;
              } // end if (logfile)
            }
            cout << "Done." << endl;
          } // end else (nobs)
        } // end for (i)
        cout << "Done." << endl << endl;
        logfile.close();
        break;
      case 5:
        cout << "Testing solution for \"" << lastword << "\"..." << endl;
        fflush(stdout);
        words_used[lastword].testsoln();
        cout << endl << "Done." << endl << endl;
        break;
      case 6:
        cout << "Current work queue size is " << N << "." << endl;
        cout << "Please enter a new size > ";
        cin  >> N;
        break;
      case 7:
        cout << "Current early termination for reading input file (# of words): " 
             << maxwords << endl;
        cout << "Please enter new value > ";
        cin  >> maxwords;
        fileread = false;
        break;
      case 8:
        cout << "Which word? > ";
        cin  >> inword;
        cout << "Testing solution for \"" << inword << "\"..." << endl;
        fflush(stdout);
        words_used[inword].testsoln();
        cout << endl << "Done." << endl << endl;
        break;
      case 9:
        cout << "Test String: " << teststring << endl;
        teststring = cleanword(teststring);
        testvector.resize(MAXD);
        m = parse(teststring, words);
        for (int i=0; i<m; i++) {
          testvector[words_used[words[i]].getord()] = i + 1;
        }
        cout << "Test Vector: " << testvector << endl;
        ret = words_used.get_guesses(testvector);
        for (int i=0; i<words_used.num_guesses(); i++) {
          cout << "ret[" << setw(3) << i << "] = " << setw(4) << ret[i] 
               << "word: " << words_used[ret[i]] << endl;
        }
        break;
      case 10:
        cout << "Enter new test string: ";
        cin.ignore(numeric_limits<streamsize>::max(),'\n');
        getline(cin, teststring);
        break;
      case 11:
        cout << "Testing model against \"" << fname << "\":" << endl;
        evalmodel(fname, words_used, 0);
        break;
      case 12:
        cout << "What file would you like to test against? > ";
        cin.ignore(numeric_limits<streamsize>::max(),'\n');
        getline(cin, altfname);
        cout << "Testing model against \"" << altfname << "\":" << endl;
        evalmodel(altfname, words_used, 0);
        break;
    }

    mainMenu.draw(0,50);
    mainMenu.addenda("Data Source: " + fname,false);
    mainMenu.addenda("Work queue : ",N,4,false);    
    mainMenu.addenda("Terminate  : ",maxwords,5,false);    
    mainMenu.addenda("Last word  : " + lastword,false);
    mainMenu.addenda("Next word  : " + nextword,false);
    mainMenu.addenda("Test string: " + teststring,true);

  } while ((idx=mainMenu.prompt()) != 0);

  return 0;
} // end main()

/*
** The processfile() function reads in a file intended to be used for training.
*/
void processfile(string fname, Dict &d, int maxwords) {
  ifstream   infile;
  ofstream   logfile;
  string     wordstring, dropword, group[NVEC+1];
  int        n, m;
  WVit       wit;
  Svect      prec_example(MAXD);
  string     words[10];
  bool       dup;

  infile.open(fname);
  if (infile.is_open()) {
    n = 0;
    d.clear();
    while (!infile.eof()) {
      infile >> wordstring;
      wordstring = cleanword(wordstring);
      
      m = parse(wordstring, words);
      //cout << endl;
      for (int j=0; j < m; j++) {
        words[j] = cleanword(words[j]);
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
            //testing code
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
    infile.close();
  } // end if (infile)
  else {
    cerr << "Bad input file name." << endl;
  } // end else (infile)

  return;
} // end processfile()

/*
** The evalmodel() function reads in a file and evaluates the extent to which 
** the model can predict the next word.  The model is considered successful
** if the correct word is contained within the first 16 guesses.
*/
void evalmodel(string fname, Dict &d, int wordno) {
  ifstream   infile;
  string     wordstring, dropword, group[NVEC+1];
  int        n, m, ord, nguesses, ntrue=0, nfalse=0;
  WVit       wit;
  Svect      prec_example(MAXD);
  string     words[10];
  bool       dup, is_present;
  int       *ret;

  infile.open(fname);
  if (infile.is_open()) {
    n = 0;
    while (!infile.eof()) {
      infile >> wordstring;
      wordstring = cleanword(wordstring);
      
      m = parse(wordstring, words);
      for (int j=0; j < m; j++) {
        if (words[j] != "") {
          wit = d.find(words[j]);
          if (n < NVEC) {
            group[n] = words[j];
            prec_example[wit->getord()] = n+1;
          }
          else if (d.check(wit)) {
            group[NVEC] = words[j];
            // checking for duplicates among the precursors - only want to drop if there are none
            // in order to keep the highest value precursor
            dup = false;
            for (int i=1; i < NVEC; i++) if (group[0] == group[i]) dup = true;
            if (!dup) dropword = group[0]; else dropword = "";
            // shift every word to the left and add to the precursor data
            for (int i = 0; i < NVEC; i++) {
              group[i] = group[i+1];
            }

            ord = d[words[j]].getord();
            is_present = false;
            if (n > wordno) {
              ret = d.get_guesses(prec_example);
              nguesses = d.num_guesses();
              //nguesses = (nguesses>16?16:nguesses);
              for (int k=0; k<nguesses; k++) {
                if (ret[k] == ord) { is_present = true; break; }
              } // end for (k)
              if (is_present) ntrue++; else nfalse++;
            } // end if (n)

            cout << setprecision(1) << fixed;
            cout << setw(5) << n << ": " << setw(15) << words[j] << " --> " << (is_present?"success":"FAILURE") 
                 << " (" << (ntrue*100.0/(ntrue+nfalse))  << "%) " << prec_example << endl;

            prec_example -= 1;                         // decrement the value of all precursor words by one
            prec_example.remove(d[dropword].getord()); // remove the oldest word from the precursors
            prec_example[ord] = NVEC;                  // add the current word to the precursors

          } // end else (n)
          n++;
        } // end if (words)
      } // end for (j)
    } // end while (infile)

    cout << "Report ===============" << endl;
    cout << setprecision(1) << fixed;
    cout << "# of successes: " << setw(4) << ntrue  << " (" << (ntrue*100.0/(ntrue+nfalse))  << "%)" << endl;
    cout << "# of failures : " << setw(4) << nfalse << " (" << (nfalse*100.0/(ntrue+nfalse)) << "%)" << endl;
  } // end if (infile)
  else {
    cerr << "Bad input file name." << endl;
  } // end else (infile)

  return;
} // end evalmodel()

/*
** The predictCalcTime() function calculates a prediction of the amount of time required
** to converge on a calculation given the standard paramters on the reference machine.
** supplying a factor (f) to the function other than 1.0 will scale the calculation by
** a constant factor.  This is done to account for variations in hardware.
*/
double predictCalcTime(int nobs, double f, bool init) {
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
  unsigned char c;
  int    len, i;
  string outword;

  // skipping any leading spaces
  i = 0;
  while (inword[i] == ' ') { i++; }
  if (i > 0) inword = inword.substr(i,inword.length());

  len = inword.length();
  for (i = 0; i < len; i++) {
    c = inword[i];
    // convert certain unicode values to mundane ascii equivalents
    //if ((int)inword[i] == -30) if ((int)inword[i+1] == -128) if ((int)inword[i+2] == -103) { c = 39;  i+=2; }
    //if ((int)inword[i] == -61) if ((int)inword[i+1] == -95)                                { c = 225; i+=1; }

    //out << (int)c << " ";
    if ((((int)c >= 97) && ((int)c <= 122)) || ((int)c >= 223) || 
        (((int)c == 39) && (i != 0) && (i != (len-1)))) { outword += c; }
    else {
      // change upper case to lower case
      if      (((int)c >= 65) && ((int)c <= 90))  { c += 32; outword += c; }
      else if (((int)c >= 192) && ((int)c <=222)) { c += 32; outword += c; }
      // use only one space in a consecutive string of spaces
      else if (outword[outword.length()-1] != ' ')     outword += ' ';
    } // end else (c)
  } // end for (i)
  //cout << endl;
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
