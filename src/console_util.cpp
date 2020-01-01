#include "console_util.hpp"

#include <conio.h>
#include <iostream>

#include "global_variables.hpp"
#include "sound_manager.hpp"

using namespace std;

// Checking Keyboard Input ---------------
const bool check_enter()
{
	return GetAsyncKeyState(VK_RETURN) & 0x8000;
}

const bool check_space()
{
	return GetAsyncKeyState(VK_SPACE) & 0x8000;
}

const bool check_escape()
{
	return GetAsyncKeyState(VK_ESCAPE) & 0x8000;
}

const bool check_left_arrow()
{
	return GetAsyncKeyState(VK_LEFT) & 0x8000;
}

const bool check_right_arrow()
{
	return GetAsyncKeyState(VK_RIGHT) & 0x8000;
}

const bool check_up_arrow()
{
	return GetAsyncKeyState(VK_UP) & 0x8000;
}

const bool check_down_arrow()
{
	return GetAsyncKeyState(VK_DOWN) & 0x8000;
}

const bool check_alt()
{
	return GetAsyncKeyState(VK_MENU) & 0x8000;
}

void check_alt_f4()
{
	if (check_alt() && GetAsyncKeyState(VK_F4) & 0x8000) exit(0);
}

// Console Manipulation ------------------
void set_font_size(const int& font_size)
{
	CONSOLE_FONT_INFOEX info = {0};
	info.cbSize = sizeof(info);
	info.dwFontSize.Y = font_size; // leave X as zero
	info.FontWeight = FW_NORMAL;
	wcscpy_s(info.FaceName, L"Lucida Console");
	SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), NULL, &info);
}

void clear_console_input_buffer()
{
	const auto clearing_var1 = new INPUT_RECORD[256];
	DWORD clearing_var2;
	ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), clearing_var1, 256, &clearing_var2);
	delete[] clearing_var1;
}

void show_console_cursor(const bool& show_flag)
{
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_CURSOR_INFO cursor_info;

	GetConsoleCursorInfo(out, &cursor_info);
	cursor_info.bVisible = show_flag; // set the cursor visibility
	SetConsoleCursorInfo(out, &cursor_info);
}

void clear_console()
{
	const COORD top_left = {0, 0};
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO screen;
	DWORD written;

	GetConsoleScreenBufferInfo(console, &screen);
	FillConsoleOutputCharacterA(
		console, ' ', screen.dwSize.X * screen.dwSize.Y, top_left, &written
	);
	FillConsoleOutputAttribute(
		console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
		screen.dwSize.X * screen.dwSize.Y, top_left, &written
	);
	SetConsoleCursorPosition(console, top_left);
}

void input_string(string& str, const unsigned int& limit)
{
	const COORD prev_cursor_pos = get_cursor_pos();
	do
	{
		cursor_pos(prev_cursor_pos);
		cout << str << ' ';
		cursor_move(-1, 0);
		show_console_cursor(true);
		const char c = _getch();
		show_console_cursor(false);

		if (c != 0x00)
		{
			check_alt_f4();

			if (check_enter() && !str.empty())
			{
				sounds.play_sound(sound_manager::enter);
				break;
			}
			else if (check_up_arrow()) continue;
			else if (check_down_arrow()) continue;
			else if (check_left_arrow()) continue;
			else if (check_right_arrow()) continue;
			else if (check_alt()) continue;
			else if (c == 0x08)
			{
				// When backspace pressed
				if (!str.empty())
				{
					str.pop_back();
				}
			}
			else if (c >= '0' && c <= '9' || c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z')
			{
				if (str.size() < limit)
				{
					str += c;
				}
			}
		}
	}
	while (true);
}

const pair<int, int> get_desktop_resolution()
{
	pair<int, int> resolution;

	resolution.first = GetSystemMetrics(SM_CXSCREEN);
	resolution.second = GetSystemMetrics(SM_CYSCREEN);

	return resolution;
}

void full_screen(const bool& yes_no)
{
	if (yes_no)
	{
		SetConsoleDisplayMode(GetStdHandle(STD_OUTPUT_HANDLE), CONSOLE_FULLSCREEN_MODE, nullptr);
	}
	else
	{
		SetConsoleDisplayMode(GetStdHandle(STD_OUTPUT_HANDLE), CONSOLE_WINDOWED_MODE, nullptr);
	}
}

const COORD get_cursor_pos()
{
	CONSOLE_SCREEN_BUFFER_INFO cbsi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cbsi);
	return cbsi.dwCursorPosition;
}

