This tool was developed for LDA (Latent Dirichlet Allocation) topic modeling.
The input to be used is obtained with LDA preprocessing software in my list of projects.

# Building project
Debug folder contains makefile for project. 
In order to build you need to install some additional libraries:
* boost/filesystem
* boost/algorithm/string
* boost/bimap
* boost/program_options
* boost/lexical_cast
* boost/tuple

# Usage

```
Usage:
lda [-h] [--validation VALIDATION] [--output OUTPUT] [--topics TOPICS] [--alpha ALPHA] [--beta BETA]
	[--iters ITERS] [--burnin BURNIN] [--savestep SAVESTEP] [--topwords TOPWORDS] [--perplexity]
	[--export_complete] --train TRAIN
	
Option arguments:
	-h [ --help ] 			print usage messages
	-t [ --train ]	arg 		path to training set file
	-v [ --validation ] arg 	path to validation set file (default: none). Needed if perplexity flag setted
	-o [ --output ] arg 		output files directory (default: same directory of input)
	--topics arg			number of topics (default: 100)
	--alpha arg			alpha hyperparameter value (default: 0.05*averageDocumentLength/#topics)
	--beta arg			beta hyperparameter value (default: 200/#wordsInVocabulary)
	--iters arg			number of Gibbs sampling iterations (default: 1000)
	--burnin arg			number of Gibbs sampling iterations considered burnin (default: iters/2)
	--savestep arg			step at which LDA is saved to harddisk (default: (iters-burnin)/10)
	--topwords arg			number of top words to show for each topic (default: 20)
	--perplexity			exports only perplexity (default: false, export variables). Needs validation
	--export_complete		exports all variables (default: false, export only topwords)
```

# Documentation
Documentation available in doc/Review.pdf.

# Sample output and bash scripts
Sample output is available in data folder, bash scripts for running sample LDA tests are in bash folder.
In details only output for kos dataset is included for size cause. If you are interested in the others dataset outputs (nips and enron) please contact me at righini.marco88 [AT] gmail.com. 

# License
Latent Dirichlet Allocation project is licensed under the terms of the [Apache License, version 2.0](http://www.apache.org/licenses/LICENSE-2.0.html). Please see the [LICENSE](LICENSE.md) file for full details.
