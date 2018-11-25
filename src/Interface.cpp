#include "Interface.hpp"
#include "CManip.hpp"
#include "Sounds.hpp"

using namespace std;

const vector<string> lines{ 20, "|" };
const vector<string> locationTextBlank{ 20,  string(51, ' ') };
const vector<string> locationOptionsBlank{ 5,  string(81, ' ') };
const string equalSigns{ string(81, '=') };

//Controls -------------------------------
void choiceLimit(unsigned int& choice, const unsigned int& ceil) noexcept {
	if (choice >= ceil) choice = ceil-1;
}
const bool upDownControls(unsigned int &choice, const unsigned int &ceil) {
	do {
		ClearConsoleInputBuffer();
		if (CheckDownArrow()) {
			choice++;
			audio.arrow(choice, ceil);
			return false;
		}
		else if (CheckUpArrow() && choice > 0) {
			choice--;
			audio.arrow(choice);
			return false;
		}
	} while (!CheckEnter());
	audio.enter();
	//audio.setVolume("enter", 100);
	return true;
}
const int upDownEscControls(unsigned int &choice, const unsigned int &ceil) {
	do {
		ClearConsoleInputBuffer();
		if (CheckDownArrow()) {
			choice++;
			audio.arrow(choice, ceil);
			return 0;
		}
		else if (CheckUpArrow() && choice > 0) {
			choice--;
			audio.arrow(choice);
			return 0;
		}
		else if (CheckEscape()) {
			return -1;
		}
	} while (!CheckEnter());
	audio.enter();
	return 1;
}
const bool leftRightControls(bool &yesNo) {
	do {
		ClearConsoleInputBuffer();
		if (CheckLeftArrow()) {
			audio.arrow();
			yesNo = false;
			return false;
		}
		else if (CheckRightArrow()) {
			audio.arrow();
			yesNo = true;
			return false;
		}
	} while (!CheckEnter());
	audio.enter();
	return true;
}
const int upDownLeftRightControls(unsigned int &choice, const unsigned int &ceil, int &points, const int &pointsPrev, const vector<int*> &variables, const vector<int> &variablesPrev, const int &inc) {
	do {
		ClearConsoleInputBuffer();
		if (CheckDownArrow()) {
			choice++;
			audio.arrow(choice, ceil);
			return false;
		}
		else if (CheckUpArrow() && choice > 0) {
			choice--;
			audio.arrow(choice);
			return false;
		}
		else if (choice < ceil-1) {
			if (points > 0 && CheckRightArrow()) {
				audio.randChangeSound();
				*(variables[choice]) += inc;
				points--;
				return 0;
			}
			else if (CheckLeftArrow()) {
				if (*(variables[choice]) > variablesPrev[choice]) {
					audio.randChangeSound();
					*(variables[choice]) -= inc;
					points++;
					return 0;
				}
			}
		}	
	} while (!CheckEnter() || choice != ceil-1);
	audio.enter();
	if (points != 0) {
		return -1;
	}
	else return 1;
}
const bool upDownLeftRightControls(unsigned int &choice, const unsigned int &ceil, const vector<int*> &variables, const vector<int> &limits) {
	do {
		ClearConsoleInputBuffer();
		if (CheckDownArrow()) {
			choice++;
			audio.arrow(choice, ceil);
			return false;
		}
		else if (CheckUpArrow() && choice > 0) {
			choice--;
			audio.arrow(choice);
			return false;
		}
		else if (choice > 0) {
		if (choice < ceil && *variables[choice - 1] < limits[choice-1] && CheckRightArrow()) {
			audio.randChangeSound();
			*(variables[choice - 1]) += 1;
			return false;
		}
		else if (choice < ceil && *variables[choice - 1] > 0 && CheckLeftArrow()) {
			audio.randChangeSound();
			*(variables[choice - 1]) -= 1;
			return false;
		}
		}
	} while (!CheckEnter() || choice != 0);
	audio.enter();

	return true;
}
const int upDownLeftRightEscControls(unsigned int &choice, const unsigned int &ceil, const vector<int*> &variables, const vector<int> &limits) {
	do {
		ClearConsoleInputBuffer();
		if (CheckDownArrow()) {
			choice++;
			audio.arrow(choice, ceil);
			return 0;
		}
		else if (CheckUpArrow() && choice > 0) {
			choice--;
			audio.arrow(choice);
			return 0;
		}
		else if (choice > 0) {
			if (choice < ceil && *variables[choice - 1] < limits[choice - 1] && CheckRightArrow()) {
				audio.randChangeSound();
				*(variables[choice - 1]) += 1;
				return 0;
			}
			else if (choice < ceil && *variables[choice - 1] > 0 && CheckLeftArrow()) {
				audio.randChangeSound();
				*(variables[choice - 1]) -= 1;
				return 0;
			}
		}
		else if (CheckEscape()) {
			return -1;
		}
	} while (!CheckEnter() || choice != 0);
	audio.enter();

	return 1;
}

