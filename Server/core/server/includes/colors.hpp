#pragma once

#include <string>
#include <map>

#define ENABLE_COLOR

enum class Color {
	BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE, GOLD, NONE
};

std::string GetColorID (Color color, bool bg = false);
std::string Dye (std::string str, Color color);
Color ColorFromString (std::string str);
std::string KeyFromColor (Color color);