#Overview
Empar is a package based on the EM algorithm for parameter estimation in discrete-time Markov models:


To compile this code you need a development environment with the GNU gcc compiler (use *make*).


Command:
```
empar <tree file> <fasta file> <model>
```
e.g.
```
./Empar test.tree data.fa ssm
```

The estimated parameters are saved in files with the same name as the fasta file and a ".dat". suffix

  <tree file>   tree in  a Newick format (any tree, not necessarily bifurcated)
  <fasta file>  input alignment in a fasta format
  <model>       model: jc, k80, k81, ssm



The nodes in the tree are labeled in the following order: first come the leaves in the order in which they appear in the Newick input tree format, followed by the internal nodes starting from the root and going top down from left to right.
For example, given a tree: *((human:0.01,ape:0.2,hamster:0.3):0.5,bird:0.4,amoeba:0.8)* the leaves take the labels 0-4: human, ape, hamster, burred, amoeba. There are two internal nodes: the 'right' one is labeled as the root (6), the 'left' one with three leaves is labeled as (5). Subsequently, the edges are listed top-down, starting from the root, from left to right: (5, 6)  0.5; (5, 3)  0.4;  (5, 4)  0.8;  (6, 0)  0.01;  (6, 1)     
  0.2;  (6, 2)  0.3. 

This is also the order in which the estimated matrices are listed in the output file.

### Note
WARNING will pop up anytime a node of valency 2 is present. The reason for it is that the parameters associated to the outgoing edges are non-identifiable. Neither are the branches stemming from such edges. In fact, one can identify only the product of the matrices and the sum of the branches.

### Output:
     model, tree details: # of leaves, nodes, edges; original branch lengths (if given);
     parameter variance is calculated from the observed Fisher information. The order follows the order of the edges and lists 
the free off-diagonal parameters of the model, e.g. JC69 lists only 1 parameter of moving to another state per matrix.

#### Output files:  
1. name_of_the_fasta_file.dat, which contains the list of all the estimated matrices in the order of the branches (and the root if in the case of the SSM)

2. name_of_the_fasta_file.cov, which is the variance-covariance matrix for the free parameters (#num_edges * #free_param)X(#num_edges*#free_param)
##### Sample output:

```
Model: Strand Symmetric
Tree:
nodes:   7
nleaves: 5
nedges:  6
Edges:
  (5, 6)  0.5
  (5, 3)  0.4
  (5, 4)  0.8
  (6, 0)  0.01
  (6, 1)  0.2
  (6, 2)  0.3
Node names: human ape hamster bird amoeba 

Reading fasta file:
Mulitple sequence alignment of length 10000bp on 5 taxa.

Reading species 1 >human

Reading species 2 >ape

Reading species 3 >hamster

Reading species 4 >bird

Reading species 5 >amoeba

Elapsed time: 17.70391 s

Likelihood: -52403.9605116558

Branch lengths: 
0.521413645823845  0.383754440749502  0.781680154271902  0.0104890363458518  0.200244346606503  0.304842430545834  

Newick Tree:
((human:0.0104890363458518,ape:0.200244346606503,hamster:0.304842430545834):0.521413645823845,bird:0.383754440749502,amoeba:0.781680154271902)
```

If <fasta file> is replaced by :test, it uses transition parameters simulated internally for which it calculates theoretical values of the joint distribution, *p*. The counts are taken to be *pN*, where *N* is the length of the alignment. This option was created for the purpose of testing.
```
./empar test2.tree :test k81
```

### IMPORTANT REMARK:
If one of the output branch lengths is given as *nan* it is due to a negative determinant of a transition matrix. This may happen for a number of reasons: the estimated matrix has a small determinant (e.g. short data) and for numerical reasons, at times it takes a negative value. Secondly, the parameters may not be identifiable (non-identifiable up to the permutation of rows in all but JC69, for which this problem does not occur): e.g. matrices whose diagonal entries are smaller or comparable to the off-diagonal ones. The algorithm attempts to recover many of this cases, however, it may happen that not all cases can be dealt with (see the reference paper for more details on the method). 

### Publications
> Kedzierska A.M. and Casanellas M.:“EM for parameter estimation of Markov models on trees” (revision).

For generating data under non-homogeneous models:
> Kedzierska A.M. and Casanellas M.: “ GenNon-h: Simulating multiple sequence alignments under the non-homogeneous dna models." (BMC Bioinformatics 2012, 13:216)


<sub> *This is a free software and it can be redistributed, modified or else as given by the terms of the GNU General Public License.* </sub>
