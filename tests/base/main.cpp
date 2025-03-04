
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

#ifdef _MSC_VER
#   ifndef NOMINMAX
#       define NOMINMAX
#   endif
#endif // _MSC_VER

#include <yas/mem_streams.hpp>
#include <yas/file_streams.hpp>
#include <yas/std_streams.hpp>
#include <yas/null_streams.hpp>
#include <yas/binary_oarchive.hpp>
#include <yas/binary_iarchive.hpp>
#include <yas/text_oarchive.hpp>
#include <yas/text_iarchive.hpp>
#include <yas/json_oarchive.hpp>
#include <yas/json_iarchive.hpp>
#include <yas/std_types.hpp>
#include <yas/serialize.hpp>
#include <yas/tools/archinfo.hpp>
#include <yas/tools/hexdump.hpp>

#ifdef YAS_SERIALIZE_BOOST_TYPES
#include <yas/boost_types.hpp>

#include <boost/fusion/container/generation/make_vector.hpp>
#include <boost/fusion/include/make_vector.hpp>
#include <boost/fusion/sequence/comparison.hpp>
#include <boost/fusion/include/comparison.hpp>
#endif // YAS_SERIALIZE_BOOST_TYPES

#ifdef YAS_SERIALIZE_ABSL_TYPES
#include <yas/abseil_types.hpp>
#endif // YAS_SERIALIZE_ABSL_TYPES


/***************************************************************************/

#include "test.hpp"
#include "include/array.hpp"
#include "include/auto_array.hpp"
#include "include/base64.hpp"
#include "include/base_object.hpp"
#include "include/bitset.hpp"
#include "include/chrono.hpp"
#include "include/complex.hpp"
#include "include/buffer.hpp"
#include "include/endian.hpp"
#include "include/enum.hpp"
#include "include/forward_list.hpp"
#include "include/fundamental.hpp"
#include "include/compacted_storage_size.hpp"
#include "include/header.hpp"
#include "include/list.hpp"
#include "include/map.hpp"
#include "include/multimap.hpp"
#include "include/multiset.hpp"
#include "include/optional.hpp"
#include "include/variant.hpp"
#include "include/pair.hpp"
#include "include/deque.hpp"
#include "include/std_streams.hpp"
#include "include/serialize.hpp"
#include "include/set.hpp"
#include "include/string.hpp"
#include "include/string_view.hpp"
#include "include/tuple.hpp"
#include "include/u16string.hpp"
#include "include/unordered_map.hpp"
#include "include/unordered_multimap.hpp"
#include "include/unordered_multiset.hpp"
#include "include/unordered_set.hpp"
#include "include/vector.hpp"
#include "include/version.hpp"
#include "include/wstring.hpp"
#include "include/one_func.hpp"
#include "include/one_memfn.hpp"
#include "include/split_func.hpp"
#include "include/split_memfn.hpp"
#include "include/serialization.hpp"
#include "include/yas_object.hpp"
#include "include/json_conformance.hpp"
#include "include/wrap_asis.hpp"
#include "include/wrap_init.hpp"

#if defined(YAS_SERIALIZE_BOOST_TYPES)
#include "include/boost_fusion_list.hpp"
#include "include/boost_fusion_map.hpp"
#include "include/boost_fusion_pair.hpp"
#include "include/boost_fusion_set.hpp"
#include "include/boost_fusion_tuple.hpp"
#include "include/boost_fusion_vector.hpp"
#include "include/boost_cont_string.hpp"
#include "include/boost_cont_wstring.hpp"
#include "include/boost_cont_vector.hpp"
#include "include/boost_cont_static_vector.hpp"
#include "include/boost_cont_stable_vector.hpp"
#include "include/boost_cont_list.hpp"
#include "include/boost_cont_slist.hpp"
#include "include/boost_cont_map.hpp"
#include "include/boost_cont_multimap.hpp"
#include "include/boost_cont_set.hpp"
#include "include/boost_cont_multiset.hpp"
#include "include/boost_cont_flat_map.hpp"
#include "include/boost_cont_flat_multimap.hpp"
#include "include/boost_cont_flat_set.hpp"
#include "include/boost_cont_flat_multiset.hpp"
#include "include/boost_cont_deque.hpp"
#include "include/boost_tuple.hpp"
#include "include/boost_variant.hpp"
#endif // defined(YAS_SERIALIZE_BOOST_TYPES)

/***************************************************************************/

