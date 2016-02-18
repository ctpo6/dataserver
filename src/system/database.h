// database.h
//
#ifndef __SDL_SYSTEM_DATABASE_H__
#define __SDL_SYSTEM_DATABASE_H__

#pragma once

#include "datatable.h"

namespace sdl { namespace db {

class database: public database_base
{
    enum class sysObj {
        //sysrscols = 3,
        sysrowsets = 5,
        sysschobjs = 34,
        syscolpars = 41,
        sysscalartypes = 50,
        sysidxstats = 54,
        sysiscols = 55,
        sysobjvalues = 60,
    };
    enum class sysPage {
        file_header = 0,
        PFS = 1,
        boot_page = 9,
    };

public:
    void load_page(page_ptr<sysallocunits> &);
    void load_page(page_ptr<sysschobjs> &);
    void load_page(page_ptr<syscolpars> &);
    void load_page(page_ptr<sysidxstats> &);
    void load_page(page_ptr<sysscalartypes> &);
    void load_page(page_ptr<sysobjvalues> &);
    void load_page(page_ptr<sysiscols> &);
    void load_page(page_ptr<sysrowsets> &);
    void load_page(page_ptr<pfs_page> &);

    template<typename T> void load_page(T&) = delete;
    template<typename T> void load_next(page_ptr<T> &);
    template<typename T> void load_prev(page_ptr<T> &);
public: // for page_iterator
    template<typename T> static
    bool is_same(T const & p1, T const & p2) {
        if (p1 && p2) {
            A_STATIC_CHECK_TYPE(page_head const * const, p1->head);
            return p1->head == p2->head;
        }
        return p1 == p2; // both nullptr
    }
    template<typename T> static
    bool is_end(T const & p) {
        if (p) {
            A_STATIC_CHECK_TYPE(page_head const * const, p->head);
            SDL_ASSERT(p->head);
            return false;
        }
        return true;
    }
    template<typename T> static
    T const & dereference(T const & p) {
        SDL_ASSERT(!is_end(p));
        return p;
    }
private: 
    template<class pointer_type>
    class page_access_t : noncopyable {
        database * const db;
    public:
        using value_type = typename pointer_type::element_type;
        using iterator = page_iterator<database, pointer_type>;
        iterator begin() {
            pointer_type p{};
            db->load_page(p);
            return iterator(db, std::move(p));
        }
        iterator end() {
            return iterator(db);
        }
        explicit page_access_t(database * p): db(p) {
            SDL_ASSERT(db);
        }
    };
    template<class T>
    using page_access = page_access_t<page_ptr<T>>;    

    class usertable_access : noncopyable {
        database * const db;
        vector_shared_usertable const & data() {
            return db->get_usertables();
        }
    public:
        using iterator = vector_shared_usertable::const_iterator;
        iterator begin() {
            return data().begin();
        }
        iterator end() {
            return data().end();
        }
        explicit usertable_access(database * p): db(p) {
            SDL_ASSERT(db);
        }
    };
    class datatable_access : noncopyable {
        database * const db;
        vector_shared_datatable const & data() {
            return db->get_datatable();
        }
    public:
        using iterator = vector_shared_datatable::const_iterator;
        iterator begin() {
            return data().begin();
        }
        iterator end() {
            return data().end();
        }
        explicit datatable_access(database * p): db(p) {
            SDL_ASSERT(db);
        }
    };
    class iam_access {
        database * const db;
        sysallocunits_row const * const alloc;
    public:
        using iterator = page_iterator<database, shared_iam_page>;
        explicit iam_access(database * p, sysallocunits_row const * a)
            : db(p), alloc(a)
        {
            SDL_ASSERT(db && alloc);
        }
        iterator begin() {
            return iterator(db, db->load_iam_page(alloc->data.pgfirstiam));
        }
        iterator end() {
            return iterator(db);
        }
    };
private:
    page_head const * load_sys_obj(sysallocunits const *, sysObj);

    template<sysObj, class T> page_ptr<T>
    get_sys_obj(sysallocunits const *);
    
    template<sysObj, class T>
    void load_sys_page(page_ptr<T> &);

#if 1 // reserved
    template<typename T> 
    page_ptr<T> get_sys_page() {
        page_ptr<T> p{};
        load_page(p);
        return p;
    }
    template<class T, class fun_type> static
    typename T::const_pointer 
    find_row_if(page_access<T> & obj, fun_type fun) {
        for (auto & p : obj) {
            if (auto found = p->find_if(fun)) {
                return found;
            }
        }
        return nullptr;
    }
#endif
    template<class T, class fun_type> static
    void for_row(page_access<T> & obj, fun_type fun) {
        for (auto & p : obj) {
            p->for_row(fun);
        }
    }
    template<class fun_type>
    unique_datatable find_table_if(fun_type);
public:
    explicit database(const std::string & fname);
    ~database();

