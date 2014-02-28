/*
 * model.cpp
 *
 *      Author: Marco Righini (righini.marco88@gmail.com)
 */

#include "model.h"
#include<boost/filesystem.hpp>
#include<iostream>
#include<boost/lexical_cast.hpp>
#include <fstream>
#include<boost/tuple/tuple.hpp>
#include <math.h>

// Struct e funzione che restituisce gli indici rispettivi del vettore ordinato
template<class T> struct compare_index {
	const T v;
	compare_index(const T& v) :
			v(v) {
	}
	bool operator ()(int a, int b) const {
		return (v[a] > v[b]);
	}
};
std::vector<int> sort_indexes(const std::vector<double> &v) {
	std::vector<int> idx(v.size());
	for (unsigned int i = 0; i != idx.size(); ++i)
		idx[i] = i;

	std::sort(idx.begin(), idx.end(), compare_index<std::vector<double> >(v));

	return idx;
}

model::model(const data& d, std::string output_path, int topics, double alpha, double beta, int iters, int burnin, int savestep, int topwords, bool perpl, bool export_complete) {
	this->d = d;
	this->output_path = output_path;
	this->topics = topics;
	this->alpha = alpha;
	this->beta = beta;
	this->iters = iters;
	this->burnin = burnin;
	this->savestep = savestep;
	this->topwords = topwords;
	this->perpl = perpl;
	this->export_complete = export_complete;

	initZeta(d.getTrain(), d.getNTermsTrain(), n_d_t, n_t_w, n_t, z,false);
}

void model::initZeta(const std::vector<std::vector<int> >& docs, int nTerms, std::vector<std::vector<int> >& n_d_t, std::vector<std::vector<int> >& n_t_w, std::vector<int>& n_t,
		std::vector<std::vector<int> >& z, bool validation) {
	n_d_t = std::vector<std::vector<int> >(docs.size(), std::vector<int>(topics, 0));
	n_t_w = std::vector<std::vector<int> >(topics, std::vector<int>(nTerms, 0));
	n_t = std::vector<int>(topics, 0);

	int nDocs = docs.size();
	for (int doc = 0; doc < nDocs; doc++) {
		std::vector<int> doc_t;
		for (unsigned int j = 0; j < docs[doc].size(); j++) {
			int w = docs[doc][j];
			int t = (int) (((double) rand() / (double(RAND_MAX) + 1)) * topics);
			doc_t.push_back(t);
			n_d_t[doc][t]++;
			if (!validation) {
				n_t_w[t][w]++;
				n_t[t]++;
			}
		}
		z.push_back(doc_t);
	}
}

void model::learn() {
	collapsedGibbsSample(d.getTrain(), d.getNTermsTrain(), n_d_t, n_t_w, n_t, z, iters, burnin, savestep, perpl, std::vector<std::vector<int> >(), std::vector<int>());
}

