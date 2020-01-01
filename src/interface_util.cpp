#include "interface_util.hpp"

#include "color_util.hpp"
#include "console_util.hpp"
#include "global_variables.hpp"
#include "sound_manager.hpp"

using namespace std;

// Controls -------------------------------
void choice_limit(unsigned int& choice, const unsigned int& ceil) noexcept
{
	if (choice >= ceil) choice = ceil - 1;
}

const bool up_down_controls(unsigned int& choice, const unsigned int& ceil)
{
	do
	{
		clear_console_input_buffer();
		if (check_down_arrow())
		{
			choice++;
			sounds.play_arrow_sound(choice, ceil);
			return false;
		}
		else if (check_up_arrow() && choice > 0)
		{
			choice--;
			sounds.play_arrow_sound(choice);
			return false;
		}
	}
	while (!check_enter());
	sounds.play_sound(sound_manager::enter);
	// sounds.setVolume("enter", 100);
	return true;
}

const int up_down_esc_controls(unsigned int& choice, const unsigned int& ceil)
{
	do
	{
		clear_console_input_buffer();
		if (check_down_arrow())
		{
			choice++;
			sounds.play_arrow_sound(choice, ceil);
			return 0;
		}
		else if (check_up_arrow() && choice > 0)
		{
			choice--;
			sounds.play_arrow_sound(choice);
			return 0;
		}
		else if (check_escape())
		{
			return -1;
		}
	}
	while (!check_enter());
	sounds.play_sound(sound_manager::enter);
	return 1;
}

const bool left_right_controls(bool& yes_no)
{
	do
	{
		clear_console_input_buffer();
		if (check_left_arrow())
		{
			sounds.play_arrow_sound();
			yes_no = false;
			return false;
		}
		else if (check_right_arrow())
		{
			sounds.play_arrow_sound();
			yes_no = true;
			return false;
		}
	}
	while (!check_enter());
	sounds.play_sound(sound_manager::enter);
	return true;
}

const int up_down_left_right_controls(unsigned int& choice, const unsigned int& ceil, int& points,
                                      const int& points_prev, const vector<int*>& variables,
                                      const vector<int>& variables_prev, const int& inc)
{
	do
	{
		clear_console_input_buffer();
		if (check_down_arrow())
		{
			choice++;
			sounds.play_arrow_sound(choice, ceil);
			return false;
		}
		else if (check_up_arrow() && choice > 0)
		{
			choice--;
			sounds.play_arrow_sound(choice);
			return false;
		}
		else if (choice < ceil - 1)
		{
			if (points > 0 && check_right_arrow())
			{
				sounds.play_sound(sound_manager::random_change);
				*(variables[choice]) += inc;
				points--;
				return 0;
			}
			else if (check_left_arrow())
			{
				if (*(variables[choice]) > variables_prev[choice])
				{
					sounds.play_sound(sound_manager::random_change);
					*(variables[choice]) -= inc;
					points++;
					return 0;
				}
			}
		}
	}
	while (!check_enter() || choice != ceil - 1);
	sounds.play_sound(sound_manager::enter);
	if (points != 0)
	{
		return -1;
	}
	else return 1;
}

const bool up_down_left_right_controls(unsigned int& choice, const unsigned int& ceil, const vector<int*>& variables,
                                       const vector<int>& limits)
{
	do
	{
		clear_console_input_buffer();
		if (check_down_arrow())
		{
			choice++;
			sounds.play_arrow_sound(choice, ceil);
			return false;
		}
		else if (check_up_arrow() && choice > 0)
		{
			choice--;
			sounds.play_arrow_sound(choice);
			return false;
		}
		else if (choice > 0)
		{
			if (choice < ceil && *variables[choice - 1] < limits[choice - 1] && check_right_arrow())
			{
				sounds.play_sound(sound_manager::random_change);
				*(variables[choice - 1]) += 1;
				return false;
			}
			else if (choice < ceil && *variables[choice - 1] > 0 && check_left_arrow())
			{
				sounds.play_sound(sound_manager::random_change);
				*(variables[choice - 1]) -= 1;
				return false;
			}
		}
	}
	while (!check_enter() || choice != 0);
	sounds.play_sound(sound_manager::enter);

	return true;
}

const int up_down_left_right_esc_controls(unsigned int& choice, const unsigned int& ceil, const vector<int*>& variables,
                                          const vector<int>& limits)
{
	do
	{
		clear_console_input_buffer();
		if (check_down_arrow())
		{
			choice++;
			sounds.play_arrow_sound(choice, ceil);
			return 0;
		}
		else if (check_up_arrow() && choice > 0)
		{
			choice--;
			sounds.play_arrow_sound(choice);
			return 0;
		}
		else if (choice > 0)
		{
			if (choice < ceil && *variables[choice - 1] < limits[choice - 1] && check_right_arrow())
			{
				sounds.play_sound(sound_manager::random_change);
				*(variables[choice - 1]) += 1;
				return 0;
			}
			else if (choice < ceil && *variables[choice - 1] > 0 && check_left_arrow())
			{
				sounds.play_sound(sound_manager::random_change);
				*(variables[choice - 1]) -= 1;
				return 0;
			}
		}
		else if (check_escape())
		{
			return -1;
		}
	}
	while (!check_enter() || choice != 0);
	sounds.play_sound(sound_manager::enter);

	return 1;
}

