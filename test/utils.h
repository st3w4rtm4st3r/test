#include <string>
#include <vector>

#ifndef UTILS_H
#define UTILS_H

namespace MoSzE {
	
	/* Convert column's alpha id to numerical index. */
	int atoi(char ch);

	/* Split string using a delimiter. */
	std::vector<std::string> split(std::string str, const std::string& delimiter);

	/* Join strings with a delimiter. */
	std::string join(const std::string& delimiter, const std::vector<std::string>& strings);

	/* Display error message on the console. */
	void error(const std::string& msg);

	template<typename T>
	void error(const std::string& msg, const T& value) {
		// need to implement it here because of implicit calls
		std::cerr << msg << " " << value << std::endl << std::endl;
	};

}

#endif	// UTILS_H
