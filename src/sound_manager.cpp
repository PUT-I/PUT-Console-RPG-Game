#include "sound_manager.hpp"

#include "character.hpp"
#include "global_variables.hpp"

using namespace std;
using namespace sf;

std::map<std::string, std::string> sound_manager::change_sound_paths_ = {
	{"arrow1", "GameFiles\\Sounds\\UI\\Change1.ogg"},
	{"arrow2", "GameFiles\\Sounds\\UI\\Change2.ogg"},
	{"arrow3", "GameFiles\\Sounds\\UI\\Change3.ogg"},
	{"arrow4", "GameFiles\\Sounds\\UI\\Change4.ogg"}
};

std::map<sound_manager::sounds_enum, std::string> sound_manager::sound_paths_ = {
	{arrow, "GameFiles\\Sounds\\UI\\Arrows.ogg"},
	{enter, "GameFiles\\Sounds\\UI\\Enter.ogg"},
	{environment, "Sounds\\Environment\\"},
	{character, "Sounds\\Characters\\"},
	{creature, "Sounds\\Creatures\\"},
	{creature_hit, "Sounds\\Fight\\Creatures\\"},
	{miss, "Sounds\\Fight\\Miss1.ogg"},
	{evasion, "Sounds\\Fight\\Evasion1.ogg"},
	{defend, "Sounds\\Fight\\Defend1.ogg"},
	{spell, "Sounds\\Items\\Spells\\"},
	{weapon, "Sounds\\Items\\Weapons\\"},
	{misc, "Sounds\\Misc\\"},
	{consumable, "Sounds\\Consumables\\"},
	{armor, "Sounds\\Armors\\"}
};

// Constructor ----------------------------
sound_manager::sound_manager()
{
	ambience_.setLoop(true);
	main_menu_.setLoop(true);
	queue_manager_ = thread(&sound_manager::manage_sound_lists, this);
	queue_manager_.detach();
}

void sound_manager::clear_sound_list()
{
	mutex_.lock();
	sounds_list_.clear();
	mutex_.unlock();
}

// Functions ------------------------------
void sound_manager::manage_sound_lists()
{
	const chrono::milliseconds timespan(500);
	while (true)
	{
		this_thread::sleep_for(timespan);
		mutex_.lock();
		sounds_list_.remove_if([](const pair<SoundBuffer, Sound>& elem)
		{
			return elem.second.getStatus() != SoundSource::Status::Playing;
		});
		mutex_.unlock();
	}
}

// UI -------------------------------------
void sound_manager::play_sound(const sounds_enum& selection, const std::string& type)
{
	if (!enabled_) { return; }

	SoundBuffer temp_buffer;
	if (selection == random_change)
	{
		const int rand_sound = rand_int(1, 4);
		temp_buffer.loadFromFile(change_sound_paths_["arrow" + to_string(rand_sound)]);
	}
	else if (selection == enter || selection == arrow)
	{
		temp_buffer.loadFromFile(sound_paths_[selection]);
	}
	else
	{
		string sound_path = "GameFiles\\Scenarios\\" + current_scenario + "\\Resources\\";
		sound_path += sound_paths_[selection];
		if (!type.empty()) { sound_path += type + ".ogg"; }
		temp_buffer.loadFromFile(sound_path);
	}

	mutex_.lock();
	sounds_list_.emplace_back(pair(temp_buffer, Sound()));
	sounds_list_.back().second = Sound(sounds_list_.back().first);
	sounds_list_.back().second.play();
	mutex_.unlock();
}

void sound_manager::play_sound(const sounds_enum& selection)
{
	play_sound(selection, "");
}

void sound_manager::play_arrow_sound()
{
	play_sound(arrow);
}

void sound_manager::play_arrow_sound(const unsigned int& choice)
{
	if (choice >= 0) { play_arrow_sound(); }
}

void sound_manager::play_arrow_sound(const unsigned int& choice, const unsigned int& ceil)
{
	if (choice < ceil) { play_arrow_sound(choice); }
}

// Characters and Enemies -----------------
void sound_manager::play_creature_sound(const string& creature_name, const string& selection)
{
	string type = creature_name;
	if (!selection.empty()) { type += "\\" + selection; }
	play_sound(creature, type);
}

// Music ---------------------------------
void sound_manager::ambience(const string& selection, const bool& play)
{
	if (enabled_ && selection != current_ambience_)
	{
		current_ambience_ = selection;
		if (selection == "NONE")
		{
			ambience(false);
		}
		else
		{
			ambience_.openFromFile(
				"GameFiles\\Scenarios\\" + current_scenario + "\\Resources\\Sounds\\Ambient\\" + selection + ".ogg");
		}
		ambience(play);
	}
}

void sound_manager::ambience(const bool& play)
{
	if (ambience_.getVolume() == 0) { return; }

	if (enabled_ && play)
	{
		if (ambience_.getStatus() != SoundSource::Status::Playing)
		{
			ambience_.play();
		}
	}
	else { ambience_.pause(); }
}

void sound_manager::ambience(float volume)
{
	if (volume > 100) { volume = 100; }
	else if (volume < 0) { volume = 0; }
	ambience_.setVolume(volume);
}

void sound_manager::main_menu(const bool& play)
{
	if (enabled_ && play)
	{
		if (main_menu_.getStatus() != SoundSource::Status::Playing) { main_menu_.play(); }
	}
	else { main_menu_.pause(); }
}

void sound_manager::main_menu_reload()
{
	main_menu_.openFromFile("GameFiles\\Scenarios\\" + current_scenario + "\\Resources\\Sounds\\MainMenu.ogg");
}

const bool& sound_manager::get_enabled() const
{
	return enabled_;
}

// Modifiers -----------------------------
void sound_manager::set_enabled(const bool& enabled)
{
	enabled_ = enabled;
}
