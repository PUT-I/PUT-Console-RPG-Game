#pragma once

#include "FrequentHeaders.h"
#include "CManip.hpp"
#include "Sounds.hpp"
#include <direct.h>

std::string utf8_to_ansi(const std::string &str_, const std::vector<std::string> &vUtf, const std::vector<std::string> &vAnsi);
std::vector<std::string> utf8_to_ansi(const std::vector<std::string> &vStr_, const std::vector<std::string> &vUtf, const std::vector<std::string> &vAnsi);

namespace files {
	//Loading txt ----------------------------
	const std::vector<std::string> loadGfx(const std::string &dir);
	const std::vector<std::string> loadGfx(const std::vector<std::string> &input);
	const std::string proccessTxtLine(const std::string &line);
	const std::vector<std::string>loadTxt(const std::string &dir);
	const std::string loadLine(const std::string &dir);

	//Directories and Files ------------------
	void loadLocalization();
	const bool exists(const std::string &path);
	void deleteAllSaves();
	void createSaveDirectory();
}