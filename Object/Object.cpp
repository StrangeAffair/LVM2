#include "Object.hpp"

const Object* Object::move(Object* dest, Object* src)
{
    dest->type = src->type;
    dest->data = src->data;

    src->type = ObjectType::None;
    src->data = nullptr;
    return dest;
}

const Object* Object::copy(Object* dest, const Object* src)
{
    if (dest->type != ObjectType::None)
        Object::clear(*dest);

    dest->type = src->type;
    dest->data = malloc(dest->type.GetSize());
    memcpy(dest->data, src->data, dest->type.GetSize());
    return dest;
}

void Object::clear(Object& dest)
{
    if (dest.data != nullptr)
        free(dest.data);
    dest.type = ObjectType::None;
    dest.data = nullptr;
}

template<>
auto Object::get(      Object& obj) -> size_t&
{
    if (obj.type == ObjectType::Size)
        return *(size_t*) obj.data;
    else
        throw "incorrect type";
}

template<>
auto Object::get(const Object& obj) -> const size_t&
{
    if (obj.type == ObjectType::Size)
        return *(size_t*) obj.data;
    else
        throw "incorrect type";
}



template<>
auto Object::get(const Object& obj) -> const int32_t&
{
    if (obj.type == ObjectType::Int32)
        return *(int32_t*) obj.data;
    else
        throw "incorrect type";
}

template<>
auto Object::get(      Object& obj) -> int32_t&
{
    if (obj.type == ObjectType::Int32)
        return *(int32_t*) obj.data;
    else
        throw "incorrect type";
}

template<>
auto Object::get(const Object& obj) -> const int64_t&
{
    if (obj.type == ObjectType::Int64)
        return *(int64_t*) obj.data;
    else
        throw "incorrect type";
}

template<>
auto Object::get(      Object& obj) -> int64_t&
{
    if (obj.type == ObjectType::Int64)
        return *(int64_t*) obj.data;
    else
        throw "incorrect type";
}

template<>
auto Object::get(const Object& obj) -> const bool&
{
    if (obj.type == ObjectType::Bool)
        return *(bool*) obj.data;
    else
        throw "incorrect type";
}

template<>
auto Object::get(      Object& obj) -> bool&
{
    if (obj.type == ObjectType::Bool)
        return *(bool*) obj.data;
    else
        throw "incorrect type";
}

template<>
void Object::set(Object& obj, const int32_t& value)
{
    if (obj.type != ObjectType::None)
        Object::clear(obj);
    obj.type = ObjectType::Int32;
    obj.data = malloc(obj.type.GetSize());

    Object::get<int32_t>(obj) = value;
}

template<>
void Object::set(Object& obj, const int64_t& value)
{
    if (obj.type != ObjectType::None)
        Object::clear(obj);
    obj.type = ObjectType::Int64;
    obj.data = malloc(obj.type.GetSize());

    Object::get<int64_t>(obj) = value;
}

template<>
void Object::set(Object& obj, const size_t&  value)
{
    if (obj.type != ObjectType::None)
        Object::clear(obj);
    obj.type = ObjectType::Size;
    obj.data = malloc(obj.type.GetSize());

    Object::get<size_t> (obj) = value;
}

template<>
void Object::set(Object& obj, const bool& value)
{
    if (obj.type != ObjectType::None)
        Object::clear(obj);
    obj.type = ObjectType::Bool;
    obj.data = malloc(obj.type.GetSize());

    Object::get<bool>(obj) = value;
}
