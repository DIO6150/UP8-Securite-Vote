#include "colors.hpp"

std::string GetColorID (Color color, bool bg) {
	if (color == Color::NONE) return ("\x1B[0m");

	std::map<Color, int> color_to_escape = {
		{Color::BLACK,		30},
		{Color::RED,		31},
		{Color::GREEN,		32},
		{Color::YELLOW,		33},
		{Color::BLUE,		34},
		{Color::MAGENTA,	35},
		{Color::CYAN,		36},
		{Color::WHITE,		37},
		{Color::GOLD,		93}
	};

	return ("\x1B[" + std::to_string (color_to_escape[color] + (bg ? 10 : 0)) + "m");
}

std::string Dye (std::string str, Color color) {
	#ifdef ENABLE_COLOR
		return (GetColorID (color) + str + "\033[0m");
	#else
		return (str);
	#endif

}

Color ColorFromString (std::string str) {
	std::map<std::string, Color> name_to_color = {
		{"BLACK",	Color::BLACK},
		{"RED",		Color::RED},
		{"GREEN",	Color::GREEN},
		{"YELLOW",	Color::YELLOW},
		{"BLUE",	Color::BLUE},
		{"MAGENTA",	Color::MAGENTA},
		{"CYAN",	Color::CYAN},
		{"WHITE",	Color::WHITE},
		{"GOLD",	Color::GOLD}
	};

	if (name_to_color.find (str) != name_to_color.end ()) {
		return (name_to_color [str]);
	}

	return (Color::NONE);
}