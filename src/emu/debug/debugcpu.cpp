// license:BSD-3-Clause
// copyright-holders:Aaron Giles
/*********************************************************************

    debugcpu.cpp

    Debugger CPU/memory interface engine.

***************************************************************************/

#include "emu.h"
#include "debugcpu.h"
#include "debugbuf.h"

#include "express.h"
#include "points.h"
#include "debugcon.h"
#include "debugvw.h"

#include "debugger.h"
#include "emuopts.h"
#include "screen.h"
#include "uiinput.h"

#include "coreutil.h"
#include "osdepend.h"
#include "xmlfile.h"


const size_t debugger_cpu::NUM_TEMP_VARIABLES = 10;

/*-------------------------------------------------
    constructor - initialize the CPU
    information for debugging
-------------------------------------------------*/

debugger_cpu::debugger_cpu(running_machine &machine)
	: m_machine(machine)
	, m_livecpu(nullptr)
	, m_breakcpu(nullptr)
	, m_symtable(nullptr)
	, m_vblank_occurred(false)
	, m_execution_state(exec_state::STOPPED)
	, m_stop_when_not_device(nullptr)
	, m_bpindex(1)
	, m_wpindex(1)
	, m_rpindex(1)
	, m_wpdata(0)
	, m_wpaddr(0)
	, m_last_periodic_update_time(0)
	, m_comments_loaded(false)
{
	m_tempvar = make_unique_clear<u64[]>(NUM_TEMP_VARIABLES);

	/* create a global symbol table */
	m_symtable = std::make_unique<symbol_table>(machine);
	m_symtable->set_memory_modified_func([this]() { set_memory_modified(true); });

	/* add "wpaddr", "wpdata" to the global symbol table */
	m_symtable->add("wpaddr", symbol_table::READ_ONLY, &m_wpaddr);
	m_symtable->add("wpdata", symbol_table::READ_ONLY, &m_wpdata);

	screen_device_iterator screen_iterator = screen_device_iterator(m_machine.root_device());
	screen_device_iterator::auto_iterator iter = screen_iterator.begin();
	const uint32_t count = (uint32_t)screen_iterator.count();

	if (count == 1)
	{
		screen_device &screen = *iter.current();
		m_symtable->add("beamx", [&screen]() { return screen.hpos(); });
		m_symtable->add("beamy", [&screen]() { return screen.vpos(); });
		m_symtable->add("frame", [&screen]() { return screen.frame_number(); });
		screen.register_vblank_callback(vblank_state_delegate(&debugger_cpu::on_vblank, this));
	}
	else if (count > 1)
	{
		for (uint32_t i = 0; i < count; i++, iter++)
		{
			screen_device &screen = *iter.current();
			m_symtable->add(string_format("beamx%d", i).c_str(), [&screen]() { return screen.hpos(); });
			m_symtable->add(string_format("beamy%d", i).c_str(), [&screen]() { return screen.vpos(); });
			m_symtable->add(string_format("frame%d", i).c_str(), [&screen]() { return screen.frame_number(); });
			screen.register_vblank_callback(vblank_state_delegate(&debugger_cpu::on_vblank, this));
		}
	}

	/* add the temporary variables to the global symbol table */
	for (int regnum = 0; regnum < NUM_TEMP_VARIABLES; regnum++)
	{
		char symname[10];
		sprintf(symname, "temp%d", regnum);
		m_symtable->add(symname, symbol_table::READ_WRITE, &m_tempvar[regnum]);
	}
}


/*-------------------------------------------------
    flush_traces - flushes all traces; this is
    useful if a trace is going on when we
    fatalerror
-------------------------------------------------*/

void debugger_cpu::flush_traces()
{
	/* this can be called on exit even when no debugging is enabled, so
	 make sure the devdebug is valid before proceeding */
	for (device_t &device : device_iterator(m_machine.root_device()))
		if (device.debug() != nullptr)
			device.debug()->trace_flush();
}



//**************************************************************************
//  MEMORY AND DISASSEMBLY HELPERS
//**************************************************************************

//-------------------------------------------------
//  comment_save - save all comments for the given
//  machine
//-------------------------------------------------

bool debugger_cpu::comment_save()
{
	bool comments_saved = false;

	// if we don't have a root, bail
	util::xml::file::ptr const root = util::xml::file::create();
	if (!root)
		return false;

	// wrap in a try/catch to handle errors
	try
	{
		// create a comment node
		util::xml::data_node *const commentnode = root->add_child("mamecommentfile", nullptr);
		if (commentnode == nullptr)
			throw emu_exception();
		commentnode->set_attribute_int("version", COMMENT_VERSION);

		// create a system node
		util::xml::data_node *const systemnode = commentnode->add_child("system", nullptr);
		if (systemnode == nullptr)
			throw emu_exception();
		systemnode->set_attribute("name", m_machine.system().name);

		// for each device
		bool found_comments = false;
		for (device_t &device : device_iterator(m_machine.root_device()))
			if (device.debug() && device.debug()->comment_count() > 0)
			{
				// create a node for this device
				util::xml::data_node *const curnode = systemnode->add_child("cpu", nullptr);
				if (curnode == nullptr)
					throw emu_exception();
				curnode->set_attribute("tag", device.tag());

				// export the comments
				if (!device.debug()->comment_export(*curnode))
					throw emu_exception();
				found_comments = true;
			}

		// flush the file
		if (found_comments)
		{
			emu_file file(m_machine.options().comment_directory(), OPEN_FLAG_WRITE | OPEN_FLAG_CREATE | OPEN_FLAG_CREATE_PATHS);
			osd_file::error filerr = file.open(m_machine.basename() + ".cmt");
			if (filerr == osd_file::error::NONE)
			{
				root->write(file);
				comments_saved = true;
			}
		}
	}
	catch (emu_exception &)
	{
		return false;
	}

	// free and get out of here
	return comments_saved;
}

//-------------------------------------------------
//  comment_load - load all comments for the given
//  machine
//-------------------------------------------------

bool debugger_cpu::comment_load(bool is_inline)
{
	// open the file
	emu_file file(m_machine.options().comment_directory(), OPEN_FLAG_READ);
	osd_file::error filerr = file.open(m_machine.basename() + ".cmt");

	// if an error, just return false
	if (filerr != osd_file::error::NONE)
		return false;

	// wrap in a try/catch to handle errors
	util::xml::file::ptr const root = util::xml::file::read(file, nullptr);
	try
	{
		// read the file
		if (!root)
			throw emu_exception();

		// find the config node
		util::xml::data_node const *const commentnode = root->get_child("mamecommentfile");
		if (commentnode == nullptr)
			throw emu_exception();

		// validate the config data version
		int version = commentnode->get_attribute_int("version", 0);
		if (version != COMMENT_VERSION)
			throw emu_exception();

		// check to make sure the file is applicable
		util::xml::data_node const *const systemnode = commentnode->get_child("system");
		const char *const name = systemnode->get_attribute_string("name", "");
		if (strcmp(name, m_machine.system().name) != 0)
			throw emu_exception();

		// iterate over devices
		for (util::xml::data_node const *cpunode = systemnode->get_child("cpu"); cpunode; cpunode = cpunode->get_next_sibling("cpu"))
		{
			const char *cputag_name = cpunode->get_attribute_string("tag", "");
			device_t *device = m_machine.root_device().subdevice(cputag_name);
			if (device != nullptr)
			{
				if(is_inline == false)
					m_machine.debugger().console().printf("@%s\n", cputag_name);

				if (!device->debug()->comment_import(*cpunode,is_inline))
					throw emu_exception();
			}
		}
	}
	catch (emu_exception &)
	{
		// clean up in case of error
		return false;
	}

	// success!
	return true;
}



