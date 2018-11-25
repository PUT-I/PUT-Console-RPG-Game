#pragma once

#include "FrequentHeaders.h"
#include "Items.hpp"

extern const std::vector<std::string> lines;
extern const std::vector<std::string> locationTextBlank;
extern const std::vector<std::string> locationOptionsBlank;
extern const std::string equalSigns;

//Controls -------------------------------
void choiceLimit(unsigned int& choice, const unsigned int& ceil) noexcept;
const bool upDownControls(unsigned int &choice, const unsigned int &ceil);
const bool leftRightControls(bool &yesNo);
const int upDownEscControls(unsigned int &choice, const unsigned int &ceil);
const int upDownLeftRightControls(unsigned int &choice, const unsigned int &ceil, int &points, const int &pointsPrev, const std::vector<int*> &variables, const std::vector<int> &variablesPrev, const int &inc);
const bool upDownLeftRightControls(unsigned int &choice, const unsigned int &ceil, const std::vector<int*> &variables, const std::vector<int> &limits);
const int upDownLeftRightEscControls(unsigned int &choice, const unsigned int &ceil, const std::vector<int*> &variables, const std::vector<int> &limits);

/*---------------------------------- Menus ----------------------------------*/
//Displaying -----------------------------
void displayOptionsNoNum(const unsigned int &choice, const std::vector<std::string> &options, const std::vector<int*> &variables);
void displayOptionsNoNum(const unsigned int &choice, const std::string &option, const std::vector<std::shared_ptr<Item>> &variables, const bool &quantity);
void displayOptionsNoNum(const unsigned int &choice, const std::vector<std::string> &options);
void displayOptionsNoNum(const unsigned int &choice, const std::vector<std::string> &options, const int &xPos, const int &yPos);
void displayOptionsMenu(const unsigned int &choice, const std::string &title, const std::vector<std::string> &options);
void displayOptionsMenu(unsigned const int &choice, const std::vector<std::string> &options);

//Displaying with controls----------------
void optionsMenu(unsigned int &choice, const std::string &title, const std::string &option, const std::vector<std::string>&list, const bool &escEnabled);
void optionsMenu(unsigned int &choice, const std::string &title, const std::vector<std::string> &options, const bool &escEnabled);
const bool confirmationMenu(const std::string &question);