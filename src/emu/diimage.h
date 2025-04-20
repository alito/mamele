// license:BSD-3-Clause
// copyright-holders:Miodrag Milanovic
/***************************************************************************

    diimage.h

    Device image interfaces.

***************************************************************************/

#pragma once

#ifndef __EMU_H__
#error Dont include this file directly; include emu.h instead.
#endif

#ifndef MAME_EMU_DIIMAGE_H
#define MAME_EMU_DIIMAGE_H

#include "utilfwd.h"

#include <memory>
#include <string>
#include <system_error>
#include <vector>


//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

enum class image_error : int
{
	INTERNAL = 1,
	UNSUPPORTED,
	INVALIDIMAGE,
	ALREADYOPEN,
	UNSPECIFIED
};

const std::error_category &image_category() noexcept;
inline std::error_condition make_error_condition(image_error e) noexcept { return std::error_condition(int(e), image_category()); }
namespace std { template <> struct is_error_condition_enum<image_error> : public std::true_type { }; }

class image_device_format
{
public:
	image_device_format(const std::string &name, const std::string &description, const std::string &extensions, const std::string &optspec);
	~image_device_format();

	const std::string &name() const noexcept { return m_name; }
	const std::string &description() const noexcept { return m_description; }
	const std::vector<std::string> &extensions() const noexcept { return m_extensions; }
	const std::string &optspec() const noexcept { return m_optspec; }

private:
	std::string                 m_name;
	std::string                 m_description;
	std::vector<std::string>    m_extensions;
	std::string                 m_optspec;
};


enum class image_init_result { PASS, FAIL };
enum class image_verify_result { PASS, FAIL };

//**************************************************************************
//  MACROS
//**************************************************************************

#define DEVICE_IMAGE_LOAD_MEMBER(_name)             image_init_result _name(device_image_interface &image)
#define DECLARE_DEVICE_IMAGE_LOAD_MEMBER(_name)     DEVICE_IMAGE_LOAD_MEMBER(_name)

#define DEVICE_IMAGE_UNLOAD_MEMBER(_name)           void _name(device_image_interface &image)
#define DECLARE_DEVICE_IMAGE_UNLOAD_MEMBER(_name)   DEVICE_IMAGE_UNLOAD_MEMBER(_name)


// ======================> device_image_interface

// class representing interface-specific live image
class device_image_interface : public device_interface
{
public:
	typedef device_delegate<image_init_result (device_image_interface &)> load_delegate;
	typedef device_delegate<void (device_image_interface &)> unload_delegate;

	typedef std::vector<std::unique_ptr<image_device_format>> formatlist_type;

	// construction/destruction
	device_image_interface(const machine_config &mconfig, device_t &device);
	virtual ~device_image_interface();

	virtual image_init_result call_load() { return image_init_result::PASS; }
	virtual image_init_result call_create(int format_type, util::option_resolution *format_options) { return image_init_result::PASS; }
	virtual void call_unload() { }
	virtual std::string call_display() { return std::string(); }
	virtual u32 unhashed_header_length() const noexcept { return 0; }
	virtual bool core_opens_image_file() const noexcept { return true; }
	virtual bool image_is_chd_type() const noexcept { return false; }
	virtual bool is_readable()  const noexcept = 0;
	virtual bool is_writeable() const noexcept = 0;
	virtual bool is_creatable() const noexcept = 0;
	virtual bool is_reset_on_load() const noexcept = 0;
	virtual bool support_command_line_image_creation() const noexcept { return false; }
	virtual const char *image_interface() const noexcept { return nullptr; }
	virtual const char *file_extensions() const noexcept = 0;
	virtual const util::option_guide &create_option_guide() const;
	virtual const char *image_type_name() const noexcept = 0;
	virtual const char *image_brief_type_name() const noexcept = 0;