void model::collapsedGibbsSample(const std::vector<std::vector<int> >& docs, int nTerms, std::vector<std::vector<int> >& n_d_t, std::vector<std::vector<int> >& n_t_w, std::vector<int>& n_t,
		std::vector<std::vector<int> >& z, int iters, int burnin, int savestep, bool perpl, const std::vector<std::vector<int> >& prior_n_t_w, const std::vector<int>& prior_n_t) {

	bool validation = false;
	if (!prior_n_t_w.empty() && !prior_n_t.empty()) {
		validation = true;
	}

	if (validation) {
		// Nel caso di sampling sul validation utilizzo i contatori precedenti sul train
		int trainTerms = prior_n_t_w[0].size();
		for (int t = 0; t < topics; t++) {
			for (int w = 0; w < trainTerms; w++) {
				n_t_w[t][w] = prior_n_t_w[t][w];
			}
		}
		for (int t = 0; t < topics; t++) {
			n_t[t] = prior_n_t[t];
		}
	}

	std::clock_t startTime;
	std::clock_t endTime;

	double Wbeta = beta * nTerms;

	for (int i = 0; i < iters; i++) {
		if (!validation) {
			std::cout << ">> iterazione #" << (i + 1);
		} else {
			std::cout << ">>> perplexity: iterazione #" << (i + 1);
		}
		startTime = clock();
		for (unsigned int doc = 0; doc < docs.size(); doc++) {
			for (unsigned int j = 0; j < docs[doc].size(); j++) {
				int w = docs[doc][j];
				int t = z[doc][j];

				// Decrementa contatori
				n_d_t[doc][t]--;
				if (!validation) {
					n_t_w[t][w]--;
					n_t[t]--;
				}

				// Calcola la distribuzione condizionale dei topic per w (cumulativa)
				std::vector<double> p(topics, 0);
				for (int k = 0; k < topics; k++) {
					p[k] = (n_d_t[doc][k] + alpha) * (n_t_w[k][w] + beta) / (n_t[k] + Wbeta);
				}
				for (int k = 1; k < topics; k++) {
					p[k] += p[k - 1];
				}
				// Campiona
				double s = ((double) rand() / ((double) (RAND_MAX) + 1)) * p[topics - 1];
				for (t = 0; t < topics; t++) {
					if (p[t] > s) {
						break;
					}
				}

				// Aggiorna contatori
				z[doc][j] = t;
				n_d_t[doc][t]++;
				if (!validation) {
					n_t_w[t][w]++;
					n_t[t]++;
				}
			}
		}
		endTime = clock();
		double timeInSeconds = (endTime - startTime) / (double) CLOCKS_PER_SEC;
		std::cout << "\t" << timeInSeconds << " s\n";

		if (perpl) {
			if ((i + 1) % (iters / 10) == 0) {
				std::cout << ">>> Computazione perplexity...\n";
				computeAndExportPerplexity(i + 1);
			}
			if (i + 1 == iters) {
				exportModel(i + 1);
			}
		} else {
			// Se savestep, esporta modello
			if (i - burnin + 1 >= 0 && (i - burnin + 1) % savestep == 0) {
				exportModel(i + 1);
			}
		}
	}

//	if (validation) {
//		// Infine, nel caso di sampling sul validation, mi decremento i vettori contatori con quelli precedenti sul train
//		int trainTerms = prior_n_t_w[0].size();
//		for (int t = 0; t < topics; t++) {
//			for (int w = 0; w < trainTerms; w++) {
//				n_t_w[t][w] -= prior_n_t_w[t][w];
//			}
//		}
//		for (int t = 0; t < topics; t++) {
//			n_t[t] -= prior_n_t[t];
//		}
//	}
}
;

