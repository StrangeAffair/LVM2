#ifndef _FunctionsTable_hpp_
#define _FunctionsTable_hpp_ 1

#include <cstdlib>
#include <string>

#include "../Command/Command.hpp"
#include "../Array/Array.hpp"

using std::string;

#ifndef _struct_Command_
#define _struct_Command_ 1
struct Command
{
    string name;

    size_t type; ///0(builtin) or 1(user defined)
    void*  data;

    //CFunction fn;
    //vector<const Command*> code;
};
#endif // _struct_Command_

template<class T>
using il = std::initializer_list<T>;

auto IL_ObjT_2_Arr_ObjT(il<ObjectType> in, Array<ObjectType>& out) -> size_t;

/*
#ifndef _Function_Ctor_
#error need constructor for Function without arguments (i.e. ctor(Function&))
#endif // _Function_Ctor_

#ifndef _Function_Dtor_
#error need destructor for Function (i.e. dtor(Function&))

#ifndef _Function_Move_
#error need move function for Function
#endif // _Function_Move_

#ifndef _Function_Copy_
#error need copy function for Function
#endif // _Function_Copy_
*/

/*
struct FunctionsTable
{
    Function* start;
    Function* finish;
    Function* top;
};

size_t size(const FunctionsTable& ftable)
{
    return ftable.top - ftable.start;
}

size_t capacity(const FunctionsTable& ftable)
{
    return ftable.finish - ftable.start;
}

void resize(FunctionsTable& ftable, size_t n)
{

}
*/

struct FunctionsTable
{
public:
    Function* start;
    Function* finish;
    Function* current;
    //vector<Function> functions;
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
public:
    static bool compare(Array<ObjectType> left, Array<ObjectType> right)
    {
        if (left.size() != right.size())
            return false;
        const ObjectType* first  = left .start;
        const ObjectType* second = right.start;
        while(first != left.finish)
        {
            if ((*first != ObjectType::Any) && (*second != ObjectType::Any))
            {
                if (*first != *second)
                    return false;
            }
            ++first;
            ++second;
        }
        return true;
    }
    static bool compare(Array<ObjectType> left, il<ObjectType> right)
    {
        if (left.size() != right.size())
            return false;
        const ObjectType* first  = left .start;
        const ObjectType* second = right.begin();
        while(first != left.finish)
        {
            if ((*first != ObjectType::Any) && (*second != ObjectType::Any))
            {
                if (*first != *second)
                    return false;
            }
            ++first;
            ++second;
        }
        return true;
    }
public:
    Function* find(string name)
    {
        if (name != "main")
        {
            fprintf(stderr, "tries not to find main\n");
            return nullptr;
        }

        for(Function* fn = start; fn != current; ++fn)
        {
            if (fn->name != name)
                continue;

            if (fn->input .size() > 0)
                continue;
            if (fn->output.size() > 0)
                continue;
            if (fn->args  .size() > 0)
                continue;

            return fn;
        }
        return nullptr;
    }

    Function* find(string name, il<string> input, il<string> output, il<string> args);

    Function* find(string name, il<ObjectType> input, il<ObjectType> output, il<ObjectType> args)
    {
        for(Function* fn = start; fn != current; ++fn)
        {
            if (fn->name != name)
                continue;

            if (!compare(fn->input , input))
                continue;
            if (!compare(fn->output, output))
                continue;
            if (!compare(fn->args  , args))
                continue;

            return fn;
        }
        return nullptr;
    }

    Function* find(string name, Array<ObjectType> input, Array<ObjectType> output, Array<ObjectType> args)
    {
        for(Function* fn = start; fn != current; ++fn)
        {
            if (fn->name != name)
                continue;

            if (!compare(fn->input , input))
                continue;
            if (!compare(fn->output, output))
                continue;
            if (!compare(fn->args  , args))
                continue;

            return fn;
        }
        return nullptr;
    }
    Function* insert(Function* fn)
    {
        if (find(fn->name, fn->input, fn->output, fn->args) != nullptr)
        {
            fprintf(stderr, "multiple definition\n");
            return nullptr;
        }
        //need to do copy to not delete builtins
        FunctionMove(*current, *fn);
        ++current;
        return find(fn->name, fn->input, fn->output, fn->args);
    }
    Function* insert(string name, CFunction fn, il<ObjectType> input, il<ObjectType> output, il<ObjectType> args)
    {
        Function temp;
        FunctionSetName    (temp, name.c_str());
        FunctionSetFunction(temp, fn);
        FunctionSetInput   (temp, input);
        FunctionSetOutput  (temp, output);
        FunctionSetArgs    (temp, args);

        FunctionMove(*current, temp);
        ++current;
        return find(name, input, output, args);
    }
    Function* insert(string name, CFunction fn, il<string> input, il<string> output, il<string> args);

};

#endif // _FunctionsTable_hpp_
