/*
Python bindings for the agent interface of the MAME learning environment.
*/
#include <Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include "learning-environment-common.h"

static PyObject *p_start_func, *p_update_func, *p_get_actions_func, *p_shutdown_func, 
	*p_check_reset_func, *p_consume_memory_func, *p_module;

static int le_video_mode = LE_VIDEO_MODE_BGRA;

static int start_game(const le_game_info *game_info) {
	PyObject *p_args, *p_value;


	/* Convert the array of buttons used to a python list */
	/* Based on Hrvoje Niksic found at https://mail.python.org/pipermail/capi-sig/2009-January/000205.html */
	PyObject *used_buttons_list = PyList_New(LE_TOTAL_BUTTONS);
	if (used_buttons_list == NULL) {
		fprintf(stderr, "Couldn't allocate a new python list\n");
	    exit(1);
	}
	for (int button_index = 0; button_index < LE_TOTAL_BUTTONS; button_index++) {
	    PyObject *button_used = PyBool_FromLong(game_info->buttons_used[button_index]);
	    if (button_used == NULL) {
	    	fprintf(stderr, "Couldn't allocate space for a bool in python conversion of used buttons\n");
	        exit(1);
	    }
	    PyList_SET_ITEM(used_buttons_list, button_index, button_used);   // reference to button_used stolen
	}

	p_args = Py_BuildValue("(siiO)", game_info->game_name, game_info->width, game_info->height, used_buttons_list);
	p_value = PyObject_CallObject(p_start_func, p_args);
	Py_DECREF(p_args);
	if (p_value != NULL) {
		Py_DECREF(p_value);
	} else {
		/* Something went wrong. Bail out */
		fprintf(stderr, "Something went wrong calling the Python-side of start_game. Bailing\n");
		PyErr_Print();					
		Py_Finalize();
		exit(1);		
	}

	return le_video_mode;
}

static void finish_game(void) {
	PyObject *p_args, *p_value;

	if (p_shutdown_func) {
		p_args = Py_BuildValue("()");
		p_value = PyObject_CallObject(p_shutdown_func, p_args);
		Py_DECREF(p_args);
		if (p_value != NULL) {
			//printf("Result of shutdown call: %ld\n", PyLong_AsLong(p_value));
			Py_DECREF(p_value);
		}
	}
	Py_DECREF(p_module);
	/* DO NOT call Py_Finalize cos of interactions with PyCalls on the other side of the interface */
	/*Py_Finalize();*/
}

//void clear_display(void);
static int update_state(int current_score, int game_over, const le_frame_buffer *frame_buffer) {
	int frame_skip, frame_byte_length;
	PyObject *p_args, *p_value;
	PyObject *p_buffer;

	frame_skip = 0;

	if (p_update_func) {
		switch (le_video_mode) {
		case LE_VIDEO_MODE_RGB:
			frame_byte_length = frame_buffer->width * frame_buffer->height * 3;
			break;
		case LE_VIDEO_MODE_BGRA:
		case LE_VIDEO_MODE_RGBA:
		case LE_VIDEO_MODE_ARGB:
		default:
			frame_byte_length = frame_buffer->width * frame_buffer->height * 4;
		}		

		p_buffer = PyMemoryView_FromMemory((char *) (frame_buffer->buffer), frame_byte_length, PyBUF_READ);
		p_args = Py_BuildValue("(iOO)", current_score, (game_over ? Py_True : Py_False), p_buffer);

		if (p_args == NULL) {
			fprintf(stderr, "Error creating arguments to update_state\n");
		} else {
			p_value = PyObject_CallObject(p_update_func, p_args);
			Py_DECREF(p_args);
			if (p_value != NULL) {
				frame_skip = PyLong_AsLong(p_value);
				Py_DECREF(p_value);
			} else {
				/* Something went wrong. Bail out */
				fprintf(stderr, "Something went wrong calling the Python-side of update_state. Bailing\n");
				PyErr_Print();					
				Py_Finalize();
				exit(1);
			}
		}
		Py_DECREF(p_buffer);
	}
	return frame_skip;
}

