#pragma once
#include <vector>
#include <string>
#include <sstream>

namespace cedit{
	class Table
	{
	public:
		//Return a markdown table with r rows and c columns
		Table(int r, int c, int w);

		std::vector<std::string> markdown();
		int lineCount();
		
		static std::vector<std::string> newTable(int r, int c, int w);
		static std::vector<std::string> newTable(std::string r, 
			std::string c, std::string w);

	private:
		int rows, lines, columns, width;
		std::string headings;
		std::vector<std::string> cells;

		std::vector<std::string> generateCells(int r, int c);
		std::string generateRow(int columns, char c);
	};
}