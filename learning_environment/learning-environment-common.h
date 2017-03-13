#ifndef LE_COMMON_H
#define LE_COMMON_H
/* Here we define structures common to the environment side and the agent side */

#include <stdint.h>

// Name of the function that will be looked for in the agent
#define LE_GET_FUNCTIONS_NAME "le_get_functions"
#define LE_TOTAL_BUTTONS 12
#define LE_VIDEO_MODE_BGRA 0
#define LE_VIDEO_MODE_RGB 1
#define LE_VIDEO_MODE_RGBA 2
#define LE_VIDEO_MODE_ARGB 3

// Structure of the frame buffer passed ot the agent.
typedef struct {
	int width, height;
	unsigned char *buffer;
} le_frame_buffer;


/* Button indices in le_actions defined below */
#define LE_BUTTON_INDEX_LEFT 0
#define LE_BUTTON_INDEX_RIGHT 1
#define LE_BUTTON_INDEX_UP 2
#define LE_BUTTON_INDEX_DOWN 3
#define LE_BUTTON_INDEX_BUTTON_1 4
#define LE_BUTTON_INDEX_BUTTON_2 5
#define LE_BUTTON_INDEX_BUTTON_3 6
#define LE_BUTTON_INDEX_BUTTON_4 7
#define LE_BUTTON_INDEX_BUTTON_5 8
#define LE_BUTTON_INDEX_BUTTON_6 9
#define LE_BUTTON_INDEX_COIN 10
#define LE_BUTTON_INDEX_PLAYER_START 11


typedef struct {
	/* buttons are in the following order
	   left, right, up, down, button 1, button 2, button 3, button 4, button 5, button 6, coin, player start
	*/
	int buttons[LE_TOTAL_BUTTONS];
} le_actions;

typedef struct {
	unsigned long size;
	uint8_t *content;
} le_memory;

typedef struct {
	const char *game_name;
	int width, height;
	int buttons_used[LE_TOTAL_BUTTONS];
} le_game_info;


typedef int (*le_game_starter)(const le_game_info *game_info);
typedef void (*le_game_finisher) (void);
typedef int (*le_state_updater) (int current_score, int game_over, const le_frame_buffer *buffer);
typedef int (*le_reset_checker) (void);
typedef void (*le_memory_consumer) (const le_memory *memory);
typedef le_actions (*le_action_getter) (void);

typedef struct {
	le_game_starter start;
	le_game_finisher finish;
	le_state_updater update;
	le_reset_checker check_reset;
	le_memory_consumer consume_memory;
	le_action_getter actions;
} le_functions;


#endif
