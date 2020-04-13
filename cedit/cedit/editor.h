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
		void handleMouseEvent(MOUSE_EVENT_RECORD);
		void handleKeyboardEvent(KEY_EVENT_RECORD);
	};
}