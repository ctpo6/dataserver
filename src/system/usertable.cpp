// usertable.cpp
//
#include "common/common.h"
#include "usertable.h"

namespace sdl { namespace db { namespace {

struct key_size_count {
    size_t & result;
    key_size_count(size_t & s) : result(s){}
    template<class T> // T = index_key_t<>
    void operator()(T) {
        result += sizeof(typename T::type);
    }
};

} // namespace

primary_key::primary_key(page_head const * p, colpars && _c, scalars && _s, orders && _o)
    : root(p)
    , colpar(std::move(_c))
    , scalar(std::move(_s))
    , order(std::move(_o))
{
    SDL_ASSERT(!colpar.empty());
    SDL_ASSERT(colpar.size() == scalar.size());
    SDL_ASSERT(colpar.size() == order.size());
    SDL_ASSERT(root);
    SDL_ASSERT(root->is_index() || root->is_data());
    SDL_ASSERT(slot_array::size(root));
}

cluster_index::cluster_index(page_head const * p,
    column_index && _c,
    column_order && _o,
    shared_usertable const & _s)
    : root(p)
    , col_index(std::move(_c))
    , col_ord(std::move(_o))
    , schema(_s)
{
    SDL_ASSERT(root && root->is_index());
    SDL_ASSERT(!col_index.empty());
    SDL_ASSERT(col_index.size() == col_ord.size());
    SDL_ASSERT(schema.get());
    init_key_length();
}

void cluster_index::init_key_length()
{
    SDL_ASSERT(!m_key_length);
    m_sub_key_length.resize(col_index.size());
    for (size_t i = 0; i < col_index.size(); ++i) {
        size_t len = 0;
        case_index_key((*this)[i].type, key_size_count(len));
        m_key_length += len;
        m_sub_key_length[i] = len;
    }
    SDL_ASSERT(m_key_length);
}

cluster_index::column_ref
cluster_index::operator[](size_t i) const
{
    SDL_ASSERT(i < size());
    return (*schema)[col_index[i]];
}

sortorder cluster_index::order(size_t i) const
{
    SDL_ASSERT(i < size());
    return col_ord[i];
}

//----------------------------------------------------------------------------

usertable::column::column(syscolpars_row const * p, sysscalartypes_row const * s)
    : colpar(p)
    , scalar(s)
    , name(col_name_t(p))
    , type(s->data.id)
    , length(p->data.length)
{
    SDL_ASSERT(colpar && scalar);    
    SDL_ASSERT(colpar->data.utype == scalar->data.id);
    SDL_ASSERT(this->type != scalartype::t_none);
    SDL_ASSERT(!this->name.empty());
}

bool usertable::column::is_fixed() const
{
    if (scalartype::is_fixed(this->type)) {
        if (!length.is_var()) {
            SDL_ASSERT(length._16 > 0);
            return true;
        }
    }
    return false;
}

//----------------------------------------------------------------------------

usertable::usertable(sysschobjs_row const * p, columns && c)
    : schobj(p)
    , m_name(col_name_t(p))
    , m_schema(std::move(c))
{
    SDL_ASSERT(schobj);
    SDL_ASSERT(schobj->is_USER_TABLE_id());

    SDL_ASSERT(!m_name.empty());
    SDL_ASSERT(!m_schema.empty());
    SDL_ASSERT(get_id()._32);

    init_offset();
}

void usertable::init_offset()
{
    size_t offset = 0;
    size_t col_index = 0;
    size_t var_index = 0;
    m_offset.resize(m_schema.size());
    for (auto & c : m_schema) {
        if (c->is_fixed()) {
            m_offset[col_index] = offset;
            offset += c->fixed_size();
        }
        else {
            m_offset[col_index] = var_index++;
        }
        ++col_index;
    }
}

size_t usertable::fixed_offset(size_t i) const
{
    SDL_ASSERT(i < this->size());
    SDL_ASSERT(m_schema[i]->is_fixed());
    return m_offset[i];
}

size_t usertable::var_offset(size_t i) const
{
    SDL_ASSERT(i < this->size());
    SDL_ASSERT(!m_schema[i]->is_fixed());
    return m_offset[i];
}

size_t usertable::count_var() const
{
    return count_if([](column_ref c){
        return !c.is_fixed();
    });
}

size_t usertable::count_fixed() const
{
    return count_if([](column_ref c){
        return c.is_fixed();
    });
}

size_t usertable::fixed_size() const
{
    size_t ret = 0;
    for_col([&ret](column_ref c){
        if (c.is_fixed()) {
            ret += c.fixed_size();
        }
    });
    return ret;
}

std::string usertable::column::type_schema(primary_key const * const PK) const
{
    column_ref d = *this;
    std::stringstream ss;
    ss << "[" << d.colpar->data.colid._32 << "] ";
    ss << d.name << " : " << scalartype::get_name(d.type) << " (";
    if (d.length.is_var())
        ss << "var";
    else 
        ss << d.length._16;
    ss << ")";
    if (d.is_fixed()) {
        ss << " fixed";
    }
    if (PK) {
        auto found = PK->find_colpar(d.colpar);
        if (found != PK->colpar.end()) {
            if (found == PK->colpar.begin())
                ss << " IsPrimaryKey";
            else
                ss << " IndexKey";
        }
    }
    return ss.str();
}

std::string usertable::type_schema(primary_key const * const PK) const
{
    usertable const & ut = *this;
    std::stringstream ss;
    ss  << "name = " << ut.m_name
        << "\nid = " << ut.get_id()._32
        << std::uppercase << std::hex 
        << " (" << ut.get_id()._32 << ")"
        << std::dec
        << "\nColumns(" << ut.m_schema.size() << ")"
        << "\n";
    for (auto & p : ut.m_schema) {
        ss << p->type_schema(PK);
        ss << "\n";
    }
    return ss.str();
}

usertable::col_index
usertable::find_col(syscolpars_row const * const p) const
{
    SDL_ASSERT(p);
    const size_t i = find_if([p](column_ref c) {
        return c.colpar == p;
    });
    if (i < size()) {
        return { m_schema[i].get(), i};
    }
    return {};
}

} // db
} // sdl

