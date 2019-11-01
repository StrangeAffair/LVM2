#include "Parser.hpp"
#include <cstdio>

/**
    function signature:
        auto <name>(<args>) -> const char*

    what this provides?
    if no error heppens than we return nullptr
    we can return "bad alloc" if allocator faild
    we can return "type mismath" if token has other type
    ...
    and we can return inner errors

    this is like returning size_t or int, but more comfortable for writing
---
    if we need error with some info that uses states, than we use error varible of class Parser
*/

//we need function name as info
void Parser::import(const char* name, Builtins::il input, Builtins::il output, Builtins::il args)
{
    Function* fn = Builtins::find(name, input, output, args);
    if (fn == nullptr)
    {
        error = string("Could not find builtin function") + name;
        return;
    }
    else
    {
        this->output.insert(fn);
        fprintf(stdout, "imported %s\n", fn->GetName());
    }
}

//why argument called retval see below
bool Parser::CheckError(const char* retval) const
{
    if (retval != nullptr)
        return true;
    if (error  != "")
        return true;
    return false;
}

void Parser::PrintError(const char* retval) const
{
    const Token* temp;
    if (Get(0, temp) == nullptr)
        fprintf(stderr, "[%d, %d] ", temp->line, temp->column);

    if (retval != nullptr)
        fprintf(stderr, "error = %s\n", retval);
    if (error  != "")
        fprintf(stderr, "error = %s\n", error.c_str());
}

/*
bool Parser::CheckError(const Token* t) const
{
    if (error != "")
    {
        fprintf(stderr, "Parser Error:\n");
        fprintf(stderr, "\t%s\n", error.c_str());
        //t can be nullptr if we raise error in method Get(size_t)...
        if (t != nullptr)
            fprintf(stderr, "\tat position (%d, %d)\n", t->line, t->column);
        return true;
    }
    else
    {
        return false;
    }
}
*/

auto Parser::Get(size_t n, Token const *& t) const -> const char*
{
    if (position + n >= length)
    {
        t = nullptr;
        return "out of vector<Token> range";
    }
    t = &input[position + n];
    return nullptr;
}

/*
auto Parser::Get(size_t n) -> const Token*
{
    if (position + n >= length)
    {
        error = "out of vector<Token> range";
        return nullptr;
    }
    return &input[position + n];
}
*/
/*
const Token* Parser::Get(size_t n) const
{
    if (position + n >= length)
    {
        //error = "out of vector<Token> range";
        return nullptr;
    }
    return &input[position + n];
}
*/
auto Parser::Skip(size_t n) -> const char*
{
    position += n;
    if (position == length)
        return "end of vector<Token>";
    if (position >  length)
        return "out of vector<Token> range";
    return nullptr;
}

auto Parser::Next() -> const char*
{
    return Skip(1);
}

/*
const Token* Parser::Skip(size_t n)
{
    position += n;
    if (position == length)
        return nullptr;
    if (position >  length)
    {
        //error = "out of vector<Token> range";
        return nullptr;
    }
    return &input[position];
}

const Token* Parser::Next()
{
    position++;
    if (position == length)
        return nullptr;
    else
        return &input[position];
}
*/

auto Parser::Compare(TokenType type) -> const char*
{
    const char*  retval = nullptr;
    const Token* temp   = nullptr;

    retval = Get(0, temp);
    if (retval != nullptr)
        return retval;
    if (temp->GetType() != type)
        return "type mismatch";
    return nullptr;
}

auto Parser::Compare(TokenType type, string data) -> const char*
{
    const char*  retval = nullptr;
    const Token* temp   = nullptr;

    retval = Get(0, temp);
    if (retval != nullptr)
        return retval;
    if (temp->GetType() != type)
        return "type mismatch";
    if (temp->GetData() != data)
        return "data mismatch";
    return nullptr;
}

auto Parser::Match(TokenType type) -> const char*
{
    const char*  retval = nullptr;
    const Token* temp   = nullptr;

    retval = Get(0, temp);
    if (retval != nullptr)
        return retval;
    if (temp->GetType() != type)
        return "type mismatch";
    Next();
    return nullptr;
}

