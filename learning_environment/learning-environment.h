#ifndef LE_H
#define LE_H

#include "learning-environment-common.h"

#include "machine.h"
#include "input.h"
#include "video.h"

int le_init(const running_machine &machine);
void le_close_display (const running_machine &machine);
void le_update_display(running_machine &machine, const bitmap_rgb32 &bitmap);
s32 le_get_input_code_value(input_code code);

/* This function has to be implemented by every automatic controller module.  It returns an 
le_functions structure, which is a bunch of function pointers to what should be called to update, 
get actions, check if we should reset and shutdown.  NULL can be returned for any of them */
le_functions le_get_functions(void);

#endif

