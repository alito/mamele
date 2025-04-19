// license:BSD-3-Clause
// copyright-holders:Aaron Giles,Olivier Galibert
/***************************************************************************

    emumem.cpp

    Functions which handle device memory access.
    Address-space related functions

***************************************************************************/

#include "emu.h"
#include <list>
#include <map>
#include "emuopts.h"
#include "debug/debugcpu.h"

#include "emumem_mud.h"
#include "emumem_hea.h"
#include "emumem_hem.h"
#include "emumem_hedp.h"
#include "emumem_heun.h"
#include "emumem_heu.h"
#include "emumem_hedr.h"
#include "emumem_hedw.h"
#include "emumem_hep.h"
#include "emumem_het.h"


//**************************************************************************
//  DEBUGGING
//**************************************************************************

#define VERBOSE 0

#if VERBOSE
template <typename Format, typename... Params> static void VPRINTF(Format &&fmt, Params &&...args)
{
	util::stream_format(std::cerr, std::forward<Format>(fmt), std::forward<Params>(args)...);
}
#else
template <typename Format, typename... Params> static void VPRINTF(Format &&, Params &&...) {}
#endif

#define VALIDATE_REFCOUNTS 0

//**************************************************************************
//  CONSTANTS
//**************************************************************************

namespace {

template <typename Delegate> struct handler_width;
template <> struct handler_width<read8_delegate> { static constexpr int value = 0; };
template <> struct handler_width<read8m_delegate> { static constexpr int value = 0; };
template <> struct handler_width<read8s_delegate> { static constexpr int value = 0; };
template <> struct handler_width<read8sm_delegate> { static constexpr int value = 0; };
template <> struct handler_width<read8mo_delegate> { static constexpr int value = 0; };
template <> struct handler_width<read8smo_delegate> { static constexpr int value = 0; };
template <> struct handler_width<write8_delegate> { static constexpr int value = 0; };
template <> struct handler_width<write8m_delegate> { static constexpr int value = 0; };
template <> struct handler_width<write8s_delegate> { static constexpr int value = 0; };
template <> struct handler_width<write8sm_delegate> { static constexpr int value = 0; };
template <> struct handler_width<write8mo_delegate> { static constexpr int value = 0; };
template <> struct handler_width<write8smo_delegate> { static constexpr int value = 0; };
template <> struct handler_width<read16_delegate> { static constexpr int value = 1; };
template <> struct handler_width<read16m_delegate> { static constexpr int value = 1; };
template <> struct handler_width<read16s_delegate> { static constexpr int value = 1; };
template <> struct handler_width<read16sm_delegate> { static constexpr int value = 1; };
template <> struct handler_width<read16mo_delegate> { static constexpr int value = 1; };
template <> struct handler_width<read16smo_delegate> { static constexpr int value = 1; };
template <> struct handler_width<write16_delegate> { static constexpr int value = 1; };
template <> struct handler_width<write16m_delegate> { static constexpr int value = 1; };
template <> struct handler_width<write16s_delegate> { static constexpr int value = 1; };
template <> struct handler_width<write16sm_delegate> { static constexpr int value = 1; };
template <> struct handler_width<write16mo_delegate> { static constexpr int value = 1; };
template <> struct handler_width<write16smo_delegate> { static constexpr int value = 1; };
template <> struct handler_width<read32_delegate> { static constexpr int value = 2; };
template <> struct handler_width<read32m_delegate> { static constexpr int value = 2; };
template <> struct handler_width<read32s_delegate> { static constexpr int value = 2; };
template <> struct handler_width<read32sm_delegate> { static constexpr int value = 2; };
template <> struct handler_width<read32mo_delegate> { static constexpr int value = 2; };
template <> struct handler_width<read32smo_delegate> { static constexpr int value = 2; };
template <> struct handler_width<write32_delegate> { static constexpr int value = 2; };
template <> struct handler_width<write32m_delegate> { static constexpr int value = 2; };
template <> struct handler_width<write32s_delegate> { static constexpr int value = 2; };
template <> struct handler_width<write32sm_delegate> { static constexpr int value = 2; };
template <> struct handler_width<write32mo_delegate> { static constexpr int value = 2; };
template <> struct handler_width<write32smo_delegate> { static constexpr int value = 2; };
template <> struct handler_width<read64_delegate> { static constexpr int value = 3; };
template <> struct handler_width<read64m_delegate> { static constexpr int value = 3; };
template <> struct handler_width<read64s_delegate> { static constexpr int value = 3; };
template <> struct handler_width<read64sm_delegate> { static constexpr int value = 3; };
template <> struct handler_width<read64mo_delegate> { static constexpr int value = 3; };
template <> struct handler_width<read64smo_delegate> { static constexpr int value = 3; };
template <> struct handler_width<write64_delegate> { static constexpr int value = 3; };
template <> struct handler_width<write64m_delegate> { static constexpr int value = 3; };
template <> struct handler_width<write64s_delegate> { static constexpr int value = 3; };
template <> struct handler_width<write64sm_delegate> { static constexpr int value = 3; };
template <> struct handler_width<write64mo_delegate> { static constexpr int value = 3; };
template <> struct handler_width<write64smo_delegate> { static constexpr int value = 3; };
} // anonymous namespace


//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> address_space_specific

// this is a derived class of address_space with specific width, endianness, and table size
template<int Level, int Width, int AddrShift, endianness_t Endian>
class address_space_specific : public address_space
{
	using uX = typename emu::detail::handler_entry_size<Width>::uX;
	using NativeType = uX;
	using this_type = address_space_specific<Level, Width, AddrShift, Endian>;

	// constants describing the native size
	static constexpr u32 NATIVE_BYTES = 1 << Width;
	static constexpr u32 NATIVE_STEP = AddrShift >= 0 ? NATIVE_BYTES << iabs(AddrShift) : NATIVE_BYTES >> iabs(AddrShift);
	static constexpr u32 NATIVE_MASK = NATIVE_STEP - 1;
	static constexpr u32 NATIVE_BITS = 8 * NATIVE_BYTES;

	static constexpr offs_t offset_to_byte(offs_t offset) { return AddrShift < 0 ? offset << iabs(AddrShift) : offset >> iabs(AddrShift); }

public:
	const handler_entry_read<Width, AddrShift, Endian> *const *m_dispatch_read;
	const handler_entry_write<Width, AddrShift, Endian> *const *m_dispatch_write;

	std::string get_handler_string(read_or_write readorwrite, offs_t byteaddress) const override;
	void dump_maps(std::vector<memory_entry> &read_map, std::vector<memory_entry> &write_map) const override;

	void unmap_generic(offs_t addrstart, offs_t addrend, offs_t addrmirror, read_or_write readorwrite, bool quiet) override;
	void install_ram_generic(offs_t addrstart, offs_t addrend, offs_t addrmirror, read_or_write readorwrite, void *baseptr) override;
	void install_bank_generic(offs_t addrstart, offs_t addrend, offs_t addrmirror, memory_bank *rbank, memory_bank *wbank) override;
	void install_readwrite_port(offs_t addrstart, offs_t addrend, offs_t addrmirror, std::string rtag, std::string wtag) override;
	void install_device_delegate(offs_t addrstart, offs_t addrend, device_t &device, address_map_constructor &map, u64 unitmask = 0, int cswidth = 0) override;
	void install_view(offs_t addrstart, offs_t addrend, offs_t addrmirror, memory_view &view) override;