static le_actions get_actions(void) {
	le_actions actions;
	PyObject *p_args, *p_value;
	int index;

	if (p_get_actions_func) {
		p_args = Py_BuildValue("()");
		p_value = PyObject_CallObject(p_get_actions_func, p_args);
		Py_DECREF(p_args);
		if (p_value != NULL) {
			PyObject *item;
			/* python list to C array code stolen from http://mail.python.org/pipermail/tutor/1999-November/000758.html */
			int arrsize, newstate;

			/* how many elements are in the Python object */
			arrsize = PyObject_Length(p_value);
			if (arrsize != LE_TOTAL_BUTTONS) {
				fprintf(stderr,"not the right number of buttons");
			} else {
				/* create a dynamic C array of integers */
				for (index = 0; index < arrsize; index++) {
					/* get the element from the list/tuple */
					item = PySequence_GetItem(p_value, index);
					/* we should check that item != NULL here */
					/* and make sure that it is a Python integer (but we don't) */
					/* assign to the C array */
					newstate = PyLong_AsLong(item);
					actions.buttons[index] = newstate;
				}
			}
			Py_DECREF(p_value);
		} else {
			/* Something went wrong. Bail out */
			fprintf(stderr, "Something went wrong calling the Python-side of get_actions. Bailing\n");
			PyErr_Print();					
			Py_Finalize();
			exit(1);			
		}
	} else {
		// Return zeros for all buttons if we don't have a useful function
		for (index = 0; index < LE_TOTAL_BUTTONS; index++) {
			actions.buttons[index] = 0;
		}
	}
	return actions;
}

static int check_reset(void) {
	PyObject *p_args, *p_value;
	int should_reset = 0;
	int truthy;

	if (p_check_reset_func) {
		p_args = Py_BuildValue("()");
		p_value = PyObject_CallObject(p_check_reset_func, p_args);
		Py_DECREF(p_args);
		if (p_value != NULL) {
			truthy = PyObject_IsTrue(p_value);
			if (truthy == -1) {
				PyErr_Print();
			} else {
				should_reset = truthy;
			}
			Py_DECREF(p_value);
		} else {
			/* Something went wrong. Bail out */
			fprintf(stderr, "Something went wrong calling the Python-side of check_reset. Bailing\n");
			PyErr_Print();					
			Py_Finalize();
			exit(1);
		}
	}
	return should_reset;
}


static void consume_memory(const le_memory_t *memory) {
	PyObject *p_value, *p_args;
	PyObject *p_memory;
	PyObject *p_start_address;
	PyObject *p_ram_region;

	if (p_consume_memory_func) {
		/* Convert the linked list to a list of (start_address, content) pairs */

		PyObject *p_ram_regions = PyList_New(0);
		if (p_ram_regions == NULL) {
			fprintf(stderr, "Couldn't allocate a new python list\n");
		    exit(1);
		}
		while (memory != NULL) {


			p_memory = PyMemoryView_FromMemory((char *)(memory->content), memory->size, PyBUF_READ);
			if (p_memory == NULL) {
				fprintf(stderr, "Error creating buffer from memory in consume_memory\n");
				PyErr_Print();
				exit(1);
			}

			p_start_address = PyLong_FromSize_t(memory->start);
			if (p_start_address == NULL) {
				fprintf(stderr, "Couldn't convert the start address to a python long\n");
				PyErr_Print();
				exit(1);				
			}

			p_ram_region = PyTuple_New(2);
			if (p_ram_region == NULL) {
				fprintf(stderr, "Couldn't create a 2-tuple\n");
				PyErr_Print();
				exit(1);								
			}

			/* PyTuple_SET_ITEM steals the references */
			PyTuple_SET_ITEM(p_ram_region, 0, p_start_address);
			PyTuple_SET_ITEM(p_ram_region, 1, p_memory);

			if (PyList_Append(p_ram_regions, p_ram_region) != 0) {
				fprintf(stderr, "Couldn't append to list of ram regions\n");
				PyErr_Print();			
				exit(1);
			}
			Py_DECREF(p_ram_region);

			memory = memory->next;
		}

		p_args = Py_BuildValue("(O)", p_ram_regions);
		if (p_args == NULL) {
			fprintf(stderr, "Error creating arguments to consume_memory\n");
			PyErr_Print();
			exit(1);
		}
		Py_DECREF(p_ram_regions);
		p_value = PyObject_CallObject(p_consume_memory_func, p_args);
		Py_DECREF(p_args);
		if (p_value != NULL) {
			/* Don't care what consume_memory returns */
			Py_DECREF(p_value);
		} else {
			/* Something went wrong. Bail out */
			fprintf(stderr, "Something went wrong calling the Python-side of consume_memory. Bailing\n");
			PyErr_Print();					
			Py_Finalize();
			exit(1);
		}
	}
}


