#include "markdownPrinter.h"

using namespace cedit;

char MarkdownPrinter::fg = MarkdownPrinter::FOREGROUND_W;
MarkdownPrinter::block MarkdownPrinter::open = block::NONE;

void MarkdownPrinter::print(std::string s, std::string l, HANDLE* output) {
	if (isHeading(l)) {
		printHeading(s, output);
	}
	else if (isQuote(l)) {
		printQuote(s, output);
	}
	else if (isCodeBlock(l) || open == block::CODE) {
		printCodeBlock(s, output);
	}
	else if (isTexBlock(l) || open == block::TEX) {
		printTextBlock(s, output);
	}
	else {
		std::cout << s;
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

bool MarkdownPrinter::isTexBlock(std::string s) {
	bool res = s == "$$";
	if (res && open == block::TEX) {
		open = block::NONE;
	}
	else if (res){
		open = block::TEX;
	}

	return res;
}

void MarkdownPrinter::printTextBlock(std::string s, HANDLE* output) {
	fg = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
	SetConsoleTextAttribute(*output, fg);
	std::cout << s;
	SetConsoleTextAttribute(*output, FOREGROUND_W);
}

bool MarkdownPrinter::isCodeBlock(std::string s) {
	bool res = std::regex_match(s, std::regex("^```(\\{.*\\})*"));

	if (res && open == block::CODE) {
		open = block::NONE;
	}
	else if (res) {
		open = block::CODE;
	}
	else if (open == block::CODE && std::regex_match(s, std::regex("^```.*"))) {
		//close any open blocks that contain text after closing "```"
		open = block::NONE;
		return true;
	}

	return res;
}

void MarkdownPrinter::printCodeBlock(std::string s, HANDLE* output) {
	fg = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
	SetConsoleTextAttribute(*output, fg);
	std::cout << s;
	SetConsoleTextAttribute(*output, FOREGROUND_W);
}