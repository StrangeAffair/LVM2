#ifndef _FunctionsTable_hpp_
#define _FunctionsTable_hpp_ 1

#include <cstdlib>

#include "../Command/Command.hpp"
#include "../Array/Array.hpp"

template<class T>
using il = std::initializer_list<T>;

struct FunctionsTable
{
public:
    Function* start;
    Function* finish;
    Function* current;
public:
    FunctionsTable()
    {
        Function* pointer = (Function*) calloc(1000, sizeof(Function));
        if (pointer == nullptr)
            throw "bad alloc";

        start   = pointer;
        finish  = pointer + 1000;
        current = start;
    }
    ~FunctionsTable()
    {
        for(Function* it = start; it != current; ++it)
            Function::dtor(*it);

        free(start);
    }
public:
    Function* insert(const char* name, CFunction fn, il<ObjectType> input, il<ObjectType> output, il<ObjectType> args)
    {
        Function temp;
        Function::ctor(temp);
        temp.SetFunction (fn);
        temp.SetSignature(name, input, output, args);

        Function::move(*current, temp);
        ++current;
        return find(name, input, output, args);
    }
    Function* insert(const char* name, size_t opcode, il<ObjectType> input, il<ObjectType> output, il<ObjectType> args)
    {
        Function temp;
        Function::ctor(temp);
        temp.SetFunction (opcode);
        temp.SetSignature(name, input, output, args);

        Function::move(*current, temp);
        ++current;
        return find(name, input, output, args);
    }
public:
    Function* find(const char* name) const
    {
        if (strncmp(name, "main", strlen("main")) != 0)
        {
            fprintf(stderr, "tries not to find main\n");
            return nullptr;
        }

        for(Function* fn = start; fn != current; ++fn)
            if (compare(fn->signature, name, {}, {}, {}) == true)
                return fn;
        return nullptr;
    }

    Function* find(const FunctionSignature& signature)  const
    {
        for(Function* fn = start; fn != current; ++fn)
            if (compare(fn->signature, signature) == true)
                return fn;
        return nullptr;
    }

    Function* find(const char* name, il<ObjectType> input, il<ObjectType> output, il<ObjectType> args)  const
    {
        for(Function* fn = start; fn != current; ++fn)
            if (compare(fn->signature, name, input, output, args) == true)
                return fn;
        return nullptr;
    }

    Function* insert(const Function* fn)
    {
        if (find(fn->signature) != nullptr)
        {
            fprintf(stderr, "multiple definition\n");
            return nullptr;
        }
        //need to do copy to not delete builtins
        Function::copy(*current, *fn);
        ++current;
        return find(fn->signature);
    }
};

#endif // _FunctionsTable_hpp_
