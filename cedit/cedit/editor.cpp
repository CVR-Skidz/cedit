#include "editor.h"

using namespace cedit;

int main() {
	Editor editor;

	return 0;
}

Editor::Editor() {
	setConsoleMode();

	session = true;
	lineCount = 0;
	y = 0;
	x = 0;

	DWORD inputEventsCount = 0;

	while (session) {
		ReadConsoleInput(input, events, EVENT_STORAGE_LENGTH, &inputEventsCount);
		processInput(inputEventsCount);
	}
}

void Editor::setConsoleMode() {
	input = GetStdHandle(STD_INPUT_HANDLE);
	GetConsoleMode(input, &consoleInputMode);

	//disable quick edit mode
	SetConsoleMode(input, ENABLE_EXTENDED_FLAGS);
	//set editor console mode
	SetConsoleMode(input, CONSOLE_MODE);
}

void Editor::processInput(int nEvents) {
	for (auto i = 0; i < nEvents; ++i) {
		switch (events[i].EventType) {
			case KEY_EVENT:
				handleKeyboardEvent(events[i].Event.KeyEvent);
				break;

			case MOUSE_EVENT:
				handleMouseEvent(events[i].Event.MouseEvent);
				break;

			case WINDOW_BUFFER_SIZE_EVENT:
				std::cout << "Received resize event" << std::endl;
				break;

			case FOCUS_EVENT:
				std::cout << "Received window focus event" << std::endl;
				break;

			case MENU_EVENT:
				std::cout << "Received menu event" << std::endl;
				break;

			default:
				std::cout << "Received unhandled input event" << std::endl;
		}
	}
}

void Editor::handleMouseEvent(MOUSE_EVENT_RECORD mouseEvent) {
	switch (mouseEvent.dwEventFlags) {
		case 0: 
			if (mouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED) {
				std::cout << "[left click]" << std::endl;
			}
			else if (mouseEvent.dwButtonState == RIGHTMOST_BUTTON_PRESSED) {
				std::cout << "[right click]" << std::endl;
			}
			break;

		case MOUSE_WHEELED:
			std::cout << "[scrolling]" << std::endl;
			break;

		case MOUSE_MOVED:
			COORD pos = mouseEvent.dwMousePosition;
			std::cout << pos.X << "," << pos.Y << std::endl;
			break;

		case MOUSE_HWHEELED:
			std::cout << "[horizontal scrolling]" << std::endl;
			break;

		case DOUBLE_CLICK:
			std::cout << "[double click]" << std::endl;
			break;
	}
}

void Editor::handleKeyboardEvent(KEY_EVENT_RECORD keyEvent) {
	CHAR key = keyEvent.uChar.AsciiChar;
	auto upper = key >= 'A' && key <= 'Z';
	auto lower = key >= 'a' && key <= 'z';
	
	if (upper || lower) {
		std::cout << key;
		std::cout << " " << (keyEvent.bKeyDown ? "clicked" : "released");
		std::cout << std::endl;
	}
}