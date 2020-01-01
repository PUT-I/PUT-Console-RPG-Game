#pragma once

#include <Windows.h>

#include "global_headers.h"

// Checking Keyboard Input ---------------
const bool check_enter() noexcept;
const bool check_space() noexcept;
const bool check_escape() noexcept;
const bool check_left_arrow() noexcept;
const bool check_right_arrow() noexcept;
const bool check_up_arrow() noexcept;
const bool check_down_arrow() noexcept;
const bool check_alt() noexcept;
void check_alt_f4() noexcept;

// Console Manipulation ------------------
void set_font_size(const int& font_size) noexcept;
void clear_console_input_buffer();
void show_console_cursor(const bool& show_flag) noexcept;
void clear_console() noexcept;

void input_string(std::string& str, const unsigned int& limit);
const std::pair<int, int> get_desktop_resolution() noexcept;
void full_screen(const bool& yes_no) noexcept;

const COORD get_cursor_pos() noexcept;
void cursor_pos(const COORD& c) noexcept;
void cursor_pos(const unsigned int& x, const unsigned int& y) noexcept;
void cursor_pos_begin() noexcept;
void cursor_move(const int& mov_x, const int& mov_y) noexcept;

void space_pause();

void adapt_location_font_size() noexcept;
void adapt_font_size();

// GFX -----------------------------------
void print_box(const unsigned int& x_pos, const unsigned int& y_pos, const unsigned int& width,
               const unsigned int& height);
void print_text(const unsigned int& x_pos, const unsigned int& y_pos, const std::string& text);
void print_text(const unsigned int& x_pos, const unsigned int& y_pos, const std::string& text, const int& variable);
void print_text(const unsigned int& x_pos, const unsigned int& y_pos, const std::string& text1,
                const std::string& text2);
void display_gfx(const unsigned int& x_pos, const unsigned int& y_pos, const std::vector<std::string>& gfx);
