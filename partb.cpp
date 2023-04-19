#include "common.h"

#include <set>
#include <map>
#include <string>
#include <vector>
#include <random>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <algorithm>

std::mt19937_64 rd{std::random_device{}()};

// validate exists to verify that the product of crossover is correct, if there
// are any errors it rectifies them by emplacing a lecturer that is not at 
// capacity in the location of the first student to exceed a lecturers capacity
// and so on until every lecturer is at capacity or below.
void validate(std::vector<int> & data, std::vector<int> const & capacities) {
	std::vector<int> studentCounts(capacities.size());
	for(auto const & v : data) {
		studentCounts[v]++;
	}

	std::vector<size_t> over{};
	std::vector<size_t> under{};
	
	for(size_t i = 0; i < studentCounts.size(); i++) {
		if(studentCounts[i] > capacities[i]) {
			over.push_back(i);
		} else if (studentCounts[i] < capacities[i]) {
			under.push_back(i);
		}
	}

	while((over.size() != 0) && (under.size() != 0)) {
		size_t val1 = over[over.size()-1];
		size_t val2 = under[under.size()-1];
		for(auto & v : data) {
			if(v == val1) {
				v = val2;
				studentCounts[val1]--;
				studentCounts[val2]++;
				if(studentCounts[val1] == capacities[val1]) {
					over.pop_back();
				}
				if(studentCounts[val2] == capacities[val2]) {
					under.pop_back();
				}
				break;
			}
		}
	}
}

void x_over(std::vector<int> & parent1, std::vector<int> & parent2, std::vector<int> const & capacities) {
	auto idx = rd() % parent1.size();

	for(; idx < parent1.size(); idx++) {
		std::swap(parent1[idx],parent2[idx]);
	}
	validate(parent1,capacities);
	validate(parent2,capacities);
}

size_t fitnessFn(const std::vector<int> & version, const std::vector<std::vector<int>> & preference) {
	size_t fitnessValue = preference[0].size() * preference.size();
	// As students are required by definition to have a ranking for every lecturer
	// This cannot ever cause any index issues, as if the sum of all spaces is greater
	// than the number of students this will not create issue as some fake students will be assigned
	// the excess lecturer places.
	for(int i = 0; i < preference.size(); i++) {
		/*for(int j = 0; j < preference[i].size(); j++) {
			if (version[i] == preference[i][j]) {
				fitnessValue -= j;
			}
		}*/
		fitnessValue -= preference[i][version[i]];
	}
	return fitnessValue;
}

int main(int argc, char** argv) {
	if (argc < 3) {
		std::cerr << "Expected form: \"app supervisors.csv students.csv\"\n";
		return 1;
	}
	
	std::vector<int> lecturerCapacities{};

	std::vector<std::vector<int>> studentPreferences{};

	//Try open files	
	std::ifstream supervisorsFile{argv[1]};
	std::ifstream studentsFile{argv[2]};

	if( !supervisorsFile.is_open() || !studentsFile.is_open()) {
		std::cerr << "One of the files entered does not exist!\n";
		return 1;
	}

	std::string buff;

	while(std::getline(supervisorsFile, buff)) {
		size_t idx = 0;
		if((idx = buff.find(',')) != std::string::npos) {
			auto temp = buff.substr(idx+1);
			// std::cout << temp << '\n';
			lecturerCapacities.push_back(std::stoi(temp));
		}
	}
	
	size_t idx = 0;
	std::cout << "Populating student preference vector" << '\n';
	while(std::getline(studentsFile,buff)) {
		studentPreferences.push_back({});
		char * pos = std::strtok(const_cast<char *>(buff.c_str()),",");
		while((pos = std::strtok(nullptr, ",")) != nullptr) {
			auto tmp = std::atoi(pos);
			//std::cout << tmp << ' ';
			studentPreferences[idx].push_back(tmp);
		}
		//std::cout << '\n';
		idx++;
	}
	std::cout << "Generating Base Population \n";
	// Close files
	supervisorsFile.close();
	studentsFile.close();

	//Generate population
	std::vector<std::vector<int>> pop{};
	
	//Next population
	std::vector<std::vector<int>> nextPop{};

	//temp placeholder, by generating it like this it guarantees that all children in the initial population are valid
	std::vector<int> temp{};
	for(int i = 0; i < lecturerCapacities.size(); i++) {
		for(int j = 0; j < lecturerCapacities[i]; j++) {
			temp.push_back(i);
		}
	}

	// generate initial population
	for(int i = 0; i < 100; i++) {
		std::shuffle(temp.begin(),temp.end(),rd);
		pop.push_back({temp.begin(),temp.end()});
	}

	// Get base fitnesses and stuff	
	/*for(const auto & v : pop) {
		for(const auto & i : v) {
			std::cout << i << ' ';
		}
		std::cout << "Fitness Value: " << fitness(v,studentPreferences) << '\n';
	}*/

	std::vector<size_t> output{};

	std::cout << "Perfect world fitness: " << (studentPreferences[0].size() * studentPreferences.size()) << '\n';
	for(int generation = 0; generation < 4000; generation++) {
		size_t avgFitness = 0;

		std::map<size_t, std::vector<std::vector<int>>> wMap{};
		std::set<size_t> wSetFitnesses{};
		std::for_each(pop.begin(),pop.end(), [&](auto & v){
				size_t fitness = fitnessFn(v,studentPreferences);
				wMap[fitness].push_back(v);
				avgFitness += fitness;
				wSetFitnesses.insert(fitness);
		});

		//std::cout << "Generation: " << generation << " Average Fitness: " << (avgFitness/pop.size()) << '\n';
		output.push_back((avgFitness/pop.size()));

		nextPop.clear();
		std::vector<size_t> fitnessVector{wSetFitnesses.begin(),wSetFitnesses.end()};
		for(size_t i = 1; i < wMap.size(); i++) {
			std::cout << fitnessVector[i] << ' ';
			for(size_t j = 0; j < (10 * i); j++) {
				fitnessVector.push_back(fitnessVector[i]);
			}
		}
		std::cout << '\n';
		// tournament
		for(size_t i = 0; i < 100; i++) {
			nextPop.push_back(tournament<std::vector<int>>(fitnessVector,wMap,rd));
		}
		// xover
		for(size_t i = 0; i < nextPop.size(); i+=2) {
			if((rd() % 1000) == 3) {
				x_over(nextPop[i], nextPop[i+1], lecturerCapacities);
			}
		}
		// mutation
		size_t numMutations = 100;
		for(size_t i = 0; i < numMutations; i++) {
			// The mutation function swaps two values to guarantee that the result is always valid
			if(rd() % 7 == 1) {
				size_t val = rd() % nextPop.size();
				size_t idx1 = rd() % nextPop[0].size();
				size_t idx2 = rd() % nextPop[0].size();
				nextPop[val][idx1] ^= nextPop[val][idx2];
				nextPop[val][idx2] ^= nextPop[val][idx1];
				nextPop[val][idx1] ^= nextPop[val][idx2];
			}
		}
		
		pop.swap(nextPop);
	}

	std::ofstream out("part2.txt");
	for(const auto & v : output) {
		out << v << ',';
	}
	out.close();

	return 0;
}
