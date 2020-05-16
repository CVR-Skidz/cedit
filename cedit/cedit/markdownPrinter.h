#pragma once
#include <string>
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

		static bool isTexBlock(std::string s);
		static void printTextBlock(std::string s, HANDLE* output);

		static bool isCodeBlock(std::string s);
		static void printCodeBlock(std::string s, HANDLE* output);

		enum block {
			NONE,
			TEX,
			CODE,
			TABLE
		};

		static block open;
		static char fg;
		constexpr static char FOREGROUND_W =
			FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
	};
}