	void install_read_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read8_delegate rhandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_read_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, rhandler); }
	void install_write_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, write8_delegate whandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_write_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, whandler); }
	void install_readwrite_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read8_delegate rhandler, write8_delegate whandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_readwrite_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, rhandler, whandler); }
	void install_read_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read16_delegate rhandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_read_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, rhandler); }
	void install_write_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, write16_delegate whandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_write_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, whandler); }
	void install_readwrite_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read16_delegate rhandler, write16_delegate whandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_readwrite_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, rhandler, whandler); }
	void install_read_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read32_delegate rhandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_read_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, rhandler); }
	void install_write_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, write32_delegate whandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_write_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, whandler); }
	void install_readwrite_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read32_delegate rhandler, write32_delegate whandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_readwrite_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, rhandler, whandler); }
	void install_read_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read64_delegate rhandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_read_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, rhandler); }
	void install_write_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, write64_delegate whandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_write_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, whandler); }
	void install_readwrite_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read64_delegate rhandler, write64_delegate whandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_readwrite_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, rhandler, whandler); }

	void install_read_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read8m_delegate rhandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_read_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, rhandler); }
	void install_write_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, write8m_delegate whandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_write_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, whandler); }
	void install_readwrite_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read8m_delegate rhandler, write8m_delegate whandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_readwrite_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, rhandler, whandler); }
	void install_read_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read16m_delegate rhandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_read_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, rhandler); }
	void install_write_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, write16m_delegate whandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_write_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, whandler); }
	void install_readwrite_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read16m_delegate rhandler, write16m_delegate whandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_readwrite_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, rhandler, whandler); }
	void install_read_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read32m_delegate rhandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_read_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, rhandler); }
	void install_write_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, write32m_delegate whandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_write_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, whandler); }
	void install_readwrite_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read32m_delegate rhandler, write32m_delegate whandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_readwrite_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, rhandler, whandler); }
	void install_read_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read64m_delegate rhandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_read_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, rhandler); }
	void install_write_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, write64m_delegate whandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_write_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, whandler); }
	void install_readwrite_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read64m_delegate rhandler, write64m_delegate whandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_readwrite_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, rhandler, whandler); }

	void install_read_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read8s_delegate rhandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_read_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, rhandler); }
	void install_write_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, write8s_delegate whandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_write_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, whandler); }
	void install_readwrite_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read8s_delegate rhandler, write8s_delegate whandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_readwrite_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, rhandler, whandler); }
	void install_read_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read16s_delegate rhandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_read_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, rhandler); }
	void install_write_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, write16s_delegate whandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_write_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, whandler); }
	void install_readwrite_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read16s_delegate rhandler, write16s_delegate whandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_readwrite_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, rhandler, whandler); }
	void install_read_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read32s_delegate rhandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_read_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, rhandler); }
	void install_write_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, write32s_delegate whandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_write_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, whandler); }
	void install_readwrite_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read32s_delegate rhandler, write32s_delegate whandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_readwrite_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, rhandler, whandler); }
	void install_read_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read64s_delegate rhandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_read_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, rhandler); }
	void install_write_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, write64s_delegate whandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_write_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, whandler); }
	void install_readwrite_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read64s_delegate rhandler, write64s_delegate whandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_readwrite_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, rhandler, whandler); }

	void install_read_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read8sm_delegate rhandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_read_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, rhandler); }
	void install_write_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, write8sm_delegate whandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_write_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, whandler); }
	void install_readwrite_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read8sm_delegate rhandler, write8sm_delegate whandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_readwrite_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, rhandler, whandler); }
	void install_read_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read16sm_delegate rhandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_read_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, rhandler); }
	void install_write_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, write16sm_delegate whandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_write_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, whandler); }
	void install_readwrite_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read16sm_delegate rhandler, write16sm_delegate whandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_readwrite_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, rhandler, whandler); }
	void install_read_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read32sm_delegate rhandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_read_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, rhandler); }
	void install_write_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, write32sm_delegate whandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_write_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, whandler); }
	void install_readwrite_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read32sm_delegate rhandler, write32sm_delegate whandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_readwrite_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, rhandler, whandler); }
	void install_read_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read64sm_delegate rhandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_read_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, rhandler); }
	void install_write_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, write64sm_delegate whandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_write_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, whandler); }
	void install_readwrite_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read64sm_delegate rhandler, write64sm_delegate whandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_readwrite_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, rhandler, whandler); }

	void install_read_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read8mo_delegate rhandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_read_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, rhandler); }
	void install_write_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, write8mo_delegate whandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_write_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, whandler); }
	void install_readwrite_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read8mo_delegate rhandler, write8mo_delegate whandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_readwrite_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, rhandler, whandler); }
	void install_read_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read16mo_delegate rhandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_read_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, rhandler); }
	void install_write_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, write16mo_delegate whandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_write_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, whandler); }
	void install_readwrite_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read16mo_delegate rhandler, write16mo_delegate whandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_readwrite_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, rhandler, whandler); }
	void install_read_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read32mo_delegate rhandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_read_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, rhandler); }
	void install_write_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, write32mo_delegate whandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_write_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, whandler); }
	void install_readwrite_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read32mo_delegate rhandler, write32mo_delegate whandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_readwrite_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, rhandler, whandler); }
	void install_read_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read64mo_delegate rhandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_read_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, rhandler); }
	void install_write_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, write64mo_delegate whandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_write_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, whandler); }
	void install_readwrite_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read64mo_delegate rhandler, write64mo_delegate whandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_readwrite_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, rhandler, whandler); }

	void install_read_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read8smo_delegate rhandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_read_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, rhandler); }
	void install_write_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, write8smo_delegate whandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_write_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, whandler); }
	void install_readwrite_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read8smo_delegate rhandler, write8smo_delegate whandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_readwrite_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, rhandler, whandler); }
	void install_read_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read16smo_delegate rhandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_read_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, rhandler); }
	void install_write_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, write16smo_delegate whandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_write_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, whandler); }
	void install_readwrite_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read16smo_delegate rhandler, write16smo_delegate whandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_readwrite_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, rhandler, whandler); }
	void install_read_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read32smo_delegate rhandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_read_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, rhandler); }
	void install_write_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, write32smo_delegate whandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_write_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, whandler); }
	void install_readwrite_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read32smo_delegate rhandler, write32smo_delegate whandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_readwrite_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, rhandler, whandler); }
	void install_read_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read64smo_delegate rhandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_read_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, rhandler); }
	void install_write_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, write64smo_delegate whandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_write_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, whandler); }
	void install_readwrite_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read64smo_delegate rhandler, write64smo_delegate whandler, u64 unitmask = 0, int cswidth = 0) override
	{ install_readwrite_handler_impl(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, rhandler, whandler); }

	using address_space::install_read_tap;
	using address_space::install_write_tap;
	using address_space::install_readwrite_tap;

	virtual memory_passthrough_handler *install_read_tap(offs_t addrstart, offs_t addrend, offs_t addrmirror, std::string name, std::function<void (offs_t offset, uX &data, uX mem_mask)> tap, memory_passthrough_handler *mph) override;
	virtual memory_passthrough_handler *install_write_tap(offs_t addrstart, offs_t addrend, offs_t addrmirror, std::string name, std::function<void (offs_t offset, uX &data, uX mem_mask)> tap, memory_passthrough_handler *mph) override;
	virtual memory_passthrough_handler *install_readwrite_tap(offs_t addrstart, offs_t addrend, offs_t addrmirror, std::string name, std::function<void (offs_t offset, uX &data, uX mem_mask)> tapr, std::function<void (offs_t offset, uX &data, uX mem_mask)> tapw, memory_passthrough_handler *mph) override;

	// construction/destruction
	address_space_specific(memory_manager &manager, device_memory_interface &memory, int spacenum, int address_width)
		: address_space(manager, memory, spacenum)
	{
		m_unmap_r = new handler_entry_read_unmapped <Width, AddrShift, Endian>(this);
		m_unmap_w = new handler_entry_write_unmapped<Width, AddrShift, Endian>(this);
		m_nop_r = new handler_entry_read_nop <Width, AddrShift, Endian>(this);
		m_nop_w = new handler_entry_write_nop<Width, AddrShift, Endian>(this);

		handler_entry::range r{ 0, 0xffffffff >> (32 - address_width) };

		switch (address_width) {
			case  1: m_root_read = new handler_entry_read_dispatch< std::max(1, Width), Width, AddrShift, Endian>(this, r, nullptr); m_root_write = new handler_entry_write_dispatch< std::max(1, Width), Width, AddrShift, Endian>(this, r, nullptr); break;
			case  2: m_root_read = new handler_entry_read_dispatch< std::max(2, Width), Width, AddrShift, Endian>(this, r, nullptr); m_root_write = new handler_entry_write_dispatch< std::max(2, Width), Width, AddrShift, Endian>(this, r, nullptr); break;
			case  3: m_root_read = new handler_entry_read_dispatch< std::max(3, Width), Width, AddrShift, Endian>(this, r, nullptr); m_root_write = new handler_entry_write_dispatch< std::max(3, Width), Width, AddrShift, Endian>(this, r, nullptr); break;
			case  4: m_root_read = new handler_entry_read_dispatch< 4, Width, AddrShift, Endian>(this, r, nullptr); m_root_write = new handler_entry_write_dispatch< 4, Width, AddrShift, Endian>(this, r, nullptr); break;
			case  5: m_root_read = new handler_entry_read_dispatch< 5, Width, AddrShift, Endian>(this, r, nullptr); m_root_write = new handler_entry_write_dispatch< 5, Width, AddrShift, Endian>(this, r, nullptr); break;
			case  6: m_root_read = new handler_entry_read_dispatch< 6, Width, AddrShift, Endian>(this, r, nullptr); m_root_write = new handler_entry_write_dispatch< 6, Width, AddrShift, Endian>(this, r, nullptr); break;
			case  7: m_root_read = new handler_entry_read_dispatch< 7, Width, AddrShift, Endian>(this, r, nullptr); m_root_write = new handler_entry_write_dispatch< 7, Width, AddrShift, Endian>(this, r, nullptr); break;
			case  8: m_root_read = new handler_entry_read_dispatch< 8, Width, AddrShift, Endian>(this, r, nullptr); m_root_write = new handler_entry_write_dispatch< 8, Width, AddrShift, Endian>(this, r, nullptr); break;
			case  9: m_root_read = new handler_entry_read_dispatch< 9, Width, AddrShift, Endian>(this, r, nullptr); m_root_write = new handler_entry_write_dispatch< 9, Width, AddrShift, Endian>(this, r, nullptr); break;
			case 10: m_root_read = new handler_entry_read_dispatch<10, Width, AddrShift, Endian>(this, r, nullptr); m_root_write = new handler_entry_write_dispatch<10, Width, AddrShift, Endian>(this, r, nullptr); break;
			case 11: m_root_read = new handler_entry_read_dispatch<11, Width, AddrShift, Endian>(this, r, nullptr); m_root_write = new handler_entry_write_dispatch<11, Width, AddrShift, Endian>(this, r, nullptr); break;
			case 12: m_root_read = new handler_entry_read_dispatch<12, Width, AddrShift, Endian>(this, r, nullptr); m_root_write = new handler_entry_write_dispatch<12, Width, AddrShift, Endian>(this, r, nullptr); break;
			case 13: m_root_read = new handler_entry_read_dispatch<13, Width, AddrShift, Endian>(this, r, nullptr); m_root_write = new handler_entry_write_dispatch<13, Width, AddrShift, Endian>(this, r, nullptr); break;
			case 14: m_root_read = new handler_entry_read_dispatch<14, Width, AddrShift, Endian>(this, r, nullptr); m_root_write = new handler_entry_write_dispatch<14, Width, AddrShift, Endian>(this, r, nullptr); break;
			case 15: m_root_read = new handler_entry_read_dispatch<15, Width, AddrShift, Endian>(this, r, nullptr); m_root_write = new handler_entry_write_dispatch<15, Width, AddrShift, Endian>(this, r, nullptr); break;
			case 16: m_root_read = new handler_entry_read_dispatch<16, Width, AddrShift, Endian>(this, r, nullptr); m_root_write = new handler_entry_write_dispatch<16, Width, AddrShift, Endian>(this, r, nullptr); break;
			case 17: m_root_read = new handler_entry_read_dispatch<17, Width, AddrShift, Endian>(this, r, nullptr); m_root_write = new handler_entry_write_dispatch<17, Width, AddrShift, Endian>(this, r, nullptr); break;
			case 18: m_root_read = new handler_entry_read_dispatch<18, Width, AddrShift, Endian>(this, r, nullptr); m_root_write = new handler_entry_write_dispatch<18, Width, AddrShift, Endian>(this, r, nullptr); break;
			case 19: m_root_read = new handler_entry_read_dispatch<19, Width, AddrShift, Endian>(this, r, nullptr); m_root_write = new handler_entry_write_dispatch<19, Width, AddrShift, Endian>(this, r, nullptr); break;
			case 20: m_root_read = new handler_entry_read_dispatch<20, Width, AddrShift, Endian>(this, r, nullptr); m_root_write = new handler_entry_write_dispatch<20, Width, AddrShift, Endian>(this, r, nullptr); break;
			case 21: m_root_read = new handler_entry_read_dispatch<21, Width, AddrShift, Endian>(this, r, nullptr); m_root_write = new handler_entry_write_dispatch<21, Width, AddrShift, Endian>(this, r, nullptr); break;
			case 22: m_root_read = new handler_entry_read_dispatch<22, Width, AddrShift, Endian>(this, r, nullptr); m_root_write = new handler_entry_write_dispatch<22, Width, AddrShift, Endian>(this, r, nullptr); break;
			case 23: m_root_read = new handler_entry_read_dispatch<23, Width, AddrShift, Endian>(this, r, nullptr); m_root_write = new handler_entry_write_dispatch<23, Width, AddrShift, Endian>(this, r, nullptr); break;
			case 24: m_root_read = new handler_entry_read_dispatch<24, Width, AddrShift, Endian>(this, r, nullptr); m_root_write = new handler_entry_write_dispatch<24, Width, AddrShift, Endian>(this, r, nullptr); break;
			case 25: m_root_read = new handler_entry_read_dispatch<25, Width, AddrShift, Endian>(this, r, nullptr); m_root_write = new handler_entry_write_dispatch<25, Width, AddrShift, Endian>(this, r, nullptr); break;
			case 26: m_root_read = new handler_entry_read_dispatch<26, Width, AddrShift, Endian>(this, r, nullptr); m_root_write = new handler_entry_write_dispatch<26, Width, AddrShift, Endian>(this, r, nullptr); break;
			case 27: m_root_read = new handler_entry_read_dispatch<27, Width, AddrShift, Endian>(this, r, nullptr); m_root_write = new handler_entry_write_dispatch<27, Width, AddrShift, Endian>(this, r, nullptr); break;
			case 28: m_root_read = new handler_entry_read_dispatch<28, Width, AddrShift, Endian>(this, r, nullptr); m_root_write = new handler_entry_write_dispatch<28, Width, AddrShift, Endian>(this, r, nullptr); break;
			case 29: m_root_read = new handler_entry_read_dispatch<29, Width, AddrShift, Endian>(this, r, nullptr); m_root_write = new handler_entry_write_dispatch<29, Width, AddrShift, Endian>(this, r, nullptr); break;
			case 30: m_root_read = new handler_entry_read_dispatch<30, Width, AddrShift, Endian>(this, r, nullptr); m_root_write = new handler_entry_write_dispatch<30, Width, AddrShift, Endian>(this, r, nullptr); break;
			case 31: m_root_read = new handler_entry_read_dispatch<31, Width, AddrShift, Endian>(this, r, nullptr); m_root_write = new handler_entry_write_dispatch<31, Width, AddrShift, Endian>(this, r, nullptr); break;
			case 32: m_root_read = new handler_entry_read_dispatch<32, Width, AddrShift, Endian>(this, r, nullptr); m_root_write = new handler_entry_write_dispatch<32, Width, AddrShift, Endian>(this, r, nullptr); break;
			default: fatalerror("Unhandled address bus width %d\n", address_width);
		}

		m_dispatch_read  = m_root_read ->get_dispatch();
		m_dispatch_write = m_root_write->get_dispatch();
	}

	std::pair<void *, void *> get_cache_info() override {
		std::pair<void *, void *> rw;
		rw.first  = m_root_read;
		rw.second = m_root_write;
		return rw;
	}

	std::pair<const void *, const void *> get_specific_info() override {
		std::pair<const void *, const void *> rw;
		rw.first  = m_dispatch_read;
		rw.second = m_dispatch_write;
		return rw;
	}

	void delayed_ref(handler_entry *e) {
		e->ref();
		m_delayed_unrefs.insert(e);
	}

	void delayed_unref(handler_entry *e) {
		m_delayed_unrefs.erase(m_delayed_unrefs.find(e));
		e->unref();
	}

	void validate_reference_counts() const override {
		handler_entry::reflist refs;
		refs.add(m_root_read);
		refs.add(m_root_write);
		refs.add(m_unmap_r);
		refs.add(m_unmap_w);
		refs.add(m_nop_r);
		refs.add(m_nop_w);
		for(handler_entry *e : m_delayed_unrefs)
			refs.add(e);
		refs.propagate();
		refs.check();
	}

	virtual void remove_passthrough(std::unordered_set<handler_entry *> &handlers) override {
		invalidate_caches(read_or_write::READWRITE);
		m_root_read->detach(handlers);
		m_root_write->detach(handlers);
	}

	// generate accessor table
	virtual void accessors(data_accessors &accessors) const override
	{
		accessors.read_byte = reinterpret_cast<u8 (*)(address_space &, offs_t)>(&read_byte_static);
		accessors.read_word = reinterpret_cast<u16 (*)(address_space &, offs_t)>(&read_word_static);
		accessors.read_word_masked = reinterpret_cast<u16 (*)(address_space &, offs_t, u16)>(&read_word_masked_static);
		accessors.read_dword = reinterpret_cast<u32 (*)(address_space &, offs_t)>(&read_dword_static);
		accessors.read_dword_masked = reinterpret_cast<u32 (*)(address_space &, offs_t, u32)>(&read_dword_masked_static);
		accessors.read_qword = reinterpret_cast<u64 (*)(address_space &, offs_t)>(&read_qword_static);
		accessors.read_qword_masked = reinterpret_cast<u64 (*)(address_space &, offs_t, u64)>(&read_qword_masked_static);
		accessors.write_byte = reinterpret_cast<void (*)(address_space &, offs_t, u8)>(&write_byte_static);
		accessors.write_word = reinterpret_cast<void (*)(address_space &, offs_t, u16)>(&write_word_static);
		accessors.write_word_masked = reinterpret_cast<void (*)(address_space &, offs_t, u16, u16)>(&write_word_masked_static);
		accessors.write_dword = reinterpret_cast<void (*)(address_space &, offs_t, u32)>(&write_dword_static);
		accessors.write_dword_masked = reinterpret_cast<void (*)(address_space &, offs_t, u32, u32)>(&write_dword_masked_static);
		accessors.write_qword = reinterpret_cast<void (*)(address_space &, offs_t, u64)>(&write_qword_static);
		accessors.write_qword_masked = reinterpret_cast<void (*)(address_space &, offs_t, u64, u64)>(&write_qword_masked_static);
	}

	// return a pointer to the read bank, or nullptr if none
	virtual void *get_read_ptr(offs_t address) const override
	{
		return m_root_read->get_ptr(address);
	}

	// return a pointer to the write bank, or nullptr if none
	virtual void *get_write_ptr(offs_t address) const override
	{
		return m_root_write->get_ptr(address);
	}

	// native read
	NativeType read_native(offs_t offset, NativeType mask)
	{
		return dispatch_read<Level, Width, AddrShift, Endian>(offs_t(-1), offset & m_addrmask, mask, m_dispatch_read);
	}

	// mask-less native read
	NativeType read_native(offs_t offset)
	{
		return dispatch_read<Level, Width, AddrShift, Endian>(offs_t(-1), offset & m_addrmask, uX(0xffffffffffffffffU), m_dispatch_read);
	}

	// native write
	void write_native(offs_t offset, NativeType data, NativeType mask)
	{
		dispatch_write<Level, Width, AddrShift, Endian>(offs_t(-1), offset & m_addrmask, data, mask, m_dispatch_write);
	}

	// mask-less native write
	void write_native(offs_t offset, NativeType data)
	{
		dispatch_write<Level, Width, AddrShift, Endian>(offs_t(-1), offset & m_addrmask, data, uX(0xffffffffffffffffU), m_dispatch_write);
	}

	// virtual access to these functions
	u8 read_byte(offs_t address) override { return Width == 0 ? read_native(address & ~NATIVE_MASK) : memory_read_generic<Width, AddrShift, Endian, 0, true>([this](offs_t offset, NativeType mask) -> NativeType { return read_native(offset, mask); }, address, 0xff); }
	u16 read_word(offs_t address) override { return Width == 1 ? read_native(address & ~NATIVE_MASK) : memory_read_generic<Width, AddrShift, Endian, 1, true>([this](offs_t offset, NativeType mask) -> NativeType { return read_native(offset, mask); }, address, 0xffff); }
	u16 read_word(offs_t address, u16 mask) override { return memory_read_generic<Width, AddrShift, Endian, 1, true>([this](offs_t offset, NativeType mask) -> NativeType { return read_native(offset, mask); }, address, mask); }
	u16 read_word_unaligned(offs_t address) override { return memory_read_generic<Width, AddrShift, Endian, 1, false>([this](offs_t offset, NativeType mask) -> NativeType { return read_native(offset, mask); }, address, 0xffff); }
	u16 read_word_unaligned(offs_t address, u16 mask) override { return memory_read_generic<Width, AddrShift, Endian, 1, false>([this](offs_t offset, NativeType mask) -> NativeType { return read_native(offset, mask); }, address, mask); }
	u32 read_dword(offs_t address) override { return Width == 2 ? read_native(address & ~NATIVE_MASK) : memory_read_generic<Width, AddrShift, Endian, 2, true>([this](offs_t offset, NativeType mask) -> NativeType { return read_native(offset, mask); }, address, 0xffffffff); }
	u32 read_dword(offs_t address, u32 mask) override { return memory_read_generic<Width, AddrShift, Endian, 2, true>([this](offs_t offset, NativeType mask) -> NativeType { return read_native(offset, mask); }, address, mask); }
	u32 read_dword_unaligned(offs_t address) override { return memory_read_generic<Width, AddrShift, Endian, 2, false>([this](offs_t offset, NativeType mask) -> NativeType { return read_native(offset, mask); }, address, 0xffffffff); }
	u32 read_dword_unaligned(offs_t address, u32 mask) override { return memory_read_generic<Width, AddrShift, Endian, 2, false>([this](offs_t offset, NativeType mask) -> NativeType { return read_native(offset, mask); }, address, mask); }
	u64 read_qword(offs_t address) override { return Width == 3 ? read_native(address & ~NATIVE_MASK) : memory_read_generic<Width, AddrShift, Endian, 3, true>([this](offs_t offset, NativeType mask) -> NativeType { return read_native(offset, mask); }, address, 0xffffffffffffffffU); }
	u64 read_qword(offs_t address, u64 mask) override { return memory_read_generic<Width, AddrShift, Endian, 3, true>([this](offs_t offset, NativeType mask) -> NativeType { return read_native(offset, mask); }, address, mask); }
	u64 read_qword_unaligned(offs_t address) override { return memory_read_generic<Width, AddrShift, Endian, 3, false>([this](offs_t offset, NativeType mask) -> NativeType { return read_native(offset, mask); }, address, 0xffffffffffffffffU); }
	u64 read_qword_unaligned(offs_t address, u64 mask) override { return memory_read_generic<Width, AddrShift, Endian, 3, false>([this](offs_t offset, NativeType mask) -> NativeType { return read_native(offset, mask); }, address, mask); }

	void write_byte(offs_t address, u8 data) override { if (Width == 0) write_native(address & ~NATIVE_MASK, data); else memory_write_generic<Width, AddrShift, Endian, 0, true>([this](offs_t offset, NativeType data, NativeType mask) { write_native(offset, data, mask); }, address, data, 0xff); }
	void write_word(offs_t address, u16 data) override { if (Width == 1) write_native(address & ~NATIVE_MASK, data); else memory_write_generic<Width, AddrShift, Endian, 1, true>([this](offs_t offset, NativeType data, NativeType mask) { write_native(offset, data, mask); }, address, data, 0xffff); }
	void write_word(offs_t address, u16 data, u16 mask) override { memory_write_generic<Width, AddrShift, Endian, 1, true>([this](offs_t offset, NativeType data, NativeType mask) { write_native(offset, data, mask); }, address, data, mask); }
	void write_word_unaligned(offs_t address, u16 data) override { memory_write_generic<Width, AddrShift, Endian, 1, false>([this](offs_t offset, NativeType data, NativeType mask) { write_native(offset, data, mask); }, address, data, 0xffff); }
	void write_word_unaligned(offs_t address, u16 data, u16 mask) override { memory_write_generic<Width, AddrShift, Endian, 1, false>([this](offs_t offset, NativeType data, NativeType mask) { write_native(offset, data, mask); }, address, data, mask); }
	void write_dword(offs_t address, u32 data) override { if (Width == 2) write_native(address & ~NATIVE_MASK, data); else memory_write_generic<Width, AddrShift, Endian, 2, true>([this](offs_t offset, NativeType data, NativeType mask) { write_native(offset, data, mask); }, address, data, 0xffffffff); }
	void write_dword(offs_t address, u32 data, u32 mask) override { memory_write_generic<Width, AddrShift, Endian, 2, true>([this](offs_t offset, NativeType data, NativeType mask) { write_native(offset, data, mask); }, address, data, mask); }
	void write_dword_unaligned(offs_t address, u32 data) override { memory_write_generic<Width, AddrShift, Endian, 2, false>([this](offs_t offset, NativeType data, NativeType mask) { write_native(offset, data, mask); }, address, data, 0xffffffff); }
	void write_dword_unaligned(offs_t address, u32 data, u32 mask) override { memory_write_generic<Width, AddrShift, Endian, 2, false>([this](offs_t offset, NativeType data, NativeType mask) { write_native(offset, data, mask); }, address, data, mask); }
	void write_qword(offs_t address, u64 data) override { if (Width == 3) write_native(address & ~NATIVE_MASK, data); else memory_write_generic<Width, AddrShift, Endian, 3, true>([this](offs_t offset, NativeType data, NativeType mask) { write_native(offset, data, mask); }, address, data, 0xffffffffffffffffU); }
	void write_qword(offs_t address, u64 data, u64 mask) override { memory_write_generic<Width, AddrShift, Endian, 3, true>([this](offs_t offset, NativeType data, NativeType mask) { write_native(offset, data, mask); }, address, data, mask); }
	void write_qword_unaligned(offs_t address, u64 data) override { memory_write_generic<Width, AddrShift, Endian, 3, false>([this](offs_t offset, NativeType data, NativeType mask) { write_native(offset, data, mask); }, address, data, 0xffffffffffffffffU); }
	void write_qword_unaligned(offs_t address, u64 data, u64 mask) override { memory_write_generic<Width, AddrShift, Endian, 3, false>([this](offs_t offset, NativeType data, NativeType mask) { write_native(offset, data, mask); }, address, data, mask); }

	// static access to these functions
	static u8 read_byte_static(this_type &space, offs_t address) { return Width == 0 ? space.read_native(address & ~NATIVE_MASK) : memory_read_generic<Width, AddrShift, Endian, 0, true>([&space](offs_t offset, NativeType mask) -> NativeType { return space.read_native(offset, mask); }, address, 0xff); }
	static u16 read_word_static(this_type &space, offs_t address) { return Width == 1 ? space.read_native(address & ~NATIVE_MASK) : memory_read_generic<Width, AddrShift, Endian, 1, true>([&space](offs_t offset, NativeType mask) -> NativeType { return space.read_native(offset, mask); }, address, 0xffff); }
	static u16 read_word_masked_static(this_type &space, offs_t address, u16 mask) { return memory_read_generic<Width, AddrShift, Endian, 1, true>([&space](offs_t offset, NativeType mask) -> NativeType { return space.read_native(offset, mask); }, address, mask); }
	static u32 read_dword_static(this_type &space, offs_t address) { return Width == 2 ? space.read_native(address & ~NATIVE_MASK) : memory_read_generic<Width, AddrShift, Endian, 2, true>([&space](offs_t offset, NativeType mask) -> NativeType { return space.read_native(offset, mask); }, address, 0xffffffff); }
	static u32 read_dword_masked_static(this_type &space, offs_t address, u32 mask) { return memory_read_generic<Width, AddrShift, Endian, 2, true>([&space](offs_t offset, NativeType mask) -> NativeType { return space.read_native(offset, mask); }, address, mask); }
	static u64 read_qword_static(this_type &space, offs_t address) { return Width == 3 ? space.read_native(address & ~NATIVE_MASK) : memory_read_generic<Width, AddrShift, Endian, 3, true>([&space](offs_t offset, NativeType mask) -> NativeType { return space.read_native(offset, mask); }, address, 0xffffffffffffffffU); }
	static u64 read_qword_masked_static(this_type &space, offs_t address, u64 mask) { return memory_read_generic<Width, AddrShift, Endian, 3, true>([&space](offs_t offset, NativeType mask) -> NativeType { return space.read_native(offset, mask); }, address, mask); }
	static void write_byte_static(this_type &space, offs_t address, u8 data) { if (Width == 0) space.write_native(address & ~NATIVE_MASK, data); else memory_write_generic<Width, AddrShift, Endian, 0, true>([&space](offs_t offset, NativeType data, NativeType mask) { space.write_native(offset, data, mask); }, address, data, 0xff); }
	static void write_word_static(this_type &space, offs_t address, u16 data) { if (Width == 1) space.write_native(address & ~NATIVE_MASK, data); else memory_write_generic<Width, AddrShift, Endian, 1, true>([&space](offs_t offset, NativeType data, NativeType mask) { space.write_native(offset, data, mask); }, address, data, 0xffff); }
	static void write_word_masked_static(this_type &space, offs_t address, u16 data, u16 mask) { memory_write_generic<Width, AddrShift, Endian, 1, true>([&space](offs_t offset, NativeType data, NativeType mask) { space.write_native(offset, data, mask); }, address, data, mask); }
	static void write_dword_static(this_type &space, offs_t address, u32 data) { if (Width == 2) space.write_native(address & ~NATIVE_MASK, data); else memory_write_generic<Width, AddrShift, Endian, 2, true>([&space](offs_t offset, NativeType data, NativeType mask) { space.write_native(offset, data, mask); }, address, data, 0xffffffff); }
	static void write_dword_masked_static(this_type &space, offs_t address, u32 data, u32 mask) { memory_write_generic<Width, AddrShift, Endian, 2, true>([&space](offs_t offset, NativeType data, NativeType mask) { space.write_native(offset, data, mask); }, address, data, mask); }
	static void write_qword_static(this_type &space, offs_t address, u64 data) { if (Width == 3) space.write_native(address & ~NATIVE_MASK, data); else memory_write_generic<Width, AddrShift, Endian, 3, false>([&space](offs_t offset, NativeType data, NativeType mask) { space.write_native(offset, data, mask); }, address, data, 0xffffffffffffffffU); }
	static void write_qword_masked_static(this_type &space, offs_t address, u64 data, u64 mask) { memory_write_generic<Width, AddrShift, Endian, 3, false>([&space](offs_t offset, NativeType data, NativeType mask) { space.write_native(offset, data, mask); }, address, data, mask); }

	handler_entry_read <Width, AddrShift, Endian> *m_root_read;
	handler_entry_write<Width, AddrShift, Endian> *m_root_write;

	std::unordered_set<handler_entry *> m_delayed_unrefs;

