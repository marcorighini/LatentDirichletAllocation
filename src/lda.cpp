/*
 * lda.cpp
 *
 *      Author: Marco Righini (righini.marco88@gmail.com)
 */

#include"lib/lda.h"
#include"lib/data.h"
#include"lib/model.h"
#include<iostream>
#include <boost/filesystem.hpp>
#include <time.h>

int main(int argc, char* argv[]) {
	std::string trainingset_path, validationset_path, output_path;
	double alpha = 0, beta = 0;
	int topics(100), iters(1000), topwords(20), burnin, savestep;
	bool perpl = false;
	bool export_complete = false;
	bool validation_good=true;
	srand(time(0));

	//##############################################################################//
	//							PROGRAM OPTIONS
	//##############################################################################//

	// Stringhe della sezione help
	std::string help_desc = std::string("print usage messages\n");
	std::string trainingset_desc = std::string("path to training set file\n");

	std::string validationset_desc = std::string("path to validation set file (default: none). Needed if perplexity flag setted");
	std::string output_desc = std::string("output files directory (default: same directory of input)");
	std::string topics_desc = std::string("number of topics (default: ") + boost::lexical_cast<std::string>(topics) + ")";
	std::string alpha_desc = std::string("alpha hyperparameter value (default: 0.05*averageDocumentLength/#topics)");
	std::string beta_desc = std::string("beta hyperparameter value (default: 200/#wordsInVocabulary)");
	std::string iters_desc = std::string("number of Gibbs sampling iterations (default: ") + boost::lexical_cast<std::string>(iters) + ")";
	std::string burnin_desc = std::string("number of Gibbs sampling iterations considered burnin (default: iters/2)");
	std::string savestep_desc = std::string("step at which LDA is saved to harddisk (default: (iters-burnin)/10)");
	std::string topwords_desc = std::string("number of top words to show for each topic (default: ") + boost::lexical_cast<std::string>(topwords) + ")";
	std::string perpl_desc = std::string("exports only perplexity (default: false, export variables). Needs validation");
	std::string export_complete_desc = std::string("exports all variables (default: false, export only topwords)");

	// Usage
	po::options_description desc(
			"\nUsage:\n"
					"lda [-h] [--validation VALIDATION] [--output OUTPUT] [--topics TOPICS] [--alpha ALPHA] [--beta BETA] [--iters ITERS] [--burnin BURNIN] [--savestep SAVESTEP] [--topwords TOPWORDS] [--perplexity] [--export_complete] --train TRAIN \n"
					"\nOption arguments:", 120);

	// Options
	desc.add_options()("help,h", help_desc.c_str())("train,t", po::value(&trainingset_path)->required(), trainingset_desc.c_str())("validation,v", po::value(&validationset_path),
			validationset_desc.c_str())("output,o", po::value(&output_path), output_desc.c_str())("topics", po::value(&topics), topics_desc.c_str())("alpha", po::value(&alpha), alpha_desc.c_str())(
			"beta", po::value(&beta), beta_desc.c_str())("iters", po::value(&iters), iters_desc.c_str())("burnin", po::value(&burnin), burnin_desc.c_str())("savestep", po::value(&savestep),
			savestep_desc.c_str())("topwords", po::value(&topwords), topwords_desc.c_str())("perplexity", perpl_desc.c_str())("export_complete", export_complete_desc.c_str());

	po::variables_map vm;

	try {
		// Parse command line
		store(parse_command_line(argc, argv, desc), vm);

		if (vm.count("help"))
			usage(desc);

		// Storage delle program options
		if (argc == 1)
			usage(desc);
		else {
			if (vm.count("train"))
				trainingset_path = vm["train"].as<std::string>();
			if (vm.count("validation"))
				validationset_path = vm["validation"].as<std::string>();
			boost::filesystem::path v_p(validationset_path);
			if (!boost::filesystem::exists(v_p) || !boost::filesystem::is_regular_file(v_p) || v_p.extension() != ".dat") {
				validation_good = false;
			}
			if (vm.count("output"))
				output_path = vm["output"].as<std::string>();
			boost::filesystem::path p(output_path);
			if (!boost::filesystem::exists(p) || !boost::filesystem::is_directory(p)) {
				boost::filesystem::path op(trainingset_path);
				output_path = op.remove_leaf().string();
			} else {
				boost::filesystem::path op(trainingset_path);
				output_path = op.remove_leaf().string();
			}
			if (vm.count("topics"))
				topics = vm["topics"].as<int>();
			if (vm.count("alpha"))
				alpha = vm["alpha"].as<double>();
			if (vm.count("beta"))
				beta = vm["beta"].as<double>();
			if (vm.count("iters"))
				iters = vm["iters"].as<int>();
			if (vm.count("burnin"))
				burnin = vm["burnin"].as<int>();
			else
				burnin = iters / 2;
			if (vm.count("savestep"))
				savestep = vm["savestep"].as<int>();
			else
				savestep = (iters - burnin) / 10;
			if (vm.count("topwords"))
				topwords = vm["topwords"].as<int>();
			if (vm.count("perplexity"))
				perpl = true;
			if (vm.count("export_complete"))
				export_complete = true;
		}

		conflicting_options(vm, "perplexity", "export_complete");
		option_dependency(vm, "perplexity", "validation");

		po::notify(vm);

		std::cout << "\n//####################### PREPROCESSING #######################//\n";

		// Controlla che, se perplexity settata, il validation set sia valido
		if (perpl == true) {
			if (!validation_good) {
				throw std::logic_error(std::string("ERROR: validation path \"") + validationset_path + std::string("\" is not valid"));
			}
		}

		// Importazione del dataset
		data d;
		std::cout << "\n> Importazione training set...\n";
		d.importTrain(trainingset_path);
		std::cout << ">> Numero di documenti train: " << d.getNDocsTrain() << "\n";
		std::cout << ">> Numero di termini train: " << d.getNTermsTrain() << "\n";
		std::cout << ">> Numero di parole train: " << d.getNWordsTrain() << "\n\n";

		// Esportazione vocabolario
		std::cout << "> Esportazione vocabolario...\n";
		filesystem::path voc_p(output_path);
		voc_p /= d.getTrainName() + "_voc.dat";
		d.exportVocabulary(voc_p.string());
		std::cout << ">> Path del vocabolario: " << voc_p.string() << "\n\n";

		// Importazione del validationset (se perplexity flag)
		if (perpl == true) {
			std::cout << "> Importazione validation set...\n";
			d.importValidation(validationset_path);
			std::cout << ">> Numero di documenti validation: " << d.getNDocsValidation() << "\n";
			std::cout << ">> Numero di parole validation: " << d.getNWordsValidation() << "\n";
			std::cout << ">> Numero di termini validation: " << d.getNTermsValidation() << "\n";
			std::cout << ">> Numero di termini totali (train+validation): " << d.getNTermsTotal() << "\n\n";
		}

		std::cout << "\n//########################## LDA ##########################//\n";

		if (alpha == 0) {
			alpha = 0.05 * d.getNWordsTrain() / d.getNDocsTrain() / topics;  // approximately 200/T
		}
		if (beta == 0) {
			beta = 200.0 / d.getNTermsTrain();								 // approximately 0.01
		}

		std::cout << "\n> Parametri del modello\n";
		std::cout << ">> train: " << trainingset_path << "\n";
		std::cout << ">> validation: " << validationset_path << "\n";
		std::cout << ">> output: " << output_path << "\n";
		std::cout << ">> topics: " << boost::lexical_cast<std::string>(topics) << "\n";
		std::cout << ">> alpha: " << boost::lexical_cast<std::string>(alpha) << "\n";
		std::cout << ">> beta: " << boost::lexical_cast<std::string>(beta) << "\n";
		std::cout << ">> iters: " << boost::lexical_cast<std::string>(iters) << "\n";
		std::cout << ">> burnin: " << boost::lexical_cast<std::string>(burnin) << "\n";
		std::cout << ">> savestep: " << boost::lexical_cast<std::string>(savestep) << "\n";
		std::cout << ">> topwords: " << boost::lexical_cast<std::string>(topwords) << "\n";
		std::cout << ">> perplexity: " << boost::lexical_cast<std::string>(perpl) << "\n";
		std::cout << ">> export_complete: " << boost::lexical_cast<std::string>(export_complete) << "\n\n";

		std::cout << "> Inizializzazione variabili zeta e relativi contatori...\n\n";
		model m(d, output_path, topics, alpha, beta, iters, burnin, savestep, topwords, perpl, export_complete);

		m.exportParams();

		std::cout << "> Gibbs sampling...\n";
		m.learn();

	} catch (std::exception& e) {
		std::cerr << e.what() << "\n";
		return 0;
	}
}

