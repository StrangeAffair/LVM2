#include "Command.hpp"
#include "../FunctionsTable/FunctionsTable.hpp"

auto FunctionCtor(Function& fn) -> size_t
{
    fn.name.clear();
    fn.type = 2;
    fn.data.Lfn.start  = nullptr;
    fn.data.Lfn.finish = nullptr;

    ArrayCtor(fn.input);
    ArrayCtor(fn.output);
    ArrayCtor(fn.args);
    return 0;
}

auto FunctionDtor(Function& fn) -> size_t
{
    if (fn.type == 1)
    {
        if (fn.data.Lfn.start != nullptr)
            free((void*) fn.data.Lfn.start);
    }

    fn.name.clear();
    fn.type = 2;

    fn.data.Lfn.start  = nullptr;
    fn.data.Lfn.finish = nullptr;

    ArrayDtor(fn.input);
    ArrayDtor(fn.output);
    ArrayDtor(fn.args);
    return 0;
}

auto FunctionMove(Function& dest, Function& src) -> size_t
{
    if (dest.name.size() != 0)
        FunctionDtor(dest);

    dest.name = src.name;
    dest.type = src.type;

    src.name.clear();
    src.type = 2;

    switch(dest.type)
    {
        case 0:
            dest.data.Cfn = src.data.Cfn;
            break;
        case 1:
            dest.data.Lfn.start  = src.data.Lfn.start;
            dest.data.Lfn.finish = src.data.Lfn.finish;
            src.data.Lfn.start  = nullptr;
            src.data.Lfn.finish = nullptr;
            break;
        default:
            dest.data.Lfn.start  = nullptr;
            dest.data.Lfn.finish = nullptr;
    }

    dest.input.start  = src.input.start;
    dest.input.finish = src.input.finish;

    src.input.start  = nullptr;
    src.input.finish = nullptr;

    dest.output.start  = src.output.start;
    dest.output.finish = src.output.finish;

    src.output.start  = nullptr;
    src.output.finish = nullptr;

    dest.args.start  = src.args.start;
    dest.args.finish = src.args.finish;

    src.args.start  = nullptr;
    src.args.finish = nullptr;
    return 0;
}

auto FunctionSetName(Function& fn, const char* name) -> size_t
{
    fn.name = name;
    return 0;
}

auto FunctionGetName(const Function& fn) -> const char*
{
    return fn.name.c_str();
}

auto FunctionSetFunction(Function& F, CFunction fn) -> void
{
    F.type     = 0;
    F.data.Cfn = fn;
}

auto FunctionSetFunction(Function& F, LFunction fn) -> void
{
    F.type     = 1;
    F.data.Lfn = fn;
}

auto FunctionSetInput(Function& fn, const Array<ObjectType>& input) -> size_t;

auto FunctionSetInput(Function& fn, il<ObjectType> input) -> size_t
{
    return IL_ObjT_2_Arr_ObjT(input, fn.input);
}

auto FunctionSetOutput(Function& fn, const Array<ObjectType>& input) -> size_t;

auto FunctionSetOutput(Function& fn, il<ObjectType> output) -> size_t
{
    return IL_ObjT_2_Arr_ObjT(output, fn.output);
}

auto FunctionSetArgs(Function& fn, const Array<ObjectType>& input) -> size_t;

auto FunctionSetArgs(Function& fn, il<ObjectType> args) -> size_t
{
    return IL_ObjT_2_Arr_ObjT(args, fn.args);
}


void call(const Command* cmd, state* st)
{
    switch(cmd->fn->type)
    {
        case 0:
        {
            /* default constructor */
            state::RunState temp;
            temp.command = cmd;
            temp.start   = nullptr;
            temp.finish  = nullptr;
            temp.current = nullptr;
            /* library implementer can redefine this params in his Cfn */

            st->cstack[st->csp] = temp;
            ++st->csp;

            fprintf(stdout, "call of \"%s\"\n", cmd->fn->name.c_str());

            cmd->fn->data.Cfn(st, cmd->fn->args.size(), cmd->args.start);
            if (st->error)
                return;

            --st->csp;
            break;
        }
        case 1:
        {
            if (cmd->args.size() != 0)
            {
                st->error = 255;
                return;
            }

            LFunction& Lfn = cmd->fn->data.Lfn;

            state::RunState temp;
            temp.command  = cmd;
            temp.start    = Lfn.start;
            temp.finish   = Lfn.finish;
            temp.current  = Lfn.start;

            st->cstack[st->csp] = temp;
            ++st->csp;

            fprintf(stdout, "call of \"%s\"\n", cmd->fn->name.c_str());

            /* be carefull. call stack should provide iterator safety */
            while(true)
            {
                state::RunState& last = st->cstack[st->csp - 1];

                if (last.current < last.start || last.current > last.finish)
                {
                    st->error = 254;
                    return;
                }

                if (last.current == last.finish)
                    break;

                call(last.current, st);
                if (st->error)
                    return;

                if ((st->flags & 1) == 0)
                {
                    ++last.current;
                }
                else
                {
                    st->flags ^= 1;
                }
            }

            --st->csp;
            break;
        }
        default:
        {
            st->error = 255;
            break;
        }
    }
}

#ifndef _Debug_PrintFunction_
#define _Debug_PrintFunction_ 1
void PrintFunction(const Function& fn)
{
    fprintf(stdout, "Printing function at address %p:\n", &fn);
    fprintf(stdout, "\tname = %s" , FunctionGetName(fn));
    fprintf(stdout, "\ttype = %d", fn.type);
    if (fn.type == 0)
        fprintf(stdout, "\tdata = %p", fn.data.Cfn);
    if (fn.type == 1)
        fprintf(stdout, "\tdata = %p, %p", fn.data.Lfn.start, fn.data.Lfn.finish);
}
#endif // _Debug_PrintFunction_

void Command::Print() const
{
    fprintf(stdout, "Command:\n");
    fprintf(stdout, "\tname = \"%s\"\n", this->fn->name.c_str());
    fprintf(stdout, "\targs:\n");
    for(size_t i = 0; i < args.size(); ++i)
    {
        Object* pointer = this->args.start;
        if (pointer[i].type == ObjectType::Int64)
            fprintf(stdout, "\t\targ[%" PRIu32 "] = %" PRIi64 "\n", i, *(int64_t*)  pointer[i].data);
        if (pointer[i].type == ObjectType::Size)
            fprintf(stdout, "\t\targ[%" PRIu32 "] = %" PRIu32 "\n", i, *(uint32_t*) pointer[i].data);
    }
}
