#include "Command.hpp"
#include "../FunctionsTable/FunctionsTable.hpp"

LFunction* LFunction::ctor(LFunction* dest) noexcept
{
    dest->start  = nullptr;
    dest->finish = nullptr;
    return dest;
}

LFunction* LFunction::ctor(LFunction* dest, size_t size) noexcept
{
    Command* pointer = (Command*) calloc(size, sizeof(Command));
    if (pointer == nullptr)
    {
        LFunction::ctor(dest);
        return nullptr;
    }

    for(size_t i = 0; i < size; ++i)
        Command::ctor(pointer + i);

    dest->start  = pointer;
    dest->finish = pointer + size;
    return dest;
}

LFunction* LFunction::dtor(LFunction* dest) noexcept
{
    if (dest->start != nullptr)
        free(dest->start);
    dest->start  = nullptr;
    dest->finish = nullptr;
    return nullptr;
}

LFunction* LFunction::copy(LFunction* dest, const LFunction* src) noexcept
{
    if (dest->start != nullptr)
        if (LFunction::dtor(dest) != nullptr)
            return nullptr;

    if (LFunction::ctor(dest, src->size()) == nullptr)
        return nullptr;

    const Command* start  =  src->start;
    const Command* finish =  src->finish;
          Command* result = dest->start;
    for(const Command* it = start; it != finish; ++it)
    {
        Command::copy(*result, *it);
        ++result;
    }
    return dest;
}

LFunction* LFunction::move(LFunction* dest, LFunction* src) noexcept
{
    dest->start  = src->start;
    dest->finish = src->finish;
    src->start  = nullptr;
    src->finish = nullptr;
    return dest;
}

size_t LFunction::size() const
{return finish - start;}

template<>
auto FunctionBase::get<CFunction>(const FunctionBase* ptr) noexcept -> const CFunction*
{
    if (ptr->type != FunctionBase::CFunction_t)
        return nullptr;
    return &ptr->data.Cfn;
}

template<>
auto FunctionBase::get<LFunction>(const FunctionBase* ptr) noexcept -> const LFunction*
{
    if (ptr->type != FunctionBase::LFunction_t)
        return nullptr;
    return &ptr->data.Lfn;
}

template<>
auto FunctionBase::get<size_t>(const FunctionBase* ptr) noexcept  -> const size_t*
{
    if (ptr->type != FunctionBase::opcode_t)
        return nullptr;
    return &ptr->data.opcode;
}


auto FunctionBase::ctor(FunctionBase& ptr) noexcept -> const FunctionBase*
{
    ptr.type = FunctionBase::none_t;
    memset(&ptr.data, 0, sizeof(FunctionBase::FunctionData));

    return &ptr;
}

//do not call this function
auto FunctionBase::dtor(FunctionBase& ptr) noexcept -> const FunctionBase*
{
    if (ptr.type == FunctionBase::LFunction_t)
        LFunction::dtor(&ptr.data.Lfn);
    return nullptr;
}



/*
template<>
auto set<LFunction>(FunctionBase* ptr, LFunction&& data) -> const LFunction*
{
    ptr->type = FunctionBase::LFunction_t;
    ptr->data.Lfn.start  = data->start;
    ptr->data.Lfn.finish = data->finish;
    data->start  = nullptr;
    data->finish = nullptr;
    return &ptr->data.Lfn;
}

template<>
auto set<CFunction>(FunctionBase* ptr, CFunction data) -> const CFunction*
{
    ptr->type     = FunctionBase::CFunction_t;
    ptr->data.Cfn = data;
    return &ptr->data.Cfn;
}

template<>
auto set<size_t>(FunctionBase* ptr, size_t data) -> const size_t*
{
    ptr->type        = FunctionBase::opcode_t;
    ptr->data.opcode = data;
    return &ptr->data.opcode;
}
*/

template<>
auto FunctionBase::move<LFunction>(FunctionBase* ptr, LFunction& data) noexcept -> const LFunction*
{
    return LFunction::move(&ptr->data.Lfn, &data);
}

