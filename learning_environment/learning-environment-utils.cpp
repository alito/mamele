#include <string>
#include <iostream>
#include <list>
#include <fstream>

#include <wordexp.h>

#include "learning-environment-utils.h"

using namespace std;

const string score_description_filename("score_description.txt");
const string gameover_description_filename("gameover_description.txt");

/*
String converter using strtol but keeping the same interface
*/
static u32 string_to_number(const string& description_string, size_t& start_position, int base=10) {
	const char *c_str = description_string.c_str();
	const char *start_string = c_str + start_position;
	char* end;

	u32 result = strtol(start_string, &end, base);

	start_position = end - c_str + 1;
	return result;
}

/*
Read until next colon, make a copy of the string, modify pointer to first character past it
*/
static std::string next_description_string(const string& description_line, size_t& start_index) 
{
	size_t original_start = start_index;
	size_t next_colon = description_line.find(':', start_index);

	if (next_colon == string::npos) {
		// not found, the whole thing is it
		start_index = string::npos;
		return description_line.substr(original_start);
	} else {
		start_index = next_colon + 1;
		return description_line.substr(original_start, next_colon - original_start);
	}
}

/* Memory ranges always start with '@:'
*/
static bool is_mem_range(const string& score_line) 
{
	return (score_line.length() > 2) && (score_line[0] == '@') && (score_line[1] == ':');
}


/* Expand posix shell variables (home directories ~ is what we are after) */
static string get_full_path(const string& path) {
	wordexp_t word_expander;
	string full_path;

	int result = wordexp(path.c_str(), &word_expander, 0);
	if (result != 0) {
		cerr << "Word expansion returned " << result << " when expanding '" << path << "' " << endl;
		full_path = "";
	} else if (word_expander.we_wordc != 1) {
		cerr << "Path should expand to one word" << endl;
		full_path = "";
	} else {
		full_path = word_expander.we_wordv[0];
	}
	
	wordfree(&word_expander);

	return full_path;
}


/* Find a readable file among the default paths */
static string find_description_filename(const string& filename, const std::string& description_files_directory) {

	string full_path;
	bool found = false;

	list<string> paths;
	if (!description_files_directory.empty()) {
		paths.push_back(description_files_directory);
	}
	paths.push_back("~/.le/");
	paths.push_back("./");

	for (list<string>::iterator it=paths.begin(), end=paths.end(); it != end; ++it) {
		full_path = get_full_path(*it);
		if (full_path.back() != '/') {
			full_path += '/';
		}
		full_path += filename;
	
		ifstream fin(full_path.c_str());
		if (fin.is_open()) {
			found = true;
			break;
		}
	}
	if (found) return full_path;
	else return "";
}


std::string find_description_for_game(const std::string& filename, const std::string& game_name) {
	string line;
	string description = "";
	bool found=false;
	string target = game_name;
	target += ":";


	ifstream fin(filename.c_str());
	if (fin.is_open()) {
		while (fin.good()) {
			getline(fin, line);
			
			size_t first_non_space = line.find_first_not_of(" \t\n");
			if (first_non_space == string::npos) continue;
			else if (line[first_non_space] == '#') continue;

			// there can be many games with the same description
			// we find the first line that it's not a game title, ie the first one that doesn't end in ':'
			if (found) {
				if (line.rfind(":") != line.find_last_not_of(" \t\n")) {
					description = line;
					break;
				}
			} else if (line.find(target) == 0) {
				found = true;
			}
		}
		fin.close();
	}

	return description;
}

/* Get where to find the current score details for the given game */
le_score_memory_description get_score_details(const std::string& game_name, const std::string& description_files_directory) {
	le_score_memory_description score_encoding;

	string filename = find_description_filename(score_description_filename, description_files_directory);

	if (filename.empty()) {
		cerr << "Could not find '" << score_description_filename << "'" << endl;
		return score_encoding;
	}


	/* we are looking in a file that looks like this
	   wakichuki:
	   parramata:

	   @:maincpu:program:4433:4:hexreadable

	 */

	string encoding_description = find_description_for_game(filename.c_str(), game_name);
	if ((encoding_description.length() > 0) && (is_mem_range(encoding_description))) {

		size_t start_index = 2;

		score_encoding.cpu = next_description_string(encoding_description, start_index);
		score_encoding.address_space_name = next_description_string(encoding_description, start_index);
		score_encoding.address = string_to_number(encoding_description, start_index);
		score_encoding.number_of_bytes = string_to_number(encoding_description, start_index);
		
		string encoding_name = next_description_string(encoding_description, start_index);
		if (encoding_name == "hexreadable") {
			score_encoding.encoding = LE_ENCODING_HEXREADABLE;
		} else {
			cerr << "Don't recognise encoding '" << encoding_name << "'" << endl;
			score_encoding.encoding = LE_ENCODING_INVALID;
		}
	} else {
		cerr << "Didn't find a score entry for " << game_name << endl;
	}

	return score_encoding;
}

/* Get where to find whether the game is over for the given game */
le_gameover get_gameover_details(const std::string& game_name, const std::string& description_files_directory) {
	le_gameover gameover;

	string filename = find_description_filename(gameover_description_filename, description_files_directory);

	if (filename.empty()) {
		cerr << "Could not find '" << gameover_description_filename << "'" << endl;
		return gameover;
	}
	

	/* we are looking in a file that looks like this
	   wakichuki:
	   parramata:
       @:maincpu:program:16389:1:1


	 */

	string description = find_description_for_game(filename.c_str(), game_name);
	string location = "";
	if (description.length() > 0) {
		if (is_mem_range(description)) {
			size_t start_index = 2;

			gameover.cpu = next_description_string(description, start_index);
			gameover.address_space_name = next_description_string(description, start_index);
			gameover.address = string_to_number(description, start_index);
			gameover.bit = string_to_number(description, start_index);
			gameover.on = string_to_number(description, start_index);
		} else {
			cerr << "not a mem range: " << description << endl;
		}

	} else {
		cerr << "Didn't find a game over entry for " << game_name << endl;
	}

	return gameover;
}