auto Parser::Match(TokenType type, string data) -> const char*
{
    const char*  retval = nullptr;
    const Token* temp   = nullptr;

    retval = Get(0, temp);
    if (retval != nullptr)
        return retval;
    if (temp->GetType() != type)
        return "type mismatch";
    if (temp->GetData() != data)
        return "data mismatch";
    Next();
    return nullptr;
}

/*
void Parser::Match(TokenType type)
{
    if (Get(0)->GetType() != type)
    {
        error = "type mismatch";
    }
    else
    {
        Next();
    }
}
*/

void Parser::ImportAll()
{
    import("const", {}, {ObjectType::Any}, {ObjectType::Any});
    import("load" , {}, {ObjectType::Any}, {ObjectType::Size});
    import("store", {ObjectType::Any}, {}, {ObjectType::Size});

    import("pop", {ObjectType::Any}, {}, {});
    import("add", {ObjectType::Int64, ObjectType::Int64}, {ObjectType::Int64}, {});
    import("inc", {ObjectType::Int64}, {ObjectType::Int64}, {});

    import("jge",  {}, {}, {ObjectType::Size});
    import("jump", {}, {}, {ObjectType::Size});
}


int Parser::Parse()
{
    ImportAll();
    if (error != "")
    {
        fprintf(stderr, "%s", error.c_str());
        return 1;
    }


    const char* retval; //error
    Function    temp;
    while(position < length)
    {
        Function::ctor(temp);

        retval = ParseFunction(&temp);
        if (CheckError(retval))
        {
            PrintError(retval);
            return 2;
        }
        output.insert(&temp);

        Function::dtor(temp);
    }

    return 0;
}

auto Parser::ParseFunction(Function* fn) -> const char*
{
    const char*  retval = nullptr;
    const Token* temp   = nullptr;

    retval = Match(TokenType::KeywordFunction);
    if (retval != nullptr)
        return retval;

    retval = Compare(TokenType::ID);
    if (retval != nullptr)
        return retval;

    retval = Get(0, temp);
    if (retval != nullptr)
        return retval;

    fn->SetName(temp->GetData().c_str());
    fn->base.type = FunctionBase::LFunction_t;
    Next();

    retval = Match(TokenType::LParen);
    if (retval != nullptr)
        return retval;

    /* insert check input arguments */

    retval = Match(TokenType::RParen);
    if (retval != nullptr)
        return retval;

    /* insert check "->" and output arguments */

    retval = Match(TokenType::Colon);
    if (retval != nullptr)
        return retval;

    /* insert specificators check */

    retval = Match(TokenType::KeywordBegin);
    if (retval != nullptr)
        return retval;

    if (fn != nullptr)
        retval = ParseCode(&fn->base.data.Lfn);
    else
        retval = ParseCode(nullptr);
    if (CheckError(retval))
        return retval;

    retval = Match(TokenType::KeywordEnd);
    //if do not do this check than Get(0) will get error
    if (retval != nullptr)
        return retval;
    return nullptr;
}

bool Parser::IsMetka() const
{
    const char*  retval;
    const Token* curr;
    const Token* next;

    retval = Get(0, curr);
    if (retval != nullptr)
        return false;
    retval = Get(1, next);
    if (retval != nullptr)
        return false;

    if ((curr->GetType() == TokenType::ID) && (next->GetType() == TokenType::Colon))
        return true;
    else
        return false;
}

/*
bool Parser::HasOpcode()
{
    const char*  retval = nullptr;
    const Token* temp   = nullptr;

    retval = Compare(TokenType::ID);
    if (retval != nullptr)
        return retval;

    retval = Get(0, temp);
    if (retval != nullptr)
        return retval;

    string name = temp->GetData();

    if (name == "jump")
        return true;
    if (name == "jge")
        return true;
    return false;
}

bool Parser::OpcodeJump()
{
    {
        Function temp;
        Function::ctor(temp);

        ReadSignature(temp.signature);

        if (output.find(temp.signature) == nullptr)
            return "could not find function name";

        cmd.fn = output.find(temp.signature);

        Function::dtor(temp);
    }
}

bool Parser::OpcodeJGE()
{
    {
        Function temp;
        Function::ctor(temp);

        ReadSignature(temp.signature);

        if (output.find(temp.signature) == nullptr)
            return "could not find function name";

        cmd.fn = output.find(temp.signature);

        Function::dtor(temp);
    }
}

void Parser::ConstructOpcode()
{
    const char*  retval = nullptr;
    const Token* temp   = nullptr;

    retval = Compare(TokenType::ID);
    if (retval != nullptr)
        return retval;

    retval = Get(0, temp);
    if (retval != nullptr)
        return retval;

    string name = temp->GetData();

    if (name == "jump")
        OpcodeJump();
    if (name == "jge")
        OpcodeJGE();
}
*/

