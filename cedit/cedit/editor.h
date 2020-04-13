#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <windows.h>

namespace cedit {
	class Editor
	{
	public:
		Editor();
		
	private:
		constexpr static unsigned int EVENT_STORAGE_LENGTH = 64;
		constexpr static DWORD CONSOLE_MODE = 
			ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT;

		bool session;
		HANDLE input;
		DWORD consoleInputMode; //mode of console before launcihng
		INPUT_RECORD events[EVENT_STORAGE_LENGTH];

		int lineCount;
		unsigned int y;
		unsigned int x;
		std::vector<std::string> lines;

		void setConsoleMode();
		void processInput(int);

		//keyboard
		constexpr static unsigned int CTRL_KEY_PRESSED = 40;
		constexpr static unsigned int CTRL_KEY_RELEASED = 32;
			//offset of ACII keycode after ctrl is pressed
			constexpr static unsigned int CTRL_KEY_OFFSET = 64; 

		constexpr static unsigned int NAVIGATION_SEQUENCE = 288;
		constexpr static unsigned int L_ARROW = 37;
		constexpr static unsigned int U_ARROW = 38;
		constexpr static unsigned int R_ARROW = 39;
		constexpr static unsigned int D_ARROW = 40;

		void handleKeyboardEvent(KEY_EVENT_RECORD);
		void handleControlSequence(KEY_EVENT_RECORD);
		void handleNavigationSequence(KEY_EVENT_RECORD);
		char getCharacterPressed(KEY_EVENT_RECORD, bool ctrl = false);

		//mouse
		void handleMouseEvent(MOUSE_EVENT_RECORD);
		
	};
}