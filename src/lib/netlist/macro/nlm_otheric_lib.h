// license:GPL-2.0+
// copyright-holders:Couriersud

#ifndef NLM_OTHER_H_
#define NLM_OTHER_H_

///
/// \file nlm_other.h
///
///
#include "netlist/nl_setup.h"

#ifndef __PLIB_PREPROCESSOR__

/* ----------------------------------------------------------------------------
 *  Netlist Macros
 * ---------------------------------------------------------------------------*/

#if !NL_AUTO_DEVICES

#define MC14584B_GATE(name)                                                    \
		NET_REGISTER_DEV(MC14584B_GATE, name)

#define MC14584B_DIP(name)                                                     \
		NET_REGISTER_DEV(MC14584B_DIP, name)

#define NE566_DIP(name)                                                        \
		NET_REGISTER_DEV(NE566_DIP, name)

#endif

/* ----------------------------------------------------------------------------
 *  External declarations
 * ---------------------------------------------------------------------------*/

// moved to net_lib.h

#endif

#endif
