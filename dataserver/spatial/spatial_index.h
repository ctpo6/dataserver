// spatial_index.h
//
#pragma once
#ifndef __SDL_SYSTEM_SPATIAL_INDEX_H__
#define __SDL_SYSTEM_SPATIAL_INDEX_H__

#include "spatial_type.h"
#include "system/index_page.h"
#include "system/page_info.h"

namespace sdl { namespace db {

#pragma pack(push, 1) 

template<class T>
struct spatial_key_t { // composite key
    using pk0_type = T;
    spatial_cell    cell_id;        // 5 bytes
    pk0_type        pk0;            // 8 bytes for int64
};

/* cell_attr:
0 � cell at least touches the object (but not 1 or 2)
1 � guarantee that object partially covers cell
2 � object covers cell */
template<class _spatial_key>
struct spatial_page_row_t {
    
    using key_type = _spatial_key;
    using pk0_type = typename key_type::pk0_type;

    enum cell_attribute : uint16 {
        t_cell_touch    = 0,
        t_cell_part     = 1,
        t_cell_cover    = 2,
    };
    struct data_type { // Record Size = 23
        int32           _0x00;          // 0x00 : 4 bytes    // 10001700
        spatial_cell    cell_id;        // 0x04 : 5 bytes = 0x6098595504        // 60985adf 04          // Column 1 Offset 0x4 Length 5 Length (physical) 5
        pk0_type        pk0;            // 0x09 : 8 bytes = 2072064 = 0x1F9E00  // 009e1f 00000000 00   // Column 4 Offset 0x9 Length 8 Length (physical) 8
        uint16          cell_attr;      // 0x11 : 2 bytes = 1                   // 0100                 // Column 2 Offset 0x11 Length 2 Length (physical) 2
        uint32          SRID;           // 0x13 : 4 bytes = 4326 = 0x10E6       // e6 100000            // Column 3 Offset 0x13 Length 4 Length(physical) 4
    };
    union {
        data_type data;
        char raw[sizeof(data_type)];
    };
    key_type const & key() const {
        static_assert(sizeof(key_type) == sizeof(data.cell_id) + sizeof(data.pk0), "");
        return reinterpret_cast<key_type const &>(data.cell_id);
    }
    bool cell_cover() const {
        SDL_ASSERT(data.cell_attr <= t_cell_cover);
        return t_cell_cover == data.cell_attr;
    }
};

//---------------------------------------------------------------

namespace bigint { // hardcoded key type

using spatial_key = spatial_key_t<int64>;
using spatial_tree_row = index_page_row_t<spatial_key>;
using spatial_page_row = spatial_page_row_t<spatial_key>;

#pragma pack(pop)

//------------------------------------------------------------------------

struct spatial_tree_row_meta: is_static {

    typedef_col_type_n(spatial_tree_row, statusA);
    typedef_col_data_n(spatial_tree_row, data.key.cell_id, cell_id);
    typedef_col_data_n(spatial_tree_row, data.key.pk0, pk0);
    typedef_col_type_n(spatial_tree_row, page);

    typedef TL::Seq<
        statusA
        ,cell_id
        ,pk0
        ,page
    >::Type type_list;
};

struct spatial_tree_row_info: is_static {
    static std::string type_meta(spatial_tree_row const &);
    static std::string type_raw(spatial_tree_row const &);
};

//------------------------------------------------------------------------

struct spatial_page_row_meta: is_static {

    typedef_col_type_n(spatial_page_row, _0x00);
    typedef_col_type_n(spatial_page_row, cell_id);
    typedef_col_type_n(spatial_page_row, pk0);
    typedef_col_type_n(spatial_page_row, cell_attr);
    typedef_col_type_n(spatial_page_row, SRID);

    typedef TL::Seq<
        _0x00
        ,cell_id
        ,pk0
        ,cell_attr
        ,SRID
    >::Type type_list;
};

struct spatial_page_row_info: is_static {
    static std::string type_meta(spatial_page_row const &);
    static std::string type_raw(spatial_page_row const &);
};

//------------------------------------------------------------------------

} // bigint

template<class T>
inline bool operator < (spatial_key_t<T> const & x, spatial_key_t<T> const & y) {
    if (x.cell_id < y.cell_id) return true;
    if (y.cell_id < x.cell_id) return false;
    return x.pk0 < y.pk0;
}

template<> struct get_type_list<bigint::spatial_tree_row> : is_static {
    using type = bigint::spatial_tree_row_meta::type_list;
};

template<> struct get_type_list<bigint::spatial_page_row> : is_static {
    using type = bigint::spatial_page_row_meta::type_list;
};

} // db
} // sdl

#endif // __SDL_SYSTEM_SPATIAL_INDEX_H__