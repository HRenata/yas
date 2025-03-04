
// Copyright (c) 2010-2019 niXman (i dot nixman dog gmail dot com). All
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

#ifndef __yas__types__std__std_string_serializers_hpp
#define __yas__types__std__std_string_serializers_hpp

#include <yas/detail/type_traits/serializer.hpp>
#include <yas/detail/tools/cast.hpp>
#include <yas/detail/tools/save_load_string.hpp>
#include <yas/detail/tools/json_tools.hpp>

#include <string>
#include <cassert>

namespace yas {
namespace detail {

/***************************************************************************/

template<std::size_t F>
struct serializer<
     type_prop::not_a_fundamental
    ,ser_case::use_internal_serializer
    ,F
    ,std::string
> {
    template<typename Archive>
    static Archive& save(Archive& ar, const std::string &str) {
        if constexpr ( F & yas::json ) {
            if ( str.empty() ) {
                ar.write("null", 4);
            } else {
                ar.write("\"", 1);
                save_string(ar, str.data(), str.size());
                ar.write("\"", 1);
            }
        } else {
            ar.write_seq_size(str.length());
            ar.write(str.data(), str.length());
        }

        return ar;
    }

    template<typename Archive>
    static Archive& load(Archive& ar, std::string &str) {
        if constexpr ( F & yas::json ) {
            char ch = ar.getch();
            if ( ch == '\"' ) {
                load_string(str, ar);
                __YAS_THROW_IF_BAD_JSON_CHARS(ar, "\"");
            } else if ( ch == 'n' ) {
                ar.ungetch(ch);
                __YAS_THROW_IF_BAD_JSON_CHARS(ar, "null");
                str.clear();
            } else if ( is_valid_for_int_and_double(ar, ch) ) {
                str += ch;
                for ( ch = ar.peekch(); is_valid_for_int_and_double(ar, ch); ch = ar.peekch() ) {
                    str += ar.getch();
                }
            } else {
                __YAS_THROW_IF_BAD_JSON_CHARS(ar, "unreachable");
            }
        } else {
            const auto size = ar.read_seq_size();
            str.resize(size);
            ar.read(__YAS_CCAST(char*, str.data()), size);
        }

        return ar;
    }
};

/***************************************************************************/

} // namespace detail
} // namespace yas

#endif // __yas__types__std__std_string_serializers_hpp
