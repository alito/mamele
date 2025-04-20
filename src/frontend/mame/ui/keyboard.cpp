// license:BSD-3-Clause
// copyright-holders:Vas Crabb
/***************************************************************************

    ui/keyboard.cpp

    Keyboard mode menu.

***************************************************************************/

#include "emu.h"
#include "ui/keyboard.h"

#include "natkeyboard.h"


namespace ui {

namespace {

constexpr uintptr_t ITEM_KBMODE         = 0x00000100;
constexpr uintptr_t ITEM_KBDEV_FIRST    = 0x00000200;

} // anonymous namespace


menu_keyboard_mode::menu_keyboard_mode(mame_ui_manager &mui, render_container &container) : menu(mui, container)
{
	set_heading(_("menu-keyboard", "Keyboard Selection"));
}

void menu_keyboard_mode::menu_activated()
{
	// scripts could have changed something behind our back
	reset(reset_options::REMEMBER_POSITION);
}

void menu_keyboard_mode::populate(float &customtop, float &custombottom)
{
	natural_keyboard &natkbd(machine().natkeyboard());

	if (natkbd.can_post())
	{
		bool const natmode(natkbd.in_use());
		item_append(
				_("menu-keyboard", "Keyboard Mode"),
				natmode ? _("menu-keyboard", "Natural") : _("menu-keyboard", "Emulated"),
				natmode ? FLAG_LEFT_ARROW : FLAG_RIGHT_ARROW,
				reinterpret_cast<void *>(ITEM_KBMODE));
		item_append(menu_item_type::SEPARATOR);
	}

	uintptr_t ref(ITEM_KBDEV_FIRST);
	for (size_t i = 0; natkbd.keyboard_count() > i; ++i, ++ref)
	{
		device_t &kbddev(natkbd.keyboard_device(i));
		bool const enabled(natkbd.keyboard_enabled(i));
		item_append(
				util::string_format(
					kbddev.owner() ? _("%1$s [root%2$s]") : _("[root%2$s]"),
					kbddev.type().fullname(),
					kbddev.tag()),
				enabled ? _("Enabled") : _("Disabled"),
				enabled ? FLAG_LEFT_ARROW : FLAG_RIGHT_ARROW,
				reinterpret_cast<void *>(ref));
	}
	item_append(menu_item_type::SEPARATOR);
}

menu_keyboard_mode::~menu_keyboard_mode()
{
}

void menu_keyboard_mode::handle(event const *ev)
{
	if (ev && uintptr_t(ev->itemref))
	{
		natural_keyboard &natkbd(machine().natkeyboard());
		uintptr_t const ref(uintptr_t(ev->itemref));
		bool left(IPT_UI_LEFT == ev->iptkey);
		bool right(IPT_UI_RIGHT == ev->iptkey);
		if (ITEM_KBMODE == ref)
		{
			if (IPT_UI_SELECT == ev->iptkey)
			{
				left = natkbd.in_use();
				right = !left;
			}
			if ((left || right) && (natkbd.in_use() != right))
			{
				natkbd.set_in_use(right);
				ev->item->set_subtext(right ? _("menu-keyboard", "Natural") : _("menu-keyboard", "Emulated"));
				ev->item->set_flags(right ? FLAG_LEFT_ARROW : FLAG_RIGHT_ARROW);
			}
		}
		else if (ITEM_KBDEV_FIRST <= ref)
		{
			auto const kbdno(ref - ITEM_KBDEV_FIRST);
			if (IPT_UI_SELECT == ev->iptkey)
			{
				left = natkbd.keyboard_enabled(kbdno);
				right = !left;
			}
			if ((left || right) && (natkbd.keyboard_enabled(kbdno) != right))
			{
				if (right)
					natkbd.enable_keyboard(kbdno);
				else
					natkbd.disable_keyboard(kbdno);
				ev->item->set_subtext(right ? _("Enabled") : _("Disabled"));
				ev->item->set_flags(right ? FLAG_LEFT_ARROW : FLAG_RIGHT_ARROW);
			}
		}
	}
}

} // namespace ui
