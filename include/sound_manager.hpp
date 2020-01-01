#pragma once

#include <list>
#include <map>
#include <mutex>
#include <thread>

#include "SFML/Audio.hpp"

int rand_int(const int& min, const int& max);

class sound_manager
{
	// Enums
public:
	enum sounds_enum
	{
		arrow,
		enter,
		random_change,
		environment,
		character,
		creature,
		creature_hit,
		miss,
		evasion,
		defend,
		spell,
		weapon,
		misc,
		consumable,
		armor
	};

private:
	bool enabled_ = true;

	// UI Sounds  -----------------------------
	std::list<std::pair<sf::SoundBuffer, sf::Sound>> sounds_list_{};

	// Music ----------------------------------
	std::string current_ambience_{};
	sf::Music ambience_{};
	sf::Music main_menu_{};

	static std::map<std::string, std::string> change_sound_paths_;
	static std::map<sounds_enum, std::string> sound_paths_;

	std::mutex mutex_{};
	std::thread queue_manager_{};

	// Constructor and a Destructor -----------
public:
	sound_manager();
	~sound_manager() = default;
	void clear_sound_list();

	// Functions ------------------------------
private:
	void manage_sound_lists();

public:
	// Arrows ---------------------------------
	void play_sound(const sounds_enum& selection, const std::string& type);
	void play_sound(const sounds_enum& selection);

	void play_arrow_sound();
	void play_arrow_sound(const unsigned int& choice);
	void play_arrow_sound(const unsigned int& choice, const unsigned int& ceil);

	// Characters, Creatures and Fight --------
	void play_creature_sound(const std::string& creature_name, const std::string& selection);

	// Music ---------------------------------
	void ambience(const std::string& selection, const bool& play);
	void ambience(const bool& play);
	void ambience(float volume);
	void main_menu(const bool& play);
	void main_menu_reload();

	// Accessors -----------------------------
	const bool& get_enabled() const;

	// Modifiers -----------------------------
	void set_enabled(const bool& enabled);
};
