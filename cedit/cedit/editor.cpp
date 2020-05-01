#include "editor.h"
#include "prompt.h"

using namespace cedit;
using namespace std::literals::string_literals;

int main(int argc, char* argv[]) {
	//open file
	if (argc > 1) {
		Editor editor = Editor(argv[1]);
		editor.start();
	}
	else {
		Editor editor;
		editor.start();
	}

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
	path = "";
}

Editor::Editor(std::string path) : Editor(){
	readFile(path);
	this->path = path;
}

void Editor::start() {
	DWORD inputEventsCount = 0;
	
	while (session) {
		getConsoleSize();
		printLines();
		ReadConsoleInput(input, events, EVENT_STORAGE_LENGTH, &inputEventsCount);
		processInput(inputEventsCount);
	}
}

void Editor::setConsoleMode() {
	input = GetStdHandle(STD_INPUT_HANDLE);
	output = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleMode(input, &consoleInputMode);
	GetConsoleCursorInfo(output, &cursor);

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
		}
	}
}

void Editor::readFile(std::string path) {
	std::ifstream file;
	file.open(path);

	if (!file.good()) {	//error reading file
		statusMessage = "Could not read from \"" + path + '"';
		return;
	}

	this->path = path;
	x = 0;
	y = 0;
	xstart = 0;
	ystart = 0;
	lineCount = 0;
	lines.clear();
	
	char charBuffer = 0;
	std::string lineBuffer = ""s;

	while (!file.eof()) {
		//read line
		while ((charBuffer = file.get()) != '\n' && !file.eof()) {
			if (charBuffer == '\t') {
				for (auto i = 0; i < TAB_SIZE; ++i) lineBuffer += ' ';
			}
			else lineBuffer += charBuffer;
		}

		lines.push_back(lineBuffer);
		lineBuffer.clear();
		++lineCount;
	}

	statusMessage = this->path;
	file.close();
}

void Editor::saveFile(std::string path) {
	if (!path.length()) {
		path = prompt("Enter file path:");
		this->path = path;
	}

	std::ofstream file;
	file.open(path);

	if (!file.good()) {
		statusMessage = "Could not write to \"" + path + '"';
		return;
	}

	for (auto line : lines) {
		file << line << std::endl;
	}

	statusMessage = "Saved file to " + path;
	file.close();
}

void Editor::clearScreen() {
	DWORD columnsCleared;
	FillConsoleOutputCharacter(output, ' ', height * width, { 0,0 }, &columnsCleared);
	FillConsoleOutputAttribute(output, BLACKONWHITE, height * width, { 0,0 }, &columnsCleared);
}

void Editor::printLines() {
	COORD originalPosition = { x, y };
	cursor.bVisible = false;
	SetConsoleCursorInfo(output, &cursor);
	SetConsoleCursorPosition(output, {0,0});

	//index of line to stop printing at
	int bufferEndPos = lineCount <= height ? lineCount : ystart + height;
	DWORD count; //count of any characters cleared

	for (auto i = ystart; i < bufferEndPos; ++i) {
		auto line = lines[i];
		std::string finalLine;
		if (xstart >= line.length()) {
			COORD clearPos = { 0,i };
			FillConsoleOutputCharacter(output, ' ', width-1, clearPos, &count);
			finalLine = ""s;
		}
		else {
			finalLine = line.substr(xstart, availableOutputLength(line) + 1);
			std::cout << finalLine;
		}

		if (screenBuffer.size() > i) {
			screenBuffer[i - ystart] = finalLine;
			COORD eolPos = { finalLine.length(), i-ystart };
			SetConsoleCursorPosition(output, eolPos);
			FillConsoleOutputCharacter(output, ' ', width - finalLine.length(), 
				eolPos, &count);
		}
		else screenBuffer.push_back(finalLine); 

		std::cout << std::endl;
	}

	//clear empty lines at end of file
	if (screenBuffer.size() > lines.size()) {
		auto diff = screenBuffer.size() - lines.size();
		COORD clearPos = { 0, lines.size() };
		DWORD columnsCleared;
		FillConsoleOutputCharacter(output, ' ', diff * width, clearPos, &columnsCleared);
	}

	printStatus();
	SetConsoleCursorPosition(output, originalPosition);
	cursor.bVisible = true;
	SetConsoleCursorInfo(output, &cursor);
}

