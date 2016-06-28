#include <iostream>
#include <cstdlib>

#include "Empar.h"

#include <stdexcept>


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
    help();
    throw std::invalid_argument("No input arguments. Tree and MSA in fasta format requiered.");

  } else if (argc >= 5) {
    help();
    throw std::invalid_argument("No input arguments. Tree and MSA in fasta format requiered.");

  }

  run(argv[1], argv[2], argv[3]);
}
