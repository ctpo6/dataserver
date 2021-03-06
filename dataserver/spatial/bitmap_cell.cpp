﻿// bitmap_cell.cpp
//
#include "common/common.h"
#include "bitmap_cell.h"
#include <map>

namespace sdl { namespace db {

#if !high_grid_optimization
#error high_grid_optimization
#endif

struct bitmap_cell : noncopyable { // experimental

#pragma pack(push, 1) 

    struct mask_256 { // 32 bytes
        static const size_t size = 256;
        union {
		    uint8 byte[size >> 3];
            uint32 _32[(size >> 3) / sizeof(uint32)];
            uint64 _64[(size >> 3) / sizeof(uint64)];
        };
		void set_bit(uint8 const b) {
			byte[b >> 3] |= 1 << (b & 0x7);
		}
		bool bit(uint8 const b) const {
			return !!(byte[b >> 3] & (1 << (b & 0x7)));
		}
        void fill(uint8 const b) {
            memset_pod(byte, b);
        }
        bool all_bits() const {
            static_assert(A_ARRAY_SIZE(_32) == 8, "");
            static_assert(A_ARRAY_SIZE(_64) == 4, "");
            static_assert(uint32(-1) == 0xFFFFFFFF, "");
            static_assert(uint64(-1) == 0xFFFFFFFFFFFFFFFF, "");
            return
                (_64[0] == uint64(-1)) &&
                (_64[1] == uint64(-1)) &&
                (_64[2] == uint64(-1)) &&
                (_64[3] == uint64(-1));
        } 
    };
    struct node_type { // 64 bytes
        mask_256 used;
        mask_256 full;
        node_type(): used{}, full{} {}
        void set_full() {
            used.fill(1);
            full.fill(1);
        }
    };
	
#pragma pack(pop)

    class allocator : noncopyable {
    public:
        using key_type = uint32;
        using node_bool = std::pair<node_type *, bool>;
        static key_type make_key(spatial_cell const cell, uint8 const depth) {
            SDL_ASSERT(cell.data.depth == spatial_cell::depth_4);
            SDL_ASSERT(depth && (depth <= 4));
            if (depth > 1) {
                spatial_cell temp = spatial_cell::init(cell, depth - 1);
                temp[3] = depth - 1;
                return temp.r32();
            }
            return 0;
        }
        node_bool get(key_type const id){
            auto it = m_map.find(id);
            if (it != m_map.end()) {
                return { &(it->second), false };
            }
            return { &(m_map[id]), true };
        }
        void erase(key_type const id) {
            auto it = m_map.find(id);
            if (it != m_map.end()) {
                m_map.erase(it);
            }
            else {
                SDL_ASSERT(0);
            }
        }
        node_type & operator[](key_type const id){
            SDL_ASSERT(m_map.find(id) != m_map.end());
            return m_map[id];
        }
        size_t size() const {
            return m_map.size();
        }
    private:
        using map_type = std::map<key_type, node_type>; // prototype
        map_type m_map;
    };

	void insert(spatial_cell); 

    size_t contains() const {
        return alloc.size();
    }
private:
    allocator alloc;
};

void bitmap_cell::insert(spatial_cell const cell)
{
    SDL_ASSERT(cell.data.depth == spatial_cell::depth_4);
    for (uint8 depth = 1; depth <= 3; ++depth) {
        auto const p = alloc.get(allocator::make_key(cell, depth));
        node_type * const node = p.first;
        if (!p.second && node->full.all_bits()) {
            return;
        }
        node->used.set_bit(cell[depth - 1]);
    }
    {
        auto const p = alloc.get(allocator::make_key(cell, spatial_cell::depth_4));
        node_type * node = p.first;
        uint8 const bit = cell[spatial_cell::depth_4 - 1];
        node->used.set_bit(bit);
        if (node->used.all_bits()) { // remove full nodes to reduce memory
            for (uint8 depth = 4; depth > 1; --depth) {
                alloc.erase(allocator::make_key(cell, depth));
                node_type & next = alloc[allocator::make_key(cell, depth - 1)];
                next.full.set_bit(cell[depth - 1]);
                if (!next.full.all_bits())
                    break;
            }
        }
    }    
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
                    using mask_256 = bitmap_cell::mask_256;
                    using node_type = bitmap_cell::node_type;
                    static_assert(sizeof(mask_256) == 32, "");
                    static_assert(sizeof(node_type) == 64, "");
                    if (1) {
                        mask_256 test = {};
                        test.fill(0xFF);
                        SDL_ASSERT(test.all_bits());
                    }
                    if (1) {
    					bitmap_cell test;
                        for (uint32 id = 0; id < 256; ++id) {
                            test.insert(spatial_cell::init(reverse_bytes(id)));
                        }
                        SDL_TRACE("contains = ", test.contains());
                    }
                }
            };
            static unit_test s_test;
        }
    } // db
} // sdl
#endif //#if SV_DEBUG