/***************************************************************************
    INTERNAL HELPERS
***************************************************************************/

/*-------------------------------------------------
    on_vblank - called when a VBLANK hits
-------------------------------------------------*/

void debugger_cpu::on_vblank(screen_device &device, bool vblank_state)
{
	/* just set a global flag to be consumed later */
	if (vblank_state)
		m_vblank_occurred = true;
}


/*-------------------------------------------------
    reset_transient_flags - reset the transient
    flags on all CPUs
-------------------------------------------------*/

void debugger_cpu::reset_transient_flags()
{
	/* loop over CPUs and reset the transient flags */
	for (device_t &device : device_iterator(m_machine.root_device()))
		device.debug()->reset_transient_flag();
	m_stop_when_not_device = nullptr;
}


//**************************************************************************
//  EXECUTION HOOKS
//**************************************************************************

void debugger_cpu::start_hook(device_t *device, bool stop_on_vblank)
{
	// stash a pointer to the current live CPU
	assert(m_livecpu == nullptr);
	m_livecpu = device;

	// can't stop on a device without a state interface
	if (m_execution_state == exec_state::STOPPED && dynamic_cast<device_state_interface *>(device) == nullptr)
	{
		if (m_stop_when_not_device == nullptr)
			m_stop_when_not_device = device;
		m_execution_state = exec_state::RUNNING;
	}
	// if we're a new device, stop now
	else if (m_stop_when_not_device != nullptr && m_stop_when_not_device != device && device->debug()->observing())
	{
		m_stop_when_not_device = nullptr;
		m_execution_state = exec_state::STOPPED;
		reset_transient_flags();
	}

	// if we're running, do some periodic updating
	if (m_execution_state != exec_state::STOPPED)
	{
		device_t *visiblecpu = m_machine.debugger().console().get_visible_cpu();
		if (device == visiblecpu && osd_ticks() > m_last_periodic_update_time + osd_ticks_per_second() / 4)
		{   // check for periodic updates
			m_machine.debug_view().update_all();
			m_machine.debug_view().flush_osd_updates();
			m_last_periodic_update_time = osd_ticks();
		}
		if (device == m_breakcpu)
		{   // check for pending breaks
			m_execution_state = exec_state::STOPPED;
			m_breakcpu = nullptr;
		}

		// if a VBLANK occurred, check on things
		if (m_vblank_occurred)
		{
			m_vblank_occurred = false;

			// if we were waiting for a VBLANK, signal it now
			if (stop_on_vblank)
			{
				m_execution_state = exec_state::STOPPED;
				m_machine.debugger().console().printf("Stopped at VBLANK\n");
			}
		}
		// check for debug keypresses
		if (m_machine.ui_input().pressed(IPT_UI_DEBUG_BREAK))
		{
			visiblecpu->debug()->ignore(false);
			visiblecpu->debug()->halt_on_next_instruction("User-initiated break\n");
		}
	}
}

void debugger_cpu::stop_hook(device_t *device)
{
	assert(m_livecpu == device);

	// if we are supposed to be stopped at this point (most likely because of a watchpoint), keep going until this CPU is live again
	if (m_execution_state == exec_state::STOPPED)
	{
		m_breakcpu = device;
		m_execution_state = exec_state::RUNNING;
	}

	// clear the live CPU
	m_livecpu = nullptr;
}

void debugger_cpu::ensure_comments_loaded()
{
	if (!m_comments_loaded)
	{
		comment_load(true);
		m_comments_loaded = true;
	}
}


//-------------------------------------------------
//  go_next_device - execute until we hit the next
//  device
//-------------------------------------------------

void debugger_cpu::go_next_device(device_t *device)
{
	m_stop_when_not_device = device;
	m_execution_state = exec_state::RUNNING;
}

void debugger_cpu::go_vblank()
{
	m_vblank_occurred = false;
	m_execution_state = exec_state::RUNNING;
}

void debugger_cpu::halt_on_next_instruction(device_t *device, util::format_argument_pack<std::ostream> &&args)
{
	// if something is pending on this CPU already, ignore this request
	if (device == m_breakcpu)
		return;

	// output the message to the console
	m_machine.debugger().console().vprintf(std::move(args));

	// if we are live, stop now, otherwise note that we want to break there
	if (device == m_livecpu)
	{
		m_execution_state = exec_state::STOPPED;
		if (m_livecpu != nullptr)
			m_livecpu->debug()->compute_debug_flags();
	}
	else
	{
		m_breakcpu = device;
	}
}

//**************************************************************************
//  DEVICE DEBUG
//**************************************************************************

//-------------------------------------------------
//  device_debug - constructor
//-------------------------------------------------

device_debug::device_debug(device_t &device)
	: m_device(device)
	, m_exec(nullptr)
	, m_memory(nullptr)
	, m_state(nullptr)
	, m_disasm(nullptr)
	, m_flags(0)
	, m_symtable(std::make_unique<symbol_table>(device.machine(), &device.machine().debugger().cpu().global_symtable(), &device))
	, m_instrhook(nullptr)
	, m_stepaddr(0)
	, m_stepsleft(0)
	, m_stopaddr(0)
	, m_stoptime(attotime::zero)
	, m_stopirq(0)
	, m_stopexception(0)
	, m_endexectime(attotime::zero)
	, m_total_cycles(0)
	, m_last_total_cycles(0)
	, m_pc_history_index(0)
	, m_bplist()
	, m_rplist(std::make_unique<std::forward_list<debug_registerpoint>>())
	, m_triggered_breakpoint(nullptr)
	, m_triggered_watchpoint(nullptr)
	, m_trace(nullptr)
	, m_hotspot_threshhold(0)
	, m_track_pc_set()
	, m_track_pc(false)
	, m_comment_set()
	, m_comment_change(0)
	, m_track_mem_set()
	, m_track_mem(false)
{
	memset(m_pc_history, 0, sizeof(m_pc_history));

	// find out which interfaces we have to work with
	device.interface(m_exec);
	device.interface(m_memory);
	device.interface(m_state);
	device.interface(m_disasm);

	// set up notifiers and clear the passthrough handlers
	if (m_memory) {
		int count = m_memory->max_space_count();
		m_phr.resize(count, nullptr);
		m_phw.resize(count, nullptr);
		for (int i=0; i != count; i++)
			if (m_memory->has_space(i)) {
				address_space &space = m_memory->space(i);
				m_notifiers.push_back(space.add_change_notifier([this, &space](read_or_write mode) { reinstall(space, mode); }));
			}
			else
				m_notifiers.push_back(-1);
	}

	// set up state-related stuff
	if (m_state != nullptr)
	{
		// add global symbol for cycles and totalcycles
		if (m_exec != nullptr)
		{
			m_symtable->add("cycles", [this]() { return m_exec->cycles_remaining(); });
			m_symtable->add("totalcycles", symbol_table::READ_ONLY, &m_total_cycles);
			m_symtable->add("lastinstructioncycles", [this]() { return m_total_cycles - m_last_total_cycles; });
		}

		// add entries to enable/disable unmap reporting for each space
		if (m_memory != nullptr)
		{
			if (m_memory->has_space(AS_PROGRAM))
				m_symtable->add(
						"logunmap",
						[&space = m_memory->space(AS_PROGRAM)] () { return space.log_unmap(); },
						[&space = m_memory->space(AS_PROGRAM)] (u64 value) { return space.set_log_unmap(bool(value)); });
			if (m_memory->has_space(AS_DATA))
				m_symtable->add(
						"logunmap",
						[&space = m_memory->space(AS_DATA)] () { return space.log_unmap(); },
						[&space = m_memory->space(AS_DATA)] (u64 value) { return space.set_log_unmap(bool(value)); });
			if (m_memory->has_space(AS_IO))
				m_symtable->add(
						"logunmap",
						[&space = m_memory->space(AS_IO)] () { return space.log_unmap(); },
						[&space = m_memory->space(AS_IO)] (u64 value) { return space.set_log_unmap(bool(value)); });
			if (m_memory->has_space(AS_OPCODES))
				m_symtable->add(
						"logunmap",
						[&space = m_memory->space(AS_OPCODES)] () { return space.log_unmap(); },
						[&space = m_memory->space(AS_OPCODES)] (u64 value) { return space.set_log_unmap(bool(value)); });
		}

		// add all registers into it
		std::string tempstr;
		for (const auto &entry : m_state->state_entries())
		{
			// TODO: floating point registers
			if (!entry->is_float())
			{
				using namespace std::placeholders;
				strmakelower(tempstr.assign(entry->symbol()));
				m_symtable->add(
						tempstr.c_str(),
						std::bind(&device_state_interface::state_int, m_state, entry->index()),
						entry->writeable() ? std::bind(&device_state_interface::set_state_int, m_state, entry->index(), _1) : symbol_table::setter_func(nullptr),
						entry->format_string());
			}
		}
	}

	// set up execution-related stuff
	if (m_exec != nullptr)
	{
		m_flags = DEBUG_FLAG_OBSERVING | DEBUG_FLAG_HISTORY;

		// if no curpc, add one
		if (m_state && !m_symtable->find("curpc"))
			m_symtable->add("curpc", std::bind(&device_state_interface::pcbase, m_state));
	}

	// set up trace
	using namespace std::placeholders;
	m_device.machine().add_logerror_callback(std::bind(&device_debug::errorlog_write_line, this, _1));
}


