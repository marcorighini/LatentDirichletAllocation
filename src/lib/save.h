/*
 * save.h
 *
 *      Author: Marco Righini (righini.marco88@gmail.com)
 */

#ifndef SAVE_H_
#define SAVE_H_

#include<vector>

class save {
public:
	save(int step, std::vector<std::vector<double> > phi, std::vector<std::vector<double> > theta, std::vector<std::vector<int> > zeta) :
			step(step), phi(phi), theta(theta), zeta(zeta) {
	}
	;
	virtual ~save();
private:
	int step;
	std::vector<std::vector<double> > phi;
	std::vector<std::vector<double> > theta;
	std::vector<std::vector<int> > zeta;
};

#endif /* SAVE_H_ */
