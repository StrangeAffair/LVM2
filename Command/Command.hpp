#ifndef _struct_Command_
#define _struct_Command_ 1

#include <vector>
#include <string>
#include <exception>
#include <initializer_list>
#include <memory>

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

using  CFunction = void (*)(state* st, size_t argc, Object* argv);

struct LFunction
{
    Command* start;
    Command* finish;
public:
    static LFunction* ctor(LFunction* dest) noexcept;
    static LFunction* ctor(LFunction* dest, size_t size) noexcept;
    static LFunction* dtor(LFunction* dest) noexcept;
public:
    static LFunction* copy(LFunction* dest, const LFunction* src) noexcept;
    static LFunction* move(LFunction* dest,       LFunction* src) noexcept;
public:
    size_t size() const;
};

struct FunctionBase
{
    FunctionBase() = default;

    FunctionBase(const FunctionBase& other) = delete;
    FunctionBase(FunctionBase&& other) = delete;
    FunctionBase& operator=(const FunctionBase&) = delete;

    enum  FunctionType
    {
        none_t      = 0,
        CFunction_t = 1,
        LFunction_t = 2,
        opcode_t    = 3
    };
    union FunctionData
    {
        CFunction Cfn;
        LFunction Lfn;
        size_t    opcode;
    };
    FunctionType type;
    FunctionData data;
public:
    static const FunctionBase* ctor(FunctionBase& fn) noexcept;
    static const FunctionBase* dtor(FunctionBase& fn) noexcept;
public:
    static const FunctionBase* copy(FunctionBase& dest, const FunctionBase& src) noexcept;
    static const FunctionBase* move(FunctionBase& dest,       FunctionBase& src) noexcept;
public:
    //throw bad_cast
    template<class T>
    static auto get(const FunctionBase* ptr) noexcept -> const T*;
public:
    //typename std::enable_if<!std::is_trivially_copy_constructible<T>::value, bool>::type = true
    //copy setter
    template<class T>
    static auto copy(FunctionBase* ptr, const T& data) noexcept -> const T*;
    //move setter
    template<class T>
    static auto move(FunctionBase* ptr,       T& data) noexcept -> const T*;
    /*
    template<class T, typename std::enable_if< std::is_trivially_copy_constructible<T>::value, bool>::type = true>
    static auto set(FunctionBase* ptr,       T   data) noexcept -> const T*;
    template<class T, typename std::enable_if<!std::is_trivially_copy_constructible<T>::value, bool>::type = true>
    static auto set(FunctionBase* ptr, const T&  data) noexcept -> const T*;

    template<class T>
    static auto set(FunctionBase* ptr,       T&& data) noexcept -> const T*;
    */
public:
    //should return int... to give error
    //base function
    template<class T>
    static void call(const FunctionBase* fn, const Array<Object>* args, state* st) noexcept;
    //main function
    static void call(const FunctionBase* fn, const Array<Object>* args, state* st) noexcept;
};

//uses main function
void call(const Command* cmd, state* st);


struct FunctionSignature
{
    FunctionSignature() = default;

    FunctionSignature(const FunctionSignature& other) = delete;
    FunctionSignature(FunctionSignature&& other) = delete;
    FunctionSignature& operator=(const FunctionSignature&) = delete;

    char*             name;
    Array<ObjectType> ilist;
    Array<ObjectType> olist;
    Array<ObjectType> alist;
public:
    //throws bad_alloc
    static auto ctor(FunctionSignature& signature) noexcept -> const FunctionSignature*;
    static auto dtor(FunctionSignature& signature) noexcept -> const FunctionSignature*;
public:
    static auto copy(FunctionSignature& dest, const FunctionSignature& src) noexcept -> const FunctionSignature*;
    static auto move(FunctionSignature& dest,       FunctionSignature& src) noexcept -> const FunctionSignature*;
public:
    bool IsEmpty() noexcept
    {return (this->name == nullptr);}
public:
    bool IsEqual(const FunctionSignature& right) const noexcept;
    bool IsEqual(const char* name, il<ObjectType> ilist, il<ObjectType> olist, il<ObjectType> alist) const noexcept;
public:
    static bool IsEqual(const Array<ObjectType>& left, const Array<ObjectType>& right) noexcept;
    static bool IsEqual(const Array<ObjectType>& left,          il<ObjectType>  right) noexcept;
};

bool compare(const Array<ObjectType>& left, const Array<ObjectType>& right);

bool compare(const Array<ObjectType>& left, il<ObjectType> right);

bool compare(const FunctionSignature& left, const FunctionSignature& right);

bool compare(const FunctionSignature& left, const char* name, il<ObjectType> ilist, il<ObjectType> olist, il<ObjectType> alist);

struct Function
{
    Function() = default;

    Function(const Function& other) = delete;
    Function(Function&& other) = delete;
    Function& operator=(const Function&) = delete;

    FunctionSignature signature;
    FunctionBase      base;
public:
    static auto ctor(Function& fn) noexcept -> const Function*;
    static auto dtor(Function& fn) noexcept -> const Function*;
public:
    static auto move(Function& dest,       Function& src) noexcept -> const Function*;
    static auto copy(Function& dest, const Function& src) noexcept -> const Function*;
public:
    auto SetName(const char* name) noexcept -> const char*;
    auto GetName()           const noexcept -> const char*;
public:
    auto SetSignature(const FunctionSignature& signature) noexcept -> const FunctionSignature*;
    auto GetSignature()                             const noexcept -> const FunctionSignature*;
public:
    auto SetSignature(const char* name, il<ObjectType> ilist, il<ObjectType> olist, il<ObjectType> alist) -> const FunctionSignature*;
    auto SetFunction (CFunction  data) noexcept -> const FunctionBase*;
    auto SetFunction (LFunction& data) noexcept -> const FunctionBase*;
    auto SetFunction (size_t     data) noexcept -> const FunctionBase*;
};

char* StringCopy(const char* name);

struct Command
{
    Function*     fn;
    Array<Object> args;
public:
    Command()
    {
        fn   = nullptr;
        ArrayCtor(args);
    }
public:
    void Print() const;
public:
    static auto ctor(Command* cmd) -> Command*;
    static auto dtor(Command* cmd) -> Command*;
public:
    static auto copy(Command& dest, const Command& src) -> Command*;
    static auto move(Command& dest,       Command& src) -> Command*;
};

void PrintFunction(const Function& fn);

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
    int      error;

    const FunctionsTable*   table;
};


#endif // _struct_Command_