//-------------------------------------------------
//  ~device_debug - constructor
//-------------------------------------------------

device_debug::~device_debug()
{
	// free breakpoints and watchpoints
	breakpoint_clear_all();
	watchpoint_clear_all();
	registerpoint_clear_all();
}

void device_debug::write_tracking(address_space &space, offs_t address, u64 data)
{
	dasm_memory_access const newAccess(space.spacenum(), address, data, history_pc(0));
	std::pair<std::set<dasm_memory_access>::iterator, bool> trackedAccess = m_track_mem_set.insert(newAccess);
	if (!trackedAccess.second)
		trackedAccess.first->m_pc = newAccess.m_pc;
}

void device_debug::reinstall(address_space &space, read_or_write mode)
{
	int id = space.spacenum();
	if (u32(mode) & u32(read_or_write::READ))
	{
		if (m_phr[id])
			m_phr[id]->remove();
		if (!m_hotspots.empty())
			switch (space.data_width())
			{
			case  8: m_phr[id] = space.install_read_tap(0, space.addrmask(), "hotspot", [this, &space](offs_t address, u8  &, u8 ) { hotspot_check(space, address); }, m_phr[id]); break;
			case 16: m_phr[id] = space.install_read_tap(0, space.addrmask(), "hotspot", [this, &space](offs_t address, u16 &, u16) { hotspot_check(space, address); }, m_phr[id]); break;
			case 32: m_phr[id] = space.install_read_tap(0, space.addrmask(), "hotspot", [this, &space](offs_t address, u32 &, u32) { hotspot_check(space, address); }, m_phr[id]); break;
			case 64: m_phr[id] = space.install_read_tap(0, space.addrmask(), "hotspot", [this, &space](offs_t address, u64 &, u64) { hotspot_check(space, address); }, m_phr[id]); break;
			}
	}
	if (u32(mode) & u32(read_or_write::WRITE))
	{
		if (m_phw[id])
			m_phw[id]->remove();
		if (m_track_mem)
			switch (space.data_width())
			{
			case  8: m_phw[id] = space.install_read_tap(0, space.addrmask(), "track_mem", [this, &space](offs_t address, u8  &data, u8 ) { write_tracking(space, address, data); }, m_phw[id]); break;
			case 16: m_phw[id] = space.install_read_tap(0, space.addrmask(), "track_mem", [this, &space](offs_t address, u16 &data, u16) { write_tracking(space, address, data); }, m_phw[id]); break;
			case 32: m_phw[id] = space.install_read_tap(0, space.addrmask(), "track_mem", [this, &space](offs_t address, u32 &data, u32) { write_tracking(space, address, data); }, m_phw[id]); break;
			case 64: m_phw[id] = space.install_read_tap(0, space.addrmask(), "track_mem", [this, &space](offs_t address, u64 &data, u64) { write_tracking(space, address, data); }, m_phw[id]); break;
			}
	}
}

void device_debug::reinstall_all(read_or_write mode)
{
	int count = m_memory->max_space_count();
	for (int i=0; i < count; i++)
		if (m_memory->has_space(i))
			reinstall(m_memory->space(i), mode);
}

//-------------------------------------------------
//  start_hook - the scheduler calls this hook
//  before beginning execution for the given device
//-------------------------------------------------

void device_debug::start_hook(const attotime &endtime)
{
	assert((m_device.machine().debug_flags & DEBUG_FLAG_ENABLED) != 0);

	m_device.machine().debugger().cpu().start_hook(&m_device, (m_flags & DEBUG_FLAG_STOP_VBLANK) != 0);

	// update the target execution end time
	m_endexectime = endtime;

	// recompute the debugging mode
	compute_debug_flags();
}


//-------------------------------------------------
//  stop_hook - the scheduler calls this hook when
//  ending execution for the given device
//-------------------------------------------------

void device_debug::stop_hook()
{
	m_device.machine().debugger().cpu().stop_hook(&m_device);
}


//-------------------------------------------------
//  interrupt_hook - called when an interrupt is
//  acknowledged
//-------------------------------------------------

void device_debug::interrupt_hook(int irqline)
{
	// see if this matches a pending interrupt request
	if ((m_flags & DEBUG_FLAG_STOP_INTERRUPT) != 0 && (m_stopirq == -1 || m_stopirq == irqline))
	{
		m_device.machine().debugger().cpu().set_execution_stopped();
		m_device.machine().debugger().console().printf("Stopped on interrupt (CPU '%s', IRQ %d)\n", m_device.tag(), irqline);
		compute_debug_flags();
	}
}


//-------------------------------------------------
//  exception_hook - called when an exception is
//  generated
//-------------------------------------------------

void device_debug::exception_hook(int exception)
{
	// see if this matches an exception breakpoint
	if ((m_flags & DEBUG_FLAG_STOP_EXCEPTION) != 0 && (m_stopexception == -1 || m_stopexception == exception))
	{
		bool matched = true;
		if (m_exception_condition && !m_exception_condition->is_empty())
		{
			try
			{
				matched = m_exception_condition->execute();
			}
			catch (expression_error &)
			{
				return;
			}
		}

		if (matched)
		{
			m_device.machine().debugger().cpu().set_execution_stopped();
			m_device.machine().debugger().console().printf("Stopped on exception (CPU '%s', exception %d, PC=%X)\n", m_device.tag(), exception, m_state->pcbase());
			compute_debug_flags();
		}
	}
}


//-------------------------------------------------
//  privilege_hook - called when privilege level is
//  changed
//-------------------------------------------------

