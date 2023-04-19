/*
 * File:      helpers.h
 * Author:    Daniel Hannon
 * 
 * Brief: This file contains some generic helper functions that
 * don't necessecarily pertain exclusively to this assignment
 */

#include <functional>
#ifndef HELPERS_H_1F2A17346D284E1B91EC1FFF52F46CAA
#define HELPERS_H_1F2A17346D284E1B91EC1FFF52F46CAA 1

#include <string>

namespace danhan {
/**
 * \brief Perform an inplace swap between two substrings
 *
 * This function takes several arguments
 * @param str1    String 1 to perform the swap on
 * @param str2    String 2 to perform the swap on
 * @param start   beginning index of the substring
 * @param length  End index of the string (size_max by default)
 */
inline void substring_swap(std::string & str1, std::string & str2, const size_t start, const size_t length = SIZE_MAX) {
	// Short circuit out of range entries
	if(start >= str1.length() || start >= str2.length()) return;

	auto itr1 = str1.begin() + start;
	auto itr2 = str2.begin() + start;
	size_t length_copied = 0;
	while((itr1 != str1.end()) &&
		  (itr2 != str2.end()) &&
		  (length_copied != length)) {
		// In-place swap without a temp (Maybe investigate std::swap as an alternative)
		*(itr1) ^= *(itr2);
		*(itr2) ^= *(itr1);
		*(itr1) ^= *(itr2);

		length_copied++;
		itr1++;
		itr2++;
	}
}
} // namespace danhan

#endif // HELPERS_H_1F2A17346D284E1B91EC1FFF52F46CAA
	
