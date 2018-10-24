#include "CManip.hpp"
#include <conio.h>

using namespace std;

int globalFontSize, storyFontInc;
string language, codePage;

Color color;

//Checking Keyboard Input ---------------
const bool CheckEnter() noexcept {
	return GetAsyncKeyState(VK_RETURN) & 0x8000;
}
const bool CheckSpace() noexcept {
	return GetAsyncKeyState(VK_SPACE) & 0x8000;
}
const bool CheckEscape() noexcept {
	return GetAsyncKeyState(VK_ESCAPE) & 0x8000;
}
const bool CheckLeftArrow() noexcept {
	return GetAsyncKeyState(VK_LEFT) & 0x8000;
}
const bool CheckRightArrow() noexcept {
	return GetAsyncKeyState(VK_RIGHT) & 0x8000;
}
const bool CheckUpArrow() noexcept {
	return GetAsyncKeyState(VK_UP) & 0x8000;
}
const bool CheckDownArrow() noexcept {
	return GetAsyncKeyState(VK_DOWN) & 0x8000;
}
const bool CheckAlt() noexcept {
	return GetAsyncKeyState(VK_MENU) & 0x8000;
}
void CheckAltF4() noexcept {
	if (CheckAlt() && GetAsyncKeyState(VK_F4) & 0x8000) exit(0);
}

//Console Manipulation ------------------
void setFontSize(const int &FontSize) noexcept {
	CONSOLE_FONT_INFOEX info = { 0 };
	info.cbSize = sizeof(info);
	info.dwFontSize.Y = FontSize; // leave X as zero
	info.FontWeight = FW_NORMAL;
	wcscpy_s(info.FaceName, L"Lucida Console");
	SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), NULL, &info);
}
void ClearConsoleInputBuffer() {
	PINPUT_RECORD ClearingVar1 = new INPUT_RECORD[256];
	DWORD ClearingVar2;
	ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), ClearingVar1, 256, &ClearingVar2);
	delete[] ClearingVar1;
}
void ShowConsoleCursor(const bool &showFlag) noexcept {
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_CURSOR_INFO     cursorInfo;

	GetConsoleCursorInfo(out, &cursorInfo);
	cursorInfo.bVisible = showFlag; // set the cursor visibility
	SetConsoleCursorInfo(out, &cursorInfo);
}
void ClearConsole() noexcept {
	COORD topLeft = { 0, 0 };
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO screen;
	DWORD written;

	GetConsoleScreenBufferInfo(console, &screen);
	FillConsoleOutputCharacterA(
		console, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written
	);
	FillConsoleOutputAttribute(
		console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
		screen.dwSize.X * screen.dwSize.Y, topLeft, &written
	);
	SetConsoleCursorPosition(console, topLeft);
}

void InputString(string &str, const unsigned int &limit) {
	COORD prevCursorPos = GetCursorPos();
	char c;
	do {
		CursorPos(prevCursorPos);
		cout << str << ' ';
		CursorMove(-1, 0);
		ShowConsoleCursor(true);
		c = _getch();
		ShowConsoleCursor(false);

		if (c != 0x00) {
			CheckAltF4();

			if (CheckEnter() && str.size() > 0) {
				audio.enter();
				break;
			}
			else if (CheckUpArrow()) continue;
			else if (CheckDownArrow()) continue;
			else if (CheckLeftArrow()) continue;
			else if (CheckRightArrow()) continue;
			else if (CheckAlt()) continue;
			else if (c == 0x08) {	//When backspace pressed
				if (str.size() > 0) {
					str.pop_back();
				}
			}
			else if (c >= '0' && c <= '9' || c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z') {
				if (str.size() < limit) {
					str += c;
				}
			}
		}
		else continue;
	} while (true);
}
const pair<int, int> GetDesktopResolution() noexcept {
	pair<int, int>resolution;

	resolution.first = GetSystemMetrics(SM_CXSCREEN);
	resolution.second = GetSystemMetrics(SM_CYSCREEN);

	return resolution;
}
void Fullscreeen(const bool &yesNo) noexcept {
	if (yesNo == true) {
		SetConsoleDisplayMode(GetStdHandle(STD_OUTPUT_HANDLE), CONSOLE_FULLSCREEN_MODE, 0);
	}
	else {
		SetConsoleDisplayMode(GetStdHandle(STD_OUTPUT_HANDLE), CONSOLE_WINDOWED_MODE, 0);
	}
}