template<>
auto FunctionBase::copy<LFunction>(FunctionBase* ptr, const LFunction& data) noexcept -> const LFunction*
{
    ptr->type = FunctionBase::LFunction_t;
    return LFunction::copy(&ptr->data.Lfn, &data);
}

template<>
auto FunctionBase::copy<CFunction>(FunctionBase* ptr, const CFunction& data) noexcept -> const CFunction*
{
    ptr->type     = FunctionBase::CFunction_t;
    ptr->data.Cfn = data;
    return &ptr->data.Cfn;
}

template<>
auto FunctionBase::copy<size_t>(FunctionBase* ptr, const size_t& data) noexcept -> const size_t*
{
    ptr->type        = FunctionBase::opcode_t;
    ptr->data.opcode = data;
    return &ptr->data.opcode;
}


auto FunctionBase::copy(FunctionBase& dest, const FunctionBase& src) noexcept -> const FunctionBase*
{
    switch(src.type)
    {
    case CFunction_t:
        copy<CFunction>(&dest, src.data.Cfn);
        return &dest;
    case LFunction_t:
        copy<LFunction>(&dest, src.data.Lfn);
        return &dest;
    case opcode_t:
        copy<size_t>(&dest, src.data.opcode);
        return &dest;
    default:
        return nullptr;
    }
}

auto FunctionBase::move(FunctionBase& dest, FunctionBase& src) noexcept -> const FunctionBase*
{
    switch(src.type)
    {
    case CFunction_t:
        copy<CFunction>(&dest, src.data.Cfn);
        return &dest;
    case LFunction_t:
        move<LFunction>(&dest, src.data.Lfn);
        return &dest;
    case opcode_t:
        copy<size_t>   (&dest, src.data.opcode);
        return &dest;
    default:
        return nullptr;
    }
}

template<>
void FunctionBase::call<CFunction>(const FunctionBase* fn, const Array<Object>* args, state* st) noexcept
{
    state::RunState temp;
    temp.command = nullptr;
    temp.start   = nullptr;
    temp.finish  = nullptr;
    temp.current = nullptr;
    /* library implementer can redefine this params in his Cfn */

    st->cstack[st->csp] = temp;
    ++st->csp;

    const CFunction& Cfn = *(get<CFunction>(fn));

    //fprintf(stdout, "call of \"%s\"\n", cmd->fn->name.c_str());

    if (args != nullptr)
        Cfn(st, (*args).size(), args->start);
    else
        Cfn(st, 0, nullptr);
    //cmd->fn->data.Cfn(st, cmd->fn->args.size(), cmd->args.start);
    if (st->error)
        return;

    --st->csp;
}

template<>
void FunctionBase::call<LFunction>(const FunctionBase* fn, const Array<Object>* args, state* st) noexcept
{
    if (args != nullptr)
    {
        st->error = 255;
        return;
    }

    const LFunction& Lfn = *(get<LFunction>(fn));

    state::RunState temp;
    temp.command  = nullptr;
    temp.start    = Lfn.start;
    temp.finish   = Lfn.finish;
    temp.current  = Lfn.start;

    st->cstack[st->csp] = temp;
    ++st->csp;

    //fprintf(stdout, "call of \"%s\"\n", cmd->fn->name.c_str());

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

        last.current->Print();
        if (last.current->fn->base.type != FunctionBase::opcode_t)
        {
            if (last.current->args.size() != 0)
                call(&last.current->fn->base, &last.current->args, st);
            else
                call(&last.current->fn->base, nullptr, st);

            if (st->error)
                return;

            if (last.current == last.finish)
                break;
            ++last.current;
        }
        else
        {
            if (last.current->args.size() != 0)
                call(&last.current->fn->base, &last.current->args, st);
            else
                call(&last.current->fn->base, nullptr, st);

            if (st->error)
                return;
            if (last.current == last.finish)
                break;
        }
    }

    --st->csp;
}