void device_debug::privilege_hook()
{
	if ((m_flags & DEBUG_FLAG_STOP_PRIVILEGE) != 0)
	{
		bool matched = true;
		if (m_privilege_condition && !m_privilege_condition->is_empty())
		{
			try
			{
				matched = m_privilege_condition->execute();
			}
			catch (expression_error &)
			{
				return;
			}
		}

		if (matched)
		{
			m_device.machine().debugger().cpu().set_execution_stopped();
			m_device.machine().debugger().console().printf("Stopped due to privilege change\n", m_device.tag());
			compute_debug_flags();
		}
	}
}

//-------------------------------------------------
//  instruction_hook - called by the CPU cores
//  before executing each instruction
//-------------------------------------------------

void device_debug::instruction_hook(offs_t curpc)
{
	running_machine &machine = m_device.machine();
	debugger_cpu& debugcpu = machine.debugger().cpu();

	// note that we are in the debugger code
	debugcpu.set_within_instruction(true);

	// update the history
	m_pc_history[m_pc_history_index++ % HISTORY_SIZE] = curpc;

	// update total cycles
	m_last_total_cycles = m_total_cycles;
	m_total_cycles = m_exec->total_cycles();

	// are we tracking our recent pc visits?
	if (m_track_pc)
	{
		const u32 crc = compute_opcode_crc32(curpc);
		m_track_pc_set.insert(dasm_pc_tag(curpc, crc));
	}

	// are we tracing?
	if (m_trace != nullptr)
		m_trace->update(curpc);

	// per-instruction hook?
	if (!debugcpu.is_stopped() && (m_flags & DEBUG_FLAG_HOOKED) != 0 && (*m_instrhook)(m_device, curpc))
		debugcpu.set_execution_stopped();

	// handle single stepping
	if (!debugcpu.is_stopped() && (m_flags & DEBUG_FLAG_STEPPING_ANY) != 0)
	{
		// is this an actual step?
		if (m_stepaddr == ~0 || curpc == m_stepaddr)
		{
			// decrement the count and reset the breakpoint
			m_stepsleft--;
			m_stepaddr = ~0;

			// if we hit 0, stop
			if (m_stepsleft == 0)
				debugcpu.set_execution_stopped();

			// update every 100 steps until we are within 200 of the end
			else if ((m_flags & DEBUG_FLAG_STEPPING_OUT) == 0 && (m_stepsleft < 200 || m_stepsleft % 100 == 0))
			{
				machine.debug_view().update_all();
				machine.debug_view().flush_osd_updates();
				machine.debugger().refresh_display();
			}
		}
	}

	// handle breakpoints
	if (!debugcpu.is_stopped() && (m_flags & (DEBUG_FLAG_STOP_TIME | DEBUG_FLAG_STOP_PC | DEBUG_FLAG_LIVE_BP)) != 0)
	{
		// see if we hit a target time
		if ((m_flags & DEBUG_FLAG_STOP_TIME) != 0 && machine.time() >= m_stoptime)
		{
			machine.debugger().console().printf("Stopped at time interval %.1g\n", machine.time().as_double());
			debugcpu.set_execution_stopped();
		}

		// check the temp running breakpoint and break if we hit it
		else if ((m_flags & DEBUG_FLAG_STOP_PC) != 0 && m_stopaddr == curpc)
		{
			machine.debugger().console().printf("Stopped at temporary breakpoint %X on CPU '%s'\n", m_stopaddr, m_device.tag());
			debugcpu.set_execution_stopped();
		}

		// check for execution breakpoints
		else if ((m_flags & DEBUG_FLAG_LIVE_BP) != 0)
			breakpoint_check(curpc);
	}

	// if we are supposed to halt, do it now
	if (debugcpu.is_stopped())
	{
		bool firststop = true;

		// load comments if we haven't yet
		debugcpu.ensure_comments_loaded();

		// reset any transient state
		debugcpu.reset_transient_flags();
		debugcpu.set_break_cpu(nullptr);

		// remember the last visible CPU in the debugger
		machine.debugger().console().set_visible_cpu(&m_device);

		// update all views
		machine.debug_view().update_all();
		machine.debugger().refresh_display();

		// wait for the debugger; during this time, disable sound output
		m_device.machine().sound().debugger_mute(true);
		while (debugcpu.is_stopped())
		{
			// flush any pending updates before waiting again
			machine.debug_view().flush_osd_updates();

			emulator_info::periodic_check();

			// clear the memory modified flag and wait
			debugcpu.set_memory_modified(false);
			if (machine.debug_flags & DEBUG_FLAG_OSD_ENABLED)
				machine.osd().wait_for_debugger(m_device, firststop);
			firststop = false;

			// if something modified memory, update the screen
			if (debugcpu.memory_modified())
			{
				machine.debug_view().update_all(DVT_DISASSEMBLY);
				machine.debugger().refresh_display();
			}

			// check for commands in the source file
			machine.debugger().console().process_source_file();

			// if an event got scheduled, resume
			if (machine.scheduled_event_pending())
				debugcpu.set_execution_running();
		}
		machine.sound().debugger_mute(false);

		// remember the last visible CPU in the debugger
		machine.debugger().console().set_visible_cpu(&m_device);
	}

	// handle step out/over on the instruction we are about to execute
	if ((m_flags & (DEBUG_FLAG_STEPPING_OVER | DEBUG_FLAG_STEPPING_OUT)) != 0 && m_stepaddr == ~0)
		prepare_for_step_overout(m_state->pcbase());

	// no longer in debugger code
	debugcpu.set_within_instruction(false);
}


//-------------------------------------------------
//  set_instruction_hook - set a hook to be
//  called on each instruction for a given device
//-------------------------------------------------

void device_debug::set_instruction_hook(debug_instruction_hook_func hook)
{
	// set the hook and also the CPU's flag for fast knowledge of the hook
	m_instrhook = hook;
	if (hook != nullptr)
		m_flags |= DEBUG_FLAG_HOOKED;
	else
		m_flags &= ~DEBUG_FLAG_HOOKED;
}


//-------------------------------------------------
//  ignore - ignore/observe a given device
//-------------------------------------------------

void device_debug::ignore(bool ignore)
{
	assert(m_exec != nullptr);

	if (ignore)
		m_flags &= ~DEBUG_FLAG_OBSERVING;
	else
		m_flags |= DEBUG_FLAG_OBSERVING;

	if (&m_device == m_device.machine().debugger().cpu().live_cpu() && ignore)
	{
		assert(m_exec != nullptr);
		go_next_device();
	}
}

//-------------------------------------------------
//  suspend
//-------------------------------------------------

void device_debug::suspend(bool suspend)
{
	assert(m_exec != nullptr);

	if (suspend) {
		m_flags |= DEBUG_FLAG_SUSPENDED;
		m_exec->suspend(SUSPEND_REASON_HALT, 1);
	}
	else {
		m_flags &= ~DEBUG_FLAG_SUSPENDED;
		m_exec->resume(SUSPEND_REASON_HALT);
	}

	if (&m_device == m_device.machine().debugger().cpu().live_cpu() && suspend)
	{
		assert(m_exec != nullptr);
		go_next_device();
	}
}

//-------------------------------------------------
//  single_step - single step the device past the
//  requested number of instructions
//-------------------------------------------------

void device_debug::single_step(int numsteps)
{
	assert(m_exec != nullptr);

	m_device.machine().rewind_capture();
	m_stepsleft = numsteps;
	m_stepaddr = ~0;
	m_flags |= DEBUG_FLAG_STEPPING;
	m_device.machine().debugger().cpu().set_execution_running();
}


//-------------------------------------------------
//  single_step_over - single step the device over
//  the requested number of instructions
//-------------------------------------------------