/*---------------------------------- Menus ----------------------------------*/
// Displaying -----------------------------
void display_options_no_num(unsigned const int& choice, const vector<string>& options, const vector<int*>& variables)
{
	unsigned int cursor_y = get_cursor_pos().Y - 1; // -1 to make the in loop more readable

	// Displaying variables -------------------
	for (unsigned int i = 0; i < variables.size(); i++)
	{
		cursor_y += 1;
		cursor_pos(8, cursor_y);
		cout << ((choice == i) ? "> " : "  ") << options[i] << *(variables[i]) << color.get("normal") << "    ";
	}

	// Displaying Other Options----------------
	if (options.size() > variables.size())
	{
		cursor_y += 1; // + 1 to make a gap between options with varables and options without

		for (unsigned int i = variables.size(); i < options.size(); i++)
		{
			cursor_y += 1;
			cursor_pos(8, cursor_y);
			cout << ((choice == i) ? "> " : "  ") << options[i];
		}
	}
}

void display_options_no_num(unsigned const int& choice, const string& option, const vector<shared_ptr<item>>& variables,
                            const bool& quantity)
{
	const int prev_cursor_x = get_cursor_pos().X;
	// Displaying Option ----------------------
	cursor_pos(prev_cursor_x, get_cursor_pos().Y + 1);
	cout << ((choice == 0) ? "> " : "  ") << option << '\n';

	// Displaying variables -------------------
	for (unsigned int i = 0; i < variables.size(); i++)
	{
		cursor_pos(prev_cursor_x, get_cursor_pos().Y + 1);
		cout << ((choice == i + 1) ? "> " : "  ") << variables[i]->get_name_localization();
		if (quantity)
		{
			cout << " x" << variables[i]->get_number();
		}
	}
}

void display_options_no_num(unsigned const int& choice, const vector<string>& options)
{
	// Displaying Options ---------------------
	for (unsigned int i = 0; i < options.size(); i++)
	{
		cout << '	' << ((choice == i) ? "> " : "  ") << options[i] << '\n';
	}
}

void display_options_no_num(unsigned const int& choice, const vector<string>& options, const int& x_pos,
                            const int& y_pos)
{
	// Displaying Options ---------------------
	for (unsigned int i = 0; i < options.size(); i++)
	{
		cursor_pos(x_pos, y_pos + i);
		cout << ((choice == i) ? "> " : "  ") << options[i];
	}
}

void display_options_menu(unsigned const int& choice, const string& title, const vector<string>& options)
{
	if (choice >= 0 && choice < options.size())
	{
		// Displaying Title -----------------------
		print_text(28 - title.size() / 2, 0, title);

		// Displaying Options ---------------------
		for (unsigned int i = 0; i < options.size(); i++)
		{
			cursor_pos(8, 4 + 2 * i);
			cout << ((choice == i) ? "> " : "  ") << i + 1 << ". " << options[i] << "   ";
		}
	}
}

void display_options_menu(unsigned const int& choice, const vector<string>& options)
{
	if (choice >= 0 && choice < options.size())
	{
		// Displaying Options ---------------------
		for (unsigned int i = 0; i < options.size(); i++)
		{
			cursor_move(1, 0);
			cout << ((choice == i) ? "> " : "  ") << i + 1 << ". " << options[i] << "    \n";
		}
	}
}

// Displaying with controls----------------
void options_menu(unsigned int& choice, const string& title, const string& option, const vector<string>& list,
                  const bool& esc_enabled)
{
	int action;

	do
	{
		if (choice >= 0 && choice <= list.size())
		{
			// Displaying Title -----------------------
			print_text(28 - title.size() / 2, 0, title);

			// Displaying Option ----------------------
			cursor_pos(8, 4);
			cout << (choice == 0 ? ">" : " ") << " " << option;

			// Displaying List ------------------------
			for (unsigned int i = 0; i < list.size(); i++)
			{
				cursor_pos(8, 4 + 2 * (i + 1));
				cout << ((choice == i + 1) ? "> " : "  ") << i + 1 << ". " << list[i];
			}
		}
		choice_limit(choice, static_cast<int>(list.size()) + 1);
		action = up_down_esc_controls(choice, static_cast<int>(list.size()) + 1);
		if (action == -1 && !esc_enabled)
		{
			action = 0;
		}
	}
	while (action == 0);

	if (action == -1)
	{
		choice = 0;
	}
}

void options_menu(unsigned int& choice, const string& title, const vector<string>& options, const bool& esc_enabled)
{
	int action;

	do
	{
		if (choice >= 0 && choice < options.size())
		{
			// Displaying Title -----------------------
			print_text(28 - title.size() / 2, 0, title);

			// Displaying Options ---------------------
			for (unsigned int i = 0; i < options.size(); i++)
			{
				cursor_pos(8, 4 + 2 * i);
				cout << ((choice == i) ? "> " : "  ") << i + 1 << ". " << options[i];
			}
		}
		choice_limit(choice, static_cast<int>(options.size()));
		action = up_down_esc_controls(choice, static_cast<int>(options.size()));
		if (action == -1 && !esc_enabled)
		{
			action = 0;
		}
	}
	while (action == 0);

	if (action == -1 && esc_enabled)
	{
		choice = 0;
	}
}

const bool confirmation_menu(const string& question)
{
	bool yes_no = false;
	const COORD prev_cursor_pos = get_cursor_pos();

	do
	{
		// Displaying Question --------------------
		cursor_pos(prev_cursor_pos);
		cout << question;

		// Displaying Options ---------------------
		cursor_pos(prev_cursor_pos.X, prev_cursor_pos.Y + 2);
		cursor_move(5, 0);
		cout << (yes_no == false ? "| " : "  ") << s_line_texts["No"] << (yes_no == false ? " |" : "  ");
		cursor_move(8, 0);
		cout << (yes_no == true ? "| " : "  ") << s_line_texts["Yes"] << (yes_no == true ? " |" : "  ");
	}
	while (!left_right_controls(yes_no));

	return yes_no;
}