private:
	template<typename READ>
	void install_read_handler_impl(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, u64 unitmask, int cswidth, READ &handler_r)
	{
		try { handler_r.resolve(); }
		catch (const binding_type_exception &) {
			osd_printf_error("Binding error while installing read handler %s for range 0x%X-0x%X mask 0x%X mirror 0x%X select 0x%X umask 0x%X\n", handler_r.name(), addrstart, addrend, addrmask, addrmirror, addrselect, unitmask);
			throw;
		}
		install_read_handler_helper<handler_width<READ>::value>(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, handler_r);
	}

	template<typename WRITE>
	void install_write_handler_impl(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, u64 unitmask, int cswidth, WRITE &handler_w)
	{
		try { handler_w.resolve(); }
		catch (const binding_type_exception &) {
			osd_printf_error("Binding error while installing write handler %s for range 0x%X-0x%X mask 0x%X mirror 0x%X select 0x%X umask 0x%X\n", handler_w.name(), addrstart, addrend, addrmask, addrmirror, addrselect, unitmask);
			throw;
		}
		install_write_handler_helper<handler_width<WRITE>::value>(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, handler_w);
	}

	template<typename READ, typename WRITE>
	void install_readwrite_handler_impl(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, u64 unitmask, int cswidth, READ &handler_r, WRITE &handler_w)
	{
		static_assert(handler_width<READ>::value == handler_width<WRITE>::value, "handler widths do not match");
		try { handler_r.resolve(); }
		catch (const binding_type_exception &) {
			osd_printf_error("Binding error while installing read handler %s for range 0x%X-0x%X mask 0x%X mirror 0x%X select 0x%X umask 0x%X\n", handler_r.name(), addrstart, addrend, addrmask, addrmirror, addrselect, unitmask);
			throw;
		}
		try { handler_w.resolve(); }
		catch (const binding_type_exception &) {
			osd_printf_error("Binding error while installing write handler %s for range 0x%X-0x%X mask 0x%X mirror 0x%X select 0x%X umask 0x%X\n", handler_w.name(), addrstart, addrend, addrmask, addrmirror, addrselect, unitmask);
			throw;
		}
		install_readwrite_handler_helper<handler_width<READ>::value>(addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, handler_r, handler_w);
	}

	template<int AccessWidth, typename READ>
	void install_read_handler_helper(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, u64 unitmask, int cswidth, const READ &handler_r)
	{
		if constexpr (Width < AccessWidth) {
			fatalerror("install_read_handler: cannot install a %d-wide handler in a %d-wide bus", 8 << AccessWidth, 8 << Width);
		} else {
			VPRINTF("address_space::install_read_handler(%*x-%*x mask=%*x mirror=%*x, space width=%d, handler width=%d, %s, %*x)\n",
					m_addrchars, addrstart, m_addrchars, addrend,
					m_addrchars, addrmask, m_addrchars, addrmirror,
					8 << Width, 8 << AccessWidth,
					handler_r.name(), data_width() / 4, unitmask);

			offs_t nstart, nend, nmask, nmirror;
			u64 nunitmask;
			int ncswidth;
			check_optimize_all("install_read_handler", 8 << AccessWidth, addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, nstart, nend, nmask, nmirror, nunitmask, ncswidth);

			if constexpr (Width == AccessWidth) {
				auto hand_r = new handler_entry_read_delegate<Width, AddrShift, Endian, READ>(this, handler_r);
				hand_r->set_address_info(nstart, nmask);
				m_root_read->populate(nstart, nend, nmirror, hand_r);
			} else {
				auto hand_r = new handler_entry_read_delegate<AccessWidth, -AccessWidth, Endian, READ>(this, handler_r);
				memory_units_descriptor<Width, AddrShift, Endian> descriptor(AccessWidth, Endian, hand_r, nstart, nend, nmask, nunitmask, ncswidth);
				hand_r->set_address_info(descriptor.get_handler_start(), descriptor.get_handler_mask());
				m_root_read->populate_mismatched(nstart, nend, nmirror, descriptor);
				hand_r->unref();
			}
			invalidate_caches(read_or_write::READ);
		}
	}

	template<int AccessWidth, typename WRITE>
	void install_write_handler_helper(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, u64 unitmask, int cswidth,
								 const WRITE &handler_w)
	{
		if constexpr (Width < AccessWidth) {
			fatalerror("install_write_handler: cannot install a %d-wide handler in a %d-wide bus", 8 << AccessWidth, 8 << Width);
		} else {
			VPRINTF("address_space::install_write_handler(%*x-%*x mask=%*x mirror=%*x, space width=%d, handler width=%d, %s, %*x)\n",
					m_addrchars, addrstart, m_addrchars, addrend,
					m_addrchars, addrmask, m_addrchars, addrmirror,
					8 << Width, 8 << AccessWidth,
					handler_w.name(), data_width() / 4, unitmask);

			offs_t nstart, nend, nmask, nmirror;
			u64 nunitmask;
			int ncswidth;
			check_optimize_all("install_write_handler", 8 << AccessWidth, addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, nstart, nend, nmask, nmirror, nunitmask, ncswidth);

			if constexpr (Width == AccessWidth) {
				auto hand_w = new handler_entry_write_delegate<Width, AddrShift, Endian, WRITE>(this, handler_w);
				hand_w->set_address_info(nstart, nmask);
				m_root_write->populate(nstart, nend, nmirror, hand_w);
			} else {
				auto hand_w = new handler_entry_write_delegate<AccessWidth, -AccessWidth, Endian, WRITE>(this, handler_w);
				memory_units_descriptor<Width, AddrShift, Endian> descriptor(AccessWidth, Endian, hand_w, nstart, nend, nmask, nunitmask, ncswidth);
				hand_w->set_address_info(descriptor.get_handler_start(), descriptor.get_handler_mask());
				m_root_write->populate_mismatched(nstart, nend, nmirror, descriptor);
				hand_w->unref();
			}
			invalidate_caches(read_or_write::WRITE);
		}
	}

	template<int AccessWidth, typename READ, typename WRITE>
	void install_readwrite_handler_helper(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, u64 unitmask, int cswidth,
									 const READ  &handler_r,
									 const WRITE &handler_w)
	{
		if constexpr (Width < AccessWidth) {
			fatalerror("install_readwrite_handler: cannot install a %d-wide handler in a %d-wide bus", 8 << AccessWidth, 8 << Width);
		} else {
			VPRINTF("address_space::install_readwrite_handler(%*x-%*x mask=%*x mirror=%*x, space width=%d, handler width=%d, %s, %s, %*x)\n",
					m_addrchars, addrstart, m_addrchars, addrend,
					m_addrchars, addrmask, m_addrchars, addrmirror,
					8 << Width, 8 << AccessWidth,
					handler_r.name(), handler_w.name(), data_width() / 4, unitmask);

			offs_t nstart, nend, nmask, nmirror;
			u64 nunitmask;
			int ncswidth;
			check_optimize_all("install_readwrite_handler", 8 << AccessWidth, addrstart, addrend, addrmask, addrmirror, addrselect, unitmask, cswidth, nstart, nend, nmask, nmirror, nunitmask, ncswidth);

			if constexpr (Width == AccessWidth) {
				auto hand_r = new handler_entry_read_delegate <Width, AddrShift, Endian, READ>(this, handler_r);
				hand_r->set_address_info(nstart, nmask);
				m_root_read ->populate(nstart, nend, nmirror, hand_r);

				auto hand_w = new handler_entry_write_delegate<Width, AddrShift, Endian, WRITE>(this, handler_w);
				hand_w->set_address_info(nstart, nmask);
				m_root_write->populate(nstart, nend, nmirror, hand_w);
			} else {
				auto hand_r = new handler_entry_read_delegate <AccessWidth, -AccessWidth, Endian, READ>(this, handler_r);
				memory_units_descriptor<Width, AddrShift, Endian> descriptor(AccessWidth, Endian, hand_r, nstart, nend, nmask, nunitmask, ncswidth);
				hand_r->set_address_info(descriptor.get_handler_start(), descriptor.get_handler_mask());
				m_root_read ->populate_mismatched(nstart, nend, nmirror, descriptor);
				hand_r->unref();

				auto hand_w = new handler_entry_write_delegate<AccessWidth, -AccessWidth, Endian, WRITE>(this, handler_w);
				descriptor.set_subunit_handler(hand_w);
				hand_w->set_address_info(descriptor.get_handler_start(), descriptor.get_handler_mask());
				m_root_write->populate_mismatched(nstart, nend, nmirror, descriptor);
				hand_w->unref();
			}
			invalidate_caches(read_or_write::READWRITE);
		}
	}
};



