/***************************************************************************
This is a pass-through driver

***************************************************************************/
#include <string>
#include <iostream>
#include <stdint.h>

#include "learning-environment-utils.h"

#include "emucore.h"
#include "emu.h"
#include "bitmap.h"
#include "emuopts.h"

#include "learning-environment-common.h"
#include "learning-environment.h"
#include <limits.h>

#include <dlfcn.h>

#define MAX_PLAYER 8


using std::cerr;
using std::endl;
using std::string;

const static int WaitFrames = 500; //Number of frames to wait till input is gotten
const static input_code no_code = input_seq::end_code;

static input_code button_codes[MAX_PLAYER][LE_TOTAL_BUTTONS];
static bool used_buttons[MAX_PLAYER][LE_TOTAL_BUTTONS];

typedef struct {
	int state;
	input_code code;
} Button;

typedef struct {
	Button button[LE_TOTAL_BUTTONS];
} le_state;


static le_state state;

static le_frame_buffer frame_buffer = {0,0,nullptr};
static le_memory_t *memory = nullptr;


static int wait_frames = WaitFrames;
static int video_buffer_byte_length = 0;
static int le_video_mode = LE_VIDEO_MODE_BGRA;

static le_score_memory_description score_memory_description;
static le_gameover game_over;

static int g_player = 1;
static bool g_initialised_input = false;
static string le_library;
static string le_args;
static void *le_lib_handle;

static le_state_updater le_update_state = NULL;
static le_action_getter le_get_actions = NULL;
static le_game_starter le_start_game = NULL;
static le_game_finisher le_finish_game = NULL;
static le_reset_checker le_check_reset = NULL;
static le_memory_consumer le_consume_memory = NULL;



/* 
Copy number of bytes from address space address_space_name attached to cpu cpu starting at offset 
address into destination 
*/
static inline void copy_from_memory (running_machine &machine, const string &cpu, 
									int address, int number_of_bytes, u8 *destination)
{
	cpu_device* p_cpu = machine.device<cpu_device>(cpu.c_str());

	// We restrict ourselves to looking in the AS_PROGRAM memory space
	address_space &space = p_cpu->space(AS_PROGRAM);

	// Copy			
	for (int i=0; i<number_of_bytes; i++)
	{
		destination[i] = space.read_byte(address+i);
	}	
}


/* Read the current score */
static int get_current_score(running_machine &machine) {
	int score = 0;
	static u8 *score_buffer = nullptr;

	if (score_memory_description.encoding == LE_ENCODING_INVALID) return 0;

	if (score_buffer == nullptr) {
		score_buffer = (u8 *) malloc (score_memory_description.number_of_bytes);
	}
	copy_from_memory (machine, score_memory_description.cpu, 
					score_memory_description.address, 
					score_memory_description.number_of_bytes, score_buffer);

	/*
	cerr << score_memory_description.number_of_bytes << " bytes:\t ";
	for (int i = 0; i < score_memory_description.number_of_bytes; i++) {

		cerr << (int) score_buffer[i];
	}
	cerr << endl;
	*/
	
	if (score_memory_description.encoding == LE_ENCODING_HEXREADABLE) {
		/* encoding that maps each hex to a decimal place
		* eg 0x34 = 34 
		* or 0x4832 = 3248
		*/
		for (int i = score_memory_description.number_of_bytes-1 ; i >= 0 ; i--) {
			score = score * 10 + ((score_buffer[i] & (unsigned char) 0xF0) >> 4);
			score = score * 10 + (score_buffer[i] & (unsigned char) 0xF);
		}
	} else {
		cerr << "Encoding type " << score_memory_description.encoding << " not implemented" << endl;
	}

	return score;
}

/* Check if game is over */
static bool is_game_over(running_machine &machine) {
	if (game_over.cpu.empty()) {
		// We don't have a game over detector
		return false;
	}

	u8 the_decider=0;
	copy_from_memory(machine, game_over.cpu, 
					game_over.address, 1, &the_decider);

	return ((the_decider & (1 << game_over.bit)) ? game_over.on : (1-game_over.on));
}

static void initialise_button_code(int player, int button_index, input_seq mame_sequence) {
	if (mame_sequence.is_valid()) {
		button_codes[player][button_index] = mame_sequence[0];
	}
}

