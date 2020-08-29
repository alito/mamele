// license:GPL-2.0+
// copyright-holders:Couriersud


#ifndef PSTREAM_H_
#define PSTREAM_H_

///
/// \file pstream.h
///

#include "pconfig.h"
#include "pfmtlog.h"
#include "pgsl.h"
#include "pstring.h"

#include <array>
#include <fstream>
#include <fstream>
#include <ios>
#include <iostream>
#include <sstream>
#include <type_traits>
#include <vector>

namespace plib {

	/// \brief wrapper around isteam read
	///
	template <typename S, typename T>
	static inline S & istream_read(S &is, T * data, size_t len)
	{
		using ct = typename S::char_type;
		static_assert((sizeof(T) % sizeof(ct)) == 0, "istream_read sizeof issue");
		// NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
		return is.read(reinterpret_cast<ct *>(data), gsl::narrow<std::streamsize>(len * sizeof(T)));
	}

	/// \brief wrapper around osteam write
	///
	template <typename S, typename T>
	static inline S & ostream_write(S &os, const T * data, size_t len)
	{
		using ct = typename S::char_type;
		static_assert((sizeof(T) % sizeof(ct)) == 0, "ostream_write sizeof issue");
		// NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
		return os.write(reinterpret_cast<const ct *>(data), gsl::narrow<std::streamsize>(len * sizeof(T)));
	}

///
///
/// putf8reader_t digests linux & dos/windows text files
///
// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions)
class putf8_reader
{
public:

	PCOPYASSIGN(putf8_reader, delete)
	virtual ~putf8_reader() = default;

	putf8_reader(putf8_reader &&rhs) noexcept
	: m_strm(std::move(rhs.m_strm))
	, m_linebuf(std::move(rhs.m_linebuf))
	{
	}

	putf8_reader(std::unique_ptr<std::istream> &&rhs) noexcept
	: m_strm(std::move(rhs))
	{
		// no bad surprises
		m_strm->imbue(std::locale::classic());
	}

	bool eof() const { return m_strm->eof(); }

	/// \brief Read a line of UTF8 characters from the stream.
	///
	/// The line will not contain a trailing linefeed
	///
	/// \param line pstring reference to the result
	/// \returns Returns false if at end of file
	///
	bool readline(pstring &line)
	{
		putf8string::code_t c = 0;
		m_linebuf = putf8string("");
		if (!this->readcode(c))
		{
			line = "";
			return false;
		}
		while (true)
		{
			if (c == 10)
				break;
			if (c != 13) // ignore CR
				m_linebuf += putf8string(1, c);
			if (!this->readcode(c))
				break;
		}
		line = m_linebuf;
		return true;
	}

	/// \brief Read a line of UTF8 characters from the stream including trailing linefeed.
	///
	/// The line will contain the trailing linefeed
	///
	/// \param line pstring reference to the result
	/// \returns Returns false if at end of file
	///
	bool readline_lf(pstring &line)
	{
		putf8string::code_t c = 0;
		m_linebuf = putf8string("");
		if (!this->readcode(c))
		{
			line = "";
			return false;
		}
		while (true)
		{
			if (c != 13) // ignore CR
				m_linebuf += putf8string(1, c);
			if (c == 10)
				break;
			if (!this->readcode(c))
				break;
		}
		line = m_linebuf;
		return true;
	}

	bool readbyte(std::istream::char_type &b)
	{
		if (m_strm->eof())
			return false;
		m_strm->read(&b, 1);
		return (!m_strm->eof());
	}

	bool readcode(putf8string::traits_type::code_t &c)
	{
		std::array<std::istream::char_type, 4> b{0};
		if (m_strm->eof())
			return false;
		m_strm->read(&b[0], 1);
		if (m_strm->eof())
			return false;
		// NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
		const std::size_t l = putf8string::traits_type::codelen(reinterpret_cast<putf8string::traits_type::mem_t *>(&b));
		for (std::size_t i = 1; i < l; i++)
		{
			m_strm->read(&b[i], 1);
			if (m_strm->eof())
				return false;
		}
		// NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
		c = putf8string::traits_type::code(reinterpret_cast<putf8string::traits_type::mem_t *>(&b));
		return true;
	}

	std::istream &stream() { return *m_strm; }
private:
	std::unique_ptr<std::istream> m_strm;
	putf8string m_linebuf;
};

// -----------------------------------------------------------------------------
// putf8writer_t: writer on top of ostream
// -----------------------------------------------------------------------------

class putf8_writer
{
public:
	explicit putf8_writer(std::ostream *strm) : m_strm(strm) {}

	putf8_writer(putf8_writer &&src) noexcept : m_strm(src.m_strm) {}

	PCOPYASSIGN(putf8_writer, delete)
	putf8_writer &operator=(putf8_writer &&src) = delete;

	virtual ~putf8_writer() = default;

	void writeline(const pstring &line) const
	{
		write(line);
		write(10);
	}

