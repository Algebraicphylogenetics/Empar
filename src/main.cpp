#include <iostream>
#include <cstdlib>

#include "Empar.h"

void help(void) {
  std::cout << "Usage: Empar <tree file> <fasta file> <model>" << std::endl;
  std::cout << std::endl;
}

// Uses the EM algorithm to estimate the parameters for a given
// alignment. Additionally, puts the estimated parameters in a file with
// the same name as the fasta file, but ending with ".dat".

int main(int argc, char* argv[]){
  // sets the precision in which to print floats and doubles.
  std::cout.precision(15);

  // Check for command line arguments
  if (argc == 1) {
    std::cout << "No input arguments. Tree and MSA in fasta format requiered." << std::endl;
    help();
    exit(1);
  } else if (argc >= 5) {
    std::cout << "Too many arguments." << std::endl;
    help();
    exit(1);
  }

  run(argv[1], argv[2], argv[3]);
}