	const image_device_format *device_get_indexed_creatable_format(int index) const noexcept { return (index < m_formatlist.size()) ? m_formatlist.at(index).get() : nullptr;  }
	const image_device_format *device_get_named_creatable_format(std::string_view format_name) const noexcept;
	const util::option_guide &device_get_creation_option_guide() const { return create_option_guide(); }

	std::string_view error();
	void seterror(std::error_condition err, const char *message = nullptr);
	void message(const char *format, ...) ATTR_PRINTF(2,3);

	bool exists() const noexcept { return !m_image_name.empty(); }

	// get image file path/name
	const char *filename() const noexcept { return m_image_name.empty() ? nullptr : m_image_name.c_str(); }
	const char *basename() const noexcept { return m_basename.empty() ? nullptr : m_basename.c_str(); }
	const char *basename_noext()  const noexcept { return m_basename_noext.empty() ? nullptr : m_basename_noext.c_str(); }
	const std::string &filetype() const noexcept { return m_filetype; }
	bool is_filetype(std::string_view candidate_filetype) const;

	bool is_open() const noexcept { return bool(m_file); }
	util::core_file &image_core_file() const noexcept { assert(is_open()); return *m_file; }
	bool is_readonly() const noexcept { return m_readonly; }

	// image file I/O wrappers
	// TODO: move away from using these and let implementations use the I/O interface directly
	// FIXME: don't swallow errors
	u64 length()
	{
		check_for_file();
		u64 result = 0;
		m_file->length(result);
		return result;
	}
	u32 fread(void *buffer, u32 length)
	{
		check_for_file();
		size_t actual;
		m_file->read(buffer, length, actual);
		return actual;
	}
	u32 fwrite(const void *buffer, u32 length)
	{
		check_for_file();
		size_t actual;
		m_file->write(buffer, length, actual);
		return actual;
	}
	std::error_condition fseek(s64 offset, int whence)
	{
		check_for_file();
		return m_file->seek(offset, whence);
	}
	u64 ftell()
	{
		check_for_file();
		u64 result = 0;
		m_file->tell(result);
		return result;
	}
	bool image_feof()
	{
		check_for_file();
		return m_file->eof();
	}

	// allocate and read into buffers
	u32 fread(std::unique_ptr<u8 []> &ptr, u32 length) { ptr = std::make_unique<u8 []>(length); return fread(ptr.get(), length); }
	u32 fread(std::unique_ptr<u8 []> &ptr, u32 length, offs_t offset) { ptr = std::make_unique<u8 []>(length); return fread(ptr.get() + offset, length - offset); }

	// access to software list item information
	const software_info *software_entry() const noexcept;
	const software_part *part_entry() const noexcept { return m_software_part_ptr; }
	const char *software_list_name() const noexcept { return m_software_list_name.c_str(); }
	bool loaded_through_softlist() const noexcept { return m_software_part_ptr != nullptr; }

	// working directory
	void set_working_directory(std::string_view working_directory) { m_working_directory = working_directory; }
	void set_working_directory(std::string &&working_directory) { m_working_directory = std::move(working_directory); }
	const std::string &working_directory() const { return m_working_directory; }

	// access to software list properties and ROM data areas
	u8 *get_software_region(std::string_view tag);
	u32 get_software_region_length(std::string_view tag);
	const char *get_feature(std::string_view feature_name) const;
	bool load_software_region(std::string_view tag, std::unique_ptr<u8[]> &ptr);

	u32 crc();
	util::hash_collection& hash() { return m_hash; }
	util::hash_collection calculate_hash_on_file(util::random_read &file) const;

	void battery_load(void *buffer, int length, int fill);
	void battery_load(void *buffer, int length, const void *def_buffer);
	void battery_save(const void *buffer, int length);

	const std::string &instance_name() const { return m_instance_name; }
	const std::string &brief_instance_name() const { return m_brief_instance_name; }
	const std::string &canonical_instance_name() const { return m_canonical_instance_name; }
	const formatlist_type &formatlist() const { return m_formatlist; }

	// loads an image file
	image_init_result load(std::string_view path);

