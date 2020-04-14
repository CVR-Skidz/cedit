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
	xstart = 0;
	ystart = 0;
	width = 0;
	height = 0;

	DWORD inputEventsCount = 0;
	readConsoleBufferSize();

	while (session) {
		ReadConsoleInput(input, events, EVENT_STORAGE_LENGTH, &inputEventsCount);
		processInput(inputEventsCount);
		printLines();
	}
}

void Editor::setConsoleMode() {
	input = GetStdHandle(STD_INPUT_HANDLE);
	output = GetStdHandle(STD_OUTPUT_HANDLE);
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
				readConsoleBufferSize();
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

void Editor::printLines() {
	system("cls");
	for (auto i : lines) {
		//print lines
	}

	printStatus();
}

void Editor::printStatus() {
	COORD statusPosition = { 0, height };
	COORD originalPosition = { x, y };
	SetConsoleCursorPosition(output, statusPosition);

	std::ostringstream status;
	status << "Recieved: " << keyPressed << "\t" << x << "," << y << "\tStart: "
		<< xstart << "," << ystart << "\tSize: " << width << "," << height;

	std::cout << status.str();
	SetConsoleCursorPosition(output, originalPosition);
}

void Editor::readConsoleBufferSize() {
	CONSOLE_SCREEN_BUFFER_INFO screen;
	GetConsoleScreenBufferInfo(output, &screen);

	width = screen.srWindow.Right - screen.srWindow.Left;
	height = screen.srWindow.Bottom - screen.srWindow.Top;
}

int Editor::availableOutputLength(std::string s) {
	if (static_cast<int>(s.length()) > width) {
		return width;
	}
	else {
		static_cast<int>(s.length());
	}
}

void Editor::handleMouseEvent(MOUSE_EVENT_RECORD mouseEvent) {
	/*switch (mouseEvent.dwEventFlags) {
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
	}*/
}

void Editor::handleKeyboardEvent(KEY_EVENT_RECORD keyEvent) {
	auto key = getCharacterPressed(keyEvent);

	//process control keys first
	if (key < 0) {
		handleControlSequence(keyEvent);
	}
	else if (keyEvent.bKeyDown) {
		keyPressed = std::string();
		keyPressed += key;
	}
}

void Editor::handleControlSequence(KEY_EVENT_RECORD keyEvent) {
	if (keyEvent.dwControlKeyState == CTRL_KEY_PRESSED) {
		auto key = getCharacterPressed(keyEvent, true);

		if (key > 0) {
			keyPressed = "ctrl + ";
			keyPressed += key;
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
			keyPressed = "right arrow";
			moveCursor(1);
			break;
		case L_ARROW:
			keyPressed = "left arrow";
			moveCursor(-1);
			break;
		case D_ARROW:
			keyPressed = "down arrow";
			moveCursor(1, true);
			break;
		case U_ARROW:
			keyPressed = "up arrow";
			moveCursor(-1, true);
			break;
		}
	}
}

void Editor::moveCursor(int amount, bool vert) {
	int newpos;

	if (vert) {
		newpos = y + amount;

		//if cursor going off screen top
		if (newpos < 0) {
			newpos = 0;
		}
		else if (newpos > height - 1) { //if cursor going off screen  bottom adjusted for status bar
			newpos = height - 1; 
		}

		y = newpos;
	}
	else {
		newpos = x + amount;

		//if cursor going off screen left
		if (newpos < 0) {
			newpos = 0;
		}
		else if (newpos > width) { //if cursor going off screen right
			newpos = width;
		}

		x = newpos;
	}

	COORD pos = { x, y };
	SetConsoleCursorPosition(output, pos);
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