//-------------------------------------------------
//  allocate - allocate memory spaces
//-------------------------------------------------

void memory_manager::allocate(device_memory_interface &memory)
{
	for (int spacenum = 0; spacenum < memory.max_space_count(); ++spacenum)
	{
		// if there is a configuration for this space, we need an address space
		address_space_config const *const spaceconfig = memory.space_config(spacenum);
		if (spaceconfig)
		{
			int level = emu::detail::handler_entry_dispatch_level(spaceconfig->addr_width());
			// allocate one of the appropriate type
			switch ((level << 8) | (spaceconfig->endianness() == ENDIANNESS_BIG ? 0x1000 : 0) |spaceconfig->data_width() | (spaceconfig->addr_shift() + 4))
			{
				case 0x0000|0x000| 8|(4+1): memory.allocate<address_space_specific<0, 0,  1, ENDIANNESS_LITTLE>>(*this, spacenum); break;
				case 0x1000|0x000| 8|(4+1): memory.allocate<address_space_specific<0, 0,  1, ENDIANNESS_BIG   >>(*this, spacenum); break;
				case 0x0000|0x100| 8|(4+1): memory.allocate<address_space_specific<1, 0,  1, ENDIANNESS_LITTLE>>(*this, spacenum); break;
				case 0x1000|0x100| 8|(4+1): memory.allocate<address_space_specific<1, 0,  1, ENDIANNESS_BIG   >>(*this, spacenum); break;

				case 0x0000|0x000| 8|(4-0): memory.allocate<address_space_specific<0, 0,  0, ENDIANNESS_LITTLE>>(*this, spacenum); break;
				case 0x1000|0x000| 8|(4-0): memory.allocate<address_space_specific<0, 0,  0, ENDIANNESS_BIG   >>(*this, spacenum); break;
				case 0x0000|0x100| 8|(4-0): memory.allocate<address_space_specific<1, 0,  0, ENDIANNESS_LITTLE>>(*this, spacenum); break;
				case 0x1000|0x100| 8|(4-0): memory.allocate<address_space_specific<1, 0,  0, ENDIANNESS_BIG   >>(*this, spacenum); break;

				case 0x0000|0x000|16|(4+3): memory.allocate<address_space_specific<0, 1,  3, ENDIANNESS_LITTLE>>(*this, spacenum); break;
				case 0x1000|0x000|16|(4+3): memory.allocate<address_space_specific<0, 1,  3, ENDIANNESS_BIG   >>(*this, spacenum); break;
				case 0x0000|0x100|16|(4+3): memory.allocate<address_space_specific<1, 1,  3, ENDIANNESS_LITTLE>>(*this, spacenum); break;
				case 0x1000|0x100|16|(4+3): memory.allocate<address_space_specific<1, 1,  3, ENDIANNESS_BIG   >>(*this, spacenum); break;

				case 0x0000|0x000|16|(4-0): memory.allocate<address_space_specific<0, 1,  0, ENDIANNESS_LITTLE>>(*this, spacenum); break;
				case 0x1000|0x000|16|(4-0): memory.allocate<address_space_specific<0, 1,  0, ENDIANNESS_BIG   >>(*this, spacenum); break;
				case 0x0000|0x100|16|(4-0): memory.allocate<address_space_specific<1, 1,  0, ENDIANNESS_LITTLE>>(*this, spacenum); break;
				case 0x1000|0x100|16|(4-0): memory.allocate<address_space_specific<1, 1,  0, ENDIANNESS_BIG   >>(*this, spacenum); break;

				case 0x0000|0x000|16|(4-1): memory.allocate<address_space_specific<0, 1, -1, ENDIANNESS_LITTLE>>(*this, spacenum); break;
				case 0x1000|0x000|16|(4-1): memory.allocate<address_space_specific<0, 1, -1, ENDIANNESS_BIG   >>(*this, spacenum); break;
				case 0x0000|0x100|16|(4-1): memory.allocate<address_space_specific<1, 1, -1, ENDIANNESS_LITTLE>>(*this, spacenum); break;
				case 0x1000|0x100|16|(4-1): memory.allocate<address_space_specific<1, 1, -1, ENDIANNESS_BIG   >>(*this, spacenum); break;

				case 0x0000|0x000|32|(4+3): memory.allocate<address_space_specific<0, 2,  3, ENDIANNESS_LITTLE>>(*this, spacenum); break;
				case 0x1000|0x000|32|(4+3): memory.allocate<address_space_specific<0, 2,  3, ENDIANNESS_BIG   >>(*this, spacenum); break;
				case 0x0000|0x100|32|(4+3): memory.allocate<address_space_specific<1, 2,  3, ENDIANNESS_LITTLE>>(*this, spacenum); break;
				case 0x1000|0x100|32|(4+3): memory.allocate<address_space_specific<1, 2,  3, ENDIANNESS_BIG   >>(*this, spacenum); break;

				case 0x0000|0x000|32|(4-0): memory.allocate<address_space_specific<0, 2,  0, ENDIANNESS_LITTLE>>(*this, spacenum); break;
				case 0x1000|0x000|32|(4-0): memory.allocate<address_space_specific<0, 2,  0, ENDIANNESS_BIG   >>(*this, spacenum); break;
				case 0x0000|0x100|32|(4-0): memory.allocate<address_space_specific<1, 2,  0, ENDIANNESS_LITTLE>>(*this, spacenum); break;
				case 0x1000|0x100|32|(4-0): memory.allocate<address_space_specific<1, 2,  0, ENDIANNESS_BIG   >>(*this, spacenum); break;

				case 0x0000|0x000|32|(4-1): memory.allocate<address_space_specific<0, 2, -1, ENDIANNESS_LITTLE>>(*this, spacenum); break;
				case 0x1000|0x000|32|(4-1): memory.allocate<address_space_specific<0, 2, -1, ENDIANNESS_BIG   >>(*this, spacenum); break;
				case 0x0000|0x100|32|(4-1): memory.allocate<address_space_specific<1, 2, -1, ENDIANNESS_LITTLE>>(*this, spacenum); break;
				case 0x1000|0x100|32|(4-1): memory.allocate<address_space_specific<1, 2, -1, ENDIANNESS_BIG   >>(*this, spacenum); break;

				case 0x0000|0x000|32|(4-2): memory.allocate<address_space_specific<0, 2, -2, ENDIANNESS_LITTLE>>(*this, spacenum); break;
				case 0x1000|0x000|32|(4-2): memory.allocate<address_space_specific<0, 2, -2, ENDIANNESS_BIG   >>(*this, spacenum); break;
				case 0x0000|0x100|32|(4-2): memory.allocate<address_space_specific<1, 2, -2, ENDIANNESS_LITTLE>>(*this, spacenum); break;
				case 0x1000|0x100|32|(4-2): memory.allocate<address_space_specific<1, 2, -2, ENDIANNESS_BIG   >>(*this, spacenum); break;

				case 0x0000|0x000|64|(4-0): memory.allocate<address_space_specific<0, 3,  0, ENDIANNESS_LITTLE>>(*this, spacenum); break;
				case 0x1000|0x000|64|(4-0): memory.allocate<address_space_specific<0, 3,  0, ENDIANNESS_BIG   >>(*this, spacenum); break;
				case 0x0000|0x100|64|(4-0): memory.allocate<address_space_specific<1, 3,  0, ENDIANNESS_LITTLE>>(*this, spacenum); break;
				case 0x1000|0x100|64|(4-0): memory.allocate<address_space_specific<1, 3,  0, ENDIANNESS_BIG   >>(*this, spacenum); break;

				case 0x0000|0x000|64|(4-1): memory.allocate<address_space_specific<0, 3, -1, ENDIANNESS_LITTLE>>(*this, spacenum); break;
				case 0x1000|0x000|64|(4-1): memory.allocate<address_space_specific<0, 3, -1, ENDIANNESS_BIG   >>(*this, spacenum); break;
				case 0x0000|0x100|64|(4-1): memory.allocate<address_space_specific<1, 3, -1, ENDIANNESS_LITTLE>>(*this, spacenum); break;
				case 0x1000|0x100|64|(4-1): memory.allocate<address_space_specific<1, 3, -1, ENDIANNESS_BIG   >>(*this, spacenum); break;

				case 0x0000|0x000|64|(4-2): memory.allocate<address_space_specific<0, 3, -2, ENDIANNESS_LITTLE>>(*this, spacenum); break;
				case 0x1000|0x000|64|(4-2): memory.allocate<address_space_specific<0, 3, -2, ENDIANNESS_BIG   >>(*this, spacenum); break;
				case 0x0000|0x100|64|(4-2): memory.allocate<address_space_specific<1, 3, -2, ENDIANNESS_LITTLE>>(*this, spacenum); break;
				case 0x1000|0x100|64|(4-2): memory.allocate<address_space_specific<1, 3, -2, ENDIANNESS_BIG   >>(*this, spacenum); break;

				case 0x0000|0x000|64|(4-3): memory.allocate<address_space_specific<0, 3, -3, ENDIANNESS_LITTLE>>(*this, spacenum); break;
				case 0x1000|0x000|64|(4-3): memory.allocate<address_space_specific<0, 3, -3, ENDIANNESS_BIG   >>(*this, spacenum); break;
				case 0x0000|0x100|64|(4-3): memory.allocate<address_space_specific<1, 3, -3, ENDIANNESS_LITTLE>>(*this, spacenum); break;
				case 0x1000|0x100|64|(4-3): memory.allocate<address_space_specific<1, 3, -3, ENDIANNESS_BIG   >>(*this, spacenum); break;

				default:
					throw emu_fatalerror("Invalid width %d/shift %d specified for address_space::allocate", spaceconfig->data_width(), spaceconfig->addr_shift());
			}
		}
	}
}

