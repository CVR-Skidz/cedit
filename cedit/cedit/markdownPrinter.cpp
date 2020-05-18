#include "markdownPrinter.h"

using namespace cedit;

char MarkdownPrinter::fg = MarkdownPrinter::FOREGROUND_W;
MarkdownPrinter::block MarkdownPrinter::open = block::NONE;
MarkdownPrinter::format MarkdownPrinter::formatting = format::NORMAL;

void MarkdownPrinter::print(std::string s, std::string l, HANDLE* output) {
	int length = 0;

	if (isHeading(l)) {
		printHeading(s, output);
	}
	else if (isQuote(l)) {
		printQuote(s, output);
	}
	else if (isCodeBlock(l, &length) || open == block::CODE) {
		printCodeBlock(s, output, length);
	}
	else if (isTexBlock(l, &length) || open == block::TEX) {
		printTexBlock(s, output, length);
	}
	else {
		//std::cout << s;
		formatLine(s, ' ', output);
	}
}

void MarkdownPrinter::reset() {
	open = block::NONE;
	formatting = format::NORMAL;
}

bool MarkdownPrinter::isHeading(std::string s) {
	return std::regex_match(s, std::regex("^#+ .*")) || s == "---";
}

void MarkdownPrinter::printHeading(std::string s, HANDLE* output) {
	fg = FOREGROUND_BLUE| FOREGROUND_GREEN | FOREGROUND_INTENSITY;
	SetConsoleTextAttribute(*output, fg);
	formatLine(s, ' ', output);
	SetConsoleTextAttribute(*output, FOREGROUND_W);
	fg = FOREGROUND_W;
}

bool MarkdownPrinter::isQuote(std::string s) {
	return std::regex_match(s, std::regex("^> .*"));
}

void MarkdownPrinter::printQuote(std::string s, HANDLE* output) {
	fg = FOREGROUND_BLUE | FOREGROUND_RED;
	SetConsoleTextAttribute(*output, fg);
	std::cout << s;
	SetConsoleTextAttribute(*output, FOREGROUND_W);
	fg = FOREGROUND_W;
}

bool MarkdownPrinter::isTexBlock(std::string s, int* endPos) {
	bool res = s == "$$";
	if (res && open == block::TEX) {
		open = block::NONE;
	}
	else if (res){
		open = block::TEX;
	}
	else if (open == block::TEX && std::regex_match(s, std::regex("\\$\\$.*"))) {
		open = block::NONE;
		*endPos = 2; //length of $$ => 2
		return true;
	}

	return res;
}

void MarkdownPrinter::printTexBlock(std::string s, HANDLE* output, int len) {
	fg = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
	SetConsoleTextAttribute(*output, fg);

	if (len > 0) {
		std::cout << s.substr(0, len);
		SetConsoleTextAttribute(*output, FOREGROUND_W);
		std::cout << s.substr(len);
	}
	else {
		std::cout << s;
		SetConsoleTextAttribute(*output, FOREGROUND_W);

	}
}

bool MarkdownPrinter::isCodeBlock(std::string s, int* pos) {
	bool res = std::regex_match(s, std::regex("^```.*"));

	if (res && open == block::CODE) {
		open = block::NONE;
		*pos = 3; //length of ``` => 3
	}
	else if (res) {
		open = block::CODE;
	}

	return res;
}

void MarkdownPrinter::printCodeBlock(std::string s, HANDLE* output, int len) {
	fg = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
	SetConsoleTextAttribute(*output, fg);

	if (len > 0) {
		std::cout << s.substr(0, len);
		SetConsoleTextAttribute(*output, FOREGROUND_W);
		std::cout << s.substr(len);
	}
	else {
		std::cout << s;
		SetConsoleTextAttribute(*output, FOREGROUND_W);
	}
}

void MarkdownPrinter::formatLine(std::string s, char delimeter, HANDLE* output) {
	size_t start = 0, end = 0; 
	std::vector<std::string> tokens = std::vector<std::string>();

	while ((end = s.find(delimeter, start)) != std::string::npos) {
		if (end) {
			emphasize(s.substr(start, end - start), output);
		}

		std::cout << s.substr(end, 1);
		start = end + 1;
	}
	emphasize(s.substr(start), output);
}

void MarkdownPrinter::emphasize(std::string s, HANDLE* output) {
	if (isHighlight(s) || formatting == format::HIGHLIGHT) {
		printHighlight(s, output);
	}
	else if (isBold(s) || formatting == format::BOLD) {
		printBold(s, output);
	}
	else {
		std::cout << s;
	}
}

bool MarkdownPrinter::isBold(std::string s) {
	bool open = std::regex_match(s, std::regex("\\*\\*[^\\*]*"));

	if (open) {
		formatting = format::BOLD;
	}
	else if (std::regex_match(s, std::regex(".*\\*\\*"))) {
		formatting = format::NORMAL;
		return true;
	}
	else if (std::regex_match(s, std::regex("\\*\\*.*\\*\\*"))) {
		//only one word
		formatting == format::NORMAL;
		return true;
	}

	return open;
}

void MarkdownPrinter::printBold(std::string s, HANDLE* output) {
	SetConsoleTextAttribute(*output, fg | BACKGROUND_BLUE);
	std::cout << s;
	
	if (formatting != format::BOLD) {
		SetConsoleTextAttribute(*output, FOREGROUND_W);
	}
}

bool MarkdownPrinter::isHighlight(std::string s) {
	bool open = std::regex_match(s, std::regex("`[^`]*"));

	if (open) {
		formatting = format::HIGHLIGHT;
	}
	else if (std::regex_match(s, std::regex("[^`]*`"))) {
		formatting = format::NORMAL;
		return true;
	}
	else if (std::regex_match(s, std::regex("`[^`]*`"))) {
		//only one word
		formatting = format::NORMAL;
		return true;
	}

	return open;
}

void MarkdownPrinter::printHighlight(std::string s, HANDLE* output) {
	SetConsoleTextAttribute(*output, fg | BACKGROUND_RED | BACKGROUND_BLUE);
	std::cout << s;

	if (formatting != format::HIGHLIGHT) {
		SetConsoleTextAttribute(*output, FOREGROUND_W);
	}
}