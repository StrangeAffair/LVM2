#ifndef _Object_hpp_
#define _Object_hpp_ 1

#include "ObjectType.hpp"
#include <cstdint>
#include <cstring>

class Object
{
public:
    ObjectType type;
    void*      data;
private:
    const Object& operator=(const Object&);
public:
    Object()
    {
        this->type = ObjectType::None;
        this->data = nullptr;
    }
public:
    /*
    void SetType(ObjectType type)
    {
        if (this->type != type)
            free(data);
        this->type = type;
        this->data = malloc(this->type.GetSize());
    }
    */
//public:
    //static void set(Object& obj, ObjectType type, initializer_list<char> bytes);
    //static void set(Object& obj, ObjectType type, size_t size, char* bytes);
public:
    template<typename T>
    static void set(Object& obj, const T& value);
public:
    template<typename T>
    static auto get(const Object& obj) -> const T&;
    template<typename T>
    static auto get(      Object& obj) -> T&;
public:
    static const Object* move (Object* dest,       Object* src);
    static const Object* copy (Object* dest, const Object* src);
    static const Object* ctor (Object* dest)
    {
        new (dest) Object();
        return dest;
    }
    static const Object* dtor (Object* dest)
    {
        if (dest->type != ObjectType::None)
            Object::clear(*dest);
        new (dest) Object();
        return dest;
    }
    static void clear(Object& dest);
};

#endif // _Object_hpp_
