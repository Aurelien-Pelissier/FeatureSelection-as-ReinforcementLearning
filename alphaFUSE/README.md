# Feature-Selection-as-Reinforcement-Learning
<img align="right" src="https://raw.githubusercontent.com/Aurelien-Pelissier/Feature-Selection-as-Reinforcement-Learning/master/img/latt.png" width=400>

Dataset often contains many features that are either redundant or irrelevant, and can thus be removed without incurring much loss of information. Decreasing the number of feature have the advantage of reducing overfitting, simplifying models, and also involve shorter training time, which makes it a key aspect in machine learning. 


This repository contains the source code to perform feature selection with a reinforcement learning approach, where the feature set state space is represented by a Direct Acyclic Graph (DAG). It provides a precompiled `C++` implementation of the FUSE 2 algorithm [1], that is an improved version of the original FUSE algorithm [2].

&nbsp;



## Running the code
#### Requirements
The algorithm is precompiled and can be run directly with the python script `FUSE.py` available in the `\FUSE` folder (requires at least `Python 3.5`). The algorithm was compiled with a `C++` compiler in Code::Blocks, any modification of the core algorithm (in `\FUSE_src`) will require to recompile the code (requires the `boost` library (available at https://www.boost.org/) and a `c++14` compiler).

#### Dataset
The dataset involve two files, the `.data` file contains a matrix `[n * f]`,  where *n* is the number of training example and *f* the number of features. the `.labels` file is an array `[n * 1]` that corresponds to the class label associated to each example.

#### Input
The script `FUSE.py` involve some input parameters  
```
    Nrepeat =  1000 #Number of time we apply FUSE
      #(Obtained best feature subset after each execution is saved in BigResuts.txt)
    
    Nt = 100000                 #Number of iteration per FUSE execution
    dataset = 'Madelon.data'    #Name of the file containing the dataset
    labels = 'Madelon.labels'   #Name of the file containing the corresponding labels
    k = 20                      #Number of nearest neighbors for reward computation
    reward = 'AUC'              #ACC or AUC
    rseed = 0                   #Seed of random generator
 ```
 For a satistically significant study, Nrepeat =  1000 and Nt = 100000 are good choices, but typically requires overnight computation.

#### Output
Each 2000 iteration during the FUSE execution, the program display the current candidate for the best feature subset (considered to be the path with highest average at the current state), along with its corresponding score:  
`
Path with Highest average:
  [ 431, 214, 543 ] -> 0.9879`
  
 At the end of the search (when the maximal number of iteration is reached), the program write the best feature subset in `BigResults.txt`, and all detailed informations in output files `Output_Tree.txt`, `Output_Reward.txt` and `Result.txt`.
(By running the script `plot_reward.py`, we can plot the evolution of the reward during the search)


&nbsp;



## References

[1]: A.  Pelissier,  "Feature Selection as Reinforcement Learning Applied to Raman Spectra for Cancer Diagnosis", Imperial University of Hokkaido, June 2018 [https://www.docdroid.net/L8GOTrg/feature-selection-aurelien-pelissier.pdf]

[2]: Gaudel, Romaric, and Michele Sebag. "Feature selection as a one-player game." International Conference on Machine Learning. 2010. [https://hal.inria.fr/inria-00484049/document].


