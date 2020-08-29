// license:GPL-2.0+
// copyright-holders:Couriersud

#ifndef NLM_OPAMP_H_
#define NLM_OPAMP_H_

///
/// \file nlm_opamp.h
///

#include "netlist/nl_setup.h"

#ifndef __PLIB_PREPROCESSOR__

/* ----------------------------------------------------------------------------
 *  Netlist Macros
 * ---------------------------------------------------------------------------*/

#if !NL_AUTO_DEVICES

#define MB3614_DIP(name)                                                       \
		NET_REGISTER_DEV(MB3614_DIP, name)

#define LM324_DIP(name)                                                        \
		NET_REGISTER_DEV(LM324_DIP, name)

#define TL081_DIP(name)                                                        \
		NET_REGISTER_DEV(TL081_DIP, name)

#define TL082_DIP(name)                                                        \
		NET_REGISTER_DEV(TL082_DIP, name)

#define TL084_DIP(name)                                                        \
		NET_REGISTER_DEV(TL084_DIP, name)

#define LM2902_DIP(name)                                                       \
		NET_REGISTER_DEV(LM2902_DIP, name)

#define LM358_DIP(name)                                                        \
		NET_REGISTER_DEV(LM358_DIP, name)

#define LM3900(name)                                                           \
		NET_REGISTER_DEV(LM3900, name)

#define UA741_DIP8(name)                                                       \
		NET_REGISTER_DEV(UA741_DIP8, name)

#define UA741_DIP10(name)                                                      \
		NET_REGISTER_DEV(UA741_DIP10, name)

#define UA741_DIP14(name)                                                      \
		NET_REGISTER_DEV(UA741_DIP14, name)

#define MC1558_DIP(name)                                                        \
		NET_REGISTER_DEV(MC1558_DIP, name)

#define LM747_DIP(name)                                                        \
		NET_REGISTER_DEV(LM747_DIP, name)

#define LM747A_DIP(name)                                                       \
		NET_REGISTER_DEV(LM747A_DIP, name)

#define MC3340_DIP(name)                                                       \
		NET_REGISTER_DEV(MC3340_DIP, name)

#define AN6551_SIL(name)                                                       \
		NET_REGISTER_DEV(AN6551_SIL, name)

#endif // NL_AUTO_DEVICES

/* ----------------------------------------------------------------------------
 *  External declarations
 * ---------------------------------------------------------------------------*/

// moved to net_lib.h

#endif // __PLIB_PREPROCESSOR__

#endif
