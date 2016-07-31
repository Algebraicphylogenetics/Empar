/*
 *  Empar.cpp
 *
 */

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <ctime>
#include <cmath>
#include <list>
#include <string>
#include <stdexcept>

#include "Empar.h"
#include "em.h"
#include "tree.h"
#include "parameters.h"
#include "alignment.h"
#include "model.h"
#include "sampling.h"
#include "miscelania.h"
#include "random.h"
#include "fisher.h"
#include "permutation.h"


// Prints an informative warning if the tree is nonidentifiable.
bool nonident_warning(Tree &T) {
  std::list<long> Lnon;
  std::list<long>::iterator it;
  std::list<long>::iterator itlast;

  for(long i = 0; i < T.nnodes; i++) {
    // Any node (root or not) with valence 2 leads to nonident.
    if(valence(T, i) == 2) {
      Lnon.push_back(i);
    }

    // A root of valence 1 leads to nonident.
    if(valence(T, i) == 1 && i == T.nleaves) {
      Lnon.push_back(i);
    }
  }

  if (Lnon.size() > 0) {
    std::cout << "WARNING: The following nodes lead to non-identifiability of the parameters: ";
    itlast = Lnon.end();
    --itlast;
    for(it=Lnon.begin(); it != Lnon.end(); it++) {
      std::cout << *it;
      if (it != itlast) {     // if not the last element
        std::cout << ", ";
      }
    }
    std::cout << "." << std::endl << std::endl;

    std::cout << "Possible causes:" << std::endl;
    std::cout << " 1) The root has valence 1. If the model has non-uniform distribution, the length of the outgoing edge cannot be recovered reliably." << std::endl;
    std::cout << " 2) There is a node (typically thought of as the root) with exactly two incident edges. In this case only the sum of lengths of the two incident edges can be recovered reliably." << std::endl << std::endl;
  }

  return Lnon.size() > 0;
}


void save_sigmas_to(const std::string& fname,
	const std::vector<std::vector<double> >& cov_matrix)
{
    std::fstream fcov;
    fcov.precision(15);
    fcov.open(fname.c_str(), std::ios::out);
    if (!fcov.is_open()) {
      std::cout << "Could not open file: covariances.dat" << std::endl;
      // TODO: exit
    }

    for (auto row : cov_matrix) {
      for (auto cell : row) {
        fcov << cell << " ";
      }
      fcov << std::endl;
    }
    fcov << std::endl;

    fcov.close();
}

