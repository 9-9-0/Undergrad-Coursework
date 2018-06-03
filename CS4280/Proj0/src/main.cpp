#include "lib/procInput.hpp"
#include "lib/incrDiv.hpp"
#include "lib/timer.h"
#include <fstream>
using namespace std;

bool set_outfile(ofstream &out, const string &filename);

int main(int argc, char** argv) {
  timer t;
  t.start("Timer Started.");
  string infile = "";
  ifstream inf;
  vector<int> input_nums;

  //Argument Parsing
  if (argc <= 1 || argc > MAX_NUM_INTS + 1) {
    cout << ERR_NUM_INPUTS;
    return 1;
  }
  //Case to address ambiguity
  else if (argc == 2) {
    //Detect if it's a file or a number
    int num = is_num(argv[1]);

    if (num == -1) {
      //Go back and swap out -1 for something else...if -1 is input as an argument OFILE error displays.
      if (!file_exists(argv[1])) {
        cout << ERR_OFILE;
        return 1;
      }
      infile = argv[1];
      inf.open(argv[1]);
    }
    else {
      inf.open(argv[1]);
      if (inf.is_open()) {
        if (treat_as_file(argv[1])) {
          infile = argv[1];

          if (!parse_nums_file(inf, input_nums)) {
            cout << ERR_INPUT_FILE;
            return 1;
          }
        }
        else {
          input_nums.push_back(num);
          inf.close();
        }
      }
      else {
        input_nums.push_back(num);
      }
    }
  }
  //Case to validate and store integer arguments
  else {
    input_nums.reserve(argc - 1);
    if (!parse_nums(argc, argv, input_nums)) {
      cout << ERR_INPUT_VAL;
      return 1;
    }
  }

  if (!neg_check(input_nums)) {
    cout << ERR_NEG;
    return 1;
  }

  //Create output file
  ofstream outf;
  if (!set_outfile(outf, infile)) {
    cout << ERR_OFILE;
    return 1;
  }

  //Perform Computation
  incr_div(input_nums, outf);

  //Cleanup
  if (outf.is_open()) outf.close();
  if (inf.is_open()) inf.close();

  t.stop("Timer Stopped.");
  cout << t.timeVal() << " seconds." << std::endl;
  return 0;
}

/* Opens the output file for the filestream object depending on whether or not an input file was supplied. */
bool set_outfile(ofstream &out, const string& filename) {
  if (filename.length() == 0) {
    out.open("screen.out");
  }
  else {
    out.open(filename + ".out");
  }
  return out.is_open();
}