#ifdef YAS_SERIALIZE_ABSL_TYPES
#include "include/absl_cont_btree_map.hpp"
#include "include/absl_cont_flat_hash_map.hpp"
#endif // YAS_SERIALIZE_ABSL_TYPES

/***************************************************************************/

#include <iostream>
#include <fstream>
#include <functional>
#include <map>

#include <cassert>
#include <cstdio>

/***************************************************************************/

// mem archives traits
template<typename OA, typename IA>
struct archive_traits {
    typedef OA oarchive_type;
    typedef IA iarchive_type;

    /** output archive */
    struct oarchive {
        oarchive()
            :oa{nullptr}
        {}

        virtual ~oarchive()
        { delete oa; }

        oarchive_type* operator->() { return oa; }

        template<typename T>
        oarchive_type& operator& (const T &v) { return (*(oa) & v); }

        oarchive_type& serialize() { return *oa; }

        template<typename Head, typename... Tail>
        oarchive_type& serialize(const Head &head, const Tail&... tail) {
            return oa->operator&(head).serialize(tail...);
        }

        template<typename... Ts>
        oarchive_type& operator()(const Ts&... ts) {
            return oa->serialize(ts...);
        }
        template<typename... Ts>
        oarchive_type& save(const Ts&... ts) {
            return oa->save(ts...);
        }

        static constexpr bool is_little_endian() { return oarchive_type::is_little_endian(); }
        static constexpr bool is_big_endian() { return oarchive_type::is_big_endian(); }
        static constexpr yas::options host_endian() { return oarchive_type::host_endian(); }
        std::size_t size() const { return stream.get_intrusive_buffer().size; }
        yas::intrusive_buffer get_intrusive_buffer() const { return stream.get_intrusive_buffer(); }
        yas::shared_buffer get_shared_buffer() const { return  yas::shared_buffer(stream.get_intrusive_buffer().data, stream.get_intrusive_buffer().size); }

        void dump(std::ostream &os = std::cout) {
            const yas::intrusive_buffer buf = stream.get_intrusive_buffer();
            os << yas::hexdump(buf.data, buf.size) << std::endl;
        }
        void print(std::ostream &os = std::cout) {
            const yas::intrusive_buffer buf = stream.get_intrusive_buffer();
            os.write(buf.data, buf.size);
            os << std::endl;
        }

        bool compare(const void *ptr, std::uint32_t size) const {
            const yas::intrusive_buffer buf = stream.get_intrusive_buffer();
//			std::cout << "exp=" << std::endl << yas::hex_dump(ptr, size) << std::endl;
//			std::cout << "buf=" << std::endl << yas::hex_dump(buf.data, buf.size) << std::endl;
            return size == buf.size ? (0 == std::memcmp(buf.data, ptr, size)) : false;
        }

        typename oarchive_type::stream_type stream;
        oarchive_type *oa;
    };

    static void ocreate(oarchive &oa, const char *archive_type) {
        ((void) archive_type);
        oa.oa = new oarchive_type(oa.stream);
    }

    /** input archive */
    struct iarchive {
        iarchive()
            :stream{}
            ,ia{}
        {}

        virtual ~iarchive()
        { delete ia; delete stream; }

        iarchive_type* operator->() { return ia; }

        template<typename T>
        iarchive_type& operator&(T &&v) { return (*(ia) & std::forward<T>(v)); }

        iarchive_type& serialize() { return *ia; }

        template<typename Head, typename... Tail>
        iarchive_type& serialize(Head &&head, Tail &&... tail) {
            return ia->operator&(std::forward<Head>(head)).serialize(std::forward<Tail>(tail)...);
        }

        template<typename... Ts>
        iarchive_type& operator()(Ts &&... ts) {
            return ia->operator()(std::forward<Ts>(ts)...);
        }
        template<typename... Ts>
        iarchive_type& load(Ts &&... ts) {
            return ia->load(std::forward<Ts>(ts)...);
        }

        bool is_little_endian() { return ia->is_little_endian(); }
        bool is_big_endian() { return ia->is_big_endian(); }
        static constexpr yas::options host_endian() { return iarchive_type::host_endian(); }
        std::size_t size() const { return stream->get_intrusive_buffer().size; }
        yas::intrusive_buffer get_intrusive_buffer() const { return stream->get_intrusive_buffer(); }

        void dump(std::ostream &os = std::cout) {
            const yas::intrusive_buffer buf = stream->get_intrusive_buffer();
            os << yas::hexdump(buf.data, buf.size) << std::endl;
        }
        void print(std::ostream &os = std::cout) {
            const yas::intrusive_buffer buf = stream->get_intrusive_buffer();
            os.write(buf.data, buf.size);
            os << std::endl;
        }

