#ifndef _ObjectType_hpp_
#define _ObjectType_hpp_ 1

#include <vector>
#include <string>
#include <cstdint>

using std::vector;
using std::string;

class ObjectType{
public:
    enum Type{
        None,
        Null,
        Any,

        Bool,

        Int8,
        Int16,
        Int32,
        Int64,

        UInt8,
        UInt16,
        UInt32,
        UInt64,

        Size,

        String255,
        Exception
    };
public:
    ObjectType()
    {this->type = Type::None;}
    ObjectType(Type type)
    {this->type = type;}
    ObjectType(const ObjectType& other)
    {this->type = other.type;}
public:
    static const ObjectType* ctor(ObjectType* type)
    {
        type->type = Type::None;
        return type;
    }
    static const ObjectType* dtor(ObjectType* type)
    {
        type->type = Type::None;
        return nullptr;
    }
    static const ObjectType* move(ObjectType* left, ObjectType* right)
    {
        left->type = right->type;
        return left;
    }
    static const ObjectType* copy(ObjectType* left, const ObjectType* right)
    {
        left->type = right->type;
        return left;
    }
public:
    const ObjectType& operator=(string name)
    {
        this->type = ObjectType::find(name);
        return *this;
    }
    const ObjectType& operator=(const ObjectType& other)
    {
        if (this == &other) return *this;
        this->type = other.type;
        return *this;
    }
public:
    friend bool operator==(const ObjectType& left, Type right)
    {return left.type == right;}
    friend bool operator!=(const ObjectType& left, Type right)
    {return left.type != right;}
public:
    friend bool operator==(const ObjectType& left, const ObjectType& right)
    {return left.type == right.type;}
    friend bool operator!=(const ObjectType& left, const ObjectType& right)
    {return left.type != right.type;}
public:
    const char* GetName() const
    {return GetTypeName(type);}
    size_t      GetSize() const
    {return GetTypeSize(type);}
public:
    static const char* GetTypeName(Type type);
    static size_t      GetTypeSize(Type type);
public:
    static Type find(string name);
public:
    Type type;
};

#endif // _ObjectType_hpp_
