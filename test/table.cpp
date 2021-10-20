#include <iostream>
#include <cctype>
#include <fstream>
#include <numeric>
#include "table.h"
#include "utils.h"

MoSzE::Table::Table(int argc, char** argv) : current_row(0), current_col(1) {
	// init table with 1 empty cell - add 1 rows (increases current_row count !!!)
	this->add({"1", "rows"});

	// handle input file (if given) -> ./exe "file" -sep ch
	if(argc > 1) {
		std::string separator;

		switch(argc) {
			case 2: separator = ";"; break;
			case 4: separator = argv[3]; break;
			default: separator = "";
		}

		if(separator == "" || !this->read(argv[1], separator)) {
			// failed to load table
			MoSzE::error("Invalid arguments");
			std::exit(1);
		}
	}

	this->print_table();
}

void MoSzE::Table::print_table() const {
	std::vector<int> cell_widths(this->current_col + 1);
	
	// row id column
	cell_widths[0] = std::to_string(this->table.size() - 1).length();

	int index = 1;
	for(int col = 0; col < this->current_col; col++) {
		// find max cell width for each column
		int max_width = 1; // includes column alpha index
		for(int row = 0; row < this->current_row; row++) {
			if(max_width < this->table[row][col].length())
				max_width = this->table[row][col].length();
		}
		cell_widths[index++] = max_width;
	}

	// find row break length
	int n = std::accumulate(cell_widths.begin(), cell_widths.end(), 1) + this->current_col;	// includes "|" cell separator
	const std::string row_break(n, '-');
	
	int length = 0;
	std::string padding;

	// add padding to the top left corner of the table
	length = cell_widths[0];
	padding = std::string(length, ' ');
	std::cout << padding << '|';

	for(int i = 0; i < this->current_col; i++) {
		// display column letters with padding
		char col_index = 'A' + i;
		length = cell_widths[i+1] - 1;
		padding = std::string(length, ' ');
		std::cout << col_index << padding << '|';
	}
	std::cout << std::endl << row_break << std::endl;

	// display table
	for(int row = 0; row < this->current_row; row++) {
		// display row index with padding
		std::string row_index = std::to_string(row + 1);
		length = cell_widths[0] - row_index.length();
		padding = (length > 0) ? std::string(length, ' ') : "";
		std::cout << row_index << padding << '|';

		for(int col = 0; col < this->current_col; col++) {
			// display cell content with padding
			length = cell_widths[col+1] - this->table[row][col].length();
			padding = std::string(length, ' ');
			std::cout << this->table[row][col] << padding << '|';
		}

		std::cout << std::endl << row_break << std::endl;
	}
}

void MoSzE::Table::parse_command(const std::string& command) {
	bool print = false;
	std::vector<std::string> args = MoSzE::split(command, " ");
	
	// pop first element
	std::string cmd = args[0];
	args.erase(args.begin());

	if(cmd == "edit") 
		print = this->edit(args);
	else if(cmd == "add") 
		print = this->add(args);
	else if(cmd == "delete") 
		print = this->del(args);
	else if(cmd == "insert") 
		print = this->insert(args);
	else if(cmd == "save")
		print = this->save(args);
	else if(cmd == "exit") 
		return;
	else 
		MoSzE::error("Unknown command:", cmd);

	if(print) 
		this->print_table();
}

bool MoSzE::Table::edit(const std::vector<std::string>& args) {
	// check if args contain the cell id and at least 1 (sub)string
	if(args.size() < 2) {
		MoSzE::error("Invalid arguments.");
		return false;
	}

	int col_index = MoSzE::atoi(args[0][0]) - 1;	// alpha index to int conversion
	int row_index = (args[0][1] - '0') - 1;  // char to int conversion

	// check if cell exists
	if(args[0].size() != 2 
		|| (!std::isdigit(args[0][1]) || row_index >= this->current_row) 
		|| (col_index == -1 || col_index >= this->current_col)) {
		
		MoSzE::error("Invalid cell:", args[0]);
		return false;
	}

	std::string value;

	if(args.size() > 2) {
		// concat string
		std::vector<std::string> strings(args.begin() + 1, args.end());
		value = MoSzE::join(" ", strings);
	}
	else {
		value = args[1];
	}

	this->table[row_index][col_index] = value;
	return true;
}

bool MoSzE::Table::add(const std::vector<std::string>& args) {	
	if(args.size() != 2) {
		MoSzE::error("Invalid arguments");
		return false;
	}
	
	// check if N is valid
	if(!std::isdigit(args[0][0])) {
		MoSzE::error("Invalid argument:", args[0]);
		return false;
	}

	// check if row/column string is valid
	if(args[1] != "rows" && args[1] != "cols") {
		MoSzE::error("Invalid argument:", args[1]);
		return false;
	}

	int n = std::stoi(args[0]);

	if(args[1] == "rows") {
		// add row
		this->current_row += n;
		for(int i = 0; i < n; i++) {
			std::vector<std::string> row(Table::COL_SIZE, "");
			this->table.push_back(row);
		}
	}
	else {
		// add column
		if(this->current_col + n > Table::COL_SIZE) {
			MoSzE::error("Couldn't add columns (max 26)");
			return false;
		}
		
		this->current_col += n;

		/*
		* Here is enough to increase the current column counter
		* because the row vectors are initialized with the size of 26 (cols).
		*/
	}

	return true;
}

