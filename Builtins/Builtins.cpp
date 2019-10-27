#include "Builtins.hpp"
#include "../FunctionsTable/FunctionsTable.hpp"

namespace{
    void CmdConst(state* st, size_t argc, Object* args)
    {
        for(size_t i = 0; i < argc; ++i)
        {
            Object::copy(&st->dstack[st->dsp], &args[i]);
            ++st->dsp;
        }
    }

    /*
    void CmdPrint(state* st, size_t argc, Object* args)
    {
        fprintf(stdout, "Data Stack:\n");
        fprintf(stdout, "\tsize = %d\n", st->dsp);
        for(auto value : st->dstack)
            fprintf(stdout, "\t%" PRId64 "\n", Object::get<int64_t>(value));
    }
    */

    void CmdInc(state* st, size_t argc, Object* args)
    {
        if (argc != 0)
        {
            st->error = 255;
            return;
        }
        Object::get<int64_t>(st->dstack[st->dsp - 1]) += 1;
    }

    void CmdStore(state* st, size_t argc, Object* args)
    {
        if (argc != 1)
        {
            st->error = 255;
            return;
        }
        size_t offset = Object::get<int64_t>(args[0]);
        if (offset >= st->MemorySize)
        {
            st->error = 1;
            return;
        }

        Object::move(&st->memory[offset], &st->dstack[st->dsp - 1]);
        --st->dsp;
    }

    void CmdLoad(state* st, size_t argc, Object* args)
    {
        if (argc != 1)
        {
            st->error = 255;
            return;
        }
        size_t offset = Object::get<int64_t>(args[0]);
        if (offset >= st->MemorySize)
        {
            st->error = 1;
            return;
        }

        Object::copy(&st->dstack[st->dsp], &st->memory[offset]);
        ++st->dsp;
    }

    void CmdAdd(state* st, size_t argc, Object* args)
    {
        if (argc != 0)
        {
            st->error = 255;
            return;
        }

        int64_t left  = Object::get<int64_t>(st->dstack[st->dsp - 2]);
        int64_t right = Object::get<int64_t>(st->dstack[st->dsp - 1]);
        Object::set(st->dstack[st->dsp - 2], left + right);

        --st->dsp;
    }

    void RaiseError(state* st, size_t argc, Object* args)
    {
        if (argc != 1)
        {
            st->error = 255;
            return;
        }
        st->error = Object::get<int64_t>(args[0]);
    }

    void CmdPop(state* st, size_t argc, Object* args)
    {
        if (argc != 0)
        {
            st->error = 255;
            return;
        }
        Object::clear(st->dstack[st->dsp - 1]);
        --st->dsp;
    }

    void CmdJGE(state* st, size_t argc, Object* args)
    {
        if (argc != 1)
        {
            st->error = 255;
            return;
        }
        int64_t left  = Object::get<int64_t>(st->dstack[st->dsp - 2]);
        int64_t right = Object::get<int64_t>(st->dstack[st->dsp - 1]);

        if (left >= right)
        {
            state::RunState& last = st->cstack[st->csp - 2];
            last.current = last.start + Object::get<int64_t>(args[0]);
            st->flags |= 1;
        }

        Object::clear(st->dstack[st->dsp - 2]);
        Object::clear(st->dstack[st->dsp - 1]);
        st->dsp -= 2;
    }

    void CmdJump(state* st, size_t argc, Object* args)
    {
        if (argc != 1)
        {
            st->error = 255;
            return;
        }
        state::RunState& last = st->cstack[st->csp - 2];
        last.current = last.start + Object::get<int64_t>(args[0]);
        st->flags |= 1;
    }

};


namespace Builtins{
    FunctionsTable builtins;

    void Init()
    {
        builtins.insert("const", CmdConst, {}, {ObjectType::Any}, {ObjectType::Any});
        builtins.insert("load" , CmdLoad , {}, {ObjectType::Any}, {ObjectType::Size});
        builtins.insert("store", CmdStore, {ObjectType::Any}, {}, {ObjectType::Size});

        builtins.insert("pop", CmdPop, {ObjectType::Any}, {}, {});
        builtins.insert("add", CmdAdd, {ObjectType::Int64, ObjectType::Int64}, {ObjectType::Int64}, {});
        builtins.insert("inc", CmdInc, {ObjectType::Int64}, {ObjectType::Int64}, {});

        builtins.insert("jge",  CmdJGE , {}, {}, {ObjectType::Size});
        builtins.insert("jump", CmdJump, {}, {}, {ObjectType::Size});

        builtins.insert("error", RaiseError, {}, {ObjectType::Int64}, {});
    }

    Function* find(string name, il input, il output, il args)
    {
        static bool flag = true;
        if (flag)
        {
            Init();
            flag = false;
        }
        return builtins.find(name, input, output, args);
    }

};
