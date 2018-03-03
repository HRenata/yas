
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

#ifndef __yas__detail__tools__enumerate_fusion_seq_hpp
#define __yas__detail__tools__enumerate_fusion_seq_hpp

#include <yas/detail/config/config.hpp>

#if defined(YAS_SERIALIZE_BOOST_TYPES)
#include <yas/detail/type_traits/type_traits.hpp>
#include <yas/detail/type_traits/serializer.hpp>
#include <yas/detail/io/serialization_exceptions.hpp>

#include <boost/fusion/sequence/intrinsic/at.hpp>
#include <boost/fusion/include/at.hpp>

namespace yas {
namespace detail {
namespace concepts {
namespace fusion_seq {

/***************************************************************************/

template<
     std::size_t F
    ,std::size_t I = 0
    ,typename Archive
    ,template<typename...> class Cont
    ,typename... Tp
>
static typename std::enable_if<I == sizeof...(Tp), Archive &>::type
apply(Archive &ar, const Cont<Tp...> &) { return ar; }

template<
     std::size_t F
    ,std::size_t I = 0
    ,typename Archive
    ,template<typename...> class Cont
    ,typename... Tp
>
static typename std::enable_if<I < sizeof...(Tp), Archive &>::type
apply(Archive &ar, const Cont<Tp...> &t) {
    ar & boost::fusion::at_c<I>(t);

    if ((F & yas::json) && I + 1 < sizeof...(Tp)) {
        ar.write(",", 1);
    }

    return apply<F, I + 1>(ar, t);
}

template<
     std::size_t F
    ,std::size_t I = 0
    ,typename Archive
    ,template<typename...> class Cont
    ,typename... Tp
>
static typename std::enable_if<I == sizeof...(Tp), Archive &>::type
apply(Archive &ar, Cont<Tp...> &) { return ar; }

template<
     std::size_t F
    ,std::size_t I = 0
    ,typename Archive
    ,template<typename...> class Cont
    ,typename... Tp
>
static typename std::enable_if<I < sizeof...(Tp), Archive &>::type
apply(Archive &ar, Cont<Tp...> &t) {
    __YAS_CONSTEXPR_IF ( (F & yas::json) && !(F & yas::compacted) ) {
        json_skipws(ar);
    }

    ar & boost::fusion::at_c<I>(t);

    __YAS_CONSTEXPR_IF ( (F & yas::json) && !(F & yas::compacted) ) {
        json_skipws(ar);
    }

    if ((F & yas::json) && I + 1 < sizeof...(Tp)) {
        YAS_THROW_IF_BAD_JSON_CHARS(ar, ",");
    }

    return apply<F, I + 1>(ar, t);
}

/***************************************************************************/

template<
        std::size_t F, typename Archive, template<typename...> class Cont, typename... Types
>
Archive &save(Archive &ar, const Cont<Types...> &cont) {
    if ( F & options::binary ) {
        ar.write(YAS_SCAST(std::uint8_t, sizeof...(Types)));
    } else if ( F & yas::text ) {
        ar.write(sizeof...(Types));
    }

    if ( F & yas::json ) { ar.write("[", 1); }

    apply<F>(ar, cont);

    if ( F & yas::json ) { ar.write("]", 1); }

    return ar;
};

template<
        std::size_t F, typename Archive, template<typename...> class Cont, typename... Types
>
Archive &load(Archive &ar, Cont<Types...> &cont) {
    if ( F & options::binary ) {
        std::uint8_t size = 0;
        ar.read(size);
        if ( size != sizeof...(Types)) { YAS_THROW_BAD_SIZE_ON_DESERIALIZE("fusion::container"); }
    } else if ( F & yas::text ) {
        std::uint32_t size = 0;
        ar.read(size);
        if ( size != sizeof...(Types)) { YAS_THROW_BAD_SIZE_ON_DESERIALIZE("fusion::container"); }
    }

    if ( F & yas::json ) { YAS_THROW_IF_BAD_JSON_CHARS(ar, "["); }

    apply<F>(ar, cont);

    if ( F & yas::json ) { YAS_THROW_IF_BAD_JSON_CHARS(ar, "]"); }

    return ar;
};

/***************************************************************************/

} // ns fusion_seq
} // ns concepts
} // ns detail
} // ns yas

#endif // defined(YAS_SERIALIZE_BOOST_TYPES)

#endif // __yas__detail__tools__enumerate_fusion_seq_hpp
