#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <windows.h>
#include <sstream>

namespace cedit {
	class Editor
	{
	public:
		Editor();
		Editor(std::string);

		void start();
		
	private:
		constexpr static unsigned int EVENT_STORAGE_LENGTH = 64;
		constexpr static unsigned int TAB_SIZE = 8;
		constexpr static DWORD CONSOLE_MODE =
			ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT;

		bool session;
		HANDLE input;
		HANDLE output;
		CONSOLE_CURSOR_INFO cursor;
		DWORD consoleInputMode; //mode of console before launching
		INPUT_RECORD events[EVENT_STORAGE_LENGTH];

		int lineCount;
		unsigned int y; //line of cursor
		unsigned int x; //column of cursor 
		unsigned int xstart; //starting index in line in editor
		unsigned int ystart; //starting index in line storage in editor
		unsigned int width; //width of console screen buffer
		unsigned int height;  //height of console screen buffer
		std::vector<std::string> lines;
		std::vector<std::string> screenBuffer;
		std::string path;

		//status bar
		std::string keyPressed;
		
		void setConsoleMode();
		void getConsoleSize();
		void processInput(int);
		std::string getBlockingInput();
		void readFile(std::string);
		void saveFile(std::string path = "");
		
		void clearScreen();
		void printLines();
		void printStatus();
		void insertCharacter(char);
		void removeCharacter(int);
		void addLine(std::string, int pos = -1);
		void concatLine(bool before = true);

		std::string convertWhitespace(std::string);
		int availableOutputLength(std::string);
		int absoluteOutputLength(std::string);
		int realLength(std::string);
		std::string currentLine();
		
		//keyboard
		constexpr static unsigned int PRINTABLE_START = '!';
		constexpr static unsigned int PRINTABLE_END = '~';
			//offset of ACII keycode after ctrl is pressed
			constexpr static unsigned int CTRL_KEY_OFFSET = 64; 

		void handleKeyboardEvent(KEY_EVENT_RECORD);
		void handleControlSequence(KEY_EVENT_RECORD);
		void handleNavigationSequence(KEY_EVENT_RECORD);
		void handleSpecialCharacter(KEY_EVENT_RECORD);

		void snapCursorToLine();
		void moveCursorVert(int);
		void moveCursorHor(int);
		void processEnter();
		char getCharacterPressed(KEY_EVENT_RECORD, bool ctrl = false);
		
		//set cursor coordiates to be within screen buffer, and sets appropriately
		//the line offsets
		void standardizeCoords();

		//displays user prompt, and returns input
		std::string prompt(std::string);

		//mouse
		void handleMouseEvent(MOUSE_EVENT_RECORD);
	};
}