//**************************************************************************
//  ADDRESS SPACE
//**************************************************************************

//-------------------------------------------------
//  address_space - constructor
//-------------------------------------------------

address_space::address_space(memory_manager &manager, device_memory_interface &memory, int spacenum)
	: address_space_installer(*memory.space_config(spacenum), manager),
		m_device(memory.device()),
		m_unmap(0),
		m_spacenum(spacenum),
		m_log_unmap(true),
		m_name(memory.space_config(spacenum)->name()),
		m_notifier_id(0),
		m_in_notification(0)
{
}


//-------------------------------------------------
//  ~address_space - destructor
//-------------------------------------------------

address_space::~address_space()
{
	m_unmap_r->unref();
	m_unmap_w->unref();
	m_nop_r->unref();
	m_nop_w->unref();
}


//-------------------------------------------------
//  prepare_map_generic - walk through an address
//  map to find implicit memory regions and
//  identify shared regions
//-------------------------------------------------

void address_space::prepare_map_generic(address_map &map, bool allow_alloc)
{
	memory_region *devregion = (m_spacenum == 0) ? m_device.memregion(DEVICE_SELF) : nullptr;
	u32 devregionsize = (devregion != nullptr) ? devregion->bytes() : 0;

	// make a pass over the address map, adjusting for the device and getting memory pointers
	for (address_map_entry &entry : map.m_entrylist)
	{
		// computed adjusted addresses first
		adjust_addresses(entry.m_addrstart, entry.m_addrend, entry.m_addrmask, entry.m_addrmirror);

		// if we have a share entry, add it to our map
		if (entry.m_share != nullptr)
		{
			// if we can't find it, add it to our map if we're allowed to
			std::string fulltag = entry.m_devbase.subtag(entry.m_share);
			memory_share *share = m_manager.share_find(fulltag);
			if (!share)
			{
				if (!allow_alloc)
					fatalerror("Trying to create share '%s' too late\n", fulltag);
				VPRINTF("Creating share '%s' of length 0x%X\n", fulltag, entry.m_addrend + 1 - entry.m_addrstart);
				share = m_manager.share_alloc(m_device, fulltag, m_config.data_width(), address_to_byte(entry.m_addrend + 1 - entry.m_addrstart), endianness());
			}
			else
			{
				std::string result = share->compare(m_config.data_width(), address_to_byte(entry.m_addrend + 1 - entry.m_addrstart), endianness());
				if (!result.empty())
					fatalerror("%s\n", result);
			}
			entry.m_memory = share->ptr();
		}

		// if this is a ROM handler without a specified region and not shared, attach it to the implicit region
		if (m_spacenum == AS_PROGRAM && entry.m_read.m_type == AMH_ROM && entry.m_region == nullptr && entry.m_share == nullptr)
		{
			// make sure it fits within the memory region before doing so, however
			if (entry.m_addrend < devregionsize)
			{
				entry.m_region = m_device.tag();
				entry.m_rgnoffs = address_to_byte(entry.m_addrstart);
			}
		}

		// validate adjusted addresses against implicit regions
		if (entry.m_region != nullptr)
		{
			// determine full tag
			std::string fulltag = entry.m_devbase.subtag(entry.m_region);

			// find the region
			memory_region *region = m_manager.machine().root_device().memregion(fulltag);
			if (region == nullptr)
				fatalerror("device '%s' %s space memory map entry %X-%X references nonexistent region \"%s\"\n", m_device.tag(), m_name, entry.m_addrstart, entry.m_addrend, entry.m_region);

			// validate the region
			if (entry.m_rgnoffs + m_config.addr2byte(entry.m_addrend - entry.m_addrstart + 1) > region->bytes())
				fatalerror("device '%s' %s space memory map entry %X-%X extends beyond region \"%s\" size (%X)\n", m_device.tag(), m_name, entry.m_addrstart, entry.m_addrend, entry.m_region, region->bytes());

			if (entry.m_share != nullptr)
				fatalerror("device '%s' %s space memory map entry %X-%X has both .region() and .share()\n", m_device.tag(), m_name, entry.m_addrstart, entry.m_addrend);
		}

		// convert any region-relative entries to their memory pointers
		if (entry.m_region != nullptr)
		{
			// determine full tag
			std::string fulltag = entry.m_devbase.subtag(entry.m_region);

			// set the memory address
			entry.m_memory = m_manager.machine().root_device().memregion(fulltag)->base() + entry.m_rgnoffs;
		}

		// allocate anonymous ram when needed
		if (!entry.m_memory && (entry.m_read.m_type == AMH_RAM || entry.m_write.m_type == AMH_RAM))
		{
			if (!allow_alloc)
				fatalerror("Trying to create memory in range %X-%X too late\n", entry.m_addrstart, entry.m_addrend);

			entry.m_memory = m_manager.anonymous_alloc(*this, address_to_byte(entry.m_addrend + 1 - entry.m_addrstart), m_config.data_width(), entry.m_addrstart, entry.m_addrend);
		}
	}
}


