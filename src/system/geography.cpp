// geography.cpp
//
#include "common/common.h"
#include "geography.h"
#include "system/page_info.h"

namespace sdl { namespace db {

//------------------------------------------------------------------------

size_t geo_multipolygon::ring_num() const
{
    SDL_ASSERT(size() != 1);
    size_t count = 0;
    auto const _end = this->end();
    auto p1 = this->begin();
    auto p2 = p1 + 1;
    while (p2 < _end) {
        if (*p1 == *p2) {
            ++count;
            p1 = ++p2;
        }
        ++p2;
    }
    return count;
}

bool geo_multipolygon::STContains(spatial_point const &) const
{
    return false;
}

//------------------------------------------------------------------------

spatial_type geo_data::get_type(vector_mem_range_t const & data_col)
{
    static_assert(sizeof(geo_data) < sizeof(geo_point), "");
    static_assert(sizeof(geo_point) < sizeof(geo_multipolygon), "");
    static_assert(sizeof(geo_multipolygon) < sizeof(geo_linestring), "");

    const size_t data_col_size = mem_size(data_col);
    if (data_col_size < sizeof(geo_data)) {
        SDL_ASSERT(0);
        return spatial_type::null;
    }
    std::vector<char> buf;
    const geo_data * data = nullptr;
    if (data_col.size() == 1) {
        data = reinterpret_cast<geo_data const *>(data_col[0].first);
    }
    else {
        buf = make_vector_n(data_col, sizeof(geo_data));
        SDL_ASSERT(buf.size() == sizeof(geo_data));
        data = reinterpret_cast<geo_data const *>(buf.data());
    }
    if (data_col_size == sizeof(geo_point)) {
        if (data->data.tag == geo_point::TYPEID) {
            return spatial_type::point;
        }
    }
    else if (data_col_size >= sizeof(geo_multipolygon)) {
        if (data->data.tag == geo_multipolygon::TYPEID) {
            return spatial_type::multipolygon;
        }
        if (data_col_size >= sizeof(geo_linestring)) {
            if (data->data.tag == geo_linestring::TYPEID) {
                return spatial_type::linestring;
            }
        }
    }
    SDL_ASSERT(!"unknown geo_data");
    return spatial_type::null;
}

//------------------------------------------------------------------------

void geo_mem::swap(geo_mem & v) {
    m_data.swap(v.m_data);
    m_buf.swap(v.m_buf);
    std::swap(m_type, v.m_type);
    std::swap(m_geography, v.m_geography);
}

const char * geo_mem::geography() const {
    if (!m_geography) {
        SDL_ASSERT(!m_buf);
        if (m_data.size() == 1) {
            m_geography = m_data[0].first;
        }
        else {
            reset_new(m_buf, make_vector(m_data));
            m_geography = m_buf->data();
        }
    }
    return m_geography;
}

std::string geo_mem::STAsText() const {
    switch (m_type) {
    case spatial_type::point:
        return to_string::type(*cast_point());
    case spatial_type::multipolygon:
        return to_string::type(*cast_multipolygon());
    case spatial_type::linestring:
        return to_string::type(*cast_linestring());
    default:
        SDL_ASSERT(0);
        return{};
    }
}

bool geo_mem::STContains(spatial_point const & p) const {
    switch (m_type) {
    case spatial_type::point:
        return cast_point()->STContains(p);
    case spatial_type::multipolygon:
        return cast_multipolygon()->STContains(p);
    case spatial_type::linestring:
        return cast_linestring()->STContains(p);
    default:
        SDL_ASSERT(0);
        return false;
    }
}

//------------------------------------------------------------------------

} // db
} // sdl

#if SDL_DEBUG
namespace sdl { namespace db { namespace {
class unit_test {
public:
    unit_test()
    {
        A_STATIC_ASSERT_IS_POD(geo_head);
        A_STATIC_ASSERT_IS_POD(geo_data);
        A_STATIC_ASSERT_IS_POD(geo_point);
        A_STATIC_ASSERT_IS_POD(geo_multipolygon);
        A_STATIC_ASSERT_IS_POD(geo_linestring);

        static_assert(sizeof(geo_head) == 6, "");
        static_assert(sizeof(geo_data) == 6, "");
        static_assert(sizeof(geo_point) == 22, "");
        static_assert(sizeof(geo_multipolygon) == 26, "");
        static_assert(sizeof(geo_linestring) == 38, "");
        {
            geo_multipolygon test{};
            SDL_ASSERT(test.begin() == test.end());
            SDL_ASSERT(test.ring_num() == 0);
            SDL_ASSERT(test.mem_size() == sizeof(geo_multipolygon)-sizeof(spatial_point));
            test.data.num_point = 1;
            SDL_ASSERT(test.mem_size() == sizeof(geo_multipolygon));
        }
    }
};
static unit_test s_test;
}
} // db
} // sdl
#endif //#if SV_DEBUG