        bool compare(const void *ptr, std::uint32_t size) const {
            const yas::intrusive_buffer buf = stream->get_intrusive_buffer();
//			std::cout << "exp=" << std::endl << yas::hex_dump(ptr, size) << std::endl;
//			std::cout << "buf=" << std::endl << yas::hex_dump(buf.data, buf.size) << std::endl;
            return size == buf.size ? (0 == std::memcmp(buf.data, ptr, size)) : false;
        }

        typename iarchive_type::stream_type *stream;
        iarchive_type *ia;
    };

    static void icreate(iarchive &ia, oarchive &oa, const char *archive_type) {
        ((void) archive_type);
        ia.stream = new typename iarchive_type::stream_type(oa.stream.get_intrusive_buffer());
        ia.ia = new iarchive_type(*(ia.stream));
    }
};

#define YAS_RUN_TEST_SKIP_TEST(...) \
    !(OA::type() & (__VA_ARGS__))
#define YAS_RUN_TEST_RUN_TEST(...) \
    1

#define YAS_RUN_TEST_CHECK_FOR_SKIP(...) \
    YAS_PP_IF( \
        __YAS_PP_TUPLE_IS_EMPTY(__VA_ARGS__) \
        ,YAS_RUN_TEST_RUN_TEST \
        ,YAS_RUN_TEST_SKIP_TEST \
    )(__VA_ARGS__)

#define YAS_RUN_TEST(log, testname, passcnt, failcnt, .../*skip for*/) { \
    if ( YAS_RUN_TEST_CHECK_FOR_SKIP(__VA_ARGS__) ) { \
        const char *artype = ( \
            yas::is_binary_archive<OA>::value ? "binary" \
                : yas::is_text_archive<OA>::value ? "text" \
                    : yas::is_json_archive<OA>::value ? "json" \
                        : "unknown" \
        ); \
        std::fprintf(stdout,"%-6s: %-24s -> ", artype, #testname); std::fflush(stdout); \
        \
        const bool passed = testname##_test<archive_traits<OA, IA>>(log, artype, #testname); \
        if ( passed ) { ++passcnt; } else { ++failcnt; } \
        std::fprintf(stdout, "%s\n", (passed ? "passed" : "failed")); std::fflush(stdout); \
    } \
}

