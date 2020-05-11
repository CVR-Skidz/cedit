#include "table.h"

using namespace cedit;
using namespace std::literals::string_literals;

Table::Table(int r, int c, int w) {
	rows = r;
	columns = c;
	width = w;

	//rows + seperator + heading row
	this->lines = r + 2;

	headings = generateRow(c, ' ');
	cells = generateCells(r, c);
}

//return table markdown
std::vector<std::string> Table::markdown() {
	std::vector<std::string> out;

	out.push_back(headings);
	out.push_back(generateRow(columns, '-'));
	for (auto i : cells) out.push_back(i);

	return out;
}

int Table::lineCount() {
	return lines;
}

std::vector<std::string> Table::newTable(int r, int c, int w) {
	Table table(r, c, w);
	return table.markdown();
}

std::vector<std::string> Table::newTable(std::string r, std::string c, std::string w) {
	std::stringstream inputBuffer;
	inputBuffer << r << ' ' << c << ' ' << w;

	int rows, columns, width;
	inputBuffer >> rows >> columns >> width;

	Table table(rows, columns, width);
	return table.markdown();
}

std::vector<std::string> Table::generateCells(int r, int c) {
	std::vector<std::string> out;

	for (int i = 0; i < r; ++i) {
		out.push_back(generateRow(c, ' '));
	}

	return out;
}

std::string Table::generateRow(int columns, char c) {
	std::string out = "|"s;

	for (int i = 0; i < columns; ++i) {
		for (int j = 0; j < width; ++j)
			out += c;
		out += "|";
	}

	return out;
}