// license:BSD-3-Clause
// copyright-holders:Ryan Holtz
//============================================================
//
//  chainmanager.h - BGFX shader chain manager
//
//  Provides loading for BGFX shader effect chains, defined
//  by chain.h and read by chainreader.h
//
//============================================================

#ifndef MAME_RENDER_BGFX_CHAINMANAGER_H
#define MAME_RENDER_BGFX_CHAINMANAGER_H

#pragma once

#include "effectmanager.h"
#include "targetmanager.h"
#include "texturemanager.h"

#include "util/utilfwd.h"

#include <map>
#include <string>
#include <string_view>
#include <utility>
#include <vector>


class running_machine;
class osd_window;
struct slider_state;
class slider_dirty_notifier;
class render_primitive;

namespace ui { class menu_item; }

class bgfx_chain;
class bgfx_slider;

class chain_manager
{
public:
	class screen_prim
	{
	public:
		screen_prim() = default;
		screen_prim(render_primitive *prim);

		render_primitive *m_prim = nullptr;
		uint16_t m_screen_width = 0;
		uint16_t m_screen_height = 0;
		uint16_t m_quad_width = 0;
		uint16_t m_quad_height = 0;
		float m_tex_width = 0.0f;
		float m_tex_height = 0.0f;
		int m_rowpixels = 0;
		uint32_t m_palette_length = 0;
		uint32_t m_flags = 0;
	};

	chain_manager(running_machine& machine, const osd_options& options, texture_manager& textures, target_manager& targets, effect_manager& effects, uint32_t window_index,
		slider_dirty_notifier& slider_notifier, uint16_t user_prescale, uint16_t max_prescale_size);
	~chain_manager();

	uint32_t update_screen_textures(uint32_t view, render_primitive *starting_prim, osd_window& window);
	uint32_t process_screen_chains(uint32_t view, osd_window& window);

	// Getters
	running_machine& machine() const { return m_machine; }
	const osd_options& options() const { return m_options; }
	texture_manager& textures() const { return m_textures; }
	target_manager& targets() const { return m_targets; }
	effect_manager& effects() const { return m_effects; }
	slider_dirty_notifier& slider_notifier() const { return m_slider_notifier; }
	uint32_t window_index() const { return m_window_index; }
	uint32_t screen_count() const { return m_screen_count; }
	bgfx_chain* screen_chain(uint32_t screen);
	std::unique_ptr<bgfx_chain> load_chain(std::string name, uint32_t screen_index);
	bool has_applicable_chain(uint32_t screen);
	std::vector<ui::menu_item> get_slider_list();
	std::vector<std::vector<float>> slider_settings();

	// Setters
	void restore_slider_settings(int32_t id, std::vector<std::vector<float>>& settings);

	void load_config(util::xml::data_node const &screennode);
	void save_config(util::xml::data_node &parentnode);

private:
	class chain_desc
	{
	public:
		chain_desc(const chain_desc &) = default;
		chain_desc(chain_desc &&) = default;
		chain_desc &operator=(const chain_desc &) = default;
		chain_desc &operator=(chain_desc &&) = default;

		chain_desc(std::string &&name, std::string &&path)
			: m_name(std::move(name))
			, m_path(std::move(path))
		{
		}

		std::string m_name;
		std::string m_path;
	};

	void load_chains();
	void destroy_chains();
	void reload_chains();

	void init_texture_converters();

	void get_default_chain_info(std::string &out_chain_name, int32_t &out_chain_index);
	void refresh_available_chains();
	void destroy_unloaded_chains();
	void find_available_chains(std::string_view root, std::string_view path);
	void parse_chain_selections(std::string_view chain_str);
	std::vector<std::string_view> split_option_string(std::string_view chain_str) const;

	void update_screen_count(uint32_t screen_count);

	void set_current_chain(uint32_t screen, int32_t chain_index);
	int32_t slider_changed(int id, std::string *str, int32_t newval);
	void create_selection_slider(uint32_t screen_index);
	bool needs_sliders();

	uint32_t count_screens(render_primitive* prim);
	void process_screen_quad(uint32_t view, uint32_t screen, screen_prim &prim, osd_window& window);

	running_machine&            m_machine;
	const osd_options&          m_options;
	texture_manager&            m_textures;
	target_manager&             m_targets;
	effect_manager&             m_effects;
	uint32_t                    m_window_index;
	uint16_t                    m_user_prescale;
	uint16_t                    m_max_prescale_size;
	slider_dirty_notifier&      m_slider_notifier;
	uint32_t                    m_screen_count;
	int32_t                     m_default_chain_index;
	std::vector<chain_desc>     m_available_chains;
	std::vector<bgfx_chain*>    m_screen_chains;
	std::vector<std::string>    m_chain_names;
	std::vector<ui::menu_item>  m_selection_sliders;
	std::vector<std::unique_ptr<slider_state>> m_core_sliders;
	std::vector<int32_t>        m_current_chain;
	std::vector<bgfx_texture*>  m_screen_textures;
	std::vector<bgfx_texture*>  m_screen_palettes;
	std::vector<bgfx_effect*>   m_converters;
	bgfx_effect *               m_adjuster;
	std::vector<screen_prim>    m_screen_prims;
	std::vector<uint8_t>        m_palette_temp;

	static inline constexpr uint32_t CHAIN_NONE = 0;
};

#endif // MAME_RENDER_BGFX_CHAINMANAGER_H
