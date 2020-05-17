#include "markdownPrinter.h"

using namespace cedit;

char MarkdownPrinter::fg = MarkdownPrinter::FOREGROUND_W;
MarkdownPrinter::block MarkdownPrinter::open = block::NONE;

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
		std::cout << s;
		//formatLine(s, ' ', output);
	}
}

void MarkdownPrinter::reset() {
	open = block::NONE;
}

bool MarkdownPrinter::isHeading(std::string s) {
	return std::regex_match(s, std::regex("^#+ .*")) || s == "---";
}

void MarkdownPrinter::printHeading(std::string s, HANDLE* output) {
	fg = FOREGROUND_BLUE| FOREGROUND_GREEN | FOREGROUND_INTENSITY;
	SetConsoleTextAttribute(*output, fg);
	std::cout << s;
	SetConsoleTextAttribute(*output, FOREGROUND_W);
}

bool MarkdownPrinter::isQuote(std::string s) {
	return std::regex_match(s, std::regex("^> .*"));
}

void MarkdownPrinter::printQuote(std::string s, HANDLE* output) {
	fg = FOREGROUND_BLUE | FOREGROUND_RED;
	SetConsoleTextAttribute(*output, fg);
	std::cout << s;
	SetConsoleTextAttribute(*output, FOREGROUND_W);
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
			tokens.push_back(s.substr(start, end - start));
		}

		start = end + 1;
	}
	tokens.push_back(s.substr(start));

	for (auto token : tokens) {
		if (isHighlight(token)) {
			printHighlight(token, output);
		}
		else if (isBold(token)) {
			printBold(token, output);
		}
		else {
			std::cout << token;
		}

		if (!token.empty()) std::cout << delimeter;
	}
}

bool MarkdownPrinter::isBold(std::string s) {
	return std::regex_match(s, std::regex("\\*\\*.*\\*\\*"));
}

void MarkdownPrinter::printBold(std::string s, HANDLE* output) {
	SetConsoleTextAttribute(*output, fg | BACKGROUND_BLUE);
	std::cout << s;
	SetConsoleTextAttribute(*output, FOREGROUND_W);
}

bool MarkdownPrinter::isHighlight(std::string s) {
	return std::regex_match(s, std::regex("`[^`]*`"));
}

void MarkdownPrinter::printHighlight(std::string s, HANDLE* output) {
	SetConsoleTextAttribute(*output, fg | BACKGROUND_GREEN);
	std::cout << s;
	SetConsoleTextAttribute(*output, FOREGROUND_W);
}