/* Initialise what we think are the keys/moves to be made to go up, down, press buttons, etc to
   the configuration in MAME 
*/
static void initialise_button_codes(running_machine &machine) {

	for (int i=0; i < MAX_PLAYER; i++) {
		for (int j=0; j < LE_TOTAL_BUTTONS; j++) {
			button_codes[i][j] = no_code;
		}
	}

	ioport_manager &ioport = machine.ioport();

	for (int player=0; player < MAX_PLAYER; player++) {
		initialise_button_code(player, LE_BUTTON_INDEX_COIN, ioport.type_seq(IPT_COIN1, player));
		initialise_button_code(player, LE_BUTTON_INDEX_PLAYER_START, ioport.type_seq(IPT_START, player));

		initialise_button_code(player, LE_BUTTON_INDEX_UP, ioport.type_seq(IPT_JOYSTICK_UP, player));
		initialise_button_code(player, LE_BUTTON_INDEX_DOWN, ioport.type_seq(IPT_JOYSTICK_DOWN, player));
		initialise_button_code(player, LE_BUTTON_INDEX_LEFT, ioport.type_seq(IPT_JOYSTICK_LEFT, player));
		initialise_button_code(player, LE_BUTTON_INDEX_RIGHT, ioport.type_seq(IPT_JOYSTICK_RIGHT, player));
		initialise_button_code(player, LE_BUTTON_INDEX_BUTTON_1, ioport.type_seq(IPT_BUTTON1, player));
		initialise_button_code(player, LE_BUTTON_INDEX_BUTTON_2, ioport.type_seq(IPT_BUTTON2, player));
		initialise_button_code(player, LE_BUTTON_INDEX_BUTTON_3, ioport.type_seq(IPT_BUTTON3, player));
		initialise_button_code(player, LE_BUTTON_INDEX_BUTTON_4, ioport.type_seq(IPT_BUTTON4, player));
		initialise_button_code(player, LE_BUTTON_INDEX_BUTTON_5, ioport.type_seq(IPT_BUTTON5, player));
		initialise_button_code(player, LE_BUTTON_INDEX_BUTTON_6, ioport.type_seq(IPT_BUTTON6, player));		
	}

}

/* Find out which buttons we actually use */
static void initialise_buttons_used(running_machine &machine) {

	/* based on code from ui/inputmap.cpp */

	/* iterate over the input ports */
	for (auto &port : machine.ioport().ports())
	{
		for (ioport_field &field : port.second->fields())
		{
			ioport_type_class type_class = field.type_class();

			if (field.enabled() && (type_class == INPUT_CLASS_CONTROLLER || type_class == INPUT_CLASS_MISC || type_class == INPUT_CLASS_KEYBOARD))
			{
				input_seq mame_sequence = field.seq(SEQ_TYPE_STANDARD);
				if (mame_sequence.is_valid()) {

					// Iterate over the button codes, find out which one this is
					bool found = false;
					u8 player = field.player();
					input_code button = mame_sequence[0];
					for (int button_index=0; button_index < LE_TOTAL_BUTTONS; button_index++) {
						if (button == button_codes[player][button_index]) {
							used_buttons[player][button_index] = true;
							found = true;
						}
					}

					if ((!found) && (button.device_class() == DEVICE_CLASS_JOYSTICK)) {
						// Check to see if it's a joystick input instead, in which case map to correct 
						// buttons
						if  (button.item_id() == ITEM_ID_XAXIS) {
							// Add left and right buttons
							used_buttons[player][LE_BUTTON_INDEX_RIGHT] = true;
							used_buttons[player][LE_BUTTON_INDEX_LEFT] = true;
						} else if (button.item_id() == ITEM_ID_YAXIS) {
							// Add up and down buttons
							used_buttons[player][LE_BUTTON_INDEX_UP] = true;
							used_buttons[player][LE_BUTTON_INDEX_DOWN] = true;							
						}
					}
				} 
			}
		}
	}
}


