/*
 * data.cpp
 *
 *      Author: Marco Righini (righini.marco88@gmail.com)
 */

#include "data.h"
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <iostream>

using namespace boost;

void data::importTrain(std::string train_path) {
	this->train_path = train_path;
	filesystem::path train_p(train_path);
	if (filesystem::exists(train_p)) {
		if (filesystem::is_regular_file(train_p) && train_p.extension() == ".dat") {
			train_name = train_p.stem().string();
			std::ifstream train_f(train_path.c_str(), std::ios_base::in);
			std::string nDocsHeader;
			std::getline(train_f, nDocsHeader);
			int idCount = 0;

			// Scansiona linea a linea
			std::string str;
			while (std::getline(train_f, str)) {
				std::vector<std::string> words;
				std::vector<int> ids;
				algorithm::split(words, str, boost::is_any_of(" "));
				for (unsigned int i = 0; i < words.size(); i++) {
					std::string w = words.at(i);

					//Verifica se la parola è presente nel vocabolario
					vocmap_type::right_const_iterator w_iter = voc.right.find(w);
					if (w_iter != voc.right.end()) {
						// Presente
						int id = w_iter->second;
						ids.push_back(id);
					} else {
						// Non ancora presente: inserisce nel vocabolario
						voc.insert(vocmap_type::value_type(idCount, w));
						ids.push_back(idCount);

						idCount++;
					}
				}

				train.push_back(ids);

				n_words_train += words.size();
				n_docs_train++;
			}

			n_terms_train = idCount;
			train_f.close();
		} else {
			throw std::logic_error(std::string("ERROR: training path \"") + train_path + std::string("\" is not valid"));
		}
	} else {
		throw std::logic_error(std::string("ERROR: training path \"") + train_path + std::string("\" not exists"));
	}
}
;

void data::exportVocabulary(std::string voc_path) {
	filesystem::path voc_p(voc_path);
	std::ofstream voc_f(voc_path.c_str(), std::ios_base::out);

	vocmap_type::right_const_iterator i;
	vocmap_type::right_const_iterator i_end;
	for (i = voc.right.begin(), i_end = voc.right.end(); i != i_end; ++i) {
		voc_f << i->first << " " << i->second << "\n";
	}

	voc_f.close();
}
;

void data::importValidation(std::string validation_path) {
	this->validation_path = validation_path;
	std::ifstream validation_f(validation_path.c_str(), std::ios_base::in);

	int idCount = voc.size();

	vocmap_type validation_voc;
	int validationT = 0;

	std::string nDocsHeader;
	std::getline(validation_f, nDocsHeader);

	// Scansiona linea a linea
	std::string str;
	while (std::getline(validation_f, str)) {
		std::vector<std::string> words;
		std::vector<int> ids;
		algorithm::split(words, str, boost::is_any_of(" "));
		for (unsigned int i = 0; i < words.size(); i++) {
			std::string w = words.at(i);

			//Verifica se la parola è presente nel vocabolario
			vocmap_type::right_const_iterator w_iter = voc.right.find(w);
			if (w_iter != voc.right.end()) {
				// Presente
				int id = w_iter->second;
				ids.push_back(id);
			} else {
				// Non ancora presente: inserisce nel vocabolario
				voc.insert(vocmap_type::value_type(idCount, w));
				ids.push_back(idCount);

				idCount++;
			}

			//Verifica se la parola è presente nel vocabolario del validation
			vocmap_type::right_const_iterator valid_w_iter = validation_voc.right.find(w);
			if (valid_w_iter == validation_voc.right.end()) {
				// Non ancora presente: inserisce nel vocabolario validation e aggiorna contatore termini
				validation_voc.insert(vocmap_type::value_type(validationT, w));
				validationT++;
			}
		}

		validation.push_back(ids);

		n_words_validation += words.size();
		n_docs_validation++;
	}

	n_terms_validation = validationT;
	n_terms_total = voc.size();
	validation_f.close();
}

data::~data() {
}
;

