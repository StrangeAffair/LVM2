#ifndef _CallStack_hpp_
#define _CallStack_hpp_ 1

#include "../Command/Command.hpp"

class CallStack
{
public:
    struct RunState
    {
        const Command* cmd;
        const Command* current;
    };
public:
    CallStack()
    {
        this->PageListStart   = nullptr;
        this->PageListFinish  = nullptr;
        this->PageListCurrent = nullptr;
        this->Current         = nullptr;
    }
public:
    RunState* push();
    RunState* pop ();
    RunState* top ();
private:
    RunState** PageListStart;
    RunState** PageListFinish;
    RunState** PageListCurrent;
    RunState*  Current;
};

#endif // _CallStack_hpp_