void Editor::printStatus() {
	COORD statusPosition = { 0, height };
	SetConsoleCursorPosition(output, statusPosition);
	SetConsoleTextAttribute(output, BACKGROUND_BLUE | BACKGROUND_GREEN);

	std::ostringstream status;
	status << statusMessage << " " << x +xstart << "," << y + ystart << " " <<
		width << "," << height << " Lines: " << lineCount;

	auto finalStatusBar = convertWhitespace(status.str());
	auto displayLength = absoluteOutputLength(finalStatusBar);
	
	//check if the status bar can not fit in buffer
	if (displayLength == width) {
		finalStatusBar = finalStatusBar.substr(0, width - 1);
		displayLength = width - 1;
	}

	std::cout << finalStatusBar;
	for (auto i = 0; i <= width - displayLength; ++i) { std::cout << ' '; }
	SetConsoleTextAttribute(output, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
}

void Editor::getConsoleSize() {
	CONSOLE_SCREEN_BUFFER_INFO screen;
	GetConsoleScreenBufferInfo(output, &screen);

	if (width != screen.dwSize.X - 1|| height != screen.dwSize.Y - 1) {
		DWORD statusErased;
		SetConsoleCursorPosition(output, { 0, (short)height });
		FillConsoleOutputCharacter(output, ' ', width, { 0, (short)height }, &statusErased);
	}

	width = screen.srWindow.Right - screen.srWindow.Left;
	height = screen.srWindow.Bottom - screen.srWindow.Top;

	COORD newBufferSize = { width+1, height+1 };
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
	if (lineCount != 0 && !(y + ystart > lineCount - 1)) {
		return lines[y + ystart];
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

	int lineLength = lines[y + ystart].length();
	 
	if (x > lineLength) {
		lines[y + ystart] += c;
	}
	else {
		lines[y + ystart].insert(x + xstart, 1, c);
	}

	moveCursorHor(1);
}

void Editor::removeCharacter(int pos) {
	if (pos < currentLine().length()) {
		lines[y + ystart].erase(lines[y + ystart].begin() + pos + xstart);
	}
}

void Editor::addLine(std::string s, int pos) {
	if (pos >= 0 && pos < lines.size()) {
		lines.insert(lines.begin() + pos, s);
	}
	else {
		lines.push_back(s);
	}
	lineCount++;
}

void Editor::concatLine(bool before) {
	int lineIndex = before ? y + ystart - 1 : y + ystart;
	int appendPoint = lines[lineIndex].length();

	lines[lineIndex] += lines[lineIndex + 1];
	lines.erase(lines.begin() + lineIndex + 1);
	x = appendPoint;

	//reposition y coordinate
	if (before) {
		if (ystart) --ystart; else --y;
	}
	
	--lineCount;
	standardizeCoords();
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
	}
}

void Editor::handleControlSequence(KEY_EVENT_RECORD keyEvent) {
	if (keyEvent.dwControlKeyState & LEFT_CTRL_PRESSED) {
		auto key = getCharacterPressed(keyEvent, true);
		
		switch (key) {
			case 'S':
				saveFile(path);
				break;
			case 'O':
				readFile(prompt("Enter file path:"));
				break;
			case 'Q':
				system("cls");
				session = false;
				break;
			case 'N':
				path = ""s;
				lineCount = 0;
				lines.clear();
				x = 0; y = 0;
				break;
		}
	}
	else {
		handleSpecialCharacter(keyEvent);
	}
}

void Editor::handleNavigationSequence(KEY_EVENT_RECORD keyEvent) {
	if (keyEvent.bKeyDown) {
		switch (keyEvent.wVirtualKeyCode) {
			case VK_RIGHT:
				moveCursorHor(1);
				break;

			case VK_LEFT:
				moveCursorHor(-1);
				break;

			case VK_DOWN:
				moveCursorVert(1);
				break;

			case VK_UP:
				moveCursorVert(-1);
				break;

			case VK_HOME:
				x = 0;
				xstart = 0;
				break;

			case VK_END: {
				x = currentLine().length();
				//standardize x axis
				auto yBuffer = ystart;
				standardizeCoords();
				ystart = yBuffer;
				break;
			}

			case VK_PRIOR:	//page up
				if (y >= height - 1) y -= (height - 1);
				else if (ystart) ystart -= (height - 1);
				else y = 0;

				x = 0;
				standardizeCoords();
				break;

			case VK_NEXT:	//page down
				y += height - 1;

				if (y > lineCount - 1) y = lineCount - 1;
				
				x = 0;
				standardizeCoords();
				break;
		}
	}
}

void Editor::handleSpecialCharacter(KEY_EVENT_RECORD keyEvent) {
	if (keyEvent.bKeyDown) {
		switch (keyEvent.wVirtualKeyCode) {
			case VK_RETURN:
				processEnter();
				break;
		
			case VK_BACK:
				if (x != 0) {
					removeCharacter(x - 1);
					if (xstart) --xstart;
					else moveCursorHor(-1);
				}
				else if (y != 0) {
					concatLine();
				}
				break;

			case VK_SPACE:
				insertCharacter(' ');
				break;

			case VK_DELETE:
				if (x != lines[y + ystart].length()) {
					removeCharacter(x);
				}
				else if (y + ystart != lineCount - 1) {
					concatLine(false);
				}
				break;

			case VK_TAB:
				for (int i = 0; i < TAB_SIZE; ++i) { insertCharacter(' '); }

			default:
				handleNavigationSequence(keyEvent);
				break;
		}
	}
}

void Editor::snapCursorToLine() {
	int lineLength = currentLine().length();

	if (x > lineLength) {
		x = lineLength;
	}
}

void Editor::moveCursorVert(int amount) {
	int newpos = y + amount;

	//if cursor going off screen top
	if (newpos < 0) {
		newpos = 0;
		if (ystart) --ystart;
	}
	else if (newpos + ystart > lineCount - 1) { //cant move passed end of file
		if (lineCount) {
			newpos = (lineCount - 1 > height - 1 ? height - 1 : lineCount - 1);
		}
		else newpos = 0;
	}
	else if (newpos > height - 1) { //if cursor going off bottom screen, adjusted for status bar
		newpos = height - 1;
		if (newpos + ystart < lineCount) ystart++;
	}

	y = newpos;
	snapCursorToLine();

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

void Editor::processEnter() {
	std::string newLine = currentLine();
	int distanceToEnd = newLine.length() - x - xstart;
	newLine = newLine.substr(x + xstart, distanceToEnd);
	std::string& currentL = lines[y + ystart];
	currentL = currentL.erase(x + xstart, currentL.length());

	//add new line;
	addLine(newLine, y + ystart + 1);
	++y;

	if (y > height - 1) {
		y = height - 1;
		++ystart;
	}

	x = 0;
	xstart = 0;
	COORD pos = { x,y };
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

void Editor::standardizeCoords() {
	auto lineLength = currentLine().length();
	int xoverflow = x - width;
	int yoverflow = y - (height - 1);

	if (xoverflow > 0) {
		x = width;
		auto newXOffset = xstart + xoverflow;
		if (x + newXOffset < lineLength) xstart += xoverflow;
		else xstart = lineLength - x - 1;
	}
	else if (xoverflow < 0) {
		xstart = 0;
	}

	if (yoverflow > 0) {
		y = height - 1;
		auto newYOffset = ystart + yoverflow;
		if (y + newYOffset < lineCount) ystart += yoverflow;
		else ystart = lineCount - y - 1;
	}
	/*else if (yoverflow < 0) {
		ystart = 0;
	}*/
}

std::string Editor::prompt(std::string message) {
	Prompt prompt = Prompt({ 0,(SHORT)height-1 }, { (SHORT)width, 1 }, input, output, message);
	clearScreen();
	return prompt.response();
}