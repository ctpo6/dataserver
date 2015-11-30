// page_info.h
//
#ifndef __SDL_SYSTEM_PAGE_INFO_H__
#define __SDL_SYSTEM_PAGE_INFO_H__

#pragma once

#include "page_head.h"
#include <sstream>

namespace sdl { namespace db {

/*template<class TList, class T>
struct is_found {
    enum { value = TL::IndexOf<TList, T>::value != -1 };
	typedef std::integral_constant<bool, value> type;
};*/

struct to_string {

    to_string() = delete;

    static const char * type(pageType);    
    static std::string type(uint8);
    static std::string type(guid_t const &);
    static std::string type(pageLSN const &);
    static std::string type(pageFileID const &);
    static std::string type(pageXdesID const &);
    static std::string type(nchar_t const * buf, size_t buf_size);

    template<size_t buf_size>
    static std::string type(nchar_t const(&buf)[buf_size]) {
        return type(buf, buf_size);
    }

    static std::string type_raw(char const * buf, size_t buf_size);

    template<size_t buf_size>
    static std::string type_raw(char const(&buf)[buf_size]) {
        return type_raw(buf, buf_size);
    }

    template <class T>
    static std::string type(T const & value) {
        std::stringstream ss;
        ss << value;
        return ss.str();
    }
};

struct page_info {
    page_info() = delete;
    static std::string type(page_head const &);
    static std::string type_meta(page_head const &);
    static std::string type_raw(page_head const &);
};

namespace impl {

    template <class type_list> struct processor;

    template <> struct processor<NullType>
    {
        template<class stream_type, class data_type>
        static void print(stream_type &, data_type const * const){}
    };

    template <class T, class U> // T = meta::col_type
    struct processor< Typelist<T, U> >
    {
        template<class stream_type, class data_type>
        static void print(stream_type & ss, data_type const * const data)
        {
            typedef typename T::type value_type;
            char const * p = reinterpret_cast<char const *>(data);
            p += T::offset;
            value_type const & value = *reinterpret_cast<value_type const *>(p);
            ss << "0x" << std::uppercase << std::hex << T::offset << ": " << std::dec;
            ss << to_string::type(value);
            ss << std::endl;
            processor<U>::print(ss, data);
        }
    };

} // impl
} // db
} // sdl

#endif // __SDL_SYSTEM_PAGE_INFO_H__