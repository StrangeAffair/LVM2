#include "CallStack.hpp"
#include <cstdlib>

static size_t ElementsPerPage = 4096 / sizeof(CallStack::RunState);

template<class T>
static T** rcalloc(T** PointerList, size_t number, size_t size)
{
    T** temp = (T**) realloc(PointerList, number * size);
    if (temp == nullptr)
        return nullptr;
}

auto CallStack::push() -> RunState*
{
    if (*PageListStart == nullptr)
    {
        {
            RunState** temp = (RunState**) realloc(PageListStart, 1 * sizeof(RunState*));
            if (temp == nullptr)
                return nullptr;
            PageListStart    = temp;
            PageListFinish   = temp + 1;
            PageListCurrent  = temp;

            *PageListCurrent = nullptr;
        }

        RunState* temp = (RunState*) calloc(ElementsPerPage, sizeof(RunState));
        if (temp == nullptr)
            return nullptr;

    }

    if (Current - *PageListCurrent >= ElementsPerPage)
    {
        if (PageListCurrent == PageListFinish)
        {
            size_t size = PageListFinish - PageListStart;
            RunState** temp = (RunState**) realloc(PageListStart, size * 2 * sizeof(RunState*));
            if (temp == nullptr)
                return nullptr;
            if (temp != PageListStart)
            {
                PageListStart   = temp;
                PageListFinish  = temp + size * 2;
                PageListCurrent = temp + size;
            }
        }

        {
            RunState* temp = (RunState*) calloc(ElementsPerPage, sizeof(RunState));
            if (temp == nullptr)
                return nullptr;

            ++PageListCurrent;
            *PageListCurrent = Current = temp;
        }
    }

    ++Current;
    return Current - 1;
}

auto CallStack::pop() -> RunState*
{
    --Current;

    if (Current - *PageListCurrent == 0)
    {
        free(Current);

        --PageListCurrent;
        Current = *PageListCurrent + ElementsPerPage;
    }

    return nullptr;
}

auto CallStack::top() -> RunState*
{
    return Current - 1;
}
