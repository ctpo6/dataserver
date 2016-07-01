// geography_info.h
//
#pragma once
#ifndef __SDL_SYSTEM_GEOGRAPHY_INFO_H__
#define __SDL_SYSTEM_GEOGRAPHY_INFO_H__

#include "geography.h"

namespace sdl { namespace db {

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

    typedef_col_data_n(geo_point, data.head.SRID, SRID);
    typedef_col_data_n(geo_point, data.head.tag, tag);
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

    typedef_col_data_n(geo_multipolygon, data.head.SRID, SRID);
    typedef_col_data_n(geo_multipolygon, data.head.tag, tag);
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

    typedef_col_data_n(geo_linestring, data.head.SRID, SRID);
    typedef_col_data_n(geo_linestring, data.head.tag, tag);
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

} // db
} // sdl

#endif // __SDL_SYSTEM_GEOGRAPHY_INFO_H__