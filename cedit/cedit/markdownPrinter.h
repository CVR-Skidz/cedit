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
	private:
		//DO NOT INSTANTIATE
		MarkdownPrinter() {}

		static bool isHeading(std::string s);
		static void printHeading(std::string s, HANDLE* output);
	};
}