	void write(const pstring &text) const
	{
		// NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
		const putf8string conv_utf8(text);
		//m_strm->write(conv_utf8.c_str(), static_cast<std::streamsize>(plib::strlen(conv_utf8.c_str()  )));
		ostream_write(*m_strm, conv_utf8.c_str(), string_info<pstring>::mem_size(conv_utf8));
	}

	void write(const pstring::value_type c) const
	{
		pstring t(1,c);
		write(t);
	}

	void flush() { m_strm->flush(); }
private:
	std::ostream *m_strm;
};

class putf8_fmt_writer : public pfmt_writer_t<putf8_fmt_writer>, public putf8_writer
{
public:

	explicit putf8_fmt_writer(std::ostream *strm)
	: putf8_writer(strm)
	{
	}

	PCOPYASSIGNMOVE(putf8_fmt_writer, delete)

	~putf8_fmt_writer() override = default;

//protected:
	void vdowrite(const pstring &s) const
	{
		write(s);
	}


private:
};

// -----------------------------------------------------------------------------
// pbinary_writer_t: writer on top of ostream
// -----------------------------------------------------------------------------

class pbinary_writer
{
public:
	explicit pbinary_writer(std::ostream &strm) : m_strm(strm) {}
	pbinary_writer(pbinary_writer &&src) noexcept : m_strm(src.m_strm) {}

	PCOPYASSIGN(pbinary_writer, delete)
	pbinary_writer &operator=(pbinary_writer &&src) = delete;

	virtual ~pbinary_writer() = default;

	template <typename T>
	void write(const T &val)
	{
		ostream_write(m_strm, &val, 1);
	}

	void write(const pstring &s)
	{
		const auto *sm = s.c_str();
		const auto sl(std::char_traits<pstring::mem_t>::length(sm));
		write(sl);
		ostream_write(m_strm, sm, sl);
	}

	template <typename T>
	void write(const std::vector<T> &val)
	{
		const auto sz(val.size());
		write(sz);
		ostream_write(m_strm, val.data(), sz);
	}

private:
	std::ostream &m_strm;
};

class pbinary_reader
{
public:
	explicit pbinary_reader(std::istream &strm) : m_strm(strm) {}
	pbinary_reader(pbinary_reader &&src) noexcept : m_strm(src.m_strm) { }

	PCOPYASSIGN(pbinary_reader, delete)
	pbinary_reader &operator=(pbinary_reader &&src) = delete;

	virtual ~pbinary_reader() = default;

	template <typename T>
	void read(T &val)
	{
		istream_read(m_strm, &val, 1);
	}

	void read( pstring &s)
	{
		std::size_t sz = 0;
		read(sz);
		std::vector<plib::string_info<pstring>::mem_t> buf(sz+1);
		m_strm.read(buf.data(), static_cast<std::streamsize>(sz));
		buf[sz] = 0;
		s = pstring(buf.data());
	}

	template <typename T>
	void read(std::vector<T> &val)
	{
		std::size_t sz = 0;
		read(sz);
		val.resize(sz);
		istream_read(m_strm, val.data(), sz);
	}

private:
	std::istream &m_strm;
};

inline void copystream(std::ostream &dest, std::istream &src)
{
	// FIXME: optimize
	std::array<std::ostream::char_type, 1024> buf; // NOLINT(cppcoreguidelines-pro-type-member-init)
	while (!src.eof())
	{
		src.read(buf.data(), 1);
		dest.write(buf.data(), 1);
	}
}

///
/// \brief utf8 filename aware ifstream wrapper
///
class ifstream : public std::ifstream
{
public:

	using filename_type = std::conditional<compile_info::win32() && (!compile_info::mingw() || compile_info::version()>=900),
		pstring_t<pwchar_traits>, pstring_t<putf8_traits>>::type;

	template <typename T>
	explicit ifstream(const pstring_t<T> name, ios_base::openmode mode = ios_base::in)
	: std::ifstream(filename_type(name).c_str(), mode)
	{
	}
};

///
/// \brief utf8 filename aware ofstream wrapper
///
class ofstream : public std::ofstream
{
public:
	using filename_type = std::conditional<compile_info::win32() && (!compile_info::mingw() || compile_info::version()>=900),
		pstring_t<pwchar_traits>, pstring_t<putf8_traits>>::type;

	template <typename T>
	explicit ofstream(const pstring_t<T> name, ios_base::openmode mode = ios_base::out | ios_base::trunc)
	: std::ofstream(filename_type(name).c_str(), mode)
	{
	}
};


struct perrlogger
{
	template <typename ... Args>
	explicit perrlogger(Args&& ... args)
	{
		h()(std::forward<Args>(args)...); // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
	}
private:
	static putf8_fmt_writer &h()
	{
		static plib::putf8_fmt_writer perr(&std::cerr);
		return perr;
	}
};

// -----------------------------------------------------------------------------
// c++17 preparation
// -----------------------------------------------------------------------------

namespace filesystem
{

	// FIXME: u8path should return a path object (c++17)

	template< class Source >
	pstring u8path( const Source& source )
	{
		return source;
	}

} // namespace filesystem

} // namespace plib

#endif // PSTREAM_H_