void cursor_pos(const COORD& c)
{
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void cursor_pos(const unsigned int& x, const unsigned int& y)
{
	COORD c;
	c.X = x;
	c.Y = y;
	cursor_pos(c);
}

void cursor_pos_begin()
{
	cursor_pos(0, 0);
}

void cursor_move(const int& mov_x, const int& mov_y)
{
	COORD c = get_cursor_pos();
	c.X += mov_x;
	c.Y += mov_y;

	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void space_pause()
{
	cout << s_line_texts["SpacePause"];
	do { clear_console_input_buffer(); }
	while (!check_space()); // Waits for SPACE press

	clear_console_input_buffer();
}

void adapt_location_font_size()
{
	const pair<int, int> resolution = get_desktop_resolution();

	if (resolution.first <= 1280) story_font_inc = 20;
	else if (resolution.first <= 1440) story_font_inc = 19;
	else if (resolution.first <= 1680) story_font_inc = 17;
	else if (resolution.first <= 1768) story_font_inc = 15;
	else if (resolution.first <= 1920) story_font_inc = 14;
	else if (resolution.first <= 2103) story_font_inc = 12;
	else if (resolution.first <= 2351) story_font_inc = 9;
	else if (resolution.first <= 2560) story_font_inc = 7;
	else if (resolution.first > 2560) story_font_inc = 1;
}

void adapt_font_size()
{
	const pair<int, int> resolution = get_desktop_resolution();

	if (resolution.first == 1680) global_font_size = 16;
	else if (resolution.first >= 3840) global_font_size = 60;
	else
	{
		const float temp = float((-3.0f / 500000.0f) * pow(resolution.first, 2) + 0.0511 * resolution.first - 51.634f);
		global_font_size = int(round(temp));
	}
}

// GFX -----------------------------------
void print_box(const unsigned int& x_pos, const unsigned int& y_pos, const unsigned int& width,
               const unsigned int& height)
{
	COORD coordinates1;
	coordinates1.X = x_pos;
	coordinates1.Y = y_pos;

	COORD coordinates2;
	coordinates2.X = x_pos + width + 1;
	coordinates2.Y = y_pos;

	for (unsigned int i = 0; i <= height + 1; i++)
	{
		cursor_pos(coordinates1);
		cout << "|";
		coordinates1.Y += 1;

		cursor_pos(coordinates2);
		coordinates2.Y += 1;
		cout << "|";
	}

	coordinates1.X = x_pos + 1;
	coordinates1.Y = y_pos;

	coordinates2.X = x_pos + 1;
	coordinates2.Y = y_pos + height + 1;

	const string equal_signs{string(width, '=')};

	cursor_pos(coordinates1);
	cout << equal_signs;

	cursor_pos(coordinates2);
	cout << equal_signs;
}

void print_text(const unsigned int& x_pos, const unsigned int& y_pos, const string& text)
{
	cursor_pos(x_pos, y_pos);
	cout << text;
}

void print_text(const unsigned int& x_pos, const unsigned int& y_pos, const string& text, const int& variable)
{
	cursor_pos(x_pos, y_pos);
	cout << text << variable;
}

void print_text(const unsigned int& x_pos, const unsigned int& y_pos, const string& text1, const string& text2)
{
	cursor_pos(x_pos, y_pos);
	cout << text1 << text2;
}

void display_gfx(const unsigned int& x_pos, const unsigned int& y_pos, const vector<string>& gfx)
{
	for (unsigned int i = 0; i < gfx.size(); i++)
	{
		cursor_pos(x_pos, y_pos + i);
		cout << gfx[i];
	}
}
