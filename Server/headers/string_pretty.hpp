#pragma once

#include <string>
#include <unordered_map>
#include <sstream>
#include <regex>
#include <vector>
#include <unistd.h>
#include <cstdio>
#include <iostream>

#include "colors.hpp"

// TODO : upgrade some of the functions

// Conversion d'un argument en string
template <typename T>
std::string ToString(const T & val) {
	std::ostringstream oss;
	oss << val;

	return oss.str ();
}

template <typename... Args>
std::string StrArgs(const std::string& format, Args&&... args) {
	std::vector<std::string> arguments = { ToString(std::forward<Args>(args))... };

	std::string result;
	size_t i = 0;

	while (i < format.length()) {
		if (format[i] == '#') {
			if (i + 1 < format.length() && format[i + 1] == '#') {
				// Cas d'échappement ##
				result += '#';
				i += 2;
			}
			else {
				// Tentative de lecture d’un indice comme #n#
				size_t start = i + 1;
				size_t end = format.find('#', start);

				if (end != std::string::npos) {
					std::string numStr = format.substr(start, end - start);
					bool isNumeric = !numStr.empty() &&
							std::all_of(numStr.begin(), numStr.end(), ::isdigit);

					if (isNumeric) {
						int index = std::stoi(numStr);

						if (index >= 1 && static_cast<size_t>(index) <= arguments.size())
							result += arguments[index - 1];

						else
							result += "[INVALID_ARG]";

						i = end + 1;
						continue;
					}
				}

				// Si ce n’est pas bien formé, on garde le `#`
				result += '#';
				i++;
			}
		} else result += format[i++];
	}

	return (result);
}

inline std::string StrRemoveColor(const std::string& input) {
	// Supprime toutes les balises de type {C:...}, {X:...}, {C:..., X:...}, {}
	std::regex colorTag(R"(\{(?:C:[^}]+|X:[^}]+|C:[^,}]+,\s*X:[^}]+)?\})");
	return std::regex_replace(input, colorTag, "");
}


inline std::string StrColor(std::string text) {
	std::ostringstream result;
	size_t i = 0;

	while (i < text.size()) {
		if (text[i] == '{') {
			size_t end = text.find('}', i);

			if (end != std::string::npos) {
				std::string marker = text.substr(i + 1, end - i - 1);
				
				if (marker.empty()) result << GetColorID (Color::NONE);  // Reset

				else {
					std::string code;
					bool valid = false;

					// Split by commas
					std::istringstream iss(marker);
					std::string part;

					while (std::getline(iss, part, ',')) {
						// Remove leading/trailing spaces
						part.erase(0, part.find_first_not_of(" \t"));
						part.erase(part.find_last_not_of(" \t") + 1);

						size_t colon = part.find(':');
						if (colon != std::string::npos) {
							std::string type = part.substr(0, colon);
							std::string color = part.substr(colon + 1);

							Color coloration;

							if (type == "C" && (coloration = ColorFromString (color)) != Color::NONE) {
								code += GetColorID (coloration);
								valid = true;
							}
							
							else if (type == "X" && (coloration = ColorFromString (color)) != Color::NONE) {
								code += GetColorID (coloration, true);
								valid = true;
							}

							else {
								// Invalid part, ignore all
								code.clear();
								valid = false;
								break;
							}
						}

						else {
							code.clear();
							valid = false;
							break;
						}
					}

					if (valid) {
						result << code;
					}
				
					else {
						// Leave the original marker
						result << "{" << marker << "}";
					}
				}

				i = end + 1;
			}

			else {
				// No closing }, just output the character
				result << text[i++];
			}
		}

		else {
			result << text[i++];
		}
	}

	result << GetColorID (Color::NONE);

	return result.str();
}

inline std::string StrTab(const std::string& format) {
	std::regex tabRegex(R"(@(\d+)@)");
	std::smatch match;

	std::string result;
	size_t pos = 0;       // position dans la chaîne d'origine
	size_t col = 0;       // position courante dans la ligne (colonne console)

	while (std::regex_search(format.begin() + pos, format.end(), match, tabRegex)) {
		size_t matchPos = match.position(0) + pos;
		std::string before = format.substr(pos, matchPos - pos);
		result += before;
		col += before.length();

		int targetCol = std::stoi(match[1].str());
		if (targetCol > static_cast<int>(col)) {
			result.append(targetCol - col, ' ');
			col = targetCol;
		}

		pos = matchPos + match.length(0);
	}

	// Ajouter ce qui reste
	std::string remaining = format.substr(pos);
	result += remaining;
	col += remaining.length();

	return result;
}

template <typename... Args>
std::string StrPretty(const std::string& format, Args&&... args)
{   
	std::string withArgs = StrArgs (format, std::forward<Args> (args)...);
	std::string withTabs = StrTab (withArgs);
	std::string withColor = StrColor (withTabs);
	return (withColor);
}