//-------------------------------------------------
//  prepare_map - allocate the address map and
//  walk through it to find implicit memory regions
//  and identify shared regions
//-------------------------------------------------

void address_space::prepare_map()
{
	// allocate the address map
	m_map = std::make_unique<address_map>(m_device, m_spacenum);

	// merge in the submaps
	m_map->import_submaps(m_manager.machine(), m_device.owner() ? *m_device.owner() : m_device, data_width(), endianness(), addr_shift());

	// extract global parameters specified by the map
	m_unmap = (m_map->m_unmapval == 0) ? 0 : ~0;
	if (m_map->m_globalmask != 0)
	{
		if (m_map->m_globalmask & ~m_addrmask)
			fatalerror("Can't set a global address mask of %08x on a %d-bits address width bus.\n", m_map->m_globalmask, addr_width());

		m_addrmask = m_map->m_globalmask;
	}

	prepare_map_generic(*m_map, true);
}

//-------------------------------------------------
//  prepare_device_map - check and walk through a
//  device-provided to to dynamically install to
//  find implicit memory regions and identify
//  shared regions
//-------------------------------------------------

void address_space::prepare_device_map(address_map &map)
{
	// Disable the test for now, some cleanup needed before
	if(0) {
		// device maps are not supposed to set global parameters
		if (map.m_unmapval)
			fatalerror("Device maps should not set the unmap value\n");

		if (map.m_globalmask && map.m_globalmask != m_addrmask)
			fatalerror("Device maps should not set the global mask\n");
	}

	prepare_map_generic(map, false);
}