template<>
void FunctionBase::call<size_t>(const FunctionBase* fn, const Array<Object>* args, state* st) noexcept
{
    size_t opcode = *(FunctionBase::get<size_t>(fn));
    switch(opcode)
    {
        //jump
        case 0x30:
        {
            if (args->size() != 1)
            {
                st->error = 255;
                return;
            }

            state::RunState& last = st->cstack[st->csp - 1];
            last.current = last.start + Object::get<int64_t>(args->start[0]);
            break;
        }
        //jge
        case 0x35:
        {
            if (args->size() != 1)
            {
                st->error = 255;
                return;
            }
            int64_t left  = Object::get<int64_t>(st->dstack[st->dsp - 2]);
            int64_t right = Object::get<int64_t>(st->dstack[st->dsp - 1]);

            state::RunState& last = st->cstack[st->csp - 1];
            if (left >= right)
                last.current = last.start + Object::get<int64_t>(args->start[0]);
            else
                last.current++;

            Object::clear(st->dstack[st->dsp - 2]);
            Object::clear(st->dstack[st->dsp - 1]);
            st->dsp -= 2;
            break;
        }
        default:
            st->error = 255;
            return;
    }
}

void FunctionBase::call(const FunctionBase* fn, const Array<Object>* args, state* st) noexcept
{
    switch(fn->type)
    {
        case FunctionBase::CFunction_t:
            call<CFunction>(fn, args, st);
            break;
        case FunctionBase::LFunction_t:
            call<LFunction>(fn, args, st);
            break;
        case FunctionBase::opcode_t:
            call<size_t>   (fn, args, st);
            break;
        default:
            st->error = 254;
            return;
    }
}

auto FunctionSignature::ctor(FunctionSignature& signature) noexcept -> const FunctionSignature*
{
    signature.name = nullptr;
    ArrayCtor(signature.ilist);
    ArrayCtor(signature.olist);
    ArrayCtor(signature.alist);
    return &signature;
}

auto FunctionSignature::dtor(FunctionSignature& signature) noexcept -> const FunctionSignature*
{
    free(signature.name);
    ArrayDtor(signature.ilist);
    ArrayDtor(signature.olist);
    ArrayDtor(signature.alist);
    return nullptr;
}

bool IsEmpty(const FunctionSignature& signature)
{
    return (signature.name == nullptr);
}

void clear  (      FunctionSignature& signature)
{
    signature.name = nullptr;
    ArrayDtor(signature.ilist);
    ArrayDtor(signature.olist);
    ArrayDtor(signature.alist);
}

char* StringCopy(const char* src)
{
    char* retval;
    if (src == nullptr)
        return nullptr;
    retval = (char*) malloc(strlen(src) + 1);
    memcpy(retval, src, strlen(src) + 1);
    return retval;
}

auto FunctionSignature::copy(FunctionSignature& dest, const FunctionSignature& src) noexcept -> const FunctionSignature*
{
    if (dest.name != nullptr)
        FunctionSignature::dtor(dest);

    dest.name = StringCopy(src.name);
    ArrayCopy(dest.ilist, src.ilist);
    ArrayCopy(dest.olist, src.olist);
    ArrayCopy(dest.alist, src.alist);
    return &dest;
}

auto FunctionSignature::move(FunctionSignature& dest, FunctionSignature& src) noexcept -> const FunctionSignature*
{
    if (dest.name != nullptr)
        FunctionSignature::dtor(dest);

    dest.name = src.name;
    src .name = nullptr;

    dest.ilist.start  = src.ilist.start;
    dest.ilist.finish = src.ilist.finish;

    src.ilist.start  = nullptr;
    src.ilist.finish = nullptr;

    dest.olist.start  = src.olist.start;
    dest.olist.finish = src.olist.finish;

    src.olist.start  = nullptr;
    src.olist.finish = nullptr;

    dest.alist.start  = src.alist.start;
    dest.alist.finish = src.alist.finish;

    src.alist.start  = nullptr;
    src.alist.finish = nullptr;

    return &dest;
}

bool FunctionSignature::IsEqual(const Array<ObjectType>& left, const Array<ObjectType>& right) noexcept
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

bool compare(const Array<ObjectType>& left, const Array<ObjectType>& right)
{
    return FunctionSignature::IsEqual(left, right);
}

bool FunctionSignature::IsEqual(const Array<ObjectType>& left, il<ObjectType> right) noexcept
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

bool compare(const Array<ObjectType>& left, il<ObjectType> right)
{
    return FunctionSignature::IsEqual(left, right);
}