    const std::string & filename() const;

    bool is_open() const;

    size_t page_count() const;

    page_head const * load_page_head(pageIndex);
    page_head const * load_page_head(pageFileID const &);

    page_head const * load_next_head(page_head const *);
    page_head const * load_prev_head(page_head const *);
    
    using page_row = std::pair<page_head const *, row_head const *>;
    page_row load_page_row(recordID const &);

    void const * start_address() const; // diagnostic only
    void const * memory_offset(void const *) const; // diagnostic only

    pageType get_pageType(pageFileID const &);
    pageFileID nextPageID(pageFileID const &);
    pageFileID prevPageID(pageFileID const &);

    page_ptr<bootpage> get_bootpage();
    page_ptr<fileheader> get_fileheader();
    page_ptr<datapage> get_datapage(pageIndex);
    page_ptr<sysallocunits> get_sysallocunits();
    page_ptr<pfs_page> get_pfs_page();

    page_access<sysallocunits> _sysallocunits{this};
    page_access<sysschobjs> _sysschobjs{this};
    page_access<syscolpars> _syscolpars{this};
    page_access<sysidxstats> _sysidxstats{this};
    page_access<sysscalartypes> _sysscalartypes{this};
    page_access<sysobjvalues> _sysobjvalues{this};
    page_access<sysiscols> _sysiscols{ this };
    page_access<sysrowsets> _sysrowsets{ this };
    page_access<pfs_page> _pfs_page{ this };

    usertable_access _usertables{this};
    datatable_access _datatables{this};

    unique_datatable find_table_name(const std::string & name);

    vector_sysallocunits_row const & find_sysalloc(schobj_id, dataType::type);
    vector_page_head const & find_datapage(schobj_id, dataType::type, pageType::type);
    
    shared_iam_page load_iam_page(pageFileID const &);

    iam_access pgfirstiam(sysallocunits_row const * it) { 
        return iam_access(this, it); 
    }
    bool is_allocated(pageFileID const &);

    auto get_access(identity<sysallocunits>)  -> decltype((_sysallocunits))   { return _sysallocunits; }
    auto get_access(identity<sysschobjs>)     -> decltype((_sysschobjs))      { return _sysschobjs; }
    auto get_access(identity<syscolpars>)     -> decltype((_syscolpars))      { return _syscolpars; }
    auto get_access(identity<sysidxstats>)    -> decltype((_sysidxstats))     { return _sysidxstats; }
    auto get_access(identity<sysscalartypes>) -> decltype((_sysscalartypes))  { return _sysscalartypes; }
    auto get_access(identity<sysobjvalues>)   -> decltype((_sysobjvalues))    { return _sysobjvalues; }
    auto get_access(identity<sysiscols>)      -> decltype((_sysiscols))       { return _sysiscols; }
    auto get_access(identity<sysrowsets>)     -> decltype((_sysrowsets))      { return _sysrowsets; }
    auto get_access(identity<pfs_page>)       -> decltype((_pfs_page))        { return _pfs_page; }
    auto get_access(identity<usertable>)      -> decltype((_usertables))      { return _usertables; }
    auto get_access(identity<datatable>)      -> decltype((_datatables))      { return _datatables; }

    template<class T> 
    auto get_access_t() -> decltype(get_access(identity<T>())) {
        return this->get_access(identity<T>());
    }
private:
    template<class fun_type>
    void for_sysschobjs(fun_type fun) {
        for (auto & p : _sysschobjs) {
            p->for_row(fun);
        }
    }
    template<class fun_type>
    void for_USER_TABLE(fun_type fun) {
        for_sysschobjs([&fun](sysschobjs::const_pointer row){
            if (row->is_USER_TABLE_id()) {
                fun(row);
            }
        });
    }
    vector_shared_usertable const & get_usertables();
    vector_shared_datatable const & get_datatable();

    page_head const * load_page_head(sysPage);
    std::vector<page_head const *> load_page_list(page_head const *);

private:
    database(const database&) = delete;
    const database& operator=(const database&) = delete;

    class data_t;
    std::unique_ptr<data_t> m_data;
};

template<class T> inline
auto get_access(database & db) -> decltype(db.get_access_t<T>()) {
    return db.get_access_t<T>();
}

template<class T> inline const char * page_name_t(identity<T>) {
    return T::name();
}
template<class T> inline const char * page_name() { 
    return page_name_t(identity<T>());
}

template<class T> inline
void database::load_next(page_ptr<T> & p)
{
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
void database::load_prev(page_ptr<T> & p)
{
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

#endif // __SDL_SYSTEM_DATABASE_H__
