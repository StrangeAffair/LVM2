#include "FunctionsTable.hpp"

auto IL_ObjT_2_Arr_ObjT(il<ObjectType> in, Array<ObjectType>& out) -> size_t
{
    ObjectType* pointer = (ObjectType*) calloc(in.size(), sizeof(ObjectType));
    if (pointer == nullptr)
        return 1;

    out.start  = pointer;
    out.finish = pointer + in.size();

    const ObjectType* first  = in .begin();
          ObjectType* second = out.start;
    while(first != in.end())
    {
        new (second) ObjectType(*first);
        ++first; ++second;
    }
    return 0;
}
