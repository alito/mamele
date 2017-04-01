/*
Example controller module for the MAME learning environment. It doesn't do anything, just sits there
To call, use something like 'mame -use_le -agent sit.so <romname>'
 */

#include "learning-environment-common.h"
#include <stdlib.h>
#include <stdio.h>

static int frame_counter = 0;

static int start_game(const le_game_info *game_info) {
	/* 
	   This is called at game start.
	*/
	fprintf(stderr,"dummy start game called\n");
	fprintf(stderr,"game name: %s\nwidth: %d\nheight: %d\n",game_info->game_name,game_info->width, game_info->height);
	fprintf(stderr,"Buttons used: ");
	for (int i=0; i < LE_TOTAL_BUTTONS; i++) {
		if (game_info->buttons_used[i]) {
			fprintf(stderr, "%d,", i);
		}
	}
	fprintf(stderr, "\n");
	return LE_VIDEO_MODE_BGRA;
}

static void finish_game(void) {
	/*
	  This is called at shutdown
	 */
	fprintf(stderr,"dummy finish game called\n");
}


static int update_state(int current_score, int game_over, const le_frame_buffer *frame_buffer) {
	/* This will be called at every frame.
	   current_score will have the score if it is known, otherwise zero
	   frame_buffer is a structure with members width and height, 
	   and a const char * buffer which consists of all the pixels from the top-left to the bottom right in BGRA format
	*/
	fprintf(stderr,"current score: %d\nframe buffer width: %d\nframe buffer height: %d\n",current_score,frame_buffer->width, frame_buffer->height);
	if (game_over) {
		fprintf(stderr, "GAME OVER\n");
	}

	frame_counter++;

	/* return the number of frames that you want skipped before this gets called again. 
	   This is only useful for performance reasons */
	return 0;
}

static le_actions get_actions (void){
	/* 
	   This will also get called once per frame.
	   Should return 0 or 1 for each of the buttons.  See ac-common.h for the order of each button
	*/

	static le_actions dummy;
	int i;
	for (i = 0; i < LE_TOTAL_BUTTONS; i++) dummy.buttons[i] = 0;
	//fprintf(stderr,"dummy get actions called\n");
	return dummy;
}

static int check_reset(void) {
	/* Do a reset at frame 1000, just to test it */
	if (frame_counter == 1000) return 1;
	else return 0;
}

static void consume_memory(const le_memory_t *memory) {

	while (memory != NULL) {
		fprintf(stderr, "Got %lu bytes of memorystarting at %lu\n", memory->size, memory->start);
		memory = memory->next;
	}
}

le_functions le_get_functions (const char* args) {
	/*
	  This is the only mandatory function and it has to be called le_get_functions 

	  args is a string with the command-line options passed in through -ac_options to mame

	  It should return what function should be called at the start of the game,
	  on each update to receive the framebuffer, which will return the actions of the agent, and which to get called
	  at shutdown.  If you don't want to receive calls for any of those four, you can set the appropriate function 
	  slot to NULL.
	 */

	le_functions handler_functions;
	handler_functions.start = start_game;
	handler_functions.finish = finish_game;
	handler_functions.update = update_state;
	handler_functions.actions = get_actions;
	handler_functions.check_reset = check_reset;
	handler_functions.consume_memory = consume_memory;
	return handler_functions;
}