	// loads a softlist item by name
	image_init_result load_software(std::string_view software_identifier);

	image_init_result finish_load();
	void unload();
	image_init_result create(std::string_view path, const image_device_format *create_format, util::option_resolution *create_args);
	image_init_result create(std::string_view path);
	bool load_software(software_list_device &swlist, std::string_view swname, const rom_entry *entry);
	std::error_condition reopen_for_write(std::string_view path);

	void set_user_loadable(bool user_loadable) noexcept { m_user_loadable = user_loadable; }
	void set_must_be_loaded(bool must_be_loaded) noexcept { m_must_be_loaded = must_be_loaded; }

	bool user_loadable() const noexcept { return m_user_loadable; }
	bool must_be_loaded() const noexcept { return m_must_be_loaded; }
	bool is_reset_and_loading() const noexcept { return m_is_reset_and_loading; }
	const std::string &full_software_name() const noexcept { return m_full_software_name; }

protected:
	// interface-level overrides
	virtual void interface_config_complete() override;

	virtual const software_list_loader &get_software_list_loader() const;
	virtual const bool use_software_list_file_extension_for_filetype() const { return false; }

	image_init_result load_internal(std::string_view path, bool is_create, int create_format, util::option_resolution *create_args);
	std::error_condition load_image_by_path(u32 open_flags, std::string_view path);
	void clear();
	bool is_loaded() const { return m_file != nullptr; }

	void set_image_filename(std::string_view filename);

	void clear_error() noexcept;

	void check_for_file() const { if (!m_file) throw emu_fatalerror("%s(%s): Illegal operation on unmounted image", device().shortname(), device().tag()); }

	void make_readonly() noexcept { m_readonly = true; }

	bool image_checkhash();

	const software_part *find_software_item(std::string_view identifier, bool restrict_to_interface, software_list_device **device = nullptr) const;
	std::string software_get_default_slot(std::string_view default_card_slot) const;

	void add_format(std::unique_ptr<image_device_format> &&format);
	void add_format(std::string &&name, std::string &&description, std::string &&extensions, std::string &&optspec);

	// error related info
	std::error_condition m_err;
	std::string m_err_message;

private:
	// variables that are only non-zero when an image is mounted
	util::core_file::ptr m_file;
	std::unique_ptr<emu_file> m_mame_file;
	std::string m_image_name;
	std::string m_basename;
	std::string m_basename_noext;
	std::string m_filetype;

	// Software information
	std::string m_full_software_name;
	const software_part *m_software_part_ptr;
	std::string m_software_list_name;

	std::vector<u32> determine_open_plan(bool is_create);
	void update_names();
	bool load_software_part(std::string_view identifier);

	bool init_phase() const;
	static std::error_condition run_hash(util::random_read &file, u32 skip_bytes, util::hash_collection &hashes, const char *types);

	// loads an image or software items and resets - called internally when we
	// load an is_reset_on_load() item
	void reset_and_load(std::string_view path);

	// creation info
	formatlist_type m_formatlist;

	// working directory; persists across mounts
	std::string m_working_directory;

	// flags
	bool m_readonly;
	bool m_created;

	// special - used when creating
	int m_create_format;
	util::option_resolution *m_create_args;

	util::hash_collection m_hash;

	std::string m_instance_name;                // e.g. - "cartridge", "floppydisk2"
	std::string m_brief_instance_name;          // e.g. - "cart", "flop2"
	std::string m_canonical_instance_name;      // e.g. - "cartridge1", "floppydisk2" - only used internally in emuopts.cpp

	// in the case of arcade cabinet with fixed carts inserted,
	// we want to disable command line cart loading...
	bool m_user_loadable;

	bool m_must_be_loaded;

	bool m_is_loading;

	bool m_is_reset_and_loading;
};

// iterator
typedef device_interface_enumerator<device_image_interface> image_interface_enumerator;

#endif  /* MAME_EMU_DIIMAGE_H */
