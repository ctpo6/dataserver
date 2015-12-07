// sysidxstats.h
//
#ifndef __SDL_SYSTEM_SYSIDXSTATS_H__
#define __SDL_SYSTEM_SYSIDXSTATS_H__

#pragma once

#include "page_head.h"

namespace sdl { namespace db {

#pragma pack(push, 1) 

/*(ObjectID = 54)
The sysidxstats system table has one row for every index or heap 
in the database, including indexes on tables returned with table valued functions 
and user defined table types.  It is visible with through the DMV sys.indexes 
and sys.sysindexes.  This table is clustered by (id, indid).  
This table also includes automatically created statistics on columns 
that don't have indexes - these statistics have automatically generated names 
of the form _WA_Sys_[ObjectID-as 8 digit hex number]_[random 8 digit hex number] - 
the query optimizer uses them to select the best excution plan when 
a join includes unindexed columns that only have statistics.*/
struct sysidxstats_row
{
    struct data_type {

        datarow_head head; // 4 bytes

	    /*id (object_id) - 4 bytes - the object_id of the table or view that this index belongs to*/
	    uint32 id;

	    /*indid (index_id) - 4 bytes - the index_id (1 for the clustered index, 
	    larger numbers for non-clustered indexes)*/
	    uint32 indid;

	    /*status - 4 bytes - Note - this is NOT the same as the column sys.sysindexes.status.
	    0x10 = pad index turned on (is_padded).*/
	    uint32 status;

	    /*intprop - 4 bytes*/
	    uint32 intprop;

	    /*fillfact (fill_factor) - 1 byte - the fill factor for the index in percent (0-100), 
	    defaults to 0.*/
	    uint8 fillfact;

	    /*type (type) - 1 byte - 0 for heap, 1 for clustered index, 2 for non-clustered index*/
	    uint8 type;

	    // tinyprop - 1 byte
	    uint8 tinyprop;
	
	    /* dataspace - 4 bytes - appears to be 1 (PRIMARY) for permanent tables, 
	    and 0 for return tables of functions and user defined table types (NO data space).*/
	    uint32 dataspace;
	
	    /*lobds - 4 bytes*/
	    uint32 lobds;

	    /*rowset - 8 bytes - appears to be the sysallocunits.container_id 
	    that this index belongs, visible through the DMV sys.partitions.hobt_id.*/
	    uint64 rowset;

	    /*name (name) - nvarchar - the name of the index.  
	    This will be programmatically generated for function generated table and 
	    user defined table types.  It will be NULL for heaps.*/
    };
    union {
        data_type data;
        char raw[sizeof(data_type)];
    };
};

#pragma pack(pop)

struct sysidxstats_row_meta {

    typedef_col_type_n(sysidxstats_row, head);
    typedef_col_type_n(sysidxstats_row, id);
    typedef_col_type_n(sysidxstats_row, indid);
    typedef_col_type_n(sysidxstats_row, status);
    typedef_col_type_n(sysidxstats_row, intprop);
    typedef_col_type_n(sysidxstats_row, fillfact);
    typedef_col_type_n(sysidxstats_row, type);
    typedef_col_type_n(sysidxstats_row, tinyprop);
    typedef_col_type_n(sysidxstats_row, dataspace);
    typedef_col_type_n(sysidxstats_row, lobds);
    typedef_col_type_n(sysidxstats_row, rowset);

    typedef TL::Seq<
        head
        ,id
        ,indid
        ,status
        ,intprop
        ,fillfact
        ,type
        ,tinyprop
        ,dataspace
        ,lobds
        ,rowset
    >::Type type_list;

    sysidxstats_row_meta() = delete;
};

struct sysidxstats_row_info {
    sysidxstats_row_info() = delete;
    static std::string type_meta(sysidxstats_row const &);
    static std::string type_raw(sysidxstats_row const &);
};

} // db
} // sdl

#endif // __SDL_SYSTEM_SYSSCALARTYPES_H__