void run(std::string tree_filename, std::string fasta_filename, std::string model_name) {
  Model Mod;                 // The model
  Counts data;               // the counts
  Parameters Par;            // the parameters
  std::vector<double> br;    // branch lengths
  double eps = 1e-8;         // The threshold for the EM algorithm.

  Parameters Parsim;         // used for simulating data.
  std::vector<double> brsim; // branch lengths of simulated data.

  std::vector<std::vector<double> > Cov;  // Covariance matrix
  std::vector<double> variances;          // The variances


  bool simulate;
  bool nonident;
  std::string parameters_filename;
  std::string covariances_filename;

  // initialize random number generator with time(0).
  random_initialize();

  parameters_filename = strip_extension(fasta_filename) + ".dat";
  covariances_filename = strip_extension(fasta_filename) + ".cov";

  // Creates the pointers to the model-specific functions.
  Mod = create_model(model_name);
  std::cout << "Model: " << Mod.name << std::endl;

  // Reads the tree.
  Tree T = read_tree(tree_filename);

  // Prints the Tree
  std::cout << "Tree:" << std::endl;
  print_tree(T);

  // Check for possible nonidentifiability issues.
  nonident = nonident_warning(T);

  // Initialize the parameters for simulation of K81 data for testing
  Parsim = create_parameters(T);

  if (fasta_filename == ":test") {      // if fasta file is :test generate random data.
    simulate = true;

    // Warn
    std::cout << "WARNING: Using simulated data " << std::endl << std::endl;

    // Generate random parameters
    random_parameters_length(T, Mod, Parsim);

    // Simulate the data
    data = random_fake_counts(T, 1000, Parsim);

    // Prints branch-lengths for future check.
    branch_lengths(Parsim, brsim);
    std::cout << "Simulated branch lengths:" << std::endl;
    print_vector(brsim);

  } else {                                  // otherwise read the data
    simulate = false;

    // Read the counts.
    std::cout << "Reading fasta file:" << std::endl;
    read_counts(T, data, fasta_filename);
    add_pseudocounts(0.01, data);
    std::cout << std::endl;
  }

  // Check whether the data and the tree match.
  if (T.nalpha != data.nalpha || T.nleaves != data.nspecies) {
    throw std::invalid_argument("The order of the sequences or their number and the phylogenetic tree do not match.");
  }

  //Par = create_parameters(T);
  //print_parameters(Par);
  //print_vector(Par.r);

  //clock_t
  long start_time, end_time;

  // Runs the EM algorithm. Par is used as initial parameters.
  // After execution, Par contains the MLE computed by the algorithm.

 // for local max over multiple iterations
  Parameters Parmax = Par;
  Model Modmax = Mod;

  float likelL = 0.0;
  float likelMax = -1000000.0;
  float timerec;
  float timemax;

  int outfiles; //whether to save output
  std::cout << "Starting the EM algorithm: " << std::endl;

  int s;
  int S = 0; //count of cases with neg branches
  for (int it_runs = 0; it_runs < 10; it_runs++) {
      Par = create_parameters(T);
      Mod = create_model(model_name);
      std::cout << it_runs << ", " ;

      start_time = clock();

      likelL= EMalgorithm(T, Mod, Par, data, eps);


      end_time = clock();
      //print_parameters(Par);

      // Choses the best permutation.
      guess_permutation(T, Mod, Par);

      branch_lengths(Par, br);

      //print_vector(br);
      s = find_negative(br);
      S +=s;
      timerec = ((float)end_time - start_time) / CLOCKS_PER_SEC;

      //assign the 1st iter time value, inc ase it's the best
      if (it_runs == 0){
        timemax = timerec;
      }

      if (likelL > likelMax){
        Parmax = Par;
        Modmax = Mod;
        timemax = timerec;
        likelMax = likelL;
      }

  }


  // If parameters are not identifiable, the computation of the covariance matrix will
  // fail as the Fisher info matrix will not be invertible.
  if (!nonident) {
    // Compute the covariance matrix using observed Fisher.
    full_MLE_observed_covariance_matrix(T, Modmax, Parmax, data, Cov);
    variances.resize(Cov.size());
    for(unsigned int i=0; i < Cov.size(); i++) {
      variances[i] = Cov[i][i];
    }

    // OUTPUT Save the sigmas into a file
    //save_sigmas_to(covariances_filename, Cov);
  }

  std::cout << std::endl;
  std::cout << "Finished." << std::endl;
  std::cout << "Likelihood: " << log_likelihood(T, Parmax, data) << std::endl ;
  std::cout << "Time: " << timemax << std::endl << std::endl;
  std::cout << "negative branches: "  << S << std::endl;
  //std::cout << "Branch lengths: " << std::endl;
  //print_vector(br);
  outfiles = 0;
  if (!nonident && outfiles) {
    std::cout << "Parameter variances: " << std::endl;
    print_vector(variances);
  }

  std::cout << "Newick Tree:" << std::endl;
  print_newick_tree(T, br);

  // if is a simulation, print the L2 distance !
  if (simulate) {
    std::cout << "L2 distance:   " << parameters_distance(Par, Parsim) << std::endl;
    std::cout << "KL divergence: " << KL_divergence(T, Par, Parsim) << std::endl;
    std::cout << std::endl;
  }

  // if it is not a simulation, store the parameters in a file !
  if (!simulate && outfiles) {
    std::fstream st;
    st.precision(15);
    st.setf(std::ios::fixed,std::ios::floatfield);
    st.open(parameters_filename.c_str(), std::ios::out);
    print_parameters(Par, st);
  }
}