void device_debug::single_step_over(int numsteps)
{
	assert(m_exec != nullptr);

	m_device.machine().rewind_capture();
	m_stepsleft = numsteps;
	m_stepaddr = ~0;
	m_flags |= DEBUG_FLAG_STEPPING_OVER;
	m_device.machine().debugger().cpu().set_execution_running();
}


//-------------------------------------------------
//  single_step_out - single step the device
//  out of the current function
//-------------------------------------------------

void device_debug::single_step_out()
{
	assert(m_exec != nullptr);

	m_device.machine().rewind_capture();
	m_stepsleft = 100;
	m_stepaddr = ~0;
	m_flags |= DEBUG_FLAG_STEPPING_OUT;
	m_device.machine().debugger().cpu().set_execution_running();
}


//-------------------------------------------------
//  go - execute the device until it hits the given
//  address
//-------------------------------------------------

void device_debug::go(offs_t targetpc)
{
	assert(m_exec != nullptr);

	m_device.machine().rewind_invalidate();
	m_stopaddr = targetpc;
	m_flags |= DEBUG_FLAG_STOP_PC;
	m_device.machine().debugger().cpu().set_execution_running();
}


//-------------------------------------------------
//  go_vblank - execute until the next VBLANK
//-------------------------------------------------

void device_debug::go_vblank()
{
	assert(m_exec != nullptr);

	m_device.machine().rewind_invalidate();
	m_flags |= DEBUG_FLAG_STOP_VBLANK;
	m_device.machine().debugger().cpu().go_vblank();
}


//-------------------------------------------------
//  go_interrupt - execute until the specified
//  interrupt fires on the device
//-------------------------------------------------

void device_debug::go_interrupt(int irqline)
{
	assert(m_exec != nullptr);

	m_device.machine().rewind_invalidate();
	m_stopirq = irqline;
	m_flags |= DEBUG_FLAG_STOP_INTERRUPT;
	m_device.machine().debugger().cpu().set_execution_running();
}

void device_debug::go_next_device()
{
	m_device.machine().debugger().cpu().go_next_device(&m_device);
}

//-------------------------------------------------
//  go_exception - execute until the specified
//  exception fires on the visible CPU
//-------------------------------------------------

void device_debug::go_exception(int exception, const char *condition)
{
	assert(m_exec != nullptr);

	m_device.machine().rewind_invalidate();
	m_stopexception = exception;
	m_exception_condition = std::make_unique<parsed_expression>(*m_symtable, condition);
	m_flags |= DEBUG_FLAG_STOP_EXCEPTION;
	m_device.machine().debugger().cpu().set_execution_running();
}


//-------------------------------------------------
//  go_milliseconds - execute until the specified
//  delay elapses
//-------------------------------------------------

void device_debug::go_milliseconds(u64 milliseconds)
{
	assert(m_exec != nullptr);

	m_device.machine().rewind_invalidate();
	m_stoptime = m_device.machine().time() + attotime::from_msec(milliseconds);
	m_flags |= DEBUG_FLAG_STOP_TIME;
	m_device.machine().debugger().cpu().set_execution_running();
}


//-------------------------------------------------
//  go_privilege - execute until execution
//  level changes
//-------------------------------------------------

void device_debug::go_privilege(const char *condition)
{
	assert(m_exec != nullptr);
	m_device.machine().rewind_invalidate();
	m_privilege_condition = std::make_unique<parsed_expression>(*m_symtable, condition);
	m_flags |= DEBUG_FLAG_STOP_PRIVILEGE;
	m_device.machine().debugger().cpu().set_execution_running();
}


//-------------------------------------------------
//  halt_on_next_instruction_impl - halt in the
//  debugger on the next instruction, internal
//  implementation which is necessary solely due
//  to templates in C++ being janky as all get out
//-------------------------------------------------

void device_debug::halt_on_next_instruction_impl(util::format_argument_pack<std::ostream> &&args)
{
	assert(m_exec != nullptr);
	m_device.machine().debugger().cpu().halt_on_next_instruction(&m_device, std::move(args));
}

//-------------------------------------------------
//  breakpoint_find - return a breakpoint at the
//  given address, or nullptr if none exists there
//-------------------------------------------------

const debug_breakpoint *device_debug::breakpoint_find(offs_t address) const
{
	auto bpitp = m_bplist.equal_range(address);
	if (bpitp.first != bpitp.second)
		return bpitp.first->second.get();

	return nullptr;
}

//-------------------------------------------------
//  breakpoint_set - set a new breakpoint,
//  returning its index
//-------------------------------------------------

int device_debug::breakpoint_set(offs_t address, const char *condition, const char *action)
{
	// allocate a new one and hook it into our list
	u32 id = m_device.machine().debugger().cpu().get_breakpoint_index();
	m_bplist.emplace(address, std::make_unique<debug_breakpoint>(this, *m_symtable, id, address, condition, action));

	// update the flags and return the index
	breakpoint_update_flags();
	return id;
}


//-------------------------------------------------
//  breakpoint_clear - clear a breakpoint by index,
//  returning true if we found it
//-------------------------------------------------

bool device_debug::breakpoint_clear(int index)
{
	// scan the list to see if we own this breakpoint
	for (auto bpit = m_bplist.begin(); bpit != m_bplist.end(); ++bpit)
		if (bpit->second->m_index == index)
		{
			m_bplist.erase(bpit);
			breakpoint_update_flags();
			return true;
		}

	// we don't own it, return false
	return false;
}


//-------------------------------------------------
//  breakpoint_clear_all - clear all breakpoints
//-------------------------------------------------

void device_debug::breakpoint_clear_all()
{
	// clear the list
	m_bplist.clear();
	breakpoint_update_flags();
}


//-------------------------------------------------
//  breakpoint_enable - enable/disable a breakpoint
//  by index, returning true if we found it
//-------------------------------------------------

bool device_debug::breakpoint_enable(int index, bool enable)
{
	// scan the list to see if we own this breakpoint
	for (auto &bpp : m_bplist)
	{
		debug_breakpoint &bp = *bpp.second;
		if (bp.m_index == index)
		{
			bp.m_enabled = enable;
			breakpoint_update_flags();
			return true;
		}
	}

	// we don't own it, return false
	return false;
}


//-------------------------------------------------
//  breakpoint_enable_all - enable/disable all
//  breakpoints
//-------------------------------------------------

void device_debug::breakpoint_enable_all(bool enable)
{
	// apply the enable to all breakpoints we own
	for (auto &bpp : m_bplist)
		breakpoint_enable(bpp.second->index(), enable);
}


//-------------------------------------------------
//  watchpoint_set - set a new watchpoint,
//  returning its index
//-------------------------------------------------

int device_debug::watchpoint_set(address_space &space, read_or_write type, offs_t address, offs_t length, const char *condition, const char *action)
{
	if (space.spacenum() >= int(m_wplist.size()))
		m_wplist.resize(space.spacenum()+1);

	// allocate a new one
	u32 id = m_device.machine().debugger().cpu().get_watchpoint_index();
	m_wplist[space.spacenum()].emplace_back(std::make_unique<debug_watchpoint>(this, *m_symtable, id, space, type, address, length, condition, action));

	return id;
}


//-------------------------------------------------
//  watchpoint_clear - clear a watchpoint by index,
//  returning true if we found it
//-------------------------------------------------

bool device_debug::watchpoint_clear(int index)
{
	// scan the list to see if we own this breakpoint
	for (auto &wpl : m_wplist)
	{
		for (auto wpi = wpl.begin(); wpi != wpl.end(); wpi++)
			if ((*wpi)->index() == index)
			{
				wpl.erase(wpi);
				return true;
			}
	}

	// we don't own it, return false
	return false;
}