/*---------------------------------- Menus ----------------------------------*/
//Displaying -----------------------------
void displayOptionsNoNum(unsigned const int &choice, const vector<string> &options, const vector<int*> &variables) {
	unsigned int cursorY = GetCursorPos().Y - 1;	// -1 to make the in loop more readable

	//Displaying variables -------------------
	for (unsigned int i = 0; i < variables.size(); i++) {
		cursorY += 1;
		CursorPos(8, cursorY);
		cout << ((choice == i) ? "> " : "  ") << options[i] << *(variables[i]) << color.get("normal") << "    ";
	}
	
	//Displaying Other Options----------------
	if (options.size() > variables.size()) {
		cursorY += 1;	// + 1 to make a gap between options with varables and options without

		for (unsigned int i = variables.size(); i < options.size(); i++) {
			cursorY += 1;
			CursorPos(8, cursorY);
			cout << ((choice == i) ? "> " : "  ") << options[i];
		}
	}
}
void displayOptionsNoNum(unsigned const int &choice, const string &option, const vector<shared_ptr<Item>> &variables, const bool &quantity) {
	int PrevCursorX = GetCursorPos().X;
	//Displaying Option ----------------------
	CursorPos(PrevCursorX, GetCursorPos().Y + 1);
	cout << ((choice == 0) ? "> " : "  ") << option << '\n';

	//Displaying variables -------------------
	for (unsigned int i = 0; i < variables.size(); i++) {
		CursorPos(PrevCursorX, GetCursorPos().Y + 1);
		cout << ((choice == i + 1) ? "> " : "  ") << variables[i]->getName();
		if (quantity) {
			cout << " x" << variables[i]->getNumber();
		}
	}
}
void displayOptionsNoNum(unsigned const int &choice, const vector<string> &options) {
	//Displaying Options ---------------------
	for (unsigned int i = 0; i < options.size(); i++) {
		cout << '	' << ((choice == i) ? "> " : "  ") << options[i] << '\n';
	}
}
void displayOptionsNoNum(unsigned const int &choice, const vector<string> &options, const int &xPos, const int &yPos) {
	//Displaying Options ---------------------
	for (unsigned int i = 0; i < options.size(); i++) {
		CursorPos(xPos, yPos + i);
		cout << ((choice == i) ? "> " : "  ") << options[i];
	}
}
void displayOptionsMenu (unsigned const int &choice, const string &title, const vector<string> &options) {
	if (choice >= 0 && choice < options.size()) {

		//Displaying Title -----------------------
		PrintText(28 - title.size() / 2, 0, title);

		//Displaying Options ---------------------
		for (unsigned int i = 0; i < options.size(); i++) {
			CursorPos(8, 4 + 2 * i);
			cout << ((choice == i) ? "> " : "  ") << i + 1 << ". " << options[i] << "   ";
		}
	}
}
void displayOptionsMenu (unsigned const int &choice, const vector<string> &options) {
	if (choice >= 0 && choice < options.size()) {
		//Displaying Options ---------------------
		for (unsigned int i = 0; i < options.size(); i++) {
			CursorMove(1, 0);
			cout << ((choice == i) ? "> " : "  ") << i + 1 << ". " << options[i] << "    \n";
		}
	}
}

//Displaying with controls----------------
void optionsMenu(unsigned int &choice, const string &title, const string &option, const vector<string>&list, const bool &escEnabled) {
	int action;

	do {
		if (choice >= 0 && choice <= list.size()) {

			//Displaying Title -----------------------
			PrintText(28 - title.size() / 2, 0, title);

			//Displaying Option ----------------------
			CursorPos(8, 4);
			cout << (choice == 0 ? ">" : " ") << " " << option;

			//Displaying List ------------------------
			for (unsigned int i = 0; i < list.size(); i++) {
				CursorPos(8, 4 + 2 * (i + 1));
				cout << ((choice == i + 1) ? "> " : "  ") << i + 1 << ". " << list[i];
			}
		}
		choiceLimit(choice, (int)list.size()+1);
		action = upDownEscControls(choice, (int)list.size() + 1);
		if (action == -1 && !escEnabled) {
			action = 0;
		}
	} while (action == 0);

	if (action == -1) {
		choice = 0;
	}
}
void optionsMenu(unsigned int &choice, const string &title, const vector<string> &options, const bool &escEnabled) {
	int action;

	do {
		if (choice >= 0 && choice < options.size()) {

			//Displaying Title -----------------------
			PrintText(28 - title.size() / 2, 0, title);

			//Displaying Options ---------------------
			for (unsigned int i = 0; i < options.size(); i++) {
				CursorPos(8, 4 + 2*i);
				cout << ((choice == i) ? "> " : "  ") << i + 1 << ". " << options[i];
			}
		}
		choiceLimit(choice, (int)options.size());
		action = upDownEscControls(choice, (int)options.size());
		if (action == -1 && !escEnabled) {
			action = 0;
		}
	} while (action == 0);

	if (action == -1 && escEnabled) {
		choice = 0;
	}
}
const bool confirmationMenu(const string &question) {
	bool yesNo = false;
	COORD prevCursorPos = GetCursorPos();

	do {
		
		//Displaying Question --------------------
		CursorPos(prevCursorPos);
		cout << question;

		//Displaying Options ---------------------
		CursorPos(prevCursorPos.X, prevCursorPos.Y + 2);
		CursorMove(5, 0);
		cout << (yesNo == false ? "| " : "  ") << sLineTexts["No"] << (yesNo == false ? " |" : "  ");
		CursorMove(8, 0);
		cout << (yesNo == true ? "| " : "  ") << sLineTexts["Yes"] << (yesNo == true ? " |" : "  ");
		

	} while (!leftRightControls(yesNo));

	return yesNo;
}