template<typename OA, typename IA>
void tests(std::ostream &log, int &p, int &e) {
    YAS_RUN_TEST(log, header, p, e);
    YAS_RUN_TEST(log, endian, p, e);
    YAS_RUN_TEST(log, version, p, e);
    YAS_RUN_TEST(log, base64, p, e);
    YAS_RUN_TEST(log, fundamental, p, e);
    YAS_RUN_TEST(log, compacted_storage_size, p, e);
    YAS_RUN_TEST(log, enum, p, e);
    YAS_RUN_TEST(log, auto_array, p, e);
    YAS_RUN_TEST(log, std_streams, p, e);
    YAS_RUN_TEST(log, one_function, p, e);
    YAS_RUN_TEST(log, split_functions, p, e);
    YAS_RUN_TEST(log, one_method, p, e);
    YAS_RUN_TEST(log, split_methods, p, e);
    YAS_RUN_TEST(log, serialize, p, e);
    YAS_RUN_TEST(log, serialization, p, e);
    YAS_RUN_TEST(log, yas_object, p, e);
    YAS_RUN_TEST(log, base_object, p, e);
    YAS_RUN_TEST(log, array, p, e);
    YAS_RUN_TEST(log, bitset, p, e);
    YAS_RUN_TEST(log, buffer, p, e);
    YAS_RUN_TEST(log, chrono, p, e)
    YAS_RUN_TEST(log, complex, p, e);
    YAS_RUN_TEST(log, string, p, e);
    YAS_RUN_TEST(log, string_view, p, e);
    YAS_RUN_TEST(log, wstring, p, e);
    YAS_RUN_TEST(log, pair, p, e);
    YAS_RUN_TEST(log, tuple, p, e);
    YAS_RUN_TEST(log, u16string, p, e);
    YAS_RUN_TEST(log, vector, p, e);
    YAS_RUN_TEST(log, list, p, e);
    YAS_RUN_TEST(log, forward_list, p, e);
    YAS_RUN_TEST(log, deque, p, e);
    YAS_RUN_TEST(log, map, p, e);
    YAS_RUN_TEST(log, set, p, e);
    YAS_RUN_TEST(log, multimap, p, e);
    YAS_RUN_TEST(log, multiset, p, e);
    YAS_RUN_TEST(log, unordered_map, p, e);
    YAS_RUN_TEST(log, unordered_set, p, e);
    YAS_RUN_TEST(log, unordered_multimap, p, e);
    YAS_RUN_TEST(log, unordered_multiset, p, e);
    YAS_RUN_TEST(log, optional, p, e);
    YAS_RUN_TEST(log, variant, p, e);
    YAS_RUN_TEST(log, wrap_asis, p, e);
    YAS_RUN_TEST(log, wrap_init, p, e);
#if defined(YAS_SERIALIZE_BOOST_TYPES)
    YAS_RUN_TEST(log, boost_fusion_pair, p, e);
    YAS_RUN_TEST(log, boost_fusion_tuple, p, e);
    YAS_RUN_TEST(log, boost_fusion_vector, p, e);
    YAS_RUN_TEST(log, boost_fusion_list, p, e);
    YAS_RUN_TEST(log, boost_fusion_set, p, e);
    YAS_RUN_TEST(log, boost_fusion_map, p, e);
    YAS_RUN_TEST(log, boost_cont_string, p, e);
    YAS_RUN_TEST(log, boost_cont_wstring, p, e);
    YAS_RUN_TEST(log, boost_cont_vector, p, e);
    YAS_RUN_TEST(log, boost_cont_static_vector, p, e);
    YAS_RUN_TEST(log, boost_cont_stable_vector, p, e);
    YAS_RUN_TEST(log, boost_cont_list, p, e);
    YAS_RUN_TEST(log, boost_cont_slist, p, e);
    YAS_RUN_TEST(log, boost_cont_map, p, e);
    YAS_RUN_TEST(log, boost_cont_multimap, p, e);
    YAS_RUN_TEST(log, boost_cont_set, p, e);
    YAS_RUN_TEST(log, boost_cont_multiset, p, e);
    YAS_RUN_TEST(log, boost_cont_flat_map, p, e);
    YAS_RUN_TEST(log, boost_cont_flat_multimap, p, e);
    YAS_RUN_TEST(log, boost_cont_flat_set, p, e);
    YAS_RUN_TEST(log, boost_cont_flat_multiset, p, e);
    YAS_RUN_TEST(log, boost_cont_deque, p, e);
    YAS_RUN_TEST(log, boost_tuple, p, e);
    YAS_RUN_TEST(log, boost_variant, p, e);
#endif // YAS_SERIALIZE_BOOST_TYPES
#ifdef YAS_SERIALIZE_ABSL_TYPES
    YAS_RUN_TEST(log, absl_cont_btree_map, p, e);
    YAS_RUN_TEST(log, absl_cont_flat_hash_map, p, e);
#endif
    YAS_RUN_TEST(log, json_conformance, p, e, yas::binary|yas::text);
}

/***************************************************************************/

struct options {
    explicit options(char **argv)
        :msg{}
        ,binary{false}
        ,text{false}
        ,json{false}
        ,endian_big{false}
        ,endian_little{false}
        ,compacted{false}
        ,logn{0}
    {
        for ( char **arg = argv+1; *arg; ++arg ) {
            static const std::map<std::string, std::function<void()>> map = {
                 {"binary"    , [this]{binary=true;       }}
                ,{"text"      , [this]{text=true;         }}
                ,{"json"      , [this]{json=true;         }}
                ,{"ebig"      , [this]{endian_big=true;   }}
                ,{"elittle"   , [this]{endian_little=true;}}
                ,{"compacted" , [this]{compacted=true;    }}
                ,{"log=stdout", [this]{logn=log_stdout;   }}
                ,{"log=stderr", [this]{logn=log_stderr;   }}
                ,{"log=file"  , [this]{logn=log_file;     }}
            };

            auto it = map.find(*arg);
            if ( it == map.end() ) {
                msg = "bad option: ";
                msg += *arg;

                return;
            }

            it->second();
        }

        int artype = binary+text+json;
        if ( artype == 0 || artype > 1 ) {
            msg = "one of binary/text/json should be specified. terminate.";

            return;
        }

        if ( binary ) {
            if ( !endian_big && !endian_little ) {
                endian_big = __YAS_BIG_ENDIAN;
                endian_little = __YAS_LITTLE_ENDIAN;
            }
            if ( endian_big && endian_little ) {
                msg = "only one of big/little can be specified. terminate.";

                return;
            }
        }

        if ( !binary && !json ) {
            if ( compacted ) {
                msg = R"("compacted" can be specified only for "binary" and "json" archives. terminate.)";

                return;
            }

            if ( endian_big || endian_little ) {
                msg = R"("ebig" or "elittle" can be specified only for "binary" archives. terminate.)";

                return;
            }
        }
    }

