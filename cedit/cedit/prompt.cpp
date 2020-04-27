#include "prompt.h"
#include "editor.h"

using namespace std::literals::string_literals;
using namespace cedit;

Prompt::Prompt(COORD pos, COORD size, HANDLE& input, HANDLE& output, std::string message) 
	: input(input), output(output), message(message) {
	width = size.X;
	height = size.Y;
	x = pos.X;
	y = pos.Y;
	xstart = 0;
	ystart = 0;
	key = 0;
	buffer = ""s;
	
	clearScreen();
	do {
		pollInput();
		printInput();
	} while (key != VK_RETURN);
}

std::string Prompt::response() {
	return buffer;
}

void Prompt::pollInput() {
	DWORD status;

	INPUT_RECORD events[1];
	ReadConsoleInput(input, events, 1, &status);
	auto keyEvent = events[0].Event.KeyEvent;
	auto vkey = keyEvent.wVirtualKeyCode;
	key = keyEvent.uChar.AsciiChar;

	if (keyEvent.dwControlKeyState & ~LEFT_CTRL_PRESSED) {
		bool isPrintable = key >= Editor::PRINTABLE_START && key <= Editor::PRINTABLE_END;
		if (keyEvent.bKeyDown) {
			if (isPrintable) {
				addCharacter(key);
			}
			else if (vkey == VK_BACK && buffer.length()) {
				buffer.erase(buffer.end() - 1);
				if (xstart) --xstart;
			}
		}
	}
}

void Prompt::printInput() {
	CONSOLE_CURSOR_INFO cursor;
	cursor.bVisible = false;
	SetConsoleCursorInfo(output, &cursor);
	SetConsoleCursorPosition(output, { x, y });

	int outputLength = width - message.length();
	if (outputLength < 0) outputLength = 0;

	std::cout << message.substr(0, width);
	//SetConsoleCursorPosition(output, { (SHORT)(x + message.length()), y });
	std::cout << buffer.substr(xstart, outputLength);

	if (!xstart) {
		for (auto i = 0; i < outputLength - buffer.length(); ++i) {
			std::cout << ' ';
		}
		
		SHORT endOfInput = message.length() + buffer.length();
		SetConsoleCursorPosition(output, { endOfInput, y });
	}

	cursor.bVisible = true;
	SetConsoleCursorInfo(output, &cursor);
}

void Prompt::clearScreen() {
	DWORD columnsCleared = 0;
	SHORT row = y;

	SetConsoleCursorPosition(output, { x, y });
	FillConsoleOutputCharacter(output, ' ', width * height, { x, y }, &columnsCleared);
	++row;
}

void Prompt::addCharacter(char c) {
	buffer += c;

	if (buffer.length() + message.length() > width) {
		++xstart;
	}
}