//-------------------------------------------------
//  watchpoint_clear_all - clear all watchpoints
//-------------------------------------------------

void device_debug::watchpoint_clear_all()
{
	for (auto &wpl : m_wplist)
		wpl.clear();
}


//-------------------------------------------------
//  watchpoint_enable - enable/disable a watchpoint
//  by index, returning true if we found it
//-------------------------------------------------

bool device_debug::watchpoint_enable(int index, bool enable)
{
	// scan the list to see if we own this watchpoint
	for (auto &wpl : m_wplist)
		for (auto &wp : wpl)
			if (wp->index() == index)
			{
				wp->setEnabled(enable);
				return true;
			}

	// we don't own it, return false
	return false;
}


//-------------------------------------------------
//  watchpoint_enable_all - enable/disable all
//  watchpoints
//-------------------------------------------------

void device_debug::watchpoint_enable_all(bool enable)
{
	// apply the enable to all watchpoints we own
	for (auto &wpl : m_wplist)
		for (auto &wp : wpl)
			wp->setEnabled(enable);
}


//-------------------------------------------------
//  registerpoint_set - set a new registerpoint,
//  returning its index
//-------------------------------------------------

int device_debug::registerpoint_set(const char *condition, const char *action)
{
	// allocate a new one
	u32 id = m_device.machine().debugger().cpu().get_registerpoint_index();
	m_rplist->emplace_front(*m_symtable, id, condition, action);

	// update the flags and return the index
	breakpoint_update_flags();
	return m_rplist->front().m_index;
}


//-------------------------------------------------
//  registerpoint_clear - clear a registerpoint by index,
//  returning true if we found it
//-------------------------------------------------

bool device_debug::registerpoint_clear(int index)
{
	// scan the list to see if we own this registerpoint
	for (auto brp = m_rplist->before_begin(); std::next(brp) != m_rplist->end(); ++brp)
		if (std::next(brp)->m_index == index)
		{
			m_rplist->erase_after(brp);
			breakpoint_update_flags();
			return true;
		}

	// we don't own it, return false
	return false;
}


//-------------------------------------------------
//  registerpoint_clear_all - clear all registerpoints
//-------------------------------------------------

void device_debug::registerpoint_clear_all()
{
	// clear the list
	m_rplist->clear();
	breakpoint_update_flags();
}


//-------------------------------------------------
//  registerpoint_enable - enable/disable a registerpoint
//  by index, returning true if we found it
//-------------------------------------------------

bool device_debug::registerpoint_enable(int index, bool enable)
{
	// scan the list to see if we own this conditionpoint
	for (debug_registerpoint &rp : *m_rplist)
		if (rp.m_index == index)
		{
			rp.m_enabled = enable;
			breakpoint_update_flags();
			return true;
		}

	// we don't own it, return false
	return false;
}


//-------------------------------------------------
//  registerpoint_enable_all - enable/disable all
//  registerpoints
//-------------------------------------------------

void device_debug::registerpoint_enable_all(bool enable)
{
	// apply the enable to all registerpoints we own
	for (debug_registerpoint &rp : *m_rplist)
		registerpoint_enable(rp.index(), enable);
}


//-------------------------------------------------
//  hotspot_track - enable/disable tracking of
//  hotspots
//-------------------------------------------------

void device_debug::hotspot_track(int numspots, int threshhold)
{
	// if we already have tracking enabled, kill it
	m_hotspots.clear();

	// only start tracking if we have a non-zero count
	if (numspots > 0)
	{
		// allocate memory for hotspots
		m_hotspots.resize(numspots);
		memset(&m_hotspots[0], 0xff, numspots*sizeof(m_hotspots[0]));

		// fill in the info
		m_hotspot_threshhold = threshhold;
	}
	reinstall_all(read_or_write::READ);
}


//-------------------------------------------------
//  history_pc - return an entry from the PC
//  history
//-------------------------------------------------

offs_t device_debug::history_pc(int index) const
{
	if (index > 0)
		index = 0;
	if (index <= -HISTORY_SIZE)
		index = -HISTORY_SIZE + 1;
	return m_pc_history[(m_pc_history_index + ARRAY_LENGTH(m_pc_history) - 1 + index) % ARRAY_LENGTH(m_pc_history)];
}


//-------------------------------------------------
//  track_pc_visited - returns a boolean stating
//  if this PC has been visited or not.  CRC32 is
//  done in this function on currently active CPU.
//  TODO: Take a CPU context as input
//-------------------------------------------------

bool device_debug::track_pc_visited(const offs_t& pc) const
{
	if (m_track_pc_set.empty())
		return false;
	const u32 crc = compute_opcode_crc32(pc);
	return m_track_pc_set.find(dasm_pc_tag(pc, crc)) != m_track_pc_set.end();
}


//-------------------------------------------------
//  set_track_pc_visited - set this pc as visited.
//  TODO: Take a CPU context as input
//-------------------------------------------------

void device_debug::set_track_pc_visited(const offs_t& pc)
{
	const u32 crc = compute_opcode_crc32(pc);
	m_track_pc_set.insert(dasm_pc_tag(pc, crc));
}


//-------------------------------------------------
//  track_mem_pc_from_address_data - returns the pc that
//  wrote the data to this address or (offs_t)(-1) for
//  'not available'.
//-------------------------------------------------

offs_t device_debug::track_mem_pc_from_space_address_data(const int& space,
															const offs_t& address,
															const u64& data) const
{
	const offs_t missing = (offs_t)(-1);
	if (m_track_mem_set.empty())
		return missing;
	std::set<dasm_memory_access>::iterator const mem_access = m_track_mem_set.find(dasm_memory_access(space, address, data, 0));
	if (mem_access == m_track_mem_set.end()) return missing;
	return mem_access->m_pc;
}


//-------------------------------------------------
//  comment_add - adds a comment to the list at
//  the given address
//-------------------------------------------------

void device_debug::comment_add(offs_t addr, const char *comment, rgb_t color)
{
	// create a new item for the list
	u32 const crc = compute_opcode_crc32(addr);
	dasm_comment const newComment = dasm_comment(addr, crc, comment, color);
	std::pair<std::set<dasm_comment>::iterator, bool> const inserted = m_comment_set.insert(newComment);
	if (!inserted.second)
	{
		// Insert returns false if comment exists
		m_comment_set.erase(inserted.first);
		m_comment_set.insert(newComment);
	}

	// force an update
	m_comment_change++;
}


//-------------------------------------------------
//  comment_remove - removes a comment at the
//  given address with a matching CRC
//-------------------------------------------------

bool device_debug::comment_remove(offs_t addr)
{
	const u32 crc = compute_opcode_crc32(addr);
	size_t const removed = m_comment_set.erase(dasm_comment(addr, crc, "", 0xffffffff));
	if (removed != 0U) m_comment_change++;
	return removed != 0U;
}


//-------------------------------------------------
//  comment_text - return the text of a comment
//-------------------------------------------------

const char *device_debug::comment_text(offs_t addr) const
{
	const u32 crc = compute_opcode_crc32(addr);
	auto comment = m_comment_set.find(dasm_comment(addr, crc, "", 0));
	if (comment == m_comment_set.end()) return nullptr;
	return comment->m_text.c_str();
}


//-------------------------------------------------
//  comment_export - export the comments to the
//  given XML data node
//-------------------------------------------------