//-------------------------------------------------
//  populate_from_map - walk the map in reverse
//  order and install the appropriate handler for
//  each case
//-------------------------------------------------

void address_space::populate_from_map(address_map *map)
{
	// no map specified, use the space-specific one
	if (map == nullptr)
		map = m_map.get();

	// no map, nothing to do
	if (map == nullptr)
		return;

	// install the handlers, using the original, unadjusted memory map
	for (const address_map_entry &entry : map->m_entrylist) {
		// map both read and write halves
		populate_map_entry(entry, read_or_write::READ);
		populate_map_entry(entry, read_or_write::WRITE);
	}

	if (VALIDATE_REFCOUNTS)
		validate_reference_counts();
}

//-------------------------------------------------
//  get_handler_string - return a string
//  describing the handler at a particular offset
//-------------------------------------------------

template<int Level, int Width, int AddrShift, endianness_t Endian> std::string address_space_specific<Level, Width, AddrShift, Endian>::get_handler_string(read_or_write readorwrite, offs_t address) const
{
	if (readorwrite == read_or_write::READ) {
		offs_t start, end;
		handler_entry_read<Width, AddrShift, Endian> *handler;
		m_root_read->lookup(address, start, end, handler);
		return handler->name();
	} else {
		offs_t start, end;
		handler_entry_write<Width, AddrShift, Endian> *handler;
		m_root_write->lookup(address, start, end, handler);
		return handler->name();
	}
}

template<int Level, int Width, int AddrShift, endianness_t Endian> void address_space_specific<Level, Width, AddrShift, Endian>::dump_maps(std::vector<memory_entry> &read_map, std::vector<memory_entry> &write_map) const
{
	read_map.clear();
	write_map.clear();
	m_root_read->dump_map(read_map);
	m_root_write->dump_map(write_map);
}


//**************************************************************************
//  DYNAMIC ADDRESS SPACE MAPPING
//**************************************************************************

//-------------------------------------------------
//  unmap - unmap a section of address space
//-------------------------------------------------

template<int Level, int Width, int AddrShift, endianness_t Endian> void address_space_specific<Level, Width, AddrShift, Endian>::unmap_generic(offs_t addrstart, offs_t addrend, offs_t addrmirror, read_or_write readorwrite, bool quiet)
{
	VPRINTF("address_space::unmap(%*x-%*x mirror=%*x, %s, %s)\n",
			m_addrchars, addrstart, m_addrchars, addrend,
			m_addrchars, addrmirror,
			(readorwrite == read_or_write::READ) ? "read" : (readorwrite == read_or_write::WRITE) ? "write" : (readorwrite == read_or_write::READWRITE) ? "read/write" : "??",
			quiet ? "quiet" : "normal");

	offs_t nstart, nend, nmask, nmirror;
	check_optimize_mirror("unmap_generic", addrstart, addrend, addrmirror, nstart, nend, nmask, nmirror);

	// read space
	if (readorwrite == read_or_write::READ || readorwrite == read_or_write::READWRITE) {
		auto handler = static_cast<handler_entry_read<Width, AddrShift, Endian> *>(quiet ? m_nop_r : m_unmap_r);
		handler->ref();
		m_root_read->populate(nstart, nend, nmirror, handler);
	}

	// write space
	if (readorwrite == read_or_write::WRITE || readorwrite == read_or_write::READWRITE) {
		auto handler = static_cast<handler_entry_write<Width, AddrShift, Endian> *>(quiet ? m_nop_w : m_unmap_w);
		handler->ref();
		m_root_write->populate(nstart, nend, nmirror, handler);
	}

	invalidate_caches(readorwrite);
}

//-------------------------------------------------
//  install_view - install a view on the bus
//-------------------------------------------------
template<int Level, int Width, int AddrShift, endianness_t Endian> void address_space_specific<Level, Width, AddrShift, Endian>::install_view(offs_t addrstart, offs_t addrend, offs_t addrmirror, memory_view &view)
{
	offs_t nstart, nend, nmask, nmirror;
	check_optimize_mirror("install_view", addrstart, addrend, addrmirror, nstart, nend, nmask, nmirror);

	auto handlers = view.make_handlers(*this, addrstart, addrend);
	m_root_read ->populate(nstart, nend, nmirror, static_cast<handler_entry_read <Width, AddrShift, Endian> *>(handlers.first));
	m_root_write->populate(nstart, nend, nmirror, static_cast<handler_entry_write<Width, AddrShift, Endian> *>(handlers.second));
	view.make_subdispatch(""); // Must be called after populate
}

memory_passthrough_handler *address_space::make_mph()
{
	m_mphs.emplace_back(std::make_unique<memory_passthrough_handler>(*this));
	return m_mphs.back().get();
}

