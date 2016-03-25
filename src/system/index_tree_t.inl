// index_tree_t.inl
//
#ifndef __SDL_SYSTEM_INDEX_TREE_T_INL__
#define __SDL_SYSTEM_INDEX_TREE_T_INL__

#pragma once

namespace sdl { namespace db { namespace todo {

inline bool index_tree::index_page::is_key_NULL() const
{
    return !(slot || head->data.prevPage);
}

inline index_tree::index_page
index_tree::begin_index() const
{
    return index_page(this, page_begin(), 0);
}

inline index_tree::index_page
index_tree::end_index() const
{
    page_head const * const h = page_end();
    return index_page(this, h, slot_array::size(h));
}

inline bool index_tree::is_begin_index(index_page const & p) const
{
    return !(p.slot || p.head->data.prevPage);
}

inline bool index_tree::is_end_index(index_page const & p) const
{
    if (p.slot == p.size()) {
        SDL_ASSERT(!p.head->data.nextPage);
        return true;
    }
    SDL_ASSERT(p.slot < p.size());
    return false;
}

inline index_tree::key_ref
index_tree::index_page::get_key(index_page_row_key const * const row) const {
    return row->data.key;
}

inline index_tree::key_ref 
index_tree::index_page::row_key(size_t const i) const {
    return get_key(index_page_key(this->head)[i]);
}

inline pageFileID const & 
index_tree::index_page::row_page(size_t const i) const {
    return index_page_key(this->head)[i]->data.page;
}

inline index_tree::row_mem
index_tree::index_page::operator[](size_t const i) const {
    return index_page_key(this->head)[i]->data;
}

inline pageFileID const & 
index_tree::index_page::find_page(key_ref key) const {
    return this->row_page(find_slot(key)); 
}

inline pageFileID const & 
index_tree::index_page::min_page() const {
    return this->row_page(0);
}

inline pageFileID const & 
index_tree::index_page::max_page() const {
    return this->row_page(this->size() - 1);
}

//----------------------------------------------------------------------

inline index_tree::row_access::iterator
index_tree::row_access::begin()
{
    return iterator(this, tree->begin_index());
}

inline index_tree::row_access::iterator
index_tree::row_access::end()
{
    return iterator(this, tree->end_index());
}

inline void index_tree::row_access::load_next(index_page & p)
{
    tree->load_next_row(p);
}

inline void index_tree::row_access::load_prev(index_page & p)
{
    tree->load_prev_row(p);
}

inline bool index_tree::row_access::is_key_NULL(iterator const & it) const
{
    return it.current.is_key_NULL();
}

//----------------------------------------------------------------------

inline index_tree::page_access::iterator
index_tree::page_access::begin()
{
    return iterator(this, tree->begin_index());
}

inline index_tree::page_access::iterator
index_tree::page_access::end()
{
    return iterator(this, tree->end_index());
}

inline void index_tree::page_access::load_next(index_page & p)
{
    tree->load_next_page(p);
}

inline void index_tree::page_access::load_prev(index_page & p)
{
    tree->load_prev_page(p);
}

inline bool index_tree::page_access::is_end(index_page const & p) const
{
    return tree->is_end_index(p);
}

//----------------------------------------------------------------------

} // todo
} // db
} // sdl

#endif // __SDL_SYSTEM_INDEX_TREE_T_INL__
