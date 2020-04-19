#include "editor.h"

using namespace cedit;
using namespace std::literals::string_literals;

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
	getConsoleSize();

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
				getConsoleSize();
				printLines();
				break;

			case FOCUS_EVENT:
				keyPressed = "window focus event"s;
				break;

			case MENU_EVENT:
				keyPressed = "menu event"s;
				break;

			default:
				keyPressed = "unhandled input event"s;
		}
	}
}

void Editor::printLines() {
	system("cls");

	//index of line to stop printing at
	int bufferEndPos = lineCount < height ? lineCount : ystart + height;

	for (auto i = ystart; i < bufferEndPos; ++i) {
		auto line = lines[i];
		std::cout << line.substr(xstart, 
			availableOutputLength(line) + 1) << std::endl;
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

	auto finalStatusBar = convertWhitespace(status.str());
	
	//check if the status bar can not fit in buffer
	if (absoluteOutputLength(finalStatusBar) == width - 1) {
		finalStatusBar = finalStatusBar.substr(0, width - 1);
	}

	std::cout << finalStatusBar;
	SetConsoleCursorPosition(output, originalPosition);
}

void Editor::getConsoleSize() {
	CONSOLE_SCREEN_BUFFER_INFO screen;
	GetConsoleScreenBufferInfo(output, &screen);

	width = screen.srWindow.Right - screen.srWindow.Left;
	height = screen.srWindow.Bottom - screen.srWindow.Top;

	COORD newBufferSize = { width + 1, height + 1 };
	SetConsoleScreenBufferSize(output, newBufferSize);
}

std::string Editor::convertWhitespace(std::string s) {
	std::ostringstream out;

	for (auto i : s) {
		if (i == '\t') {
			for (auto j = 0; j < TAB_SIZE; ++j) { out << " "; }
		}
		else {
			out << i;
		}
	}

	return out.str();
}

// Returns the avaialbe length of the string printable to the console buffer
// after scrolling horizontally to the current cursor position.
int Editor::availableOutputLength(std::string s) {
	int len = realLength(s);
	if (len - xstart > width) {
		return width - 1;
	}
	else {
		return len - xstart;
	}
}

// Returns the available length of the string printable to the console buffer
// without regarding the position of the cursor.
//
// Useful for fixed position components.
int Editor::absoluteOutputLength(std::string s) {
	int len = realLength(s);

	if (len > width) {
		return width - 1;
	}
	else {
		return len;
	}
}

std::string Editor::currentLine() {
	if (lineCount != 0 && !(y - ystart > lineCount - 1)) {
		return lines[y - ystart];
	}
	else {
		return ""s;
	}
}

int Editor::realLength(std::string s) {
	int lineLength = s.length();

	for (auto i : s) {
		if (i == '\t') {
			lineLength += TAB_SIZE - 1;
		}
	}

	//lineLength -= lineLength ? 1 : 0;

	return lineLength;
}

void Editor::insertCharacter(char c) {
	if (!lineCount) {
		addLine("");
	}

	int lineLength = lines[y].length();

	if (x > lineLength) {
		lines[y] += c;
	}
	else {
		lines[y].insert(x, 1, c);
	}

	moveCursorHor(1);
}

void Editor::addLine(std::string s) {
	lines.push_back(s);
	lineCount++;
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
		insertCharacter(key);
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
	else {
		keyPressed = std::to_string(keyEvent.wVirtualKeyCode);
	}
}

void Editor::handleNavigationSequence(KEY_EVENT_RECORD keyEvent) {
	if (keyEvent.bKeyDown) {
		switch (keyEvent.wVirtualKeyCode) {
			case R_ARROW:
				keyPressed = "right arrow";
				moveCursorHor(1);
				break;

			case L_ARROW:
				keyPressed = "left arrow";
				moveCursorHor(-1);
				break;

			case D_ARROW:
				keyPressed = "down arrow";
				moveCursorVert(1);
				break;

			case U_ARROW:
				keyPressed = "up arrow";
				moveCursorVert(-1);
				break;

			default:
				keyPressed = std::to_string(keyEvent.wVirtualKeyCode);
				break;
		}
	}
}

void Editor::moveCursorVert(int amount) {
	int newpos = y + amount;

	//if cursor going off screen top
	if (newpos < 0) {
		newpos = 0;
		if (ystart) --ystart;
	}
	else if (newpos > lineCount - 1) { //cant move passed end of file
		newpos = (lineCount ? lineCount - 1 : 0);
	}
	else if (newpos > height - 1) { //if cursor going off bottom screen, adjusted for status bar
		newpos = height - 1;
		if (newpos + ystart < lineCount - 1) ystart++;
	}

	y = newpos;

	COORD pos = { x, y };
	SetConsoleCursorPosition(output, pos);
}

void Editor::moveCursorHor(int amount) {
	int newpos = x + amount;
	std::string line = currentLine();
	int lineLength = realLength(line);

	//if cursor going off screen left
	if (newpos < 0) {
		newpos = 0;
		if (xstart) { --xstart; }
	}
	else if (newpos > lineLength) { //cant move passed end of line
		newpos = lineLength;
	}
	else if (newpos > width) { //if cursor going off screen right
		newpos = width;
		if (newpos + xstart < lineLength) xstart++;
	}

	x = newpos;

	COORD pos = { x, y };
	SetConsoleCursorPosition(output, pos);
}

char Editor::getCharacterPressed(KEY_EVENT_RECORD keyEvent, bool ctrl) {
	CHAR key = keyEvent.uChar.AsciiChar;
	auto printable = key >= PRINTABLE_START && key <= PRINTABLE_END;
	
	if (ctrl) {
		auto newEvent = keyEvent;
		newEvent.uChar.AsciiChar = key + CTRL_KEY_OFFSET;
		return getCharacterPressed(newEvent);
	}
	else {
		return printable ? key : -1;
	}
}