bool FunctionSignature::IsEqual(const FunctionSignature& right) const noexcept
{
    size_t llength = (this->name == nullptr ? 0 : strlen(this->name));
    size_t rlength = (right.name == nullptr ? 0 : strlen(right.name));
    size_t  length = std::min(llength, rlength);
    if (strncmp(this->name, right.name, length) != 0)
        return false;
    if (compare(this->ilist, right.ilist) == false)
        return false;
    if (compare(this->olist, right.olist) == false)
        return false;
    if (compare(this->alist, right.alist) == false)
        return false;
    return true;
}

bool compare(const FunctionSignature& left, const FunctionSignature& right)
{
    return left.IsEqual(right);
}

bool FunctionSignature::IsEqual(const char* name, il<ObjectType> ilist, il<ObjectType> olist, il<ObjectType> alist) const noexcept
{
    size_t llength = (this->name == nullptr ? 0 : strlen(this->name));
    size_t rlength = (      name == nullptr ? 0 : strlen(      name));
    size_t  length = std::min(llength, rlength);
    if (strncmp(this->name, name, length) != 0)
        return false;
    if (compare(this->ilist, ilist) == false)
        return false;
    if (compare(this->olist, olist) == false)
        return false;
    if (compare(this->alist, alist) == false)
        return false;
    return true;
}

bool compare(const FunctionSignature& left, const char* name, il<ObjectType> ilist, il<ObjectType> olist, il<ObjectType> alist)
{
    return left.IsEqual(name, ilist, olist, alist);
}


//Function
auto Function::ctor(Function& fn) noexcept -> const Function*
{
    if (FunctionSignature::ctor(fn.signature) == nullptr)
        return nullptr;
    if (FunctionBase     ::ctor(fn.base)      == nullptr)
        return nullptr;
    return &fn;
}

auto Function::dtor(Function& fn) noexcept -> const Function*
{
    if (FunctionSignature::dtor(fn.signature) != nullptr)
        return &fn;
    if (FunctionBase     ::dtor(fn.base)      != nullptr)
        return &fn;
    return nullptr;
}

auto Function::move(Function& dest, Function& src) noexcept -> const Function*
{
    if (!IsEmpty(dest.signature))
        Function::dtor(dest);

    if (FunctionSignature::move(dest.signature, src.signature) == nullptr)
        return nullptr;
    if (FunctionBase     ::move(dest.base     , src.base)      == nullptr)
        return nullptr;
    return &dest;
}

auto Function::copy(Function& dest, const Function& src) noexcept -> const Function*
{
    if (!IsEmpty(dest.signature))
        Function::dtor(dest);

    if (FunctionSignature::copy(dest.signature, src.signature) == nullptr)
        return nullptr;
    if (FunctionBase     ::copy(dest.base     , src.base)      == nullptr)
        return nullptr;
    return &dest;
}

auto Function::SetName(const char* name) noexcept -> const char*
{
    return (this->signature.name = StringCopy(name));
}

auto Function::GetName() const noexcept -> const char*
{
    return (this->signature.name);
}

auto Function::SetFunction(CFunction fn) noexcept -> const FunctionBase*
{
    if (FunctionBase::copy<CFunction>(&(this->base), fn) == nullptr)
        return nullptr;
    else
        return &(this->base);
}

auto Function::SetFunction(LFunction& fn) noexcept -> const FunctionBase*
{
    if (FunctionBase::move<LFunction>(&(this->base), fn) == nullptr)
        return nullptr;
    else
        return &(this->base);
}

auto Function::SetFunction(size_t opcode) noexcept -> const FunctionBase*
{
    if (FunctionBase::copy<size_t>(&(this->base), opcode) == nullptr)
        return nullptr;
    else
        return &(this->base);
}

auto Function::SetSignature(const FunctionSignature& sig) noexcept -> const FunctionSignature*
{
    this->signature.name  = sig.name;
    this->signature.ilist = sig.ilist;
    this->signature.olist = sig.olist;
    this->signature.alist = sig.alist;
    return &(this->signature);
}

