#pragma once

#include "FrequentHeaders.h"
#include "Global.hpp"
#include "Sounds.hpp"
#include <Windows.h>

extern int globalFontSize, storyFontInc;
extern std::string language, codePage;

//Colors --------------------------------
class Color {
private:
	bool greyscale = false;
	const std::string red     { 0x1b, '[', '1', ';', '3', '1', 'm' };
	const std::string green   { 0x1b, '[', '1', ';', '9', '2', 'm' };
	const std::string blue    { 0x1b, '[', '1', ';', '3', '4', 'm' };
	const std::string yellow  { 0x1b, '[', '1', ';', '3', '3', 'm' };
	const std::string grey	  { 0x1b, '[', '1', ';', '3', '0', 'm' };
	const std::string normal  { 0x1b, '[', '0', ';', '3', '9', 'm' };

	//Construnctors and a destructor ---------
public:
	Color() noexcept {}
	~Color() {}

	//Modifiers ------------------------------
	void setGreyscale(const bool &yesNo) noexcept { greyscale = yesNo; }

	//Accessors ------------------------------
	const std::string& get(const std::string &choice) const noexcept {
			 if (choice == "grey"	 || choice == "5") return grey;
		else if (choice == "normal"  || choice == "0" || greyscale == true) return normal;
		else if (choice == "red"     || choice == "1") return red;
		else if (choice == "green"   || choice == "2") return green;
		else if (choice == "blue"    || choice == "3") return blue;
		else if (choice == "yellow"  || choice == "4") return yellow;
		else return normal;
	}
	const bool& getGreyscale() const noexcept { return greyscale; }
};

extern Color color;



//Checking Keyboard Input ---------------
const bool CheckEnter() noexcept;
const bool CheckSpace() noexcept;
const bool CheckEscape() noexcept;
const bool CheckLeftArrow() noexcept;
const bool CheckRightArrow() noexcept;
const bool CheckUpArrow() noexcept;
const bool CheckDownArrow() noexcept;
const bool CheckAlt() noexcept;
void CheckAltF4() noexcept;

//Console Manipulation ------------------
void setFontSize(const int &FontSize) noexcept;
void ClearConsoleInputBuffer();
void ShowConsoleCursor(const bool &showFlag) noexcept;
void ClearConsole() noexcept;

void InputString(std::string &str, const unsigned int &limit);
const std::pair<int, int> GetDesktopResolution() noexcept;
void Fullscreeen(const bool &yesNo) noexcept;

const COORD GetCursorPos() noexcept;
void CursorPos(const COORD &c) noexcept;
void CursorPos(const unsigned int &x, const unsigned int &y) noexcept;
void CursorPosBegin() noexcept;
void CursorMove(const int &movX, const int &movY) noexcept;

void SpacePause();

void adaptLocationFontSize() noexcept;
void adaptFontSize();

//GFX -----------------------------------
void PrintBox (const unsigned int &xPos, const unsigned int &yPos, const unsigned int &width, const unsigned int &height);
void PrintText(const unsigned int &xPos, const unsigned int &yPos, const std::string &text);
void PrintText(const unsigned int &xPos, const unsigned int &yPos, const std::string &text, const int &variable);
void PrintText(const unsigned int &xPos, const unsigned int &yPos, const std::string &text1, const std::string &text2);
void DisplayGfx(const unsigned int &xPos, const unsigned int &yPos, const std::vector<std::string> &gfx);