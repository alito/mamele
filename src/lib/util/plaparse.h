// license:BSD-3-Clause
// copyright-holders:Aaron Giles, Curt Coder, hap
/***************************************************************************

    plaparse.h

    Parser for Berkeley standard (aka Espresso) PLA files into raw fusemaps.

***************************************************************************/

#ifndef MAME_UTIL_PLAPARSE_H
#define MAME_UTIL_PLAPARSE_H

#pragma once

#include "jedparse.h"


/***************************************************************************
    FUNCTION PROTOTYPES
***************************************************************************/

/* parse a file (read into memory) into a jed_data structure */
int pla_parse(util::random_read &src, jed_data *result);

#endif // MAME_UTIL_PLAPARSE_H
