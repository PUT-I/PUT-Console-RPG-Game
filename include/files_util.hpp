#pragma once

#include "global_headers.h"
#include "item.hpp"

std::string utf8_to_ansi(std::string str, const std::vector<std::string>& utf_vector,
                         const std::vector<std::string>& ansi_vector);
std::vector<std::string> utf8_to_ansi(std::vector<std::string> string_vector,
                                      const std::vector<std::string>& utf_vector,
                                      const std::vector<std::string>& ansi_vector);

namespace files
{
	// Loading txt ----------------------------
	const std::vector<std::string> load_gfx(const std::string& dir);
	const std::vector<std::string> load_gfx(const std::vector<std::string>& input);
	const std::string proccess_txt_line(const std::string& line);
	const std::vector<std::string> load_txt(const std::string& dir);
	const std::string load_line(const std::string& dir);

	// Directories and Files ------------------
	void load_localization();
	const bool exists(const std::string& path);
	void delete_all_saves();
	void create_save_directory();

	// Saving and loading collections ---------
	void save(std::ofstream& out, const std::vector<std::shared_ptr<item>>& inv);

	template <class T>
	void load_map(const json& item_tab, std::map<std::string, T>& map)
	{
		std::shared_ptr<item> item = std::make_shared<T>();
		map.clear();

		for (const json& e : item_tab["tab"])
		{
			item->load(e["itemInfo"]);
			map[e["key"]] = *std::dynamic_pointer_cast<T>(item);
		}
	}

	const std::vector<std::shared_ptr<item>> load(std::ifstream& in);
	const std::vector<std::shared_ptr<item>> load(const std::string& file_dir, const std::string& which);

	void load_graphics();
}
