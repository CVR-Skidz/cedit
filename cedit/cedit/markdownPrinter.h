#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <regex>
#include <windows.h>

namespace cedit {
	// Wrappers for regex patterns to identify markdown components
	class MarkdownPrinter
	{
	public:
		// Prints s to the output stream pointed to by output, with
		// formatting corresponding to the markdown content in l.
		static void print(std::string s, std::string l, HANDLE* output);
		static void reset();
	private:
		//DO NOT INSTANTIATE
		MarkdownPrinter() {}

		static bool isHeading(std::string s);
		static void printHeading(std::string s, HANDLE* output);

		static bool isQuote(std::string s);
		static void printQuote(std::string s, HANDLE* ouptut);

		static bool isTexBlock(std::string s, int* endPos);
		static void printTexBlock(std::string s, HANDLE* output, int len);

		static bool isCodeBlock(std::string s, int* endPos);
		static void printCodeBlock(std::string s, HANDLE* output, int len);

		//formats each token in a line
		static void formatLine(std::string s, char delimeter, HANDLE* output);
		//helper function for formatLine
		static void emphasize(std::string s, HANDLE* output);
		static bool isBold(std::string s);
		static void printBold(std::string s, HANDLE* output);
		static bool isHighlight(std::string s);
		static void printHighlight(std::string s, HANDLE* output);

		enum block {
			NONE,
			TEX,
			CODE,
			TABLE
		};

		enum format {
			BOLD,
			HIGHLIGHT,
			NORMAL
		};

		static block open;
		static format formatting;
		static char fg;
		constexpr static char FOREGROUND_W =
			FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
	};
}