auto Function::SetSignature(const char* name, il<ObjectType> ilist, il<ObjectType> olist, il<ObjectType> alist) -> const FunctionSignature*
{
    if (this->signature.name != nullptr)
    {
        ArrayDtor(this->signature.ilist);
        ArrayDtor(this->signature.olist);
        ArrayDtor(this->signature.alist);
    }

    this->signature.name = StringCopy(name);
    ArrayFromIL(this->signature.ilist, ilist);
    ArrayFromIL(this->signature.olist, olist);
    ArrayFromIL(this->signature.alist, alist);
    return &(this->signature);
}

void call(const Command* cmd, state* st)
{
    if (cmd->args.size() != 0)
        FunctionBase::call(&cmd->fn->base, &cmd->args, st);
    else
        FunctionBase::call(&cmd->fn->base, nullptr, st);
    /*
    switch(cmd->fn.base->type)
    {
        case 0:
        {
            // default constructor
            state::RunState temp;
            temp.command = cmd;
            temp.start   = nullptr;
            temp.finish  = nullptr;
            temp.current = nullptr;
            // library implementer can redefine this params in his Cfn

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

            // be carefull. call stack should provide iterator safety
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
    */
}

auto Command::ctor(Command* cmd) -> Command*
{
    cmd->fn = nullptr;
    ArrayCtor(cmd->args);
    return cmd;
}

auto Command::dtor(Command* cmd) -> Command*
{
    cmd->fn = nullptr;
    ArrayDtor(cmd->args);
    return nullptr;
}

auto Command::copy(Command& dest, const Command& src) -> Command*
{
    dest.fn = src.fn;
    ArrayCtor(dest.args, src.args.size());
    for(size_t i = 0; i < src.args.size(); ++i)
        Object::copy(&dest.args.start[i], &src.args.start[i]);
    return &dest;
}

auto Command::move(Command& dest, Command& src) -> Command*
{
    dest.fn = src.fn;
    ArrayCtor(dest.args, src.args.size());
    for(size_t i = 0; i < src.args.size(); ++i)
        Object::move(&dest.args.start[i], &src.args.start[i]);
    return &dest;
}

#ifndef _Debug_PrintFunction_
#define _Debug_PrintFunction_ 1
void PrintFunction(const Function& fn)
{
    fprintf(stdout, "Printing function at address %p:\n", &fn);
    fprintf(stdout, "\tname = %s\n" , fn.GetName());

    const FunctionBase& base = fn.base;
    if (base.type == FunctionBase::CFunction_t)
    {
        fprintf(stdout, "\ttype = %d (CFunction_t)\n", base.type);
        const CFunction& Cfn = *(FunctionBase::get<CFunction>(&base));
        fprintf(stdout, "\tdata = %p\n", Cfn);
    }
    if (base.type == FunctionBase::LFunction_t)
    {
        fprintf(stdout, "\ttype = %d (LFunction_t)\n", base.type);
        const LFunction& Lfn = *(FunctionBase::get<LFunction>(&base));
        fprintf(stdout, "\tdata = %p, %p\n", Lfn.start, Lfn.finish);
    }
    if (base.type == FunctionBase::opcode_t)
    {
        fprintf(stdout, "\ttype = %d (opcode_t)\n", base.type);
        const size_t& opcode = *(FunctionBase::get<size_t>(&base));
        fprintf(stdout, "\topcode = 0x%x\n", opcode);
    }
}
#endif // _Debug_PrintFunction_

void Command::Print() const
{
    fprintf(stdout, "Command:\n");
    PrintFunction(*(this->fn));
    //fprintf(stdout, "\tname = \"%s\"\n", this->fn->signature.name);
    fprintf(stdout, "\targs:\n");
    for(size_t i = 0; i < args.size(); ++i)
    {
        Object* pointer = this->args.start;
        if (pointer[i].type == ObjectType::Int64)
            fprintf(stdout, "\t\targ[%" PRIu32 "] = %" PRIi64 "\n", i, *(int64_t*)  pointer[i].data);
        //if (pointer[i].type == ObjectType::Size)
            //fprintf(stdout, "\t\targ[%" PRIu32 "] = %" PRIu32 "\n", i, *(uint32_t*) pointer[i].data);
    }
}
