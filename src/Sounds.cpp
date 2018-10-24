#include "Sounds.hpp"

using namespace std;
using namespace sf;

int randInt(const int &min, const int &max) {
	if (max <= min) return min;
	random_device rd;
	mt19937 gen(rd());
	const uniform_int_distribution<int > d(min, max);
	return d(gen);
}

//Constructor ----------------------------
Audio::Audio() {
	ambience_.setLoop(true);
	mainMenu_.setLoop(true);
	queueMng = thread(&Audio::manageQueues, this);
	queueMng.detach();
}

//Functions ------------------------------
void Audio::manageQueues() {
	chrono::milliseconds timespan(1000);
	while (true) {
		this_thread::sleep_for(timespan);
		if (ui_.size() != 0){
			while (ui_.size() != 0) {
				if (ui_.front().getStatus() != SoundSource::Status::Stopped) break;
				mutex_.lock();
				uiBuffers.pop();
				ui_.pop();
				mutex_.unlock();
			}
		}
		if (inventory_.size() != 0) {
			while (inventory_.size() != 0) {
				if (inventory_.front().getStatus() != SoundSource::Status::Stopped) break;
				mutex_.lock();
				inventoryBuffers.pop();
				inventory_.pop();
				mutex_.unlock();
			}
		}
		if (other_.size() != 0) {
			while (other_.size() != 0) {
				if (other_.front().getStatus() != SoundSource::Status::Stopped) break;
				mutex_.lock();
				otherBuffers.pop();
				other_.pop();
				mutex_.unlock();
			}
		}
	}
}

//UI -------------------------------------
void Audio::ui(const string &selection) {
	if (inventory_.size() > 0) {
		if (inventory_.front().getStatus() == SoundSource::Status::Playing
			|| inventory_.back().getStatus() == SoundSource::Status::Playing) { return; }
	}

	SoundBuffer tempBuffer;
	if (selection == "arrow") { tempBuffer.loadFromFile("GameFiles\\Sounds\\UI\\Arrows.ogg"); }
	else if (selection == "enter") { tempBuffer.loadFromFile("GameFiles\\Sounds\\UI\\Enter.ogg"); }
	else if (selection == "randChangeSound") {
			const int randSound = randInt(0, 3);
			switch (randSound) {
				case 0: {
						tempBuffer.loadFromFile("GameFiles\\Sounds\\UI\\Change1.ogg");
						break;
					}
				case 1: {
						tempBuffer.loadFromFile("GameFiles\\Sounds\\UI\\Change2.ogg");
						break;
					}
				case 2: {
						tempBuffer.loadFromFile("GameFiles\\Sounds\\UI\\Change3.ogg");
						break;
					}
				case 3: {
						tempBuffer.loadFromFile("GameFiles\\Sounds\\UI\\Change4.ogg");
						break;
					}
			}
	}

	mutex_.lock();
	uiBuffers.push(tempBuffer);
	ui_.push(Sound(uiBuffers.back()));
	ui_.back().play();
	mutex_.unlock();
	return;
}
void Audio::arrow(){
	if (enabled) {
		ui("arrow");
	}
}
void Audio::arrow(const unsigned int &choice){
	if (choice >= 0) arrow();
}
void Audio::arrow(const unsigned int &choice, const unsigned int & ceil){
	if (choice < ceil) arrow(choice);
}
void Audio::enter(){
	if (enabled) {
		ui("enter");
	}
}
void Audio::randChangeSound(){
	if (enabled) {
		ui("randChangeSound");
	}
}

