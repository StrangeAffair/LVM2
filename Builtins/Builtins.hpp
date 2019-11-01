#ifndef _Builtins_
#define _Builtins_ 1

#include "../Command/Command.hpp"
#include <initializer_list>

namespace Builtins
{
    using il = std::initializer_list<ObjectType>;
    using std::string;

    extern FunctionsTable builtins;
    extern Function* find(const char* name, il input, il output, il args);
};

#endif // _Builtins_
