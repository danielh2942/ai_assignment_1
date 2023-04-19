/*
 * File:      common.h
 * Author:    Daniel Hannon
 * 
 * Copyright: 2023 Daniel Hannon 
 */

#include <map>
#include <string>
#include <vector>
#include <random>


#ifndef COMMON_H_4FD8A9AF3C7A4509AAB5D9C0E1BA445D
#define COMMON_H_4FD8A9AF3C7A4509AAB5D9C0E1BA445D 1

// Tournament to select best parents, each is pitted against 3 others
template<typename T>
inline T tournament(std::vector<size_t> const & fitnesses, 
					std::map<size_t,std::vector<T>> const & mapValues, 
					std::mt19937_64 & rd) {

	size_t best_value = fitnesses[rd() % fitnesses.size()];
	for(size_t i = 0; i < 3; i++) {
		size_t temp = fitnesses[rd() % fitnesses.size()];
		if(temp > best_value) best_value = temp;
	}
	auto selection = mapValues.at(best_value);
	return selection[rd() % selection.size()];
}

#endif // COMMON_H_4FD8A9AF3C7A4509AAB5D9C0E1BA445D
	
