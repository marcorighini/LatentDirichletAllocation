/*
 * data.h
 *
 *      Author: Marco Righini (righini.marco88@gmail.com)
 */

#ifndef DATA_H_
#define DATA_H_

#include <vector>
#include <boost/bimap.hpp>

typedef boost::bimap<int, std::string> vocmap_type;

class data {
public:
	data() :
			n_docs_train(0), n_terms_train(0), n_words_train(0), n_docs_validation(0), n_words_validation(0), n_terms_validation(0),n_terms_total(0) {
	}
	;
	void importTrain(std::string train_path);
	void exportVocabulary(std::string voc_path);
	void importValidation(std::string validation_path);

	virtual ~data();

	int getNDocsTrain() const {
		return n_docs_train;
	}

	int getNTermsTrain() const {
		return n_terms_train;
	}

	int getNWordsTrain() const {
		return n_words_train;
	}

	int getNDocsValidation() const {
		return n_docs_validation;
	}

	int getNTermsValidation() const {
		return n_terms_validation;
	}

	int getNWordsValidation() const {
		return n_words_validation;
	}

	int getNTermsTotal() const {
		return n_terms_total;
	}

	const std::vector<std::vector<int> >& getTrain() const {
		return train;
	}

	const std::string& getTrainName() const {
		return train_name;
	}

	const std::vector<std::vector<int> >& getValidation() const {
		return validation;
	}

	const vocmap_type& getVoc() const {
		return voc;
	}

	const std::string& getTrainPath() const {
		return train_path;
	}

	const std::string& getValidationPath() const {
		return validation_path;
	}

private:
	int n_docs_train;
	int n_terms_train;
	int n_words_train;
	int n_docs_validation;
	int n_words_validation;
	int n_terms_validation;
	int n_terms_total;
	std::string train_path;
	std::string validation_path;
	std::string train_name;
	std::vector<std::vector<int> > train;
	std::vector<std::vector<int> > validation;
	vocmap_type voc;
};

#endif /* DATA_H_ */