bool device_debug::comment_export(util::xml::data_node &curnode)
{
	// iterate through the comments
	for (const auto & elem : m_comment_set)
	{
		util::xml::data_node *datanode = curnode.add_child("comment", util::xml::normalize_string(elem.m_text.c_str()));
		if (datanode == nullptr)
			return false;
		datanode->set_attribute_int("address", elem.m_address);
		datanode->set_attribute_int("color", elem.m_color);
		datanode->set_attribute("crc", string_format("%08X", elem.m_crc).c_str());
	}
	return true;
}


//-------------------------------------------------
//  comment_import - import the comments from the
//  given XML data node
//-------------------------------------------------

bool device_debug::comment_import(util::xml::data_node const &cpunode, bool is_inline)
{
	// iterate through nodes
	for (util::xml::data_node const *datanode = cpunode.get_child("comment"); datanode; datanode = datanode->get_next_sibling("comment"))
	{
		// extract attributes
		offs_t address = datanode->get_attribute_int("address", 0);
		rgb_t color = datanode->get_attribute_int("color", 0);

		u32 crc;
		sscanf(datanode->get_attribute_string("crc", nullptr), "%08X", &crc);

		// add the new comment
		if(is_inline == true)
			m_comment_set.insert(dasm_comment(address, crc, datanode->get_value(), color));
		else
			m_device.machine().debugger().console().printf(" %08X - %s\n", address, datanode->get_value());
	}
	return true;
}


//-------------------------------------------------
//  compute_opcode_crc32 - determine the CRC of
//  the opcode bytes at the given address
//-------------------------------------------------

u32 device_debug::compute_opcode_crc32(offs_t pc) const
{
	std::vector<u8> opbuf;
	debug_disasm_buffer buffer(device());

	// disassemble the current instruction and get the flags
	u32 dasmresult = buffer.disassemble_info(pc);
	buffer.data_get(pc, dasmresult & util::disasm_interface::LENGTHMASK, true, opbuf);

	// return a CRC of the exact count of opcode bytes
	return core_crc32(0, &opbuf[0], opbuf.size());
}


//-------------------------------------------------
//  trace - trace execution of a given device
//-------------------------------------------------

void device_debug::trace(FILE *file, bool trace_over, bool detect_loops, bool logerror, const char *action)
{
	// delete any existing tracers
	m_trace = nullptr;

	// if we have a new file, make a new tracer
	if (file != nullptr)
		m_trace = std::make_unique<tracer>(*this, *file, trace_over, detect_loops, logerror, action);
}


//-------------------------------------------------
//  trace_printf - output data into the given
//  device's tracefile, if tracing
//-------------------------------------------------

void device_debug::trace_printf(const char *fmt, ...)
{
	if (m_trace != nullptr)
	{
		va_list va;
		va_start(va, fmt);
		m_trace->vprintf(fmt, va);
		va_end(va);
	}
}


//-------------------------------------------------
//  compute_debug_flags - compute the global
//  debug flags for optimal efficiency
//-------------------------------------------------

void device_debug::compute_debug_flags()
{
	running_machine &machine = m_device.machine();
	debugger_cpu& debugcpu = machine.debugger().cpu();

	// clear out global flags by default, keep DEBUG_FLAG_OSD_ENABLED
	machine.debug_flags &= DEBUG_FLAG_OSD_ENABLED;
	machine.debug_flags |= DEBUG_FLAG_ENABLED;

	// if we are ignoring this CPU, or if events are pending, we're done
	if ((m_flags & DEBUG_FLAG_OBSERVING) == 0 || machine.scheduled_event_pending() || machine.save_or_load_pending())
		return;

	// if we're stopped, keep calling the hook
	if (debugcpu.is_stopped())
		machine.debug_flags |= DEBUG_FLAG_CALL_HOOK;

	// if we're tracking history, or we're hooked, or stepping, or stopping at a breakpoint
	// make sure we call the hook
	if ((m_flags & (DEBUG_FLAG_HISTORY | DEBUG_FLAG_HOOKED | DEBUG_FLAG_STEPPING_ANY | DEBUG_FLAG_STOP_PC | DEBUG_FLAG_LIVE_BP)) != 0)
		machine.debug_flags |= DEBUG_FLAG_CALL_HOOK;

	// also call if we are tracing
	if (m_trace != nullptr)
		machine.debug_flags |= DEBUG_FLAG_CALL_HOOK;

	// if we are stopping at a particular time and that time is within the current timeslice, we need to be called
	if ((m_flags & DEBUG_FLAG_STOP_TIME) && m_endexectime <= m_stoptime)
		machine.debug_flags |= DEBUG_FLAG_CALL_HOOK;
}


//-------------------------------------------------
//  prepare_for_step_overout - prepare things for
//  stepping over an instruction
//-------------------------------------------------

void device_debug::prepare_for_step_overout(offs_t pc)
{
	debug_disasm_buffer buffer(device());

	// disassemble the current instruction and get the flags
	u32 dasmresult = buffer.disassemble_info(pc);

	// if flags are supported and it's a call-style opcode, set a temp breakpoint after that instruction
	if ((dasmresult & util::disasm_interface::SUPPORTED) != 0 && (dasmresult & util::disasm_interface::STEP_OVER) != 0)
	{
		int extraskip = (dasmresult & util::disasm_interface::OVERINSTMASK) >> util::disasm_interface::OVERINSTSHIFT;
		pc = buffer.next_pc_wrap(pc, dasmresult & util::disasm_interface::LENGTHMASK);

		// if we need to skip additional instructions, advance as requested
		while (extraskip-- > 0) {
			u32 result = buffer.disassemble_info(pc);
			pc = buffer.next_pc_wrap(pc, result & util::disasm_interface::LENGTHMASK);
		}
		m_stepaddr = pc;
	}

	// if we're stepping out and this isn't a step out instruction, reset the steps until stop to a high number
	if ((m_flags & DEBUG_FLAG_STEPPING_OUT) != 0)
	{
		if ((dasmresult & util::disasm_interface::SUPPORTED) != 0 && (dasmresult & util::disasm_interface::STEP_OUT) == 0)
			m_stepsleft = 100;
		else
			m_stepsleft = 1;
	}
}


//-------------------------------------------------
//  breakpoint_update_flags - update the device's
//  breakpoint flags
//-------------------------------------------------

void device_debug::breakpoint_update_flags()
{
	// see if there are any enabled breakpoints
	m_flags &= ~DEBUG_FLAG_LIVE_BP;
	for (auto &bpp : m_bplist)
		if (bpp.second->m_enabled)
		{
			m_flags |= DEBUG_FLAG_LIVE_BP;
			break;
		}

	if ( ! ( m_flags & DEBUG_FLAG_LIVE_BP ) )
	{
		// see if there are any enabled registerpoints
		for (debug_registerpoint &rp : *m_rplist)
		{
			if (rp.m_enabled)
			{
				m_flags |= DEBUG_FLAG_LIVE_BP;
			}
		}
	}

	// push the flags out globally
	if (m_device.machine().debugger().cpu().live_cpu() != nullptr)
		m_device.machine().debugger().cpu().live_cpu()->debug()->compute_debug_flags();
}


//-------------------------------------------------
//  breakpoint_check - check the breakpoints for
//  a given device
//-------------------------------------------------

