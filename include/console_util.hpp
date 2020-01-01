#pragma once

#include <string>
#include <vector>
#include <Windows.h>


// Checking Keyboard Input ---------------
const bool check_enter();
const bool check_space();
const bool check_escape();
const bool check_left_arrow();
const bool check_right_arrow();
const bool check_up_arrow();
const bool check_down_arrow();
const bool check_alt();
void check_alt_f4();

// Console Manipulation ------------------
void set_font_size(const int& font_size);
void clear_console_input_buffer();
void show_console_cursor(const bool& show_flag);
void clear_console();

void input_string(std::string& str, const unsigned int& limit);
const std::pair<int, int> get_desktop_resolution();
void full_screen(const bool& yes_no);

const COORD get_cursor_pos();
void cursor_pos(const COORD& c);
void cursor_pos(const unsigned int& x, const unsigned int& y);
void cursor_pos_begin();
void cursor_move(const int& mov_x, const int& mov_y);

void space_pause();

void adapt_location_font_size();
void adapt_font_size();

// GFX -----------------------------------
void print_box(const unsigned int& x_pos, const unsigned int& y_pos, const unsigned int& width,
               const unsigned int& height);
void print_text(const unsigned int& x_pos, const unsigned int& y_pos, const std::string& text);
void print_text(const unsigned int& x_pos, const unsigned int& y_pos, const std::string& text, const int& variable);
void print_text(const unsigned int& x_pos, const unsigned int& y_pos, const std::string& text1,
                const std::string& text2);
void display_gfx(const unsigned int& x_pos, const unsigned int& y_pos, const std::vector<std::string>& gfx);