    std::string msg;
    bool binary;
    bool text;
    bool json;
    bool endian_big;
    bool endian_little;
    bool compacted;

    enum { log_stdout, log_stderr, log_file };
    int logn;
};

/***************************************************************************/

int main(int, char **argv) {
    options opts(argv);
    if ( !opts.msg.empty() ) {
        std::cerr << "command line parse error: " << opts.msg << std::endl;

        return EXIT_FAILURE;
    }

    std::cout << std::endl
    << "/***************************************************/" << std::endl
    << "cmdline: ";
    for ( char **arg = argv+1; *arg; ++arg ) {
        std::cout << *arg << ' ';
    }
    std::cout << std::endl << std::endl;

    std::ofstream logfile;
    if ( opts.logn == options::log_file ) {
        const char *fname = std::strrchr(argv[0], '/');
        assert(fname != nullptr);

        logfile.open(fname+1);
        assert(logfile.good());
    }

    auto &log = __YAS_SCAST(std::ostream &, (
        opts.logn == options::log_stdout
            ? std::cout
            : opts.logn == options::log_stderr
                ? std::cerr
                : logfile
    ));

    int passed = 0, failed = 0;
    try {
        if ( opts.binary ) {
            if ( opts.endian_little ) {
                if ( opts.compacted ) {
                    constexpr std::size_t binary_opts = yas::binary|yas::elittle|yas::compacted;
                    tests<
                         yas::binary_oarchive<yas::mem_ostream, binary_opts>
                        ,yas::binary_iarchive<yas::mem_istream, binary_opts>
                    >(log, passed, failed);
                    
                    tests<
                         yas::binary_oarchive<yas::vector_ostream<char>, binary_opts>
                        ,yas::binary_iarchive<yas::mem_istream, binary_opts>
                    >(log, passed, failed);
                    
                    tests<
                         yas::binary_oarchive<yas::vector_ostream<int8_t>, binary_opts>
                        ,yas::binary_iarchive<yas::mem_istream, binary_opts>
                    >(log, passed, failed);
                    
                    tests<
                         yas::binary_oarchive<yas::vector_ostream<uint8_t>, binary_opts>
                        ,yas::binary_iarchive<yas::mem_istream, binary_opts>
                    >(log, passed, failed);
                    
                } else {
                    constexpr std::size_t binary_opts = yas::binary|yas::elittle;
                    tests<
                         yas::binary_oarchive<yas::mem_ostream, binary_opts>
                        ,yas::binary_iarchive<yas::mem_istream, binary_opts>
                    >(log, passed, failed);
                    
                    tests<
                         yas::binary_oarchive<yas::vector_ostream<char>, binary_opts>
                        ,yas::binary_iarchive<yas::mem_istream, binary_opts>
                    >(log, passed, failed);
                    
                    tests<
                         yas::binary_oarchive<yas::vector_ostream<int8_t>, binary_opts>
                        ,yas::binary_iarchive<yas::mem_istream, binary_opts>
                    >(log, passed, failed);
                    
                    tests<
                         yas::binary_oarchive<yas::vector_ostream<uint8_t>, binary_opts>
                        ,yas::binary_iarchive<yas::mem_istream, binary_opts>
                    >(log, passed, failed);
                }
            } else {
                if ( opts.compacted ) {
                    constexpr std::size_t binary_opts = yas::binary|yas::ebig|yas::compacted;
                    tests<
                         yas::binary_oarchive<yas::mem_ostream, binary_opts>
                        ,yas::binary_iarchive<yas::mem_istream, binary_opts>
                    >(log, passed, failed);
                    
                    tests<
                         yas::binary_oarchive<yas::vector_ostream<char>, binary_opts>
                        ,yas::binary_iarchive<yas::mem_istream, binary_opts>
                    >(log, passed, failed);
                    
                    tests<
                         yas::binary_oarchive<yas::vector_ostream<int8_t>, binary_opts>
                        ,yas::binary_iarchive<yas::mem_istream, binary_opts>
                    >(log, passed, failed);
                    
                    tests<
                         yas::binary_oarchive<yas::vector_ostream<uint8_t>, binary_opts>
                        ,yas::binary_iarchive<yas::mem_istream, binary_opts>
                    >(log, passed, failed);
                    
                } else {
                    constexpr std::size_t binary_opts = yas::binary|yas::ebig;
                    tests<
                         yas::binary_oarchive<yas::mem_ostream, binary_opts>
                        ,yas::binary_iarchive<yas::mem_istream, binary_opts>
                    >(log, passed, failed);
                    
                    tests<
                         yas::binary_oarchive<yas::vector_ostream<char>, binary_opts>
                        ,yas::binary_iarchive<yas::mem_istream, binary_opts>
                    >(log, passed, failed);
                    
                    tests<
                         yas::binary_oarchive<yas::vector_ostream<int8_t>, binary_opts>
                        ,yas::binary_iarchive<yas::mem_istream, binary_opts>
                    >(log, passed, failed);
                    
                    tests<
                         yas::binary_oarchive<yas::vector_ostream<uint8_t>, binary_opts>
                        ,yas::binary_iarchive<yas::mem_istream, binary_opts>
                    >(log, passed, failed);
                }
            }
        }

        if ( opts.text ) {
            constexpr std::size_t text_opts = yas::text|yas::ehost;
            tests<
                 yas::text_oarchive<yas::mem_ostream, text_opts>
                ,yas::text_iarchive<yas::mem_istream, text_opts>
            >(log, passed, failed);
            
            tests<
                 yas::text_oarchive<yas::vector_ostream<char>, text_opts>
                ,yas::text_iarchive<yas::mem_istream, text_opts>
            >(log, passed, failed);

            tests<
                 yas::text_oarchive<yas::vector_ostream<int8_t>, text_opts>
                ,yas::text_iarchive<yas::mem_istream, text_opts>
            >(log, passed, failed);

            tests<
                 yas::text_oarchive<yas::vector_ostream<uint8_t>, text_opts>
                ,yas::text_iarchive<yas::mem_istream, text_opts>
            >(log, passed, failed);
        }

        if ( opts.json ) {
            if ( opts.compacted ) {
                constexpr std::size_t json_opts = yas::json|yas::ehost|yas::compacted;
                tests<
                     yas::json_oarchive<yas::mem_ostream, json_opts>
                    ,yas::json_iarchive<yas::mem_istream, json_opts>
                >(log, passed, failed);
                
                tests<
                     yas::json_oarchive<yas::vector_ostream<char>, json_opts>
                    ,yas::json_iarchive<yas::mem_istream, json_opts>
                >(log, passed, failed);

                tests<
                     yas::json_oarchive<yas::vector_ostream<int8_t>, json_opts>
                    ,yas::json_iarchive<yas::mem_istream, json_opts>
                >(log, passed, failed);

                tests<
                     yas::json_oarchive<yas::vector_ostream<uint8_t>, json_opts>
                    ,yas::json_iarchive<yas::mem_istream, json_opts>
                >(log, passed, failed);
            
            } else {
                constexpr std::size_t json_opts = yas::json|yas::ehost;
                tests<
                     yas::json_oarchive<yas::mem_ostream, json_opts>
                    ,yas::json_iarchive<yas::mem_istream, json_opts>
                >(log, passed, failed);
                
                tests<
                     yas::json_oarchive<yas::vector_ostream<char>, json_opts>
                    ,yas::json_iarchive<yas::mem_istream, json_opts>
                >(log, passed, failed);

                tests<
                     yas::json_oarchive<yas::vector_ostream<int8_t>, json_opts>
                    ,yas::json_iarchive<yas::mem_istream, json_opts>
                >(log, passed, failed);

                tests<
                     yas::json_oarchive<yas::vector_ostream<uint8_t>, json_opts>
                    ,yas::json_iarchive<yas::mem_istream, json_opts>
                >(log, passed, failed);
            }
        }
    } catch (const std::exception &ex) {
        std::cout << "[std::exception]: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    } catch (...) {
        std::cout << "[unknown exception]: " << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << std::endl
    << "/***************************************************/" << std::endl
    << "> passed tests: " << passed << std::endl
    << "> failed tests: " << failed << std::endl
    << "> host endian : " << (__YAS_LITTLE_ENDIAN ? "little" : "big") << std::endl
    << "> host bits   : " << sizeof(void *)*8 << std::endl
    << "> YAS version : " << YAS_VERSION_STRING << std::endl
    << "/***************************************************/"
    << std::endl;

    return failed;
}

/***************************************************************************/
