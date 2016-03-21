// database.inl
//
#ifndef __SDL_SYSTEM_DATABASE_INL__
#define __SDL_SYSTEM_DATABASE_INL__

#pragma once

namespace sdl { namespace db {

template<class T> inline
auto get_access(database & db) -> decltype(db.get_access_t<T>()) {
    return db.get_access_t<T>();
}

inline const char * page_name_t(identity<datatable>) { return "datatable"; }
inline const char * page_name_t(identity<usertable>) { return "usertable"; }

template<class T> inline const char * page_name_t(identity<T>) { return T::name(); }
template<class T> inline const char * page_name() { 
    return page_name_t(identity<T>());
}

template<typename T> inline
void database::load_page(page_ptr<T> & p) {
    if (auto h = load_sys_obj(database::sysObj_t<T>::id)) {
        reset_new(p, h);
    }
    else {
        SDL_ASSERT(0);
        p.reset();
    }
}

template<class T> inline
void database::load_next(page_ptr<T> & p) {
    if (p) {
        A_STATIC_CHECK_TYPE(page_head const * const, p->head);
        if (auto h = this->load_next_head(p->head)) {
            A_STATIC_CHECK_TYPE(page_head const *, h);
            reset_new(p, h);
        }
        else {
            p.reset();
        }
    }
    else {
        SDL_ASSERT(0);
    }
}

template<class T> inline
void database::load_prev(page_ptr<T> & p) {
    if (p) {
        A_STATIC_CHECK_TYPE(page_head const * const, p->head);
        if (auto h = this->load_prev_head(p->head)) {
            A_STATIC_CHECK_TYPE(page_head const *, h);
            reset_new(p, h);
        }
        else {
            SDL_ASSERT(0);
            p.reset();
        }
    }
    else {
        SDL_ASSERT(0);
    }
}

} // db
} // sdl

#endif // __SDL_SYSTEM_DATABASE_INL__