void model::exportModel(int iteration) {
	std::cout << ">> Esportazione modello...\n";

	// Computa theta
	std::vector<std::vector<double> > theta;
	if (export_complete) {
		theta = computeTheta(n_d_t);
	}

	// Computa phi
	std::vector<std::vector<double> > phi = computePhi(n_t_w);

	// Computa topwords
	std::vector<std::vector<boost::tuple<std::string, double> > > tw(topics, std::vector<boost::tuple<std::string, double> >(topwords, boost::tuple<std::string, double>()));
	for (int t = 0; t < topics; t++) {
		std::vector<int> sorted_words_ids = sort_indexes(phi[t]);
		for (int i = 0; i < topwords; i++) {
			tw[t][i].get<0>() = d.getVoc().left.at(sorted_words_ids[i]);
			tw[t][i].get<1>() = phi[t][sorted_words_ids[i]];
		}
	}

	// Salva su vettore
	save save(iteration, phi, theta, z);
	saves.push_back(save);

	//Esporta su file phi, theta, z, topwords
	boost::filesystem::path o_p(output_path);

	if (export_complete) {
		// Esporta theta
		boost::filesystem::path theta_p(o_p);
		theta_p /= d.getTrainName() + "_" + boost::lexical_cast<std::string>(iteration) + "_theta.dat";
		std::ofstream theta_f(theta_p.c_str(), std::ios_base::out);
		for (int doc = 0; doc < d.getNDocsTrain(); doc++) {
			for (int t = 0; t < topics; t++) {
				if (t != 0) {
					theta_f << " ";
				}
				theta_f << theta[doc][t];
			}
			if (doc != d.getNDocsTrain() - 1) {
				theta_f << "\n";
			}
		}
		theta_f.close();

		// Esporta phi
		boost::filesystem::path phi_p(o_p);
		phi_p /= d.getTrainName() + "_" + boost::lexical_cast<std::string>(iteration) + "_phi.dat";
		std::ofstream phi_f(phi_p.c_str(), std::ios_base::out);
		for (int t = 0; t < topics; t++) {
			for (int w = 0; w < d.getNTermsTrain(); w++) {
				if (w != 0) {
					phi_f << " ";
				}
				phi_f << phi[t][w];
			}
			if (t != topics - 1) {
				phi_f << "\n";
			}
		}
		phi_f.close();

		// Esporta zeta
		boost::filesystem::path z_p(o_p);
		z_p /= d.getTrainName() + "_" + boost::lexical_cast<std::string>(iteration) + "_z.dat";
		std::ofstream z_f(z_p.c_str(), std::ios_base::out);
		for (int doc = 0; doc < d.getNDocsTrain(); doc++) {
			for (unsigned int w = 0; w < z[doc].size(); w++) {
				if (w != 0) {
					z_f << " ";
				}
				z_f << z[doc][w];
			}
			if (doc != d.getNDocsTrain() - 1) {
				z_f << "\n";
			}
		}
		z_f.close();
	}

	boost::filesystem::path tw_p(o_p);
	tw_p /= d.getTrainName() + "_" + boost::lexical_cast<std::string>(iteration) + "_topwords.dat";
	std::ofstream tw_f(tw_p.c_str(), std::ios_base::out);
	for (int t = 0; t < topics; t++) {
		tw_f << "Topic " << (t + 1);
		for (int w = 0; w < topwords; w++) {
			tw_f << "\n\t" << tw[t][w].get<0>() << "\t" << tw[t][w].get<1>();
		}
		if (t != topics - 1) {
			tw_f << "\n";
		}
	}
	tw_f.close();
}

std::vector<std::vector<double> > model::computeTheta(const std::vector<std::vector<int> >& n_d_t) {
	int nDocs = n_d_t.size();

	std::vector<std::vector<double> > theta(nDocs, std::vector<double>(topics, 0));

	for (int doc = 0; doc < nDocs; doc++) {
		double sum = 0;
		for (int t = 0; t < topics; t++) {
			theta[doc][t] = n_d_t[doc][t] + alpha;
			sum += theta[doc][t];
		}
		for (int t = 0; t < topics; t++) {
			theta[doc][t] = theta[doc][t] / sum;
		}
	}

	return theta;
}

std::vector<std::vector<double> > model::computePhi(const std::vector<std::vector<int> >& n_t_w) {
	int nTerms = n_t_w[0].size();

	std::vector<std::vector<double> > phi(topics, std::vector<double>(nTerms, 0));

	for (int t = 0; t < topics; t++) {
		double sum = 0;
		for (int w = 0; w < nTerms; w++) {
			phi[t][w] = n_t_w[t][w] + beta;
			sum += phi[t][w];
		}
		for (int w = 0; w < d.getNTermsTrain(); w++) {
			phi[t][w] = phi[t][w] / sum;
		}
	}

	return phi;
}

