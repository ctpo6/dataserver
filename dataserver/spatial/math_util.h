// math_util.h
//
#pragma once
#ifndef __SDL_SYSTEM_MATH_UTIL_H__
#define __SDL_SYSTEM_MATH_UTIL_H__

#include "spatial_type.h"

namespace sdl { namespace db {

struct math_util : is_static {

    static bool get_line_intersect(point_2D &,
        point_2D const & a, point_2D const & b,  // line1 (a,b)
        point_2D const & c, point_2D const & d); // line2 (c,d)

    static bool line_intersect(
        point_2D const & a, point_2D const & b,  // line1 (a,b)
        point_2D const & c, point_2D const & d); // line2 (c,d)

    static bool line_rect_intersect(point_2D const & a, point_2D const & b, rect_2D const & rc);
    enum class contains_t {
        none,
        intersect,
        rect_inside,
        poly_inside
    };
    static contains_t contains(vector_point_2D const &, rect_2D const &);
    static bool point_inside(point_2D const & p, rect_2D const & rc);    
};

inline bool math_util::point_inside(point_2D const & p, rect_2D const & rc) {
    SDL_ASSERT(!(rc.rb < rc.lt));    
    return (p.X >= rc.lt.X) && (p.X <= rc.rb.X) &&
           (p.Y >= rc.lt.Y) && (p.Y <= rc.rb.Y);
}

using pair_size_t = std::pair<size_t, size_t>;

template<class iterator, class fun_type>
pair_size_t find_range(iterator first, iterator last, fun_type less) {
    SDL_ASSERT(first != last);
    auto p1 = first;
    auto p2 = p1;
    auto it = first; ++it;
    for  (; it != last; ++it) {
        if (less(*it, *p1)) {
            p1 = it;
        }
        else if (less(*p2, *it)) {
            p2 = it;
        }
    }
    return { p1 - first, p2 - first };
}

} // db
} // sdl

#endif // __SDL_SYSTEM_MATH_UTIL_H__