auto Parser::ParseCode(LFunction* code) -> const char*
{
    const char*  retval = nullptr;
    const Token* temp   = nullptr;

    /**
        parse code consists of 2 parts
        1)just look that all tokens have correct types and functions have c
            1.look that tokens have correct type
            2.match functions with signatures
            3.make list metkas (to match it size_t later)
        2)compile program to threaded code
    */

    map<string, size_t> MetkaList;
    size_t count = 0;
    size_t save  = position;
    while(true)
    {
        retval = Compare(TokenType::KeywordEnd);
        if (retval == nullptr)
            break;
        if (strncmp(retval, "type mismatch", strlen("type mismatch")) != 0)
            return retval;

        if (IsMetka())
        {
            retval = Get(0, temp);
            if (retval != nullptr)
                return retval;

            string name = temp->GetData();
            if (MetkaList.find(name) != MetkaList.end())
                return "multuple definition of metka";

            MetkaList[name] = count;
            Skip(2);
            continue;
        }

        retval = ParseCommand(nullptr, MetkaList);
        if (CheckError(retval))
            return retval;

        ++count;
    }

    Array<Command> commands;
    if (ArrayCtor(commands, count) != 0)
        return "bad alloc";

    count = 0;

    position = save;
    for(Command* it = commands.start; it != commands.finish;)
    {
        fprintf(stdout, "%d %d\n", it - commands.start, count);

        retval = Compare(TokenType::KeywordEnd);
        if (retval == nullptr)
            break;
        if (strncmp(retval, "type mismatch", strlen("type mismatch")) != 0)
            return retval;

        if (IsMetka())
        {
            Skip(2);
            continue;
        }

        retval = ParseCommand(it, MetkaList);
        if (CheckError(retval))
            return retval;

        ++it; //we should use increment not in cycle head because of "continue" word
        ++count;
    }
    while(IsMetka())
        Skip(2);

    for(Command* it = commands.start; it != commands.finish; ++it)
        it->Print();
    code->start  = commands.start;
    code->finish = commands.finish;
    return nullptr;
}

auto Parser::ParseArgn(Array<Object>* args, size_t n, const map<string, size_t>& MetkaList) -> const char*
{
    const char*  retval = nullptr;
    const Token* temp   = nullptr;

    if (n != 0)
        Match(TokenType::Comma);

    retval = Get(0, temp);
    if (retval != nullptr)
        return retval;
    switch(temp->GetType())
    {
        case TokenType::Number:
        {
            if (args != nullptr)
            {
                const char* str   = temp->GetData().c_str();
                int64_t     value = 0;
                sscanf(str, "%" PRId64, &value);

                Object::set(*(args->start + n), value);
            }
            Next();
            break;
        }
        case TokenType::ID:
        {
            if (args != nullptr)
            {
                string name = temp->GetData();
                if (MetkaList.find(name) == MetkaList.end())
                    return "could not find metka in MetkaList";
                int64_t value = MetkaList.find(name)->second;
                Object::set(*(args->start + n), value);
            }
            Skip(2);
            break;
        }
        default:
            return "argument is not a number nor metka";
    }
    return nullptr;
}

size_t Parser::ParseArgs(Array<Object>* args, const map<string, size_t>& MetkaList)
{
    const char*  retval = nullptr;
    const Token* temp   = nullptr;

    size_t count = 0;
    while(true)
    {
        retval = Get(0, temp);
        if (retval != nullptr)
        {
            error = retval;
            return 0;
        }

        if ((temp->GetType()  == TokenType::KeywordEnd))
            return count;
        if ((temp->GetType() == TokenType::ID) && ((temp + 1)->GetType() != TokenType::Semicolon))
            return count;
        ParseArgn(args, count++, MetkaList);
    }
}

