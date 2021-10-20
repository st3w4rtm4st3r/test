#include <string>
#include <vector>

#ifndef TABLE_H
#define TABLE_H

namespace MoSzE {

	class Table {

	private:
		// fields
		static const int COL_SIZE = 26;
		std::vector<std::vector<std::string>> table;
		int current_row;
		int current_col;

		// methods
		bool edit(const std::vector<std::string>& args);
		bool add(const std::vector<std::string>& args);
		bool del(const std::vector<std::string>& args);
		bool insert(const std::vector<std::string>& args);
		bool save(const std::vector<std::string>& args) const;
		bool read(const std::string& path, const std::string& separator);

	public:
		// methods
		Table(int argc, char** argv);
		void print_table() const;
		void parse_command(const std::string& command);
	};

}

#endif	// TABLE_H
