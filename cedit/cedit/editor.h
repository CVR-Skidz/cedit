#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <windows.h>
#include <sstream>

namespace cedit {
	class Editor
	{
	public:
		Editor();
		
	private:
		constexpr static unsigned int EVENT_STORAGE_LENGTH = 64;
		constexpr static unsigned int TAB_SIZE = 8;
		constexpr static DWORD CONSOLE_MODE =
			ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT;

		bool session;
		HANDLE input;
		HANDLE output;
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

		//status bar
		std::string keyPressed;
		
		void setConsoleMode();
		void getConsoleSize();
		void processInput(int);
		
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
		constexpr static unsigned int CTRL_KEY_PRESSED = 40;
		constexpr static unsigned int CTRL_KEY_RELEASED = 32;
			//offset of ACII keycode after ctrl is pressed
			constexpr static unsigned int CTRL_KEY_OFFSET = 64; 

		constexpr static unsigned int NAVIGATION_SEQUENCE = 288;
		constexpr static unsigned int L_ARROW = 37;
		constexpr static unsigned int U_ARROW = 38;
		constexpr static unsigned int R_ARROW = 39;
		constexpr static unsigned int D_ARROW = 40;
		constexpr static unsigned int DEL_KEY = 46;

		constexpr static unsigned int ENTER = 13;
		constexpr static unsigned int BACKSPACE = 8;
		constexpr static unsigned int SPACE = 32;

		void handleKeyboardEvent(KEY_EVENT_RECORD);
		void handleControlSequence(KEY_EVENT_RECORD);
		void handleNavigationSequence(KEY_EVENT_RECORD);
		void handleSpecialCharacter(KEY_EVENT_RECORD);

		void snapCursorToLine();
		void moveCursorVert(int);
		void moveCursorHor(int);
		void processEnter();
		char getCharacterPressed(KEY_EVENT_RECORD, bool ctrl = false);

		//mouse
		void handleMouseEvent(MOUSE_EVENT_RECORD);
		
		//debug
		void fillLines() {
			for (auto i = 0; i < 20; ++i) {
				lines.push_back("Test" + std::to_string(i));
			}
			lines.push_back("TestTestTestTestTestTestTestTestTestTestTestTestTestTestTest");
			lineCount = lines.size();
		}
	};
}