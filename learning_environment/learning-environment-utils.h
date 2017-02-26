#ifndef _LE_UTILS_H
#define _LE_UTILS_H 1

#include <string>

#include "emu.h"

#define LE_ENCODING_INVALID 0
#define LE_ENCODING_HEXREADABLE 1

struct le_score_memory_description {
	/* Description of memory range */
    le_score_memory_description() : cpu(NULL), address_space_name(NULL), address(0), number_of_bytes(0), encoding(LE_ENCODING_INVALID) { }
    char *cpu;
    char *address_space_name;
	u32 address, number_of_bytes;
	int encoding;
};

struct le_gameover {
    le_gameover() : cpu(NULL), address_space_name(NULL), address(0), bit(0), on(0) {}
    char *cpu;
    char *address_space_name;
    u32 address;
    u8 bit;
    bool on;
};

/* Get where to find the current score details and its encoding for the given game */
le_score_memory_description get_score_details(const std::string& game_name);

/* Get where to find the game over details for the given game */
le_gameover get_gameover_details(const std::string& game_name);


#endif /* _LE_UTILS_H */

