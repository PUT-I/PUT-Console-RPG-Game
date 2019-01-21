#pragma once

#include "FrequentHeaders.h"
#include "SFML/Audio.hpp"

int randInt(const int &min, const int &max);

class Audio {
private:
	bool enabled = true;

	//UI Sounds  -----------------------------
	std::queue<sf::SoundBuffer> uiBuffers{};
	std::queue<sf::Sound> ui_{};

	//Characters, Creatures and Fight --------
	std::queue<sf::SoundBuffer> otherBuffers{};
	std::queue<sf::Sound> other_{};

	//Inventory ------------------------------
	std::queue<sf::SoundBuffer> inventoryBuffers{};
	std::queue<sf::Sound> inventory_{};

	//Music ----------------------------------
	std::string currentAmbience{};
	sf::Music ambience_{};
	sf::Music mainMenu_{};

	std::mutex mutex_{};
	std::thread queueMng{};

public:
	//Constructor and a Destructor -----------
	Audio();
	~Audio() = default;

	//Functions ------------------------------
private:
	void manageQueues();

	//Arrows ---------------------------------
	void ui(const std::string &selection);
public:
	void arrow();
	void arrow(const unsigned int& choice);
	void arrow(const unsigned int& choice, const unsigned int &ceil);

	//Enter ---------------------------------
	void enter();

	//Change --------------------------------
	void randChangeSound();

	//Characters, Creatures and Fight --------
	void otherClear();
	void other(const std::string &selection, const std::string &type);
	void enviroment(const std::string &type);
	void creature(const std::string &creature, const std::string &selection);
	void character(const std::string &selection);

	void miss();
	void evasion();
	void defend();
	void weapon(const std::string &type);
	

	//Inventory ------------------------------
private:
	void inventory(const std::string &selection, const std::string &type);
public:
	void spell(const std::string &type);
	void armor(const std::string &type);
	void consumable(const std::string &type);

	//Music ---------------------------------
	void ambience(const std::string &selection, const bool &play);
	void ambience(const bool &play);
	void ambience(float volume);
	void mainMenu(const bool &play);
	void mainMenuReload();

	void misc(const std::string &selection);

	//Accessors -----------------------------
	const bool& getEnabled() const noexcept { return enabled; }

	//Modifiers -----------------------------
	void setEnabled(const bool &yesNo) noexcept;
};

extern Audio audio;