//-------------------------------------------------
//  install_read_tap - install a read tap on the bus
//-------------------------------------------------

template<int Level, int Width, int AddrShift, endianness_t Endian> memory_passthrough_handler *address_space_specific<Level, Width, AddrShift, Endian>::install_read_tap(offs_t addrstart, offs_t addrend, offs_t addrmirror, std::string name, std::function<void (offs_t offset, uX &data, uX mem_mask)> tap, memory_passthrough_handler *mph)
{
	offs_t nstart, nend, nmask, nmirror;
	check_optimize_mirror("install_read_tap", addrstart, addrend, addrmirror, nstart, nend, nmask, nmirror);
	if (!mph)
		mph = make_mph();

	auto handler = new handler_entry_read_tap<Width, AddrShift, Endian>(this, *mph, name, tap);
	m_root_read->populate_passthrough(nstart, nend, nmirror, handler);
	handler->unref();

	invalidate_caches(read_or_write::READ);

	return mph;
}

//-------------------------------------------------
//  install_write_tap - install a write tap on the bus
//-------------------------------------------------

template<int Level, int Width, int AddrShift, endianness_t Endian> memory_passthrough_handler *address_space_specific<Level, Width, AddrShift, Endian>::install_write_tap(offs_t addrstart, offs_t addrend, offs_t addrmirror, std::string name, std::function<void (offs_t offset, uX &data, uX mem_mask)> tap, memory_passthrough_handler *mph)
{
	offs_t nstart, nend, nmask, nmirror;
	check_optimize_mirror("install_write_tap", addrstart, addrend, addrmirror, nstart, nend, nmask, nmirror);
	if (!mph)
		mph = make_mph();

	auto handler = new handler_entry_write_tap<Width, AddrShift, Endian>(this, *mph, name, tap);
	m_root_write->populate_passthrough(nstart, nend, nmirror, handler);
	handler->unref();

	invalidate_caches(read_or_write::WRITE);

	return mph;
}
//-------------------------------------------------
//  install_write_tap - install a read and a write tap on the bus
//-------------------------------------------------

template<int Level, int Width, int AddrShift, endianness_t Endian> memory_passthrough_handler *address_space_specific<Level, Width, AddrShift, Endian>::install_readwrite_tap(offs_t addrstart, offs_t addrend, offs_t addrmirror, std::string name, std::function<void (offs_t offset, uX &data, uX mem_mask)> tapr, std::function<void (offs_t offset, uX &data, uX mem_mask)> tapw, memory_passthrough_handler *mph)
{
	offs_t nstart, nend, nmask, nmirror;
	check_optimize_mirror("install_readwrite_tap", addrstart, addrend, addrmirror, nstart, nend, nmask, nmirror);
	if (!mph)
		mph = make_mph();

	auto rhandler = new handler_entry_read_tap <Width, AddrShift, Endian>(this, *mph, name, tapr);
	m_root_read ->populate_passthrough(nstart, nend, nmirror, rhandler);
	rhandler->unref();

	auto whandler = new handler_entry_write_tap<Width, AddrShift, Endian>(this, *mph, name, tapw);
	m_root_write->populate_passthrough(nstart, nend, nmirror, whandler);
	whandler->unref();

	invalidate_caches(read_or_write::READWRITE);

	return mph;
}





//-------------------------------------------------
//  install_device_delegate - install the memory map
//  of a live device into this address space
//-------------------------------------------------

template<int Level, int Width, int AddrShift, endianness_t Endian> void address_space_specific<Level, Width, AddrShift, Endian>::install_device_delegate(offs_t addrstart, offs_t addrend, device_t &device, address_map_constructor &delegate, u64 unitmask, int cswidth)
{
	check_address("install_device_delegate", addrstart, addrend);
	address_map map(*this, addrstart, addrend, unitmask, cswidth, m_device, delegate);
	map.import_submaps(m_manager.machine(), device, data_width(), endianness(), addr_shift());
	prepare_device_map(map);
	populate_from_map(&map);
}



//-------------------------------------------------
//  install_readwrite_port - install a new I/O port
//  handler into this address space
//-------------------------------------------------

template<int Level, int Width, int AddrShift, endianness_t Endian> void address_space_specific<Level, Width, AddrShift, Endian>::install_readwrite_port(offs_t addrstart, offs_t addrend, offs_t addrmirror, std::string rtag, std::string wtag)
{
	VPRINTF("address_space::install_readwrite_port(%*x-%*x mirror=%*x, read=\"%s\" / write=\"%s\")\n",
			m_addrchars, addrstart, m_addrchars, addrend,
			m_addrchars, addrmirror,
			rtag.empty() ? "(none)" : rtag, wtag.empty() ? "(none)" : wtag);

	offs_t nstart, nend, nmask, nmirror;
	check_optimize_mirror("install_readwrite_port", addrstart, addrend, addrmirror, nstart, nend, nmask, nmirror);

	// read handler
	if (rtag != "")
	{
		// find the port
		ioport_port *port = device().owner()->ioport(rtag);
		if (port == nullptr)
			throw emu_fatalerror("Attempted to map non-existent port '%s' for read in space %s of device '%s'\n", rtag, m_name, m_device.tag());

		// map the range and set the ioport
		auto hand_r = new handler_entry_read_ioport<Width, AddrShift, Endian>(this, port);
		m_root_read->populate(nstart, nend, nmirror, hand_r);
	}

	if (wtag != "")
	{
		// find the port
		ioport_port *port = device().owner()->ioport(wtag);
		if (port == nullptr)
			fatalerror("Attempted to map non-existent port '%s' for write in space %s of device '%s'\n", wtag, m_name, m_device.tag());

		// map the range and set the ioport
		auto hand_w = new handler_entry_write_ioport<Width, AddrShift, Endian>(this, port);
		m_root_write->populate(nstart, nend, nmirror, hand_w);
	}

	invalidate_caches(rtag != "" ? wtag != "" ? read_or_write::READWRITE : read_or_write::READ : read_or_write::WRITE);
}


//-------------------------------------------------
//  install_bank_generic - install a range as
//  mapping to a particular bank
//-------------------------------------------------

template<int Level, int Width, int AddrShift, endianness_t Endian> void address_space_specific<Level, Width, AddrShift, Endian>::install_bank_generic(offs_t addrstart, offs_t addrend, offs_t addrmirror, memory_bank *rbank, memory_bank *wbank)
{
	VPRINTF("address_space::install_readwrite_bank(%*x-%*x mirror=%*x, read=\"%s\" / write=\"%s\")\n",
			m_addrchars, addrstart, m_addrchars, addrend,
			m_addrchars, addrmirror,
			(rbank != nullptr) ? rbank->tag() : "(none)", (wbank != nullptr) ? wbank->tag() : "(none)");

	offs_t nstart, nend, nmask, nmirror;
	check_optimize_mirror("install_bank_generic", addrstart, addrend, addrmirror, nstart, nend, nmask, nmirror);

	// map the read bank
	if (rbank != nullptr)
	{
		auto hand_r = new handler_entry_read_memory_bank<Width, AddrShift, Endian>(this, *rbank);
		hand_r->set_address_info(nstart, nmask);
		m_root_read->populate(nstart, nend, nmirror, hand_r);
	}

	// map the write bank
	if (wbank != nullptr)
	{
		auto hand_w = new handler_entry_write_memory_bank<Width, AddrShift, Endian>(this, *wbank);
		hand_w->set_address_info(nstart, nmask);
		m_root_write->populate(nstart, nend, nmirror, hand_w);
	}

	invalidate_caches(rbank ? wbank ? read_or_write::READWRITE : read_or_write::READ : read_or_write::WRITE);
}


//-------------------------------------------------
//  install_ram_generic - install a simple fixed
//  RAM region into the given address space
//-------------------------------------------------

template<int Level, int Width, int AddrShift, endianness_t Endian> void address_space_specific<Level, Width, AddrShift, Endian>::install_ram_generic(offs_t addrstart, offs_t addrend, offs_t addrmirror, read_or_write readorwrite, void *baseptr)
{
	VPRINTF("address_space::install_ram_generic(%s-%s mirror=%s, %s, %p)\n",
			m_addrchars, addrstart, m_addrchars, addrend,
			m_addrchars, addrmirror,
			(readorwrite == read_or_write::READ) ? "read" : (readorwrite == read_or_write::WRITE) ? "write" : (readorwrite == read_or_write::READWRITE) ? "read/write" : "??",
			baseptr);

	offs_t nstart, nend, nmask, nmirror;
	check_optimize_mirror("install_ram_generic", addrstart, addrend, addrmirror, nstart, nend, nmask, nmirror);

	// map for read
	if (readorwrite == read_or_write::READ || readorwrite == read_or_write::READWRITE)
	{
		auto hand_r = new handler_entry_read_memory<Width, AddrShift, Endian>(this, baseptr);
		hand_r->set_address_info(nstart, nmask);
		m_root_read->populate(nstart, nend, nmirror, hand_r);
	}

	// map for write
	if (readorwrite == read_or_write::WRITE || readorwrite == read_or_write::READWRITE)
	{
		auto hand_w = new handler_entry_write_memory<Width, AddrShift, Endian>(this, baseptr);
		hand_w->set_address_info(nstart, nmask);
		m_root_write->populate(nstart, nend, nmirror, hand_w);
	}

	invalidate_caches(readorwrite);
}

//**************************************************************************
//  MEMORY MAPPING HELPERS
//**************************************************************************

int address_space::add_change_notifier(std::function<void (read_or_write)> n)
{
	int id = m_notifier_id++;
	m_notifiers.emplace_back(notifier_t{ std::move(n), id });
	return id;
}

void address_space::remove_change_notifier(int id)
{
	for(auto i = m_notifiers.begin(); i != m_notifiers.end(); i++)
		if (i->m_id == id) {
			m_notifiers.erase(i);
			return;
		}
	fatalerror("Unknown notifier id %d, double remove?\n", id);
}