void device_debug::breakpoint_check(offs_t pc)
{
	debugger_cpu& debugcpu = m_device.machine().debugger().cpu();

	// see if we match
	auto bpitp = m_bplist.equal_range(pc);
	for (auto bpit = bpitp.first; bpit != bpitp.second; ++bpit)
	{
		debug_breakpoint &bp = *bpit->second;
		if (bp.hit(pc))
		{
			// halt in the debugger by default
			debugcpu.set_execution_stopped();

			// if we hit, evaluate the action
			if (!bp.m_action.empty())
				m_device.machine().debugger().console().execute_command(bp.m_action, false);

			// print a notification, unless the action made us go again
			if (debugcpu.is_stopped())
			{
				m_device.machine().debugger().console().printf("Stopped at breakpoint %X\n", bp.m_index);
				m_triggered_breakpoint = &bp;
			}
			break;
		}
	}

	// see if we have any matching registerpoints
	for (debug_registerpoint &rp : *m_rplist)
	{
		if (rp.hit())
		{
			// halt in the debugger by default
			debugcpu.set_execution_stopped();

			// if we hit, evaluate the action
			if (!rp.m_action.empty())
			{
				m_device.machine().debugger().console().execute_command(rp.m_action, false);
			}

			// print a notification, unless the action made us go again
			if (debugcpu.is_stopped())
			{
				m_device.machine().debugger().console().printf("Stopped at registerpoint %X\n", rp.m_index);
			}
			break;
		}
	}
}


//-------------------------------------------------
//  watchpoint_check - check the watchpoints
//  for a given CPU and address space
//-------------------------------------------------

//-------------------------------------------------
//  hotspot_check - check for hotspots on a
//  memory read access
//-------------------------------------------------

void device_debug::hotspot_check(address_space &space, offs_t address)
{
	offs_t curpc = m_device.state().pcbase();

	// see if we have a match in our list
	unsigned int hotindex;
	for (hotindex = 0; hotindex < m_hotspots.size(); hotindex++)
		if (m_hotspots[hotindex].m_access == address && m_hotspots[hotindex].m_pc == curpc && m_hotspots[hotindex].m_space == &space)
			break;

	// if we didn't find any, make a new entry
	if (hotindex == m_hotspots.size())
	{
		// if the bottom of the list is over the threshold, print it
		hotspot_entry &spot = m_hotspots[m_hotspots.size() - 1];
		if (spot.m_count > m_hotspot_threshhold)
			m_device.machine().debugger().console().printf("Hotspot @ %s %08X (PC=%08X) hit %d times (fell off bottom)\n", space.name(), spot.m_access, spot.m_pc, spot.m_count);

		// move everything else down and insert this one at the top
		memmove(&m_hotspots[1], &m_hotspots[0], sizeof(m_hotspots[0]) * (m_hotspots.size() - 1));
		m_hotspots[0].m_access = address;
		m_hotspots[0].m_pc = curpc;
		m_hotspots[0].m_space = &space;
		m_hotspots[0].m_count = 1;
	}

	// if we did find one, increase the count and move it to the top
	else
	{
		m_hotspots[hotindex].m_count++;
		if (hotindex != 0)
		{
			hotspot_entry temp = m_hotspots[hotindex];
			memmove(&m_hotspots[1], &m_hotspots[0], sizeof(m_hotspots[0]) * hotindex);
			m_hotspots[0] = temp;
		}
	}
}



//**************************************************************************
//  TRACER
//**************************************************************************

//-------------------------------------------------
//  tracer - constructor
//-------------------------------------------------

device_debug::tracer::tracer(device_debug &debug, FILE &file, bool trace_over, bool detect_loops, bool logerror, const char *action)
	: m_debug(debug)
	, m_file(file)
	, m_action((action != nullptr) ? action : "")
	, m_detect_loops(detect_loops)
	, m_logerror(logerror)
	, m_loops(0)
	, m_nextdex(0)
	, m_trace_over(trace_over)
	, m_trace_over_target(~0)
{
	memset(m_history, 0, sizeof(m_history));
}


//-------------------------------------------------
//  ~tracer - destructor
//-------------------------------------------------

device_debug::tracer::~tracer()
{
	// make sure we close the file if we can
	fclose(&m_file);
}


//-------------------------------------------------
//  update - log to the tracefile the data for a
//  given instruction
//-------------------------------------------------

void device_debug::tracer::update(offs_t pc)
{
	// are we in trace over mode and in a subroutine?
	if (m_trace_over && m_trace_over_target != ~0)
	{
		if (m_trace_over_target != pc)
			return;
		m_trace_over_target = ~0;
	}

	if (m_detect_loops)
	{
		// check for a loop condition
		int count = 0;
		for (auto & elem : m_history)
			if (elem == pc)
				count++;

		// if more than 1 hit, just up the loop count and get out
		if (count > 1)
		{
			m_loops++;
			return;
		}

		// if we just finished looping, indicate as much
		if (m_loops != 0)
			fprintf(&m_file, "\n   (loops for %d instructions)\n\n", m_loops);
		m_loops = 0;
	}

	// execute any trace actions first
	if (!m_action.empty())
		m_debug.m_device.machine().debugger().console().execute_command(m_action, false);

	debug_disasm_buffer buffer(m_debug.device());
	std::string instruction;
	offs_t next_pc, size;
	u32 dasmresult;
	buffer.disassemble(pc, instruction, next_pc, size, dasmresult);

	// output the result
	fprintf(&m_file, "%s: %s\n", buffer.pc_to_string(pc).c_str(), instruction.c_str());

	// do we need to step the trace over this instruction?
	if (m_trace_over && (dasmresult & util::disasm_interface::SUPPORTED) != 0 && (dasmresult & util::disasm_interface::STEP_OVER) != 0)
	{
		int extraskip = (dasmresult & util::disasm_interface::OVERINSTMASK) >> util::disasm_interface::OVERINSTSHIFT;
		offs_t trace_over_target = buffer.next_pc_wrap(pc, dasmresult & util::disasm_interface::LENGTHMASK);

		// if we need to skip additional instructions, advance as requested
		while (extraskip-- > 0)
			trace_over_target = buffer.next_pc_wrap(trace_over_target, buffer.disassemble_info(trace_over_target) & util::disasm_interface::LENGTHMASK);

		m_trace_over_target = trace_over_target;
	}

	// log this PC
	m_nextdex = (m_nextdex + 1) % TRACE_LOOPS;
	m_history[m_nextdex] = pc;
	fflush(&m_file);
}


//-------------------------------------------------
//  vprintf - generic print to the trace file
//-------------------------------------------------

void device_debug::tracer::vprintf(const char *format, va_list va)
{
	// pass through to the file
	vfprintf(&m_file, format, va);
	fflush(&m_file);
}


//-------------------------------------------------
//  flush - flush any pending changes to the trace
//  file
//-------------------------------------------------

void device_debug::tracer::flush()
{
	fflush(&m_file);
}


//-------------------------------------------------
//  dasm_pc_tag - constructor
//-------------------------------------------------

device_debug::dasm_pc_tag::dasm_pc_tag(const offs_t& address, const u32& crc)
	: m_address(address),
		m_crc(crc)
{
}

//-------------------------------------------------
//  dasm_memory_access - constructor
//-------------------------------------------------

device_debug::dasm_memory_access::dasm_memory_access(const int& address_space,
														const offs_t& address,
														const u64& data,
														const offs_t& pc)
	: m_address_space(address_space),
		m_address(address),
		m_data(data),
		m_pc(pc)
{
}

//-------------------------------------------------
//  dasm_comment - constructor
//-------------------------------------------------

device_debug::dasm_comment::dasm_comment(offs_t address, u32 crc, const char *text, rgb_t color)
	: dasm_pc_tag(address, crc),
		m_text(text),
		m_color(std::move(color))
{
}


//-------------------------------------------------
//  dasm_comment - constructor
//-------------------------------------------------

void device_debug::errorlog_write_line(const char *line)
{
	if (m_trace && m_trace->logerror())
		trace_printf("%s", line);
}
