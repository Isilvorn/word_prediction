#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>

using namespace std;

void processfile(string fname);

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

void processfile(string fname) {
  ifstream infile;
  string   wordstring, group[5];
  int      n;


  infile.open(fname);
  if (infile.is_open()) {
    n = 0;
    while (!infile.eof()) {
      infile >> wordstring;
      if (n < 5) group[n] = wordstring;
      else {
	for (int i=0; i<4; i++) group[i]=group[i+1]; // shift every word to the left
	group[4] = wordstring;
	cout   << group[0] << " " << group[1] << " " << group[2] << " " << group[3] << " " << group[4] << endl;
      } // end else (n)
      n++;
    } // end while (infile)
  } // end if (infile)
  else {
    cerr << "Bad input file name." << endl;
  }

  return;
}
