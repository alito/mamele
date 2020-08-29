// license:BSD-3-Clause
// copyright-holders:Nathan Woods
/***************************************************************************

    ui/state.cpp

    Menus for saving and loading state

***************************************************************************/

#include "emu.h"
#include "ui/state.h"

#include "emuopts.h"
#include "inputdev.h"


namespace ui {

/***************************************************************************
    ANONYMOUS NAMESPACE
***************************************************************************/

namespace {

//-------------------------------------------------
//  keyboard_input_item_name
//-------------------------------------------------

std::string keyboard_input_item_name(input_item_id id)
{
	if (id >= ITEM_ID_A && id <= ITEM_ID_Z)
		return std::string(1, char(id - ITEM_ID_A + 'a'));
	if (id >= ITEM_ID_0 && id <= ITEM_ID_9)
		return std::string(1, char(id - ITEM_ID_0 + '0'));

	// only supported for A-Z/0-9
	throw false;
}


//-------------------------------------------------
//  code_item_pair
//-------------------------------------------------

std::pair<std::string, std::string> code_item_pair(const running_machine &machine, input_item_id id)
{
	// only supported for A-Z|0-9
	assert((id >= ITEM_ID_A && id <= ITEM_ID_Z) || (id >= ITEM_ID_0 && id <= ITEM_ID_9));
	input_code const code = input_code(DEVICE_CLASS_KEYBOARD, 0, ITEM_CLASS_SWITCH, ITEM_MODIFIER_NONE, id);

	return std::make_pair(keyboard_input_item_name(id), machine.input().code_name(code));
}

} // anonymous namespace


/***************************************************************************
    FILE ENTRY
***************************************************************************/

std::string menu_load_save_state_base::s_last_file_selected;


//-------------------------------------------------
//  file_entry ctor
//-------------------------------------------------

menu_load_save_state_base::file_entry::file_entry(std::string &&file_name, std::string &&visible_name, const std::chrono::system_clock::time_point &last_modified)
	: m_file_name(std::move(file_name))
	, m_visible_name(std::move(visible_name))
	, m_last_modified(last_modified)
{
}


/***************************************************************************
    BASE CLASS FOR LOAD AND SAVE
***************************************************************************/

//-------------------------------------------------
//  ctor
//-------------------------------------------------

menu_load_save_state_base::menu_load_save_state_base(mame_ui_manager &mui, render_container &container, const char *header, const char *footer, bool must_exist)
	: menu(mui, container)
	, m_header(header)
	, m_footer(footer)
	, m_must_exist(must_exist)
	, m_was_paused(false)
	, m_keys_released(false)
{
}


//-------------------------------------------------
//  dtor
//-------------------------------------------------

menu_load_save_state_base::~menu_load_save_state_base()
{
	// resume if appropriate (is the destructor really the right place
	// to do this sort of activity?)
	if (!m_was_paused)
		machine().resume();
}


//-------------------------------------------------
//  populate
//-------------------------------------------------

void menu_load_save_state_base::populate(float &customtop, float &custombottom)
{
	// build the "filename to code" map, if we have not already (if it were not for the
	// possibility that the system keyboard can be changed at runtime, I would put this
	// into a static)
	if (m_filename_to_code_map.empty())
	{
		// loop through A-Z/0-9
		for (input_item_id id = ITEM_ID_A; id <= ITEM_ID_Z; id++)
			m_filename_to_code_map.emplace(code_item_pair(machine(), id));
		for (input_item_id id = ITEM_ID_0; id <= ITEM_ID_9; id++)
			m_filename_to_code_map.emplace(code_item_pair(machine(), id));

		// do joysticks
		input_class const &sticks = machine().input().device_class(DEVICE_CLASS_JOYSTICK);
		if (sticks.enabled())
		{
			for (int i = 0; sticks.maxindex() >= i; ++i)
			{
				input_device const *const stick = sticks.device(i);
				if (stick)
				{
					for (input_item_id j = ITEM_ID_BUTTON1; (ITEM_ID_BUTTON32 >= j) && (stick->maxitem() >= j); ++j)
					{
						input_device_item const *const item = stick->item(j);
						if (item && (item->itemclass() == ITEM_CLASS_SWITCH))
						{
							m_filename_to_code_map.emplace(
									util::string_format("joy%i-%i", i, j - ITEM_ID_BUTTON1 + 1),
									machine().input().code_name(item->code()));
						}
					}
				}
			}
		}
	}

	// open the state directory
	osd::directory::ptr dir = osd::directory::open(state_directory());

	// create a separate vector, so we can add sorted entries to the menu
	std::vector<const file_entry *> m_entries_vec;

	// populate all file entries
	m_file_entries.clear();
	if (dir)
	{
		const osd::directory::entry *entry;
		while ((entry = dir->read()) != nullptr)
		{
			if (core_filename_ends_with(entry->name, ".sta"))
			{
				// get the file name of the entry
				std::string file_name = core_filename_extract_base(entry->name, true);

				// try translating it
				std::string visible_name = get_visible_name(file_name);

				// and proceed
				file_entry fileent(std::string(file_name), std::move(visible_name), entry->last_modified);
				auto iter = m_file_entries.emplace(std::make_pair(std::move(file_name), std::move(fileent))).first;
				m_entries_vec.push_back(&iter->second);
			}
		}
	}

	// sort the vector; put recently modified state files at the top
	std::sort(
			m_entries_vec.begin(),
			m_entries_vec.end(),
			[] (const file_entry *a, const file_entry *b)
			{
				return a->last_modified() > b->last_modified();
			});

	// add the entries
	for (const file_entry *entry : m_entries_vec)
	{
		// get the time as a local time string
		char time_string[128];
		auto last_modified_time_t = std::chrono::system_clock::to_time_t(entry->last_modified());
		std::strftime(time_string, sizeof(time_string), "%c", std::localtime(&last_modified_time_t));

		// format the text
		std::string text = util::string_format("%s: %s",
				entry->visible_name(),
				time_string);

		// append the menu item
		void *const itemref = itemref_from_file_entry(*entry);
		item_append(std::move(text), std::string(), 0, itemref);

		// is this item selected?
		if (entry->file_name() == s_last_file_selected)
			set_selection(itemref);
	}

	if (m_entries_vec.empty())
	{
		item_append(_("No save states found"), std::string(), 0, nullptr);
		set_selection(nullptr);
	}
	item_append(menu_item_type::SEPARATOR);

	// set up custom render proc
	customtop = ui().get_line_height() + 3.0f * ui().box_tb_border();
	custombottom = ui().get_line_height() + 3.0f * ui().box_tb_border();

	// pause if appropriate
	m_was_paused = machine().paused();
	if (!m_was_paused)
		machine().pause();

	// get ready to poll inputs
	machine().input().reset_polling();
	m_keys_released = false;
}


//-------------------------------------------------
//  handle
//-------------------------------------------------

void menu_load_save_state_base::handle()
{
	// process the menu
	event const *const event = process(0);

	// process the event
	if (event && (event->iptkey == IPT_UI_SELECT))
	{
		if (event->itemref)
		{
			// user selected one of the entries
			file_entry const &entry = file_entry_from_itemref(event->itemref);
			slot_selected(std::string(entry.file_name()));
		}
	}
	else
	{
		// poll inputs
		std::string name = poll_inputs();
		if (!name.empty())
			try_select_slot(std::move(name));
	}
}


//-------------------------------------------------
//  get_visible_name
//-------------------------------------------------

std::string menu_load_save_state_base::get_visible_name(const std::string &file_name)
{
	auto const iter = m_filename_to_code_map.find(file_name);
	if (iter != m_filename_to_code_map.end())
		return iter->second;

	// otherwise these are the same
	return file_name;
}


//-------------------------------------------------
//  poll_inputs
//-------------------------------------------------

std::string menu_load_save_state_base::poll_inputs()
{
	input_code const code = machine().input().poll_switches();
	if (INPUT_CODE_INVALID == code)
	{
		m_keys_released = true;
	}
	else if (m_keys_released)
	{
		input_item_id const id = code.item_id();

		// keyboard A-Z and 0-9
		if (((ITEM_ID_A <= id) && (ITEM_ID_Z >= id)) || ((ITEM_ID_0 <= id) && (ITEM_ID_9 >= id)))
			return keyboard_input_item_name(id);

		// joystick buttons
		if ((DEVICE_CLASS_JOYSTICK == code.device_class()) && (ITEM_CLASS_SWITCH == code.item_class()) && (ITEM_MODIFIER_NONE == code.item_modifier()) && (ITEM_ID_BUTTON1 <= id) && (ITEM_ID_BUTTON32 >= id))
			return util::string_format("joy%i-%i", code.device_index(), id - ITEM_ID_BUTTON1 + 1);
	}
	return "";
}


//-------------------------------------------------
//  try_select_slot
//-------------------------------------------------

void menu_load_save_state_base::try_select_slot(std::string &&name)
{
	if (!m_must_exist || is_present(name))
		slot_selected(std::move(name));
}


//-------------------------------------------------
//  slot_selected
//-------------------------------------------------

void menu_load_save_state_base::slot_selected(std::string &&name)
{
	// handle it
	process_file(std::string(name));

	// record the last slot touched
	s_last_file_selected = std::move(name);

	// no matter what, pop out
	menu::stack_pop(machine());
}


//-------------------------------------------------
//  custom_render - perform our special rendering
//-------------------------------------------------

void menu_load_save_state_base::custom_render(void *selectedref, float top, float bottom, float origx1, float origy1, float origx2, float origy2)
{
	extra_text_render(top, bottom, origx1, origy1, origx2, origy2, m_header, nullptr);
	if (m_footer)
	{
		char const *const text[] = { m_footer };
		draw_text_box(
				std::begin(text), std::end(text),
				origx1, origx2, origy2 + ui().box_tb_border(), origy2 + bottom,
				ui::text_layout::CENTER, ui::text_layout::NEVER, false,
				ui().colors().text_color(), ui().colors().background_color(), 1.0f);
	}
}


//-------------------------------------------------
//  itemref_from_file_entry
//-------------------------------------------------

void *menu_load_save_state_base::itemref_from_file_entry(const menu_load_save_state_base::file_entry &entry)
{
	return (void *)&entry;
}


//-------------------------------------------------
//  file_entry_from_itemref
//-------------------------------------------------

const menu_load_save_state_base::file_entry &menu_load_save_state_base::file_entry_from_itemref(void *itemref)
{
	return *((const file_entry *)itemref);
}


//-------------------------------------------------
//  state_name
//-------------------------------------------------

std::string menu_load_save_state_base::state_directory() const
{
	const char *stateopt = machine().options().state_name();
	return util::string_format("%s%s%s",
			machine().options().state_directory(),
			PATH_SEPARATOR,
			machine().get_statename(stateopt));
}


//-------------------------------------------------
//  is_present
//-------------------------------------------------

bool menu_load_save_state_base::is_present(const std::string &name) const
{
	return m_file_entries.find(name) != m_file_entries.end();
}


/***************************************************************************
    LOAD STATE
***************************************************************************/

//-------------------------------------------------
//  ctor
//-------------------------------------------------

menu_load_state::menu_load_state(mame_ui_manager &mui, render_container &container)
	: menu_load_save_state_base(mui, container, _("Load State"), _("Select state to load"), true)
{
}


//-------------------------------------------------
//  process_file
//-------------------------------------------------

void menu_load_state::process_file(std::string &&file_name)
{
	machine().schedule_load(std::move(file_name));
}


/***************************************************************************
    SAVE STATE
***************************************************************************/

//-------------------------------------------------
//  ctor
//-------------------------------------------------

menu_save_state::menu_save_state(mame_ui_manager &mui, render_container &container)
	: menu_load_save_state_base(mui, container, _("Save State"), _("Press a key or joystick button, or select state to overwrite"), false)
{
}


//-------------------------------------------------
//  process_file
//-------------------------------------------------

void menu_save_state::process_file(std::string &&file_name)
{
	machine().schedule_save(std::move(file_name));
}


} // namespace ui
