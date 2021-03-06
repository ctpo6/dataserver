// sysidxstats.cpp
//
#include "common/common.h"
#include "sysidxstats.h"
#include "system/page_info.h"

namespace sdl { namespace db {

static_col_name(sysidxstats_row_meta, head);
static_col_name(sysidxstats_row_meta, id);
static_col_name(sysidxstats_row_meta, indid);
static_col_name(sysidxstats_row_meta, status);
static_col_name(sysidxstats_row_meta, intprop);
static_col_name(sysidxstats_row_meta, fillfact);
static_col_name(sysidxstats_row_meta, type);
static_col_name(sysidxstats_row_meta, tinyprop);
static_col_name(sysidxstats_row_meta, dataspace);
static_col_name(sysidxstats_row_meta, lobds);
static_col_name(sysidxstats_row_meta, rowset);
static_col_name(sysidxstats_row_meta, name);

std::string sysidxstats_row_info::type_meta(sysidxstats_row const & row)
{
    return processor_row::type_meta(row);
}

std::string sysidxstats_row_info::type_raw(sysidxstats_row const & row)
{
    return to_string::type_raw(row.raw);
}

std::string sysidxstats_row_info::col_name(sysidxstats_row const & row)
{
    return to_string::type_nchar(row.data.head, sysidxstats_row_meta::name::offset);
}

} // db
} // sdl

#if SDL_DEBUG
namespace sdl {
    namespace db {
        namespace {
            class unit_test {
            public:
                unit_test()
                {
                    SDL_TRACE_FILE;
                    A_STATIC_ASSERT_IS_POD(sysidxstats_row);
                    static_assert(sizeof(sysidxstats_row) == 39, "");
                }
            };
            static unit_test s_test;
        }
    } // db
} // sdl
#endif //#if SV_DEBUG