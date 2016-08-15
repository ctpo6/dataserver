// transform.h
//
#pragma once
#ifndef __SDL_SYSTEM_TRANSFORM_H__
#define __SDL_SYSTEM_TRANSFORM_H__

#include "spatial_type.h"
#include "interval_cell.h"

namespace sdl { namespace db {

struct transform : is_static {
    using grid_size = spatial_grid::grid_size;
    static spatial_cell make_cell(spatial_point const &, spatial_grid const = {});
    static spatial_point spatial(point_2D const &);
    static spatial_point spatial(spatial_cell const &, spatial_grid const = {});
    static point_XY<int> d2xy(spatial_cell::id_type, grid_size const = grid_size::HIGH); // hilbert::d2xy
    static point_2D cell_point(spatial_cell const &, spatial_grid const = {}); // returns point inside square 1x1
    static void cell_range(interval_cell &, spatial_point const &, Meters, spatial_grid const = {}); // cell_distance
    static void cell_rect(interval_cell &, spatial_rect const &, spatial_grid const = {});
};

inline spatial_point transform::spatial(spatial_cell const & cell, spatial_grid const grid) {
    return transform::spatial(transform::cell_point(cell, grid));
}

} // db
} // sdl

#endif // __SDL_SYSTEM_TRANSFORM_H__