size_t Parser::ParseList_ObjectType(Array<ObjectType>* dest)
{
    const char*  retval = nullptr;
    const Token* temp   = nullptr;

    size_t bracket = 0; //bracket type
    size_t start   = position;
    size_t finish  = position;
    size_t count;

    count = 0;
    while(true)
    {
        retval = Get(0, temp);
        if (retval != nullptr)
        {
            error = retval;
            return 0;
        }
        if (temp->GetType() == TokenType::LBrace)
        {
            bracket = 1;
            Next();
            break;
        }
        if (temp->GetType() == TokenType::LBracket)
        {
            bracket = 2;
            Next();
            break;
        }
        if (temp->GetType() == TokenType::LParen)
        {
            bracket = 3;
            Next();
            break;
        }
        error = "expected '(' or '[' or '{'";
        return count;
    }

    retval = Get(0, temp);
    if (retval != nullptr)
        return count;
    if (temp->GetType() != TokenType::ObjectType)
        return count;
    ++count;
    Next();

    while(true)
    {
        retval = Get(0, temp);
        if (retval != nullptr)
        {
            error = retval;
            return 0;
        }
        if (temp->GetType() == TokenType::RBrace)
        {
            if (bracket == 1)
            {
                Next();
                break;
            }
            else
            {
                error = "expected }";
                return 0;
            }
        }
        if (temp->GetType() == TokenType::RBracket)
        {
            if (bracket == 2)
            {
                Next();
                break;
            }
            else
            {
                error = "expected ]";
                return 0;
            }
        }
        if (temp->GetType() == TokenType::RParen)
        {
            if (bracket == 3)
            {
                Next();
                break;
            }
            else
            {
                error = "expected )";
                return 0;
            }
        }

        if ((temp->GetType() != TokenType::Comma))
        {
            error = "expected ,";
            return 0;
        }
        Next();

        retval = Get(0, temp);
        if (retval != nullptr)
        {
            error = retval;
            return 0;
        }
        if ((temp->GetType() != TokenType::ObjectType))
        {
            error = "expected ObjectType";
            return 0;
        }
        Next();
        ++count;
    }

    finish = position;

    ArrayCtor(*dest, count);
    if (dest->size() != count)
    {
        error = "bad alloc";
        return 0;
    }
    position = start;

    count = 0;
    while(true)
    {
        retval = Get(0, temp);
        if (retval != nullptr)
        {
            error = retval;
            return 0;
        }
        if (temp->GetType() == TokenType::LBrace)
        {
            bracket = 1;
            Next();
            break;
        }
        if (temp->GetType() == TokenType::LBracket)
        {
            bracket = 2;
            Next();
            break;
        }
        if (temp->GetType() == TokenType::LParen)
        {
            bracket = 3;
            Next();
            break;
        }
        error = "expected '(' or '[' or '{'";
        return count;
    }

    retval = Get(0, temp);
    if (retval != nullptr)
    {
        error = retval;
        return 0;
    }
    if (temp->GetType() == TokenType::ObjectType)
        *(dest->start + 0) = ObjectType::find(temp->GetData());
    ++count;
    Next();

    while(true)
    {
        retval = Get(0, temp);
        if (retval != nullptr)
        {
            error = retval;
            return 0;
        }
        if ((temp->GetType() == TokenType::RBrace) || (temp->GetType() == TokenType::RBracket) || (temp->GetType() == TokenType::RParen))
        {
            Next();
            break;
        }

        if ((temp->GetType() != TokenType::Comma))
        {
            error = "expected comma";
            return 0;
        }
        Next();

        retval = Get(0, temp);
        if (retval != nullptr)
        {
            error = retval;
            return 0;
        }
        if ((temp->GetType() != TokenType::ObjectType))
        {
            error = "expected ObjectType";
            return 0;
        }
        *(dest->start + count) = ObjectType::find(temp->GetData());
        Next();
        ++count;
    }

    return count;
}