//Characters and Enemies -----------------
void Audio::otherClear() {
	mutex_.lock();
	while (other_.empty() == false) { other_.pop(); }
	while (otherBuffers.empty() == false) { otherBuffers.pop(); }
	mutex_.unlock();
}
void Audio::other(const string &selection, const string &type) {
	SoundBuffer tempBuffer;
	String scenarioDir = "GameFiles\\Scenarios\\" + currentScenario + "\\Resources\\";

	if (selection == "enviroment")		{ tempBuffer.loadFromFile(scenarioDir + "Sounds\\Enviroment\\" + type + ".ogg"); }
	else if (selection == "character")	{ tempBuffer.loadFromFile(scenarioDir + "Sounds\\Characters\\" + type + ".ogg"); }
	else if (selection == "creature")	{ tempBuffer.loadFromFile(scenarioDir + "Sounds\\Creatures\\" + type + ".ogg"); }
	else if (selection == "creatureHit"){ tempBuffer.loadFromFile(scenarioDir + "Sounds\\Fight\\Creatures\\" + type + ".ogg"); }
	else if (selection == "character")	{ tempBuffer.loadFromFile(scenarioDir + "Sounds\\Characters\\" + type + ".ogg"); }
	else if (selection == "miss")		{ tempBuffer.loadFromFile(scenarioDir + "Sounds\\Fight\\Miss1.ogg"); }
	else if (selection == "evasion")	{ tempBuffer.loadFromFile(scenarioDir + "Sounds\\Fight\\Evasion1.ogg"); }
	else if (selection == "defend")		{ tempBuffer.loadFromFile(scenarioDir + "Sounds\\Fight\\Defend1.ogg"); }
	else if (selection == "spell")		{ tempBuffer.loadFromFile(scenarioDir + "Sounds\\Items\\Spells\\" + type + ".ogg"); }
	else if (selection == "weapon")		{ tempBuffer.loadFromFile(scenarioDir + "Sounds\\Items\\Weapons\\" + type + ".ogg"); }
	else if (selection == "misc")		{ tempBuffer.loadFromFile(scenarioDir + "Sounds\\Misc\\" + type + ".ogg"); }

	mutex_.lock();
	otherBuffers.push(tempBuffer);
	other_.push(Sound(otherBuffers.back()));
	other_.back().play();
	mutex_.unlock();
	return;
}
void Audio::enviroment(const string &type) {
	if (enabled) {
		other("enviroment", type);
	}
}
void Audio::miss() {
	if (enabled) { other("miss", ""); }
}
void Audio::evasion() {
	if (enabled) { other("evasion", ""); }
}
void Audio::defend() {
	if (enabled) { other("defend", ""); }
}
void Audio::weapon(const string &type) {
	if (enabled) { other("weapon", type); }
}
void Audio::creature(const string &creature, const string &selection) {
	if (enabled) {
		if (selection == "hit") { other("creatureHit", creature); }
		else { other("creature", creature + "\\" + selection); }
	}
}
void Audio::character(const string &selection) {
	if (enabled) { other("character", selection); }
}

//Inventory ------------------------------
void Audio::inventory(const string &selection, const string &type) {
	SoundBuffer tempBuffer;
	String scenarioDir = "GameFiles\\Scenarios\\" + currentScenario + "\\Resources\\";

	if (selection == "consumable") { tempBuffer.loadFromFile(scenarioDir + "Sounds\\Items\\Consumables\\" + type+ ".ogg"); }
	else if (selection == "armor") { tempBuffer.loadFromFile(scenarioDir + "Sounds\\Items\\Armors\\" + type + ".ogg"); }
	else if (selection == "spell") { tempBuffer.loadFromFile(scenarioDir + "Sounds\\Items\\Spells\\" + type + ".ogg"); }

	mutex_.lock();
	inventoryBuffers.push(tempBuffer);
	inventory_.push(Sound(inventoryBuffers.back()));
	inventory_.back().play();
	mutex_.unlock();
	return;
}
void Audio::spell(const string &type) {
	if (enabled) {
		inventory("spell", type);
	}
}
void Audio::armor(const string &type) {
	if (enabled) {
		inventory("armor", type);
	}
}
void Audio::consumable(const string &type) {
	if (enabled) {
		inventory("consumable", type);
	}
}

//Music ---------------------------------
void Audio::ambience(const string &selection, const bool &play) {
	if (enabled && selection != currentAmbience) {
		currentAmbience = selection;
		if (selection == "NONE") {
			ambience(false);
		}
		else ambience_.openFromFile("GameFiles\\Scenarios\\" + currentScenario + "\\Resources\\Sounds\\Ambient\\" + selection + ".ogg");
		ambience(play);
	}
}
void Audio::ambience(const bool &play) {
	if (ambience_.getVolume() == 0) return;
	else if (enabled && play) {
		if(ambience_.getStatus() != SoundSource::Status::Playing) {
			ambience_.play();
		}
	}
	else ambience_.pause();
}
void Audio::ambience(float volume) {
	if (volume > 100) { volume = 100; }
	else if (volume < 0) { volume = 0; }
	ambience_.setVolume(volume);
}
void Audio::mainMenu(const bool &play){
	if (enabled && play) {
		if(mainMenu_.getStatus() != SoundSource::Status::Playing) mainMenu_.play();
	}
	else mainMenu_.pause();
}
void Audio::mainMenuReload() {
	mainMenu_.openFromFile("GameFiles\\Scenarios\\" + currentScenario + "\\Resources\\Sounds\\MainMenu.ogg");
}

void Audio::misc(const string & selection) {
	if (enabled) { other("misc", selection); }
}

//Modifiers -----------------------------
void Audio::setEnabled(const bool &yesNo) noexcept {
	if (yesNo != enabled) {
		enabled = yesNo;
	}
}

Audio audio;