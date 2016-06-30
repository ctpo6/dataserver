// geography.h
//
#pragma once
#ifndef __SDL_SYSTEM_GEOGRAPHY_H__
#define __SDL_SYSTEM_GEOGRAPHY_H__

#include "page_head.h"
#include "spatial_type.h"

namespace sdl { namespace db {

#pragma pack(push, 1) 

struct geo_data_meta;
struct geo_data_info;

struct geo_data { // 6 bytes

    using meta = geo_data_meta;
    using info = geo_data_info;

    struct data_type {
        uint32  SRID;       // 0x00 : 4 bytes // E6100000 = 4326 (WGS84 � SRID 4326)
        uint16  tag;        // 0x04 : 2 bytes // = TYPEID
    };
    union {
        data_type data;
        char raw[sizeof(data_type)];
    };
    static spatial_type get_type(vector_mem_range_t const &);
};

//------------------------------------------------------------------------

struct geo_point_meta;
struct geo_point_info;

struct geo_point { // 22 bytes
    
    using meta = geo_point_meta;
    using info = geo_point_info;

    static const uint16 TYPEID = (uint16)spatial_type::point; // 3073

    struct data_type {
        uint32  SRID;           // 0x00 : 4 bytes // E6100000 = 4326 (WGS84 � SRID 4326)
        uint16  tag;            // 0x04 : 2 bytes // = TYPEID
        spatial_point point;    // 0x06 : 16 bytes
    };
    union {
        data_type data;
        char raw[sizeof(data_type)];
    };
};

//------------------------------------------------------------------------

struct geo_multipolygon_meta;
struct geo_multipolygon_info;

struct geo_multipolygon { // = 26 bytes

    using meta = geo_multipolygon_meta;
    using info = geo_multipolygon_info;

    static const uint16 TYPEID = (uint16)spatial_type::multipolygon; // 1025

    struct data_type {
        uint32  SRID;               // 0x00 : 4 bytes // E6100000 = 4326 (WGS84 � SRID 4326)
        uint16  tag;                // 0x04 : 2 bytes // = TYPEID
        uint32  num_point;          // 0x06 : 4 bytes // EC010000 = 0x01EC = 492 = POINTS COUNT
        spatial_point points[1];    // 0x0A : 16 bytes * point_num
    };
    union {
        data_type data;
        char raw[sizeof(data_type)];
    };
    size_t size() const { 
        return data.num_point;
    } 
    spatial_point const & operator[](size_t i) const {
        SDL_ASSERT(i < this->size());
        return data.points[i];
    }
    spatial_point const * begin() const {
        return data.points;
    }
    spatial_point const * end() const {
        return data.points + this->size();
    }
    size_t mem_size() const {
        return sizeof(data_type)-sizeof(spatial_point)+sizeof(spatial_point)*size();
    }
    size_t ring_num() const;

    template<class fun_type>
    void for_ring(fun_type fun) const;
};

template<class fun_type>
void geo_multipolygon::for_ring(fun_type fun) const
{
    SDL_ASSERT(size() != 1);
    size_t count = 0;
    auto const _end = this->end();
    auto p1 = this->begin();
    auto p2 = p1 + 1;
    while (p2 < _end) {
        if (*p1 == *p2) {
            ++count;
            ++p2;
            fun(p1, p2);
            p1 = p2;
        }
        ++p2;
    }
}

//------------------------------------------------------------------------

struct geo_linestring_meta;
struct geo_linestring_info;

struct geo_linestring { // = 38 bytes, linesegment

    using meta = geo_linestring_meta;
    using info = geo_linestring_info;

    static const uint16 TYPEID = (uint16)spatial_type::linestring; // 5121

    struct data_type {
        uint32  SRID;               // 0x00 : 4 bytes // E6100000 = 4326 (WGS84 � SRID 4326)
        uint16  tag;                // 0x04 : 2 bytes = TYPEID
        spatial_point first;        // 0x06 : 16 bytes
        spatial_point second;       // 0x16 : 16 bytes
    };
    union {
        data_type data;
        char raw[sizeof(data_type)];
    };
    static size_t size() { 
        return 2;
    } 
    spatial_point const & operator[](size_t i) const {
        SDL_ASSERT(i < size());
        return (0 == i) ? data.first : data.second;
    }
};

#pragma pack(pop)

//------------------------------------------------------------------------

struct geo_data_meta: is_static {

    typedef_col_type_n(geo_data, SRID);
    typedef_col_type_n(geo_data, tag);

    typedef TL::Seq<
        SRID
        ,tag
    >::Type type_list;
};

struct geo_data_info: is_static {
    static std::string type_meta(geo_data const &);
    static std::string type_raw(geo_data const &);
};

//------------------------------------------------------------------------

struct geo_point_meta: is_static {

    typedef_col_type_n(geo_point, SRID);
    typedef_col_type_n(geo_point, tag);
    typedef_col_type_n(geo_point, point);

    typedef TL::Seq<
        SRID
        ,tag
        , point
    >::Type type_list;
};

struct geo_point_info: is_static {
    static std::string type_meta(geo_point const &);
    static std::string type_raw(geo_point const &);
};

//------------------------------------------------------------------------

struct geo_multipolygon_meta: is_static {

    typedef_col_type_n(geo_multipolygon, SRID);
    typedef_col_type_n(geo_multipolygon, tag);
    typedef_col_type_n(geo_multipolygon, num_point);

    typedef TL::Seq<
        SRID
        ,tag
        ,num_point
    >::Type type_list;
};

struct geo_multipolygon_info: is_static {
    static std::string type_meta(geo_multipolygon const &);
    static std::string type_raw(geo_multipolygon const &);
};

//------------------------------------------------------------------------

struct geo_linestring_meta: is_static {

    typedef_col_type_n(geo_linestring, SRID);
    typedef_col_type_n(geo_linestring, tag);
    typedef_col_type_n(geo_linestring, first);
    typedef_col_type_n(geo_linestring, second);

    typedef TL::Seq<
        SRID
        ,tag
        ,first
        ,second
    >::Type type_list;
};

struct geo_linestring_info: is_static {
    static std::string type_meta(geo_linestring const &);
    static std::string type_raw(geo_linestring const &);
};

//------------------------------------------------------------------------

using geography_t = vector_mem_range_t; //FIXME: support for STContains()

} // db
} // sdl

#endif // __SDL_SYSTEM_GEOGRAPHY_H__