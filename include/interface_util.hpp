#pragma once

#include "global_headers.h"
#include "item.hpp"

// Controls -------------------------------
void choice_limit(unsigned int& choice, const unsigned int& ceil) noexcept;
const bool up_down_controls(unsigned int& choice, const unsigned int& ceil);
const bool left_right_controls(bool& yes_no);
const int up_down_esc_controls(unsigned int& choice, const unsigned int& ceil);
const int up_down_left_right_controls(unsigned int& choice, const unsigned int& ceil, int& points,
                                      const int& points_prev, const std::vector<int*>& variables,
                                      const std::vector<int>& variables_prev, const int& inc);
const bool up_down_left_right_controls(unsigned int& choice, const unsigned int& ceil,
                                       const std::vector<int*>& variables, const std::vector<int>& limits);
const int up_down_left_right_esc_controls(unsigned int& choice, const unsigned int& ceil,
                                          const std::vector<int*>& variables, const std::vector<int>& limits);

/*---------------------------------- Menus ----------------------------------*/
// Displaying -----------------------------
void display_options_no_num(const unsigned int& choice, const std::vector<std::string>& options,
                            const std::vector<int*>& variables);
void display_options_no_num(const unsigned int& choice, const std::string& option,
                            const std::vector<std::shared_ptr<item>>& variables, const bool& quantity);
void display_options_no_num(const unsigned int& choice, const std::vector<std::string>& options);
void display_options_no_num(const unsigned int& choice, const std::vector<std::string>& options, const int& x_pos,
                            const int& y_pos);
void display_options_menu(const unsigned int& choice, const std::string& title,
                          const std::vector<std::string>& options);
void display_options_menu(unsigned const int& choice, const std::vector<std::string>& options);

// Displaying with controls----------------
void options_menu(unsigned int& choice, const std::string& title, const std::string& option,
                  const std::vector<std::string>& list, const bool& esc_enabled);
void options_menu(unsigned int& choice, const std::string& title, const std::vector<std::string>& options,
                  const bool& esc_enabled);
const bool confirmation_menu(const std::string& question);