const COORD GetCursorPos() noexcept {
	CONSOLE_SCREEN_BUFFER_INFO cbsi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cbsi);
	return cbsi.dwCursorPosition;
}
void CursorPos(const COORD &c) noexcept {
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}
void CursorPos(const unsigned int &x, const unsigned int &y) noexcept {
	COORD c;
	c.X = x;
	c.Y = y;
	CursorPos(c);
}
void CursorPosBegin() noexcept {
	CursorPos(0, 0);
}
void CursorMove(const int &movX, const int &movY) noexcept {
	COORD c = GetCursorPos();
	c.X += movX;
	c.Y += movY;

	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void SpacePause() {
	cout << sLineTexts["SpacePause"];
	do { ClearConsoleInputBuffer(); } while (!CheckSpace()); //Waits for SPACE press

	ClearConsoleInputBuffer();
}

void adaptLocationFontSize() noexcept {
	pair<int, int> resolution = GetDesktopResolution();

	if (resolution.first <= 1280) storyFontInc = 20;
	else if (resolution.first <= 1440) storyFontInc = 19;
	else if (resolution.first <= 1680) storyFontInc = 17;
	else if (resolution.first <= 1768) storyFontInc = 15;
	else if (resolution.first <= 1920) storyFontInc = 14;
	else if (resolution.first <= 2103) storyFontInc = 12;
	else if (resolution.first <= 2351) storyFontInc = 9;
	else if (resolution.first <= 2560) storyFontInc = 7;
	else if (resolution.first >  2560) storyFontInc = 1;
}
void adaptFontSize() {
	pair<int, int> resolution = GetDesktopResolution();

	if (resolution.first == 1680) globalFontSize = 16;
	else if (resolution.first >= 3840) globalFontSize = 60;
	else {
		float temp = float((-3.0f / 500000.0f)*pow(resolution.first, 2) + 0.0511*resolution.first - 51.634f);
		globalFontSize = int( round(temp) );
	}
	//else if (resolution.first <= 1280) globalFontSize = 5;
	//else if (resolution.first >= 1360 && resolution.first <= 1368) globalFontSize = 8;
	//else if (resolution.first == 1440) globalFontSize = 10;
	//else if (resolution.first == 1600) globalFontSize = 15;
	//else if (resolution.first == 1768) globalFontSize = 20;
	//else if (resolution.first == 1920) globalFontSize = 25;
	//else if (resolution.first == 2103) globalFontSize = 30;
	//else if (resolution.first == 2351) globalFontSize = 38;
	//else if (resolution.first >= 2560 && resolution.first < 3840) globalFontSize = 43;
	//else if (resolution.first >= 3840) globalFontSize = 32;
}

//GFX -----------------------------------
void PrintBox (const unsigned int &xPos, const unsigned int &yPos, const unsigned int &width, const unsigned int &height) {
	COORD coordinates1;
	coordinates1.X = xPos;
	coordinates1.Y = yPos;

	COORD coordinates2;
	coordinates2.X = xPos + width + 1;
	coordinates2.Y = yPos;

	for (unsigned int i = 0; i <= height + 1; i++) {

		CursorPos(coordinates1);
		cout << "|";
		coordinates1.Y += 1;

		CursorPos(coordinates2);
		coordinates2.Y += 1;
		cout << "|";
	}

	coordinates1.X = xPos + 1;
	coordinates1.Y = yPos;

	coordinates2.X = xPos + 1;
	coordinates2.Y = yPos + height + 1;

	const string equalSigns{ string(width, '=') };

		CursorPos(coordinates1);
		cout << equalSigns;

		CursorPos(coordinates2);
		cout << equalSigns;
}
void PrintText(const unsigned int &xPos, const unsigned int &yPos, const string &text) {
	CursorPos(xPos, yPos);
	cout << text;
}
void PrintText(const unsigned int &xPos, const unsigned int &yPos, const string &text, const int &variable) {
	CursorPos(xPos, yPos);
	cout << text << variable;
}
void PrintText(const unsigned int &xPos, const unsigned int &yPos, const string &text1, const string &text2) {
	CursorPos(xPos, yPos);
	cout << text1 << text2;
}
void DisplayGfx (const unsigned int &xPos, const unsigned int &yPos, const vector<string> &gfx) {
	for (unsigned int i = 0; i < gfx.size(); i++) {
		CursorPos(xPos, yPos + i);
		cout << gfx[i];
	}
}