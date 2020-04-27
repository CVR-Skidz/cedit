#pragma once
#include <string>
#include <Windows.h>
#include <iostream>

namespace cedit {
	class Prompt
	{
	public:
		Prompt(COORD, COORD, HANDLE&, HANDLE&, std::string);

		std::string response();

	private:
		HANDLE& input;
		HANDLE& output;
		std::string buffer;
		std::string message;

		unsigned int width;
		unsigned int height;
		SHORT x, xstart;
		SHORT y, ystart;
		char key;

		void pollInput();
		void printInput();
		void clearScreen();
		void addCharacter(char);
	};
}