bool MoSzE::Table::del(const std::vector<std::string>& args) {
	if(args.size() != 1) {
		MoSzE::error("Invalid arguments");
		return false;
	}

	if(std::isdigit(args[0][0])) {
		int index = std::stoi(args[0]) - 1;
		
		// check if can remove row
		if(index >= this->current_row || current_row == 1) {
			MoSzE::error("Couldn\'t delete row:", args[0]);
			return false;
		}

		// remove row
		this->table.erase(this->table.begin() + index);
		this->current_row--;
	}
	else if(std::isalpha(args[0][0])) {
		int index = MoSzE::atoi(args[0][0]) - 1;

		// check if can remove column
		if(index >= this->current_col || this->current_col == 1) {
			MoSzE::error("Couldn\'t delete column:", args[0]);
			return false;
		}
		
		// remove column
		for(int row = 0; row < this->current_row; row++) {
			for(int col = index; col < this->current_col; col++) {
				if(col == this->current_col - 1) {
					// label last cells as empty
					this->table[row][col] = "";
				}
				else {
					// move cells backwards by one
					this->table[row][col] = this->table[row][col+1];
				}
			}
		}
		this->current_col--;
	}
	else {
		MoSzE::error("Invalid argument:", args[0]);
		return false;
	}

	return true;
}

bool MoSzE::Table::insert(const std::vector<std::string>& args) {
	if(args.size() != 4) {
		MoSzE::error("Invalid arguments");
		return false;
	}

	// check if N is valid
	if(!std::isdigit(args[0][0])) {
		MoSzE::error("Invalid argument:", args[0]);
		return false;
	}

	// check if row/column string is valid
	if(args[1] != "rows" && args[1] != "cols") {
		MoSzE::error("Invalid argument:", args[1]);
		return false;
	}

	// check if before/after string is valid
	if(args[2] != "before" && args[2] != "after") {
		MoSzE::error("Invalid argument:", args[2]);
		return false;
	}

	if(std::isdigit(args[3][0])) {
		int index = std::stoi(args[3]) - 1;

		// check if row exists
		if(index < 0 || index >= this->current_row) {
			MoSzE::error("Invalid row:", index + 1);
			return false;
		}
		
		// insert row
		this->add({"1", "rows"});	// add 1 rows (increases current_row count !!!)
		int row_index = (args[2] == "before") ? (index - 1) : (index + 1);
		std::vector<std::string> row(Table::COL_SIZE, "");
		this->table.insert(this->table.begin() + row_index, row);
	}
	else if(std::isalpha(args[3][0])) {
		// check if column exists
		int index = MoSzE::atoi(args[3][0]) - 1;
		if(index < 0 || index >= this->current_col) {
			MoSzE::error("Invalid column:", index + 1);
			return false;
		}
		
		// insert column
		int col_index = (args[2] == "before") ? (index - 1) : (index + 1);

		for(int row = 0; row < this->table.size(); row++) {
			for(int col = this->table[row].size() - 2; col >= col_index; col--) {
				// move cells forward by one
				this->table[row][col+1] = this->table[row][col];

				// label starting cells as empty
				if(col == col_index) {
					this->table[row][col] = "";
				}
			}
		}

		this->current_col++;
	}
	else {
		MoSzE::error("Invalid argmuent:", args[3]);
		return false;
	}

	return true;
}

bool MoSzE::Table::save(const std::vector<std::string>& args) const {
	if(args.size() == 1 || args.size() == 3) {
		if(args.size() == 3) {
			// check if flag is valid
			if(args[1] != "-sep") {
				MoSzE::error("Invalid flag:", args[1]);
				return false;
			}

			// check if separator is valid
			if(args[2].length() != 1) {
				MoSzE::error("Use 1 character separators only");
				return false;
			}
		}

		// save to file
		std::string separator = (args.size() == 3) ? args[2] : ";";
		std::ofstream file(args[0]);
		
		for(int row = 0; row < this->current_row; row++) {
			for(int col = 0; col < this->current_col; col++) {
				// write cell content to file
				file << this->table[row][col];

				// add separator
				if(col != this->current_col - 1)
					file << separator;
			}

			file << std::endl;
		}

		file.close();
		return true;
	}
	else {
		MoSzE::error("Invalid arguments");
		return false;
	}
}

bool MoSzE::Table::read(const std::string& path, const std::string& separator) {
	std::ifstream file(path);
	
	if(file.fail()) {
		file.close();
		return false;
	}

	int row = 0;
	char col = 0;
	std::string line;

	while(std::getline(file, line)) {
		std::vector<std::string> strings = MoSzE::split(line, separator);
		
		// insert columns if neccessary
		if(strings.size() > this->current_col) {
			std::string cols = std::to_string(strings.size() - this->current_col);
			this->add({cols, "cols"});
		}

		// copy cell content to table
		col = 0;
		for(int i = 0; i < strings.size(); i++) {
			this->table[row][col++] = strings[i];
		}

		this->add({"1", "rows"});
		row++;
	}

	// remove last (empty) row
	this->del({std::to_string(this->current_row)});

	file.close();
	return true;
}