auto Parser::ReadSignature(FunctionSignature& signature) -> const char*
{
    const char*  retval = nullptr;
    const Token* temp   = nullptr;

    retval = Compare(TokenType::ID);
    if (retval != nullptr)
        return retval;

    retval = Get(0, temp);
    if (retval != nullptr)
        return retval;

    string name = temp->GetData();
    signature.name = StringCopy(name.c_str());
    Next();

    retval = Match(TokenType::LParen);
    if (retval != nullptr)
        return retval;

    retval = Compare(TokenType::LParen);
    if (retval == nullptr)
    {
        ParseList_ObjectType(&signature.ilist);
        if (CheckError(nullptr))
            return "internal error";
    }

    retval = Compare(TokenType::LBracket);
    if (retval == nullptr)
    {
        ParseList_ObjectType(&signature.alist);
        if (CheckError(nullptr))
            return "internal error";
    }

    retval = Compare(TokenType::Arrow);
    if (retval == nullptr)
    {
        Match(TokenType::Arrow);

        retval = Compare(TokenType::LParen);
        if (retval == nullptr)
        {
            ParseList_ObjectType(&signature.olist);
            if (CheckError(nullptr))
                return "internal error";
        }
    }

    retval = Match(TokenType::RParen);
    if (retval != nullptr)
        return retval;

    return nullptr;
}

/*
void Parser::MakeCommand(Command& cmd, const map<string, size_t>& MetkaList)
{
    string name = Get(0)->GetData();
    Next();

    {
        Function temp;
        temp.name = name;
        ParseArumentString(&temp);

        if (output.find(temp.name, temp.input, temp.output, temp.args) == nullptr)
        {
            error = "could not find function name";
            return;
        }

        cmd.fn = output.find(temp.name, temp.input, temp.output, temp.args);
    }

    size_t argc = cmd.fn->args.size();
    cmd.args = (Object*) calloc(argc, sizeof(Object));

}
*/

auto Parser::Command1(const map<string, size_t>& MetkaList) -> const char*
{
    const char*  retval = nullptr;
    const Token* temp   = nullptr;

    size_t argc = 0;
    {
        Function temp;
        Function::ctor(temp);

        ReadSignature(temp.signature);

        if (output.find(temp.signature) == nullptr)
            return "could not find function name";

        argc = temp.signature.alist.size();

        Function::dtor(temp);
    }

    size_t save  = position;
    size_t count = 0;

    count = ParseArgs(nullptr, MetkaList);
    if (CheckError(nullptr))
        return "internal error";

    if (count != argc)
        return"wrong arguments number";
    return nullptr;
}

auto Parser::Command2(Command& cmd, const map<string, size_t>& MetkaList) -> const char*
{
    const char*  retval = nullptr;
    const Token* temp   = nullptr;
    //Maker (all checks should be done)

    {
        Function temp;
        Function::ctor(temp);

        ReadSignature(temp.signature);

        if (output.find(temp.signature) == nullptr)
            return "could not find function name";

        cmd.fn = output.find(temp.signature);

        Function::dtor(temp);
    }

    ArrayCtor(cmd.args, cmd.fn->signature.alist.size());

    ParseArgs(&cmd.args, MetkaList);
    if (CheckError(nullptr))
        return "internal error";
    return nullptr;
}


auto Parser::ParseCommand(Command* cmd, const map<string, size_t>& MetkaList) -> const char*
{
    if (cmd == nullptr)
    {
        return Command1(MetkaList);
    }
    else
    {
        return Command2(*cmd, MetkaList);
    }

    /*
    //name
    if (Get(0)->GetType() != TokenType::ID)
    {
        error = "incorrect TokenType of comman name";
        return;
    }

    string name = Get(0)->GetData();
    Next();
    {
        Function temp;
        temp.name = name;
        ParseArumentString(&temp);

        if (output.find(temp.name, temp.input, temp.output, temp.args) == nullptr)
        {
            error = "could not find function name";
            return;
        }

        if (cmd != nullptr)
            cmd->fn = output.find(temp.name, temp.input, temp.output, temp.args);
    }

    //args

    size_t argc = 0;
    size_t save = position;

    size_t count;
    count = ParseArgs(nullptr, MetkaList);
    if (CheckError(Get(0)))
        return;

    if (cmd != nullptr)
    {
        cmd->args = (Object*) calloc(argc, sizeof(Object));
        if (cmd->args == nullptr)
        {
            error = "could not allocate memory";
            return;
        }

        position = save;
        count = ParseArgs(cmd, MetkaList);
        if (CheckError(Get(0)))
            return;
    }
    */
}
