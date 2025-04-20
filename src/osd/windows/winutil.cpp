// license:BSD-3-Clause
// copyright-holders:Aaron Giles
//============================================================
//
//  winutil.cpp - Win32 OSD core utility functions
//
//============================================================

#include "winutil.h"

// lib/util
#include "timeconv.h"

// osd
#include "strconv.h"

// standard windows headers
#include <windows.h>
#include <direct.h>


//============================================================
//  win_attributes_to_entry_type
//============================================================

osd::directory::entry::entry_type win_attributes_to_entry_type(DWORD attributes)
{
	if (attributes == 0xFFFFFFFF)
		return osd::directory::entry::entry_type::NONE;
	else if (attributes & FILE_ATTRIBUTE_DIRECTORY)
		return osd::directory::entry::entry_type::DIR;
	else
		return osd::directory::entry::entry_type::FILE;
}



//============================================================
//  win_time_point_from_filetime
//============================================================

std::chrono::system_clock::time_point win_time_point_from_filetime(LPFILETIME file_time)
{
	auto converted_file_time = util::ntfs_duration_from_filetime(file_time->dwHighDateTime, file_time->dwLowDateTime);
	return util::system_clock_time_point_from_ntfs_duration(converted_file_time);
}



//============================================================
//  win_is_gui_application
//============================================================

BOOL win_is_gui_application()
{
	static BOOL is_gui_frontend;
	static BOOL is_first_time = TRUE;
	HMODULE module;
	BYTE *image_ptr;
	IMAGE_DOS_HEADER *dos_header;
	IMAGE_NT_HEADERS *nt_headers;
	IMAGE_OPTIONAL_HEADER *opt_header;

	// is this the first time we've been ran?
	if (is_first_time)
	{
		is_first_time = FALSE;

		// get the current module
		module = GetModuleHandleUni();
		if (!module)
			return FALSE;
		image_ptr = (BYTE*) module;

		// access the DOS header
		dos_header = (IMAGE_DOS_HEADER *) image_ptr;
		if (dos_header->e_magic != IMAGE_DOS_SIGNATURE)
			return FALSE;

		// access the NT headers
		nt_headers = (IMAGE_NT_HEADERS *) ((BYTE*)(dos_header) + (DWORD)(dos_header->e_lfanew));
		if (nt_headers->Signature != IMAGE_NT_SIGNATURE)
			return FALSE;

		// access the optional header
		opt_header = &nt_headers->OptionalHeader;
		switch (opt_header->Subsystem)
		{
			case IMAGE_SUBSYSTEM_WINDOWS_GUI:
				is_gui_frontend = TRUE;
				break;

			case IMAGE_SUBSYSTEM_WINDOWS_CUI:
				is_gui_frontend = FALSE;
				break;
		}
	}
	return is_gui_frontend;
}

//============================================================
//  osd_subst_env
//============================================================
std::string osd_subst_env(std::string_view src)
{
	std::wstring const w_src = osd::text::to_wstring(src);
	std::vector<wchar_t> buffer(w_src.size() + 2);
	DWORD length(ExpandEnvironmentStringsW(w_src.c_str(), &buffer[0], buffer.size()));
	while (length && (buffer.size() < length))
	{
		buffer.clear();
		buffer.resize(length + 1);
		length = ExpandEnvironmentStringsW(w_src.c_str(), &buffer[0], buffer.size());
	}

	std::string dst;
	if (length)
		osd::text::from_wstring(dst, &buffer[0]);
	return dst;
}

//-------------------------------------------------
//  Universal way to get module handle
//-------------------------------------------------

HMODULE WINAPI GetModuleHandleUni()
{
	MEMORY_BASIC_INFORMATION mbi;
	VirtualQuery((LPCVOID)GetModuleHandleUni, &mbi, sizeof(mbi));
	return (HMODULE)mbi.AllocationBase;
}


//============================================================
//  win_error_to_error_condition
//============================================================

std::error_condition win_error_to_error_condition(DWORD error) noexcept
{
	// TODO: work out if there's a better way to do this
	switch (error)
	{
	case ERROR_SUCCESS:
		return std::error_condition();

	case ERROR_INVALID_HANDLE:
		return std::errc::bad_file_descriptor;

	case ERROR_OUTOFMEMORY:
		return std::errc::not_enough_memory;

	case ERROR_NOT_SUPPORTED:
		return std::errc::not_supported;

	case ERROR_FILE_NOT_FOUND:
	case ERROR_PATH_NOT_FOUND:
	case ERROR_INVALID_NAME:
		return std::errc::no_such_file_or_directory;

	case ERROR_FILENAME_EXCED_RANGE:
		return std::errc::filename_too_long;

	case ERROR_ACCESS_DENIED:
	case ERROR_SHARING_VIOLATION:
		return std::errc::permission_denied;

	case ERROR_ALREADY_EXISTS:
		return std::errc::file_exists;

	case ERROR_TOO_MANY_OPEN_FILES:
		return std::errc::too_many_files_open;

	case ERROR_WRITE_FAULT:
	case ERROR_READ_FAULT:
		return std::errc::io_error;

	case ERROR_HANDLE_DISK_FULL:
	case ERROR_DISK_FULL:
		return std::errc::no_space_on_device;

	case ERROR_PATH_BUSY:
	case ERROR_BUSY:
		return std::errc::device_or_resource_busy;

	case ERROR_FILE_TOO_LARGE:
		return std::errc::file_too_large;

	case ERROR_INVALID_ACCESS:
	case ERROR_NEGATIVE_SEEK:
	case ERROR_BAD_ARGUMENTS:
		return std::errc::invalid_argument;

	default:
		return std::error_condition(error, std::system_category());
	}
}
