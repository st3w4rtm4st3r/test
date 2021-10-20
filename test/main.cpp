#include <iostream>
#include <string>
#include "table.h"

int main(int argc, char** argv) {
	MoSzE::Table table(argc, argv);
	std::string command;

	while(command != "exit") {
		std::cout << "> ";
		std::getline(std::cin, command);
		table.parse_command(command);
	}
	
	return 0;
}
