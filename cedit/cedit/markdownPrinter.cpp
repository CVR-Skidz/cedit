#include "markdownPrinter.h"

using namespace cedit;

void MarkdownPrinter::print(std::string s, std::string l, HANDLE* output) {
	if (isHeading(l)) {
		printHeading(s, output);
	}
	else {
		std::cout << s;
	}
}

bool MarkdownPrinter::isHeading(std::string s) {
	return std::regex_match(s, std::regex("^#+ .*"));
}

void MarkdownPrinter::printHeading(std::string s, HANDLE* output) {
	SetConsoleTextAttribute(*output, FOREGROUND_BLUE);
	std::cout << s;
	SetConsoleTextAttribute(*output, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
}