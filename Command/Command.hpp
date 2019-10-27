#ifndef _struct_Command_
#define _struct_Command_ 1

#include <vector>
#include <string>
#include <exception>
#include <initializer_list>

#include <cstdio>
#include <cstdint>
#include <cinttypes>

using std::vector;
using std::string;

template<class T>
using il = std::initializer_list<T>;

#include "../Object/Object.hpp"
#include "../Array/Array.hpp"

struct Command;
struct state;
struct FunctionsTable;
struct Function;

void call(const Command* cmd, state* st);

using  CFunction = void (*)(state* st, size_t argc, Object* argv);
using  LFunction = Array<const Command>;

struct Function
{
    Function() = default;

    Function(const Function& other) = delete;
    Function(Function&& other) = delete;

    union FunctionTypes
    {
        CFunction Cfn;
        LFunction Lfn;
    };

    string        name;
    size_t        type; ///0(builtin) or 1(user defined)
    FunctionTypes data;

    Array<ObjectType> input;
    Array<ObjectType> output;
    Array<ObjectType> args;
};

#define _Function_Ctor_ 1
auto FunctionCtor(Function& fn) -> size_t;

#define _Function_Dtor_ 1
auto FunctionDtor(Function& fn) -> size_t;

auto FunctionMove(Function& dest, Function& src) -> size_t;

auto FunctionSetName(Function& fn, const char* name) -> size_t;

auto FunctionGetName(const Function& fn) -> const char*;

auto FunctionSetFunction(Function&, CFunction) -> void;

auto FunctionSetFunction(Function&, LFunction) -> void;

auto FunctionSetInput(Function& fn, const Array<ObjectType>& input) -> size_t;

auto FunctionSetInput(Function& fn, il<ObjectType> input) -> size_t;

auto FunctionSetOutput(Function& fn, const Array<ObjectType>& input) -> size_t;

auto FunctionSetOutput(Function& fn, il<ObjectType> input) -> size_t;

auto FunctionSetArgs(Function& fn, const Array<ObjectType>& input) -> size_t;

auto FunctionSetArgs(Function& fn, il<ObjectType> input) -> size_t;

#define _Function_Move_ 1
auto move(Function& dest, Function& src) -> size_t;

#define _Function_Copy_ 1
auto copy(Function& dest, const Function& src) -> size_t;

struct Command
{
    Function*       fn;
    Array<Object> args;
public:
    Command()
    {
        fn   = nullptr;
        ArrayCtor(args);
    }
public:
    void Print() const;
};

/*
struct Function
{
    union FunctionTypes
    {
        CFunction Cfn;
        LFunction Lfn;
    };

    string        name;
    size_t        type; ///0(builtin) or 1(user defined)
    FunctionTypes data;

    Pointer<ObjectType> input;
    Pointer<ObjectType> output;
    Pointer<ObjectType> args;
public:
    const char* error;
public:
    Function()
    {
        this->name = "";
        this->type = 2;
    }
public:
    Function(string name, CFunction fn, il<ObjectType> input_, il<ObjectType> output_, il<ObjectType> args_):
        name  (name),
        input (),
        output(),
        args  ()
    {
        Pointer<ObjectType>::resize(&input , input_ .size());
        Pointer<ObjectType>::resize(&output, output_.size());
        Pointer<ObjectType>::resize(&args  , args_  .size());

              ObjectType* first;
        const ObjectType* second;

        first  = input .start;
        second = input_.begin();
        for(;first != input.finish; ++first, ++second)
        {
            ObjectType::copy(first, second);
        }

        first  = output .start;
        second = output_.begin();
        for(;first != output.finish; ++first, ++second)
        {
            ObjectType::copy(first, second);
        }

        first  = args .start;
        second = args_.begin();
        for(;first != args.finish; ++first, ++second)
        {
            ObjectType::copy(first, second);
        }

        this->type     = 0;
        this->data.Cfn = fn;
    }
public:
    void Print()
    {
        while(true)
        {
            if (this->type == 0)
            {
                fprintf(stdout, "Builtin function \"%s\":\n", this->name.c_str());

                fprintf(stdout, "\tinput:\n");
                for(size_t i = 0; i < input.size(); ++i)
                    fprintf(stdout, "\t\t%d ", (input.start + i)->type);
                fprintf(stdout, "\n");
                fprintf(stdout, "\toutput:\n");
                for(size_t i = 0; i < output.size(); ++i)
                    fprintf(stdout, "\t\t%d ", (output.start + i)->type);
                fprintf(stdout, "\n");
                fprintf(stdout, "\targs:\n");
                for(size_t i = 0; i < args.size(); ++i)
                    fprintf(stdout, "\t\t%d ", (args.start + i)->type);
                fprintf(stdout, "\n");

                fprintf(stdout, "\tfn pointer = %p\n", this->data.Cfn);
                break;
            }
            if (this->type == 1)
            {
                fprintf(stdout, "function \"%s\":\n", this->name.c_str());

                fprintf(stdout, "\tinput:\n");
                for(size_t i = 0; i < input.size(); ++i)
                    fprintf(stdout, "\t\t%d ", (input.start + i)->type);
                fprintf(stdout, "\n");
                fprintf(stdout, "\toutput:\n");
                for(size_t i = 0; i < output.size(); ++i)
                    fprintf(stdout, "\t\t%d ", (output.start + i)->type);
                fprintf(stdout, "\n");
                fprintf(stdout, "\targs:\n");
                for(size_t i = 0; i < args.size(); ++i)
                    fprintf(stdout, "\t\t%d ", (args.start + i)->type);
                fprintf(stdout, "\n");

                fprintf(stdout, "\tcode:\n");
                for(const Command* it = this->data.Lfn.start; it != this->data.Lfn.finish; ++it)
                    it->Print();
            }
        }
    }
};
*/

struct state{
    static const size_t CallStackSize = 256;
    static const size_t DataStackSize = 256;
    static const size_t MemorySize    = 256;
    struct RunState{
        const Command* command;
        const Command* start;
        const Command* finish;
        const Command* current;
    };

    RunState cstack[CallStackSize];
    size_t   csp;
    Object   dstack[DataStackSize];
    size_t   dsp;
    Object   memory[MemorySize];
    int               error;
    int               flags;

    FunctionsTable*   table;
};


#endif // _struct_Command_
