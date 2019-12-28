// license:BSD-3-Clause
// copyright-holders:Nicola Salmoria, Aaron Giles, Nathan Woods, Maurizio Petrarota
/***************************************************************************

    ui/miscmenu.h

    Internal MAME menus for the user interface.

***************************************************************************/

#ifndef MAME_FRONTEND_UI_MISCMENU_H
#define MAME_FRONTEND_UI_MISCMENU_H

#pragma once

#include "ui/menu.h"

#include "crsshair.h"
#include "emuopts.h"

#include <utility>
#include <vector>


namespace ui {

class menu_keyboard_mode : public menu
{
public:
	menu_keyboard_mode(mame_ui_manager &mui, render_container &container);
	virtual ~menu_keyboard_mode();

private:
	virtual void populate(float &customtop, float &custombottom) override;
	virtual void handle() override;
};

class menu_network_devices : public menu
{
public:
	menu_network_devices(mame_ui_manager &mui, render_container &container);
	virtual ~menu_network_devices();

private:
	virtual void populate(float &customtop, float &custombottom) override;
	virtual void handle() override;
};

class menu_bookkeeping : public menu
{
public:
	menu_bookkeeping(mame_ui_manager &mui, render_container &container);
	virtual ~menu_bookkeeping();

private:
	virtual void populate(float &customtop, float &custombottom) override;
	virtual void handle() override;

	attotime prevtime;
};

class menu_crosshair : public menu
{
public:
	menu_crosshair(mame_ui_manager &mui, render_container &container);
	virtual ~menu_crosshair();

private:
	enum {
		CROSSHAIR_ITEM_VIS = 0,
		CROSSHAIR_ITEM_PIC,
		CROSSHAIR_ITEM_AUTO_TIME
	};

	/* internal crosshair menu item data */
	struct crosshair_item_data
	{
		render_crosshair *crosshair = nullptr;
		uint8_t     type = 0U;
		uint8_t     player = 0U;
		uint8_t     min = 0U, max = 0U;
		uint32_t    cur = 0U;
		uint8_t     defvalue = 0U;
		std::string last_name;
		std::string next_name;
	};

	virtual void populate(float &customtop, float &custombottom) override;
	virtual void handle() override;

	std::vector<crosshair_item_data> m_data;
	std::vector<std::string> m_pics;
};

class menu_quit_game : public menu
{
public:
	menu_quit_game(mame_ui_manager &mui, render_container &container);
	virtual ~menu_quit_game();

private:
	virtual void populate(float &customtop, float &custombottom) override;
	virtual void handle() override;
};

class menu_bios_selection : public menu
{
public:
	menu_bios_selection(mame_ui_manager &mui, render_container &container);
	virtual ~menu_bios_selection();

private:
	virtual void populate(float &customtop, float &custombottom) override;
	virtual void handle() override;
};


//-------------------------------------------------
//  export menu
//-------------------------------------------------

class menu_export : public menu
{
public:
	menu_export(mame_ui_manager &mui, render_container &container, std::vector<const game_driver*> &&list);
	virtual ~menu_export();

private:
	virtual void populate(float &customtop, float &custombottom) override;
	virtual void handle() override;

	std::vector<const game_driver*> m_list;
};

//-------------------------------------------------
//  machine configure menu
//-------------------------------------------------

class menu_machine_configure : public menu
{
public:
	menu_machine_configure(
			mame_ui_manager &mui,
			render_container &container,
			game_driver const &drv,
			std::function<void (bool, bool)> &&handler = nullptr,
			float x0 = 0.0f, float y0 = 0.0f);
	virtual ~menu_machine_configure();

protected:
	virtual void custom_render(void *selectedref, float top, float bottom, float x, float y, float x2, float y2) override;

private:
	using s_bios = std::vector<std::pair<std::string, int>>;

	enum
	{
		ADDFAV = 1,
		DELFAV,
		SAVE,
		CONTROLLER,
		VIDEO,
		BIOS,
		ADVANCED,
		LAST = ADVANCED
	};

	virtual void populate(float &customtop, float &custombottom) override;
	virtual void handle() override;

	void setup_bios();

	std::function<void (bool, bool)> const m_handler;
	game_driver const &m_drv;
	emu_options m_opts;
	float const m_x0;
	float const m_y0;
	s_bios m_bios;
	std::size_t m_curbios;
	bool const m_was_favorite;
	bool m_want_favorite;
};

//-------------------------------------------------
//  plugins configure menu
//-------------------------------------------------

class menu_plugins_configure : public menu
{
public:
	menu_plugins_configure(mame_ui_manager &mui, render_container &container);
	virtual ~menu_plugins_configure();

protected:
	virtual void populate(float &customtop, float &custombottom) override;
	virtual void handle() override;

	virtual void custom_render(void *selectedref, float top, float bottom, float x, float y, float x2, float y2) override;
};

} // namespace ui

#endif // MAME_FRONTEND_UI_MISCMENU_H