/* first word of args is assumed to be the module name, with the second half being the args passed to the module */
le_functions le_get_functions (const char* args) {
	le_functions python_functions;

	PyObject *p_name, *p_dict;
	PyObject *p_args, *p_value;
	int success = 1;
	char *module_name = NULL, *module_args = NULL;
	char *module_path, *module_basename;
	char *module_name_path_copy=NULL, *module_name_basename_copy=NULL;
	char *path_insert_statement=NULL;
	size_t path_insert_statement_length;
	const char *p;

	/*Look for a space.  Chop there for module name.  Rest if any is arguments */
	if ((args != NULL) && (strlen(args) > 0)) {
		p = args;
		while ((p != NULL) && (*p != '\0') && (*p != ' ')) p++;
		if (p != NULL) {
			module_name = strndup(args, p-args);
			if (*p != '\0') {
				module_args = strdup(p+1);
			} else {
				module_args = strdup("");
			}
		}
	}

	if ((module_name == NULL) || (strlen(module_name) == 0)) {
		fprintf(stderr,"Empty module name.  Please supply as first argument of le_args\n");
		exit(1);
	}

	/* Find the path to the module and add it to Python's import path */
	/* POSIX basename and dirname don't seem like the nicest. They may modify inputs */
	
	module_name_path_copy = strdup(module_name);
	module_path = dirname(module_name_path_copy);

	module_name_basename_copy = strdup(module_name);
	module_basename = basename(module_name_basename_copy);

	/* if the basename ends with .py, chop that bit off */
	if ((strlen(module_basename) > 3) && (strcmp(module_basename+strlen(module_basename)-3, ".py") == 0)) {
		module_basename[strlen(module_basename) - 3] = '\0';
	}

	/* Following setup code 'templated' on python docs example */
	Py_Initialize();

	/* Prepend module path directory, so that we don't have to put the controller modules somewhere 
	in the path every time */
	path_insert_statement_length = strlen("import sys; sys.path.insert(0,'')") + strlen(module_path) + 1;
	path_insert_statement = (char *) malloc(path_insert_statement_length);
	sprintf(path_insert_statement, "import sys; sys.path.insert(0,'%s')", module_path);
	if (PyRun_SimpleString(path_insert_statement) == -1) {
		fprintf(stderr,"couldn't add '%s' to PYTHONPATH", module_path);
	} 

	p_name = PyUnicode_DecodeFSDefault(module_basename);
	p_module = PyImport_Import(p_name);
	Py_DECREF(p_name);

	if (p_module != NULL) {
		PyObject *p_get_functions_func;
		p_dict = PyModule_GetDict(p_module);
		/* p_dict is a borrowed reference */
		/* borrowed must not be Py_DECREF-ed */

		p_get_functions_func = PyDict_GetItemString(p_dict, "le_get_functions");

		if (p_get_functions_func && PyCallable_Check(p_get_functions_func)) {
			p_args = Py_BuildValue("(s)", module_args);
			p_value = PyObject_CallObject(p_get_functions_func, p_args);
			Py_DECREF(p_args);
			if (p_value != NULL) {
				int arrsize;

				/* how many elements are in the Python object */
				arrsize = PyObject_Length(p_value);
				if (arrsize != 6) {
					fprintf(stderr,"not the right number of functions returned by le_get_functions\n");
				} else {
					/* FIXME: Is this leaking python references? */
					p_start_func = PySequence_GetItem(p_value, 0);
					p_update_func = PySequence_GetItem(p_value, 1);
					p_get_actions_func = PySequence_GetItem(p_value, 2);
					p_check_reset_func = PySequence_GetItem(p_value, 3);
					p_shutdown_func = PySequence_GetItem(p_value, 4);
					p_consume_memory_func = PySequence_GetItem(p_value, 5);
					if (!p_start_func || (!PyCallable_Check(p_start_func))) {
						if (p_start_func && p_start_func != Py_None) {
							fprintf(stderr,"Start function not usable, skipping\n");
						}
						p_start_func = NULL;
					}					
					if (!p_update_func || (!PyCallable_Check(p_update_func))) {
						if (p_update_func && p_update_func != Py_None) {
							fprintf(stderr,"Update function not usable, skipping\n");
						}
						p_update_func = NULL;
					}
					if (!p_get_actions_func || (!PyCallable_Check(p_get_actions_func))) {
						if (p_get_actions_func && p_get_actions_func != Py_None) {
							fprintf(stderr,"Get actions function not usable, skipping\n");
						}
						p_get_actions_func = NULL;
					}
					if (!p_check_reset_func || (!PyCallable_Check(p_check_reset_func))) {
						if (p_check_reset_func && p_check_reset_func != Py_None) {
							fprintf(stderr,"Check reset function not usable, skipping\n");
						}
						p_check_reset_func = NULL;
					}					
					if (!p_shutdown_func || (!PyCallable_Check(p_shutdown_func))) {
						if (p_shutdown_func && p_shutdown_func != Py_None) {
							fprintf(stderr,"Shutdown function not usable, skipping\n");
						}
						p_shutdown_func = NULL;
					}
					if (!p_consume_memory_func || (!PyCallable_Check(p_consume_memory_func))) {
						if (p_consume_memory_func && p_consume_memory_func != Py_None) {
							fprintf(stderr,"Consume memory function not usable, skipping\n");
						}
						p_consume_memory_func = NULL;
					}					
				}
				Py_DECREF(p_value);
			}
			else {
				PyErr_Print();
				fprintf(stderr,"python interface definition failed\n");
				success = 0;
			}

		} else {
			if (PyErr_Occurred()) {
				PyErr_Print();
			}

			fprintf(stderr,"Interface defining function 'le_get_functions' not found in module %s\n", module_name);
			success = 0;
		}


		if (!success) {
			Py_DECREF(p_module);
		}
	}
	else {
		PyErr_Print();
		fprintf(stderr, "Failed to load python module %s\n", module_name);
		success = 0;
	}
	if (!success) {
		Py_Finalize();
		exit(1);
	}

	if (module_name) free(module_name);
	if (module_args) free(module_args);
	if (module_name_path_copy) free(module_name_path_copy);
	if (module_name_basename_copy) free(module_name_basename_copy);
	if (path_insert_statement) free(path_insert_statement);

	if (p_start_func) {
		python_functions.start = start_game;
	} else {
		python_functions.start = NULL;
	}

	if (p_shutdown_func) {
		python_functions.finish = finish_game;
	} else {
		python_functions.finish = NULL;
	}

	if (p_update_func) {
		python_functions.update = update_state;
	} else {
		python_functions.update = NULL;
	}

	if (p_check_reset_func) {
		python_functions.check_reset = check_reset;
	} else {
		python_functions.check_reset = NULL;
	}

	if (p_consume_memory_func) {
		python_functions.consume_memory = consume_memory;
	} else {
		python_functions.consume_memory = NULL;
	}

	if (p_get_actions_func) {
		python_functions.actions = get_actions;
	} else {
		python_functions.actions = NULL;
	}
	return python_functions;
}