static void initialise_input(running_machine &machine) {
	initialise_button_codes(machine);
	for (int i = 0; i < LE_TOTAL_BUTTONS; i++) state.button[i].state = 0;
	for (int i = 0; i < LE_TOTAL_BUTTONS; i++) {
		state.button[i].code = button_codes[g_player - 1][i];
	}
	initialise_buttons_used(machine);	
}

static void process_events(const running_machine &machine) {
	le_actions actions;

	if (le_get_actions) {
		actions = (*le_get_actions)();

		for (int index = 0; index < LE_TOTAL_BUTTONS; index++) {
			if (actions.buttons[index]) {
				if (!state.button[index].state) {
					state.button[index].state = 1;
				}
			} else {
				if (state.button[index].state) {
					state.button[index].state = 0;
				}
			}
		}
	}
}


static void extract_main_memory(running_machine &machine) {
	/* 
	Copy the main RAM 
	*/

	cpu_device* p_cpu = machine.device<cpu_device>("maincpu");

	// We restrict ourselves to looking in the AS_PROGRAM memory space
	address_space &space = p_cpu->space(AS_PROGRAM);
				
	if (memory == nullptr) {
		// Initialise the structure that we use to transfer
		le_memory_t * last_node = nullptr;
		for (address_map_entry &entry : space.map()->m_entrylist) {
			if ((entry.m_read.m_type == AMH_RAM) && (entry.m_write.m_type == AMH_RAM)) {
				// Found a RAM section
				le_memory_t * memory_node = (le_memory_t *) malloc(sizeof(le_memory_t));
				memory_node->next = last_node;
				memory_node->start = entry.m_addrstart;
				memory_node->size = space.address_to_byte(entry.m_addrend - entry.m_addrstart + 1);
				memory_node->content = (uint8_t *) malloc(memory_node->size);
				last_node = memory_node;
			}
		}
		memory = last_node;
	}

	le_memory_t *memory_node = memory;

	while (memory_node != nullptr) {
		uint8_t *target = memory_node->content;
		offs_t end_address = memory_node->start + memory_node->size - 1;
		for (offs_t byte_address = memory_node->start; byte_address <= end_address; ++byte_address)
		{
			*target++ = space.read_byte(byte_address);
		}
		memory_node = memory_node->next;
	}					
}


int le_init(const running_machine &machine)
{

	le_functions le_functions_;
	le_functions (*le_get_functions) (const char *args);
	const char *error_message;
	char library_file_full_path[PATH_MAX + 1];

	le_library = machine.options().learning_environment();
	if (le_library.empty()) {
		cerr << "Need to specify controller" << endl;
		return 1;
	}

	le_args = machine.options().learning_environment_options();
	g_player = machine.options().learning_environment_player();

	if ((g_player < 1) || (g_player > MAX_PLAYER)) {
		cerr << "Player should be between 1 and " << MAX_PLAYER << endl;
		return 1;
	}

	// Get score encoding and location
	std::string game_name = machine.system().name;
	std::string location_files_directory = machine.options().learning_environment_data_path();
	score_memory_description = get_score_details(game_name, location_files_directory);
	game_over = get_gameover_details(game_name, location_files_directory);


	/* Load given environment */
	if (realpath(le_library.c_str(), library_file_full_path) == NULL) {
		cerr << "Failed to find full path of " << le_library << endl;
		return 1;
	}
	/*fprintf(stderr,"About to load %s\n",le_library.c_str());*/
	le_lib_handle = dlopen(library_file_full_path,RTLD_LAZY | RTLD_GLOBAL);
	if (!le_lib_handle) {
		cerr << "Failed to load " << library_file_full_path << ": " << dlerror() << endl;
		return 1;
	}

	/*fprintf(stderr,"About to get function %s\n",LE_GET_FUNCTIONS_NAME);*/
	le_get_functions = (le_functions (*)(const char *)) dlsym(le_lib_handle,LE_GET_FUNCTIONS_NAME);
	error_message = dlerror();
	if (error_message) {
		cerr << "Could not find " LE_GET_FUNCTIONS_NAME " symbol in " << le_library << ": " << error_message << endl;
		return 1;
	}
	/*fprintf(stderr,"About to call function\n");*/
	le_functions_ = (*le_get_functions)(le_args.c_str());

	le_start_game = le_functions_.start;
	le_finish_game = le_functions_.finish;
	le_update_state = le_functions_.update;
	le_get_actions = le_functions_.actions;
	le_check_reset = le_functions_.check_reset;
	le_consume_memory = le_functions_.consume_memory;

	/* initialise buttons used to all unused */
	for (int player=0; player < MAX_PLAYER; player++) {
		for (int button_index=0; button_index < LE_TOTAL_BUTTONS; button_index++) {
			used_buttons[player][button_index] = false;
		}		
	}


	// cerr << "LE up" << endl;
	return 0;
}

