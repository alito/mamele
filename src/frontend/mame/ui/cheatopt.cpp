// license:BSD-3-Clause
// copyright-holders:Nicola Salmoria, Aaron Giles, Nathan Woods
/*********************************************************************

    ui/cheatopt.cpp

    Internal menu for the cheat interface.

*********************************************************************/

#include "emu.h"
#include "cheat.h"
#include "mame.h"

#include "ui/ui.h"
#include "ui/menu.h"
#include "ui/cheatopt.h"


namespace ui {

// itemrefs for key menu items
#define ITEMREF_CHEATS_RESET_ALL            ((void *) 0x0001)
#define ITEMREF_CHEATS_RELOAD_ALL           ((void *) 0x0002)
#define ITEMREF_CHEATS_FIRST_ITEM           ((void *) 0x0003)


/*-------------------------------------------------
    menu_cheat - handle the cheat menu
-------------------------------------------------*/

void menu_cheat::handle()
{
	/* process the menu */
	const event *menu_event = process(PROCESS_LR_REPEAT);


	/* handle events */
	if (menu_event != nullptr && menu_event->itemref != nullptr)
	{
		bool changed = false;

		/* clear cheat comment on any movement or keypress */
		machine().popmessage();

		/* handle reset all + reset all cheats for reload all option */
		if ((menu_event->itemref == ITEMREF_CHEATS_RESET_ALL || menu_event->itemref == ITEMREF_CHEATS_RELOAD_ALL) && menu_event->iptkey == IPT_UI_SELECT)
		{
			for (auto &curcheat : mame_machine_manager::instance()->cheat().entries())
				if (curcheat->select_default_state())
					changed = true;
		}

		/* handle individual cheats */
		else if (menu_event->itemref >= ITEMREF_CHEATS_FIRST_ITEM)
		{
			cheat_entry *curcheat = reinterpret_cast<cheat_entry *>(menu_event->itemref);
			const char *string;
			switch (menu_event->iptkey)
			{
				/* if selected, activate a oneshot */
				case IPT_UI_SELECT:
					changed = curcheat->activate();
					break;

				/* if cleared, reset to default value */
				case IPT_UI_CLEAR:
					changed = curcheat->select_default_state();
					break;

				/* left decrements */
				case IPT_UI_LEFT:
					changed = curcheat->select_previous_state();
					break;

				/* right increments */
				case IPT_UI_RIGHT:
					changed = curcheat->select_next_state();
					break;

				/* bring up display comment if one exists */
				case IPT_UI_DISPLAY_COMMENT:
				case IPT_UI_UP:
				case IPT_UI_DOWN:
					string = curcheat->comment();
					if (string != nullptr && string[0] != 0)
						machine().popmessage(_("Cheat Comment:\n%s"), string);
					break;
			}
		}

		/* handle reload all  */
		if (menu_event->itemref == ITEMREF_CHEATS_RELOAD_ALL && menu_event->iptkey == IPT_UI_SELECT)
		{
			/* re-init cheat engine and thus reload cheats/cheats have already been turned off by here */
			mame_machine_manager::instance()->cheat().reload();

			/* display the reloaded cheats */
			reset(reset_options::REMEMBER_REF);
			machine().popmessage(_("All cheats reloaded"));
		}

		/* if things changed, update */
		if (changed)
			reset(reset_options::REMEMBER_REF);
	}
}


/*-------------------------------------------------
    menu_cheat_populate - populate the cheat menu
-------------------------------------------------*/

menu_cheat::menu_cheat(mame_ui_manager &mui, render_container &container) : menu(mui, container)
{
}

void menu_cheat::populate(float &customtop, float &custombottom)
{
	/* iterate over cheats */
	std::string text;
	std::string subtext;

	/* add a separator */
	item_append(menu_item_type::SEPARATOR);

	// add other cheats
	if (!mame_machine_manager::instance()->cheat().entries().empty()) {
		for (auto &curcheat : mame_machine_manager::instance()->cheat().entries())
		{
			uint32_t flags;
			curcheat->menu_text(text, subtext, flags);
			if (text == MENU_SEPARATOR_ITEM)
				item_append(menu_item_type::SEPARATOR, flags);
			else
				item_append(text, subtext, flags, curcheat.get());
		}

		/* add a separator */
		item_append(menu_item_type::SEPARATOR);

		/* add a reset all option */
		item_append(_("Reset All"), "", 0, (void *)ITEMREF_CHEATS_RESET_ALL);

		/* add a reload all cheats option */
		item_append(_("Reload All"), "", 0, (void *)ITEMREF_CHEATS_RELOAD_ALL);
	}
}

menu_cheat::~menu_cheat()
{
}

} // namespace ui
