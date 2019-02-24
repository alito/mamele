#ifndef _LE_UTILS_H
#define _LE_UTILS_H 1

#include <string>

#include "emu.h"

#define LE_ENCODING_INVALID 0
#define LE_ENCODING_HEXREADABLE 1

struct le_score_memory_description {
	/* Description of memory range */
    le_score_memory_description() : cpu(), address_space_name(), address(0), number_of_bytes(0), encoding(LE_ENCODING_INVALID) { }
    std::string cpu;
    std::string address_space_name;
	u32 address, number_of_bytes;
	int encoding;
};

struct le_gameover {
    le_gameover() : cpu(), address_space_name(), address(0), bit(0), on(0) {}
    std::string cpu;
    std::string address_space_name;
    u32 address;
    u8 bit;
    bool on;
};

/* Get where to find the current score details and its encoding for the given game. 
   description_files_directory is where to look for the description file first
 */
le_score_memory_description get_score_details(const std::string& game_name, const std::string& description_files_directory);

/* Get where to find the game over details for the given game
   description_files_directory is where to look for the description file first 
*/
le_gameover get_gameover_details(const std::string& game_name, const std::string& description_files_directory);


#endif /* _LE_UTILS_H */