void le_close_display (const running_machine &machine)
{

	// cerr << "close display called" << endl;


	if (le_finish_game) (*le_finish_game)();

	if (frame_buffer.buffer) {
		free(frame_buffer.buffer);
		frame_buffer.buffer = NULL;
	}
	if (le_lib_handle) {
		dlclose(le_lib_handle);
	}

	// cerr << "LE close" << endl;
}


/* invoked by main tree code to update bitmap into screen */
void le_update_display(running_machine &machine, const bitmap_rgb32 &bitmap)
{
	int current_score;
	int game_over;
	le_game_info game_info;
	s32 width, height;

	static bool initialised_buffer = false;
	static int frame_skip = 0;
	static int frame_count = 0;

	// machine isn't properly initialised in mame the first few times this is called
	if (wait_frames > 0) {
		wait_frames --;
		return;
	}


	// Wait until we've initialised input
	if (!g_initialised_input) {
		initialise_input(machine);
		g_initialised_input = true;
	} 

	width = bitmap.width();
	height = bitmap.height();

	if (!initialised_buffer) {
		initialised_buffer = true;

		//cerr << "at open: " << width << " x " << height << endl;

		if (le_start_game) {
			game_info.game_name = machine.system().name;
			game_info.width = width;
			game_info.height = height;

			// buttons actually used
			for (int button_index=0; button_index < LE_TOTAL_BUTTONS; button_index++) {
				game_info.buttons_used[button_index] = used_buttons[g_player-1][button_index];
			}

			le_video_mode = (*le_start_game) (&game_info);
			if (le_video_mode != LE_VIDEO_MODE_BGRA) {
				cerr << "Modes other than BGRA have not been implemented yet!" << endl;
				exit(1);
			}
		}

		switch (le_video_mode) {
		case LE_VIDEO_MODE_RGB:
			video_buffer_byte_length = sizeof(unsigned char) * 3 * width * height;
			break;
		case LE_VIDEO_MODE_BGRA:
		default:
			video_buffer_byte_length = sizeof(unsigned char) * 4 * width * height;
		}

		frame_buffer.buffer = (unsigned char *) malloc(video_buffer_byte_length);
		frame_buffer.width = width;
		frame_buffer.height = height;
	}	

	frame_count++;

	/* If we are meant to skip more frames, quit now */
	if (--frame_skip >= 0) {
		process_events(machine);
		return;
	}

	/* copy the contents of the bitmap to the framebuffer pointer we are going to pass around */
	/* We are copying in the same format that it comes in, one full pixel at a time */
	int x, y;
	u32 *base;
	unsigned int *idest = (unsigned int *) frame_buffer.buffer;
	//fprintf(stderr, "bitmap format %d\n", bitmap.format());
	for (y = 0; y < height; y++) {
		base = reinterpret_cast<u32 *> (bitmap.raw_pixptr(y, 0));
		for (x = 0; x < width; x++) {
			*idest++ = *base++;
		}
	}

	current_score = get_current_score(machine);
	game_over = (is_game_over(machine) ? 1 : 0);

	if (le_update_state) frame_skip = (*le_update_state)(current_score, game_over, &frame_buffer);

	if (le_consume_memory) {
		extract_main_memory(machine);
		le_consume_memory(memory);
	}

	process_events(machine);

	// Check if we should be resetting the machine
	if ((le_check_reset) && (le_check_reset())) {
		machine.schedule_soft_reset();
		wait_frames = WaitFrames;
	}

	return;
}





s32 le_get_input_code_value(input_code code) {
	for (int index=0; index < LE_TOTAL_BUTTONS; index++) {
		if (state.button[index].code == code) {
			return state.button[index].state;
		}
	}
	return 0;
}

