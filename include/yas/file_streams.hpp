
// Copyright (c) 2010-2018 niXman (i dot nixman dog gmail dot com). All
// rights reserved.
//
// This file is part of YAS(https://github.com/niXman/yas) project.
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//
//
// Boost Software License - Version 1.0 - August 17th, 2003
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
//
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

#ifndef __yas__file_streams_hpp
#define __yas__file_streams_hpp

#include <yas/detail/config/config.hpp>
#include <yas/detail/tools/noncopyable.hpp>
#include <yas/detail/io/io_exceptions.hpp>

#include <string>
#include <cstdio>

namespace yas {

/***************************************************************************/

enum file_mode {
     file_trunc  = 1u<<0
    ,file_append = 1u<<1
    ,file_nobuf  = 1u<<2
};

/***************************************************************************/

struct file_ostream {
	YAS_NONCOPYABLE(file_ostream)
    YAS_MOVABLE(file_ostream)

	file_ostream(const char *fname, std::size_t m = 0)
		:file(nullptr)
	{
		const bool exists = file_exists(fname);
		if ( exists && !m ) {
            YAS_THROW_FILE_ALREADY_EXISTS();
        }
		if ( !exists && (m & file_append) ) {
            YAS_THROW_FILE_IS_NOT_EXISTS();
        }

        const char *fmode = file_mode_str(m);
		file = std::fopen(fname, fmode);
		if ( !file ) {
            YAS_THROW_ERROR_OPEN_FILE();
        }

        if ( m & file_nobuf ) {
            std::setvbuf(file, nullptr, _IONBF, 0);
        }
	}
	virtual ~file_ostream() {
		std::fclose(file);
	}

	std::size_t write(const void *ptr, std::size_t size) {
		return std::fwrite(ptr, 1, size, file);
	}

	void flush() { std::fflush(file); }

private:
	static const char* file_mode_str(std::size_t m) {
        m &= ~file_nobuf;
        if ( !m || (m & file_trunc) ) {
            return "wb";
        } else if ( m & file_append ) {
            return "ab";
        }

        YAS_THROW_BAD_FILE_MODE();
    }
	static bool file_exists(const char *fname) {
		FILE* file = std::fopen(fname, "r");
		if ( file ) {
			std::fclose(file);
			return true;
		}
		return false;
	}

	std::FILE *file;
}; // struct file_ostream

/***************************************************************************/

struct file_istream {
    YAS_NONCOPYABLE(file_istream)
    YAS_MOVABLE(file_istream)

	file_istream(const char *fname, std::size_t m = 0)
		:file(std::fopen(fname, "rb"))
	{
		if ( !file ) {
            YAS_THROW_ERROR_OPEN_FILE();
        }

        if ( m & file_nobuf ) {
            std::setvbuf(file, nullptr, _IONBF, 0);
        }
	}
	virtual ~file_istream() {
		std::fclose(file);
	}

	std::size_t read(void *ptr, std::size_t size) {
		return std::fread(ptr, 1, size, file);
	}

	bool empty() const { return peekch() == EOF; }
	char peekch() const {
        // TODO: optimize
        int ch = std::getc(file);
        std::ungetc(ch, file);

        return YAS_SCAST(char, ch);
    }
    char getch() { return YAS_SCAST(char, std::getc(file)); }
	void ungetch(char ch) { std::ungetc(ch, file); }

private:
	std::FILE *file;
}; // struct file_istream

/***************************************************************************/

} // ns yas

#endif // __yas__file_streams_hpp
