#include "common.h"
#include "helpers.h"

#include <set>
#include <map>
#include <string>
#include <random>
#include <vector>
#include <utility>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <functional>

// Generate a random number using the mersenne twister algorithm
std::mt19937_64 randNum{std::random_device{}()};

// Perform a 1 point crossover
std::pair<std::string, std::string> one_point_xover(std::string & str1, std::string & str2) {
	std::pair<std::string,std::string> output{};
	output.first = {str1};
	output.second = {str2};
	size_t val = randNum() % (str1.length() -1);
	val = std::max<size_t>(1,val);
	danhan::substring_swap(output.first,output.second,val);
	return output;
}

// generic operations for the genetic modeling, it takes two FPs as args as the same operations need to be performed
std::vector<size_t> genetic_modeling(std::string &                                                  goal, 
								     std::function<void(std::string &)>                             mutateFn,
								     std::function<size_t(std::string const &,std::string const &)> fitnessFn) {
	std::vector<size_t> scores{};
	
	std::vector<std::string> mTemp{};
	std::vector<std::string> mVec{};
	for(size_t i = 0; i < 100; i++) {
		std::string temp(goal.length(),'0');
		std::size_t mutations = randNum() % 99;
		for(size_t i = 0; i < mutations; i++) {
			mutateFn(temp);
		}
		mVec.push_back(temp);
	}

	for(size_t generation = 0; generation < 100; generation++) {
		size_t                                     avgFitness = 0;
		std::map<size_t, std::vector<std::string>> wMap{};
		std::set<size_t>						   wSetFitnesses{};
		std::for_each(mVec.begin(),mVec.end(), [&](auto & v){
				size_t fitness = fitnessFn(v,goal);
				wMap[fitness].push_back(v);
				avgFitness += fitness;
				wSetFitnesses.insert(fitness);
		});
		avgFitness /= mVec.size();
		scores.push_back(avgFitness);
		std::cout << "Generation: " << generation << " Average Fitness: " << avgFitness << " Size: " << mVec.size() << '\n';
		std::cout << "Unique Fitnesses: " << wMap.size() << '\n';

		std::vector<size_t> fitnessVector{wSetFitnesses.begin(),wSetFitnesses.end()};
		for(size_t i = 1; i < wMap.size(); i++) {
			std::cout << fitnessVector[i] << ' ';
			for(size_t j = 0; j < (10 * i); j++) {
				fitnessVector.push_back(fitnessVector[i]);
			}
		}
		std::cout << '\n';

		mTemp.clear();
		for(size_t i = 0; i < 100; i++) {
			// Select n many random values
			mTemp.push_back(tournament<std::string>(fitnessVector,wMap,randNum));
		}

		for(size_t i = 0; i < 98; i+=2) {
			auto v = one_point_xover(mTemp[i],mTemp[i+1]);
			mTemp[i] = v.first;
			mTemp[i+1] = v.second;
		}

		size_t numMutations = 1000;
		for(size_t i = 0; i < numMutations; i++) {
			if(randNum() % 7 == 1) {
				mutateFn(mTemp[randNum() % mTemp.size()]);
			}
		}
		mVec.swap(mTemp);
	}

	std::map<size_t,size_t> mMap{};
	for(auto & i : mVec) {
		mMap[fitnessFn(i,goal)]++;
	}

	for(auto & i : mMap) {
		std::cout << i.first << ": " << i.second << "\n";
	}
	return scores;
}

void part_a_mutate_string(std::string & str) {
	str[randNum() % str.length()] ^= 1;
}

size_t part_a_get_fitness(std::string const & str, [[maybe_unused]] std::string const & tgt) {
	return std::count(str.begin(),str.end(),'1');
}

size_t part_b_get_fitness(std::string const & str, std::string const & tgt) {
	size_t fitness = 0;
	for(size_t i = 0; i < str.length(); i++) {
		fitness += (str[i] == tgt[i]);
	}
	return fitness;
}

size_t part_c_get_fitness(std::string const & str, std::string const & tgt) {
	size_t temp = part_a_get_fitness(str,tgt);
	if (temp == 0) {
		return 2 * tgt.length();
	} else {
		return part_b_get_fitness(str,tgt);
	}
}

void part_d_mutate_string(std::string & str) {
	bool pos = (randNum() % 2 == 0);
	size_t idx = randNum() % str.length();
	if(pos) {
		str[idx]++;
	} else {
		str[idx]--;
	}

	if(str[idx] < '0') str[idx] = '9';
	if(str[idx] > '9') str[idx] = '0';
}

size_t part_d_get_fitness(std::string const & str, std::string const & tgt) {
	size_t max       = tgt.length() * 5;
	size_t deduction = 0;

	for(size_t i = 0; i < str.length(); i++) {
		char test = str[i] & 0x0F;
		char tar = tgt[i] & 0x0F;
		// This should give a value from 0 - 5
		deduction += std::min(std::abs((test + tar) % 10),
							  std::abs((tar - test) % 10));
	}

	return max - deduction;
}

int main() {
	std::string goalA(30,'1');
	std::cout << "Part A:\n";
	{
		auto output = genetic_modeling(goalA, part_a_mutate_string, part_a_get_fitness);
		std::ofstream out("partA.txt");
		for(const auto & v : output) {
			out << v << ',';
		}
		out.close();
	}
	std::cout << std::endl;

	std::cout << "Generating Target String for Part 2\n";
	std::string goalB(30,'0');
	size_t mutations = randNum() % 99;
	for(size_t i = 0; i < mutations; i++) {
		part_a_mutate_string(goalB);
	}
	std::cout << "Target String: " << goalB << std::endl;
	{
		auto output = genetic_modeling(goalB, part_a_mutate_string, part_b_get_fitness);
		std::ofstream out("partB.txt");
		for(const auto & v : output) {
			out << v << ',';
		}
		out.close();
	}

	std::string goalC(30,'0');
	mutations = randNum() % 99;
	for(size_t i = 0; i < mutations; i++) {
		part_a_mutate_string(goalC);
	}
	std::cout << std::endl;
	std::cout << "Part C:\n";
	{
		auto output = genetic_modeling(goalC, part_a_mutate_string, part_c_get_fitness);
		std::ofstream out("partC.txt");
		for(const auto & v: output) {
			out << v << ',';
		}
		out.close();
	}

	std::string goalD(30,'0');
	for(int i = 0; i < 1000; i++) {
		part_d_mutate_string(goalD);
	}
	std::cout << "\n\nPart D:\nTarget String: " << goalD << "\nFitness: "<< (goalD.length() * 5) << "\n";
	{
		auto output = genetic_modeling(goalD,part_d_mutate_string,part_d_get_fitness);
		std::ofstream out("partD.txt");
		for(const auto & v : output) {
			out << v << ',';
		}
		out.close();
	}
	return 0;
}
