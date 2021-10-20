#include <iostream>
#include <cctype>
#include "utils.h"

int MoSzE::atoi(char ch) {
	if(std::isalpha(ch)) 
		return std::isupper(ch) ? (ch-64) : (ch-96);

	return -1;
}

std::vector<std::string> MoSzE::split(std::string str, const std::string& delimiter) {
	std::vector<std::string> results;
	int position = str.find(delimiter);

	while(position != std::string::npos) {
		std::string substr = str.substr(0, position);
		results.push_back(substr);
		str.erase(0, position + delimiter.length());
		position = str.find(delimiter);
	}

	// last substring
	if(str != "")
		results.push_back(str);
	
	return results;
}

std::string MoSzE::join(const std::string& delimiter, const std::vector<std::string>& strings) {
	std::string result;
	
	for(int i = 0; i < strings.size(); i++) {
		result.append(strings[i]);
		
		if(i != strings.size() - 1)
			result.append(delimiter);
	}

	return result;
}

void MoSzE::error(const std::string& msg) {
	MoSzE::error(msg, "");
}