void model::computeAndExportPerplexity(int iteration) {
	// Inizializza zeta e contatori per gibbs sampling sul validation set
	std::vector<std::vector<int> > test_n_d_t;
	std::vector<std::vector<int> > test_n_t_w;
	std::vector<int> test_n_t;
	std::vector<std::vector<int> > test_z;

	initZeta(d.getValidation(), d.getNTermsTotal(), test_n_d_t, test_n_t_w, test_n_t, test_z,true);

	// Gibbs sampling sul validation (utilizzando i contatori sul train come prior)
	collapsedGibbsSample(d.getValidation(), d.getNTermsTotal(), test_n_d_t, test_n_t_w, test_n_t, test_z, iters / 5, 9999, 99999, false, n_t_w, n_t);

	// Computa theta e riarrangia phi del train
	std::vector<std::vector<double> > theta = computeTheta(test_n_d_t);
	std::vector<std::vector<double> > phi = computePhi(test_n_t_w);
//	std::vector<std::vector<double> > phi = computeTestPhi(n_t_w, d.getNTermsTotal());

// Computa perplexity
	double pred_l = 0;
	int testDocs = d.getValidation().size();
	for (int doc = 0; doc < testDocs; doc++) {
		std::vector<int> docWords = d.getValidation()[doc];
		int nWordDoc = docWords.size();
		for (int j = 0; j < nWordDoc; j++) {
			int w = docWords[j];
			double word_l = 0;
			for (int k = 0; k < topics; k++) {
				word_l += phi[k][w] * theta[doc][k];
			}
			pred_l += log(word_l);
		}
	}
	double perplexity = exp(-(pred_l) / d.getNWordsValidation());

	// Esporta perplexity
	boost::filesystem::path o_p(output_path);
	boost::filesystem::path p_p(o_p);
	p_p /= d.getTrainName() + "_perpl.dat";
	std::ofstream p_f(p_p.c_str(), std::ios_base::app);
	p_f << boost::lexical_cast<std::string>(iteration) << "\t" << boost::lexical_cast<std::string>(perplexity) << "\n";
	p_f.close();
}

//std::vector<std::vector<double> > model::computeTestPhi(const std::vector<std::vector<int> >& n_t_w, int nTotalTerm) {
//	std::vector<std::vector<double> > phi(topics, std::vector<double>(nTotalTerm, 0));
//
//	int nOldTerms = n_t_w[0].size();
//	for (int t = 0; t < topics; t++) {
//		double sum = 0;
//		for (int w = 0; w < nTotalTerm; w++) {
//			if (w < nOldTerms) {
//				phi[t][w] = n_t_w[t][w] + beta;
//			} else {
//				phi[t][w] = beta;
//			}
//			sum += phi[t][w];
//		}
//		for (int w = 0; w < nTotalTerm; w++) {
//			phi[t][w] = phi[t][w] / sum;
//		}
//	}
//
//	return phi;
//}

void model::exportParams() {
	boost::filesystem::path o_p(output_path);
	o_p /= d.getTrainName() + "_params.data";
	std::ofstream o_f(o_p.c_str(), std::ios_base::out);

	o_f << "train=" << d.getTrainPath() << "\n";
	o_f << "validation=" << d.getValidationPath() << "\n";
	o_f << "output: " << output_path << "\n";
	o_f << "topics: " << boost::lexical_cast<std::string>(topics) << "\n";
	o_f << "alpha: " << boost::lexical_cast<std::string>(alpha) << "\n";
	o_f << "beta: " << boost::lexical_cast<std::string>(beta) << "\n";
	o_f << "iters: " << boost::lexical_cast<std::string>(iters) << "\n";
	o_f << "burnin: " << boost::lexical_cast<std::string>(burnin) << "\n";
	o_f << "savestep: " << boost::lexical_cast<std::string>(savestep) << "\n";
	o_f << "topwords: " << boost::lexical_cast<std::string>(topwords) << "\n";
	o_f << "perplexity: " << boost::lexical_cast<std::string>(perpl) << "\n";
	o_f << "export-complete: " << boost::lexical_cast<std::string>(export_complete);

	o_f.close();
}

model::~model() {
}

