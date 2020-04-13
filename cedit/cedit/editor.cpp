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
	auto key = getCharacterPressed(keyEvent);

	//process control keys first
	if (key < 0) {
		handleControlSequence(keyEvent);
	}
	else {
		std::cout << key;
		std::cout << " " << (keyEvent.bKeyDown ? "clicked" : "released");
		std::cout << std::endl;
	}
}

void Editor::handleControlSequence(KEY_EVENT_RECORD keyEvent) {
	if (keyEvent.dwControlKeyState == CTRL_KEY_PRESSED) {
		auto key = getCharacterPressed(keyEvent, true);

		if (key > 0) {
			std::cout << "ctrl + " << key << std::endl;
		}
	}
	else if(keyEvent.dwControlKeyState == NAVIGATION_SEQUENCE){
		handleNavigationSequence(keyEvent);
	}
}

void Editor::handleNavigationSequence(KEY_EVENT_RECORD keyEvent) {
	if (keyEvent.bKeyDown) {
		switch (keyEvent.wVirtualKeyCode) {
		case R_ARROW:
			std::cout << "right arrow" << std::endl;
			break;
		case L_ARROW:
			std::cout << "left arrow" << std::endl;
			break;
		case D_ARROW:
			std::cout << "down arrow" << std::endl;
			break;
		case U_ARROW:
			std::cout << "up arrow" << std::endl;
			break;
		}
	}
}

char Editor::getCharacterPressed(KEY_EVENT_RECORD keyEvent, bool ctrl) {
	CHAR key = keyEvent.uChar.AsciiChar;
	auto upper = key >= 'A' && key <= 'Z';
	auto lower = key >= 'a' && key <= 'z';
	
	if (ctrl) {
		auto newEvent = keyEvent;
		newEvent.uChar.AsciiChar = key + CTRL_KEY_OFFSET;
		return getCharacterPressed(newEvent);
	}
	else {
		return (upper || lower) ? key : -1;
	}
}