/*
 * lda.h
 *
 *      Author: Marco Righini (righini.marco88@gmail.com)
 */

#ifndef LDA_H_
#define LDA_H_

#include <boost/program_options.hpp>

using namespace boost;

namespace po = program_options;

// Funzione che lancia un'eccezione se sono state richieste due opzioni in conflitto tra loro
void conflicting_options(const po::variables_map& vm, const char* opt1, const char* opt2){
	if (vm.count(opt1) && !vm[opt1].defaulted() && vm.count(opt2) && !vm[opt2].defaulted())
		throw std::logic_error(std::string("Conflicting options '") + opt1 + "' and '" + opt2 + "'.");
}

// Funzione che lancia un'eccezione se l'opzione voluta è stata richiesta senza quella da cui dipende
void option_dependency(const po::variables_map& vm, const char* for_what, const char* required_option){
	if (vm.count(for_what) && !vm[for_what].defaulted())
		if (vm.count(required_option) == 0 || vm[required_option].defaulted())
			throw std::logic_error(std::string("Option '") + for_what
				+ "' requires option '" + required_option + "'.");
}

// Funzione che stampa l'uso consentito del programma
void usage(const po::options_description& desc){
	std::cout << desc << "\n";
	exit(1);
}

#endif /* LDA_H_ */
