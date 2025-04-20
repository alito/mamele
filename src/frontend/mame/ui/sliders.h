// license:BSD-3-Clause
// copyright-holders:Nicola Salmoria, Aaron Giles, Nathan Woods
/***************************************************************************

    ui/sliders.h

    Internal MAME menus for the user interface.

***************************************************************************/

#ifndef MAME_FRONTEND_UI_SLIDERS_H
#define MAME_FRONTEND_UI_SLIDERS_H

#pragma once

#include "ui/menu.h"


namespace ui {

class menu_sliders : public menu
{
public:
	menu_sliders(mame_ui_manager &mui, render_container &container, bool menuless_mode = false);
	virtual ~menu_sliders() override;

protected:
	virtual void custom_render(void *selectedref, float top, float bottom, float x, float y, float x2, float y2) override;
	virtual void menu_activated() override;
	virtual void menu_deactivated() override;

private:
	virtual void populate(float &customtop, float &custombottom) override;
	virtual void handle(event const *ev) override;

	bool const m_menuless_mode;
	bool m_hidden;
};

} // namespace ui

#endif // MAME_FRONTEND_UI_SLIDERS_H
