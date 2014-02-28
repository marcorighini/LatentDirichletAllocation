/*
 * model.h
 *
 *      Author: Marco Righini (righini.marco88@gmail.com)
 */

#ifndef MODEL_H_
#define MODEL_H_

#include"data.h"
#include"save.h"
#include<vector>

class model {
public:
	model(const data& d, std::string output_path, int topics, double alpha, double beta, int iters, int burnin, int savestep, int topwords, bool perpl, bool export_complete);
	void learn();
	void collapsedGibbsSample(const std::vector<std::vector<int> >& docs, int nTerms, std::vector<std::vector<int> >& n_d_t, std::vector<std::vector<int> >& n_t_w, std::vector<int>& n_t,
			std::vector<std::vector<int> >& z, int iters, int burnin, int savestep, bool perpl, const std::vector<std::vector<int> >& prior_n_t_w, const std::vector<int>& prior_n_t);
	void exportParams();
	virtual ~model();
private:
	data d;
	std::string output_path;
	int topics;
	double alpha;
	double beta;
	int iters;
	int burnin;
	int savestep;
	int topwords;
	bool perpl;
	bool export_complete;
	std::vector<std::vector<int> > n_d_t;
	std::vector<std::vector<int> > n_t_w;
	std::vector<int> n_t;
	std::vector<std::vector<int> > z;
	std::vector<save> saves;
	void initZeta(const std::vector<std::vector<int> >& docs, int nTerms, std::vector<std::vector<int> >& n_d_t, std::vector<std::vector<int> >& n_t_w, std::vector<int>& n_t,
			std::vector<std::vector<int> >& z,bool validation);
	void exportModel(int iteration);
	std::vector<std::vector<double> > computeTheta(const std::vector<std::vector<int> >& n_d_t);
	std::vector<std::vector<double> > computePhi(const std::vector<std::vector<int> >& n_t_w);
	void computeAndExportPerplexity(int iteration);
//	std::vector<std::vector<double> > computeTestPhi(const std::vector<std::vector<int> >& n_t_w, int nTotalTerm);
};

#endif /* MODEL_H_ */
