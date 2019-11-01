#include <cstdio>
#include <cinttypes>
#include <fstream>
#include <iostream>

#include "Lexer/Lexer.hpp"
#include "Parser/Parser.hpp"
#include "Command/Command.hpp"
#include "Builtins/Builtins.hpp"

/*
void PrintError(state* st)
{
    for(size_t i = 0; i < st->csp; ++i)
    {
        const string& name     = FunctionGetName(st->cstack[i].command->fn);
        const size_t  position = st->cstack[i].current - st->cstack[i].start;

        if (st->cstack[i].command->fn->base.type == 0)
            fprintf(stderr, "in builtin function \"%s\":\n"    , name.c_str());
        if (st->cstack[i].command->fn->base.type == 1)
            fprintf(stderr, "in function \"%s\", command %d:\n", name.c_str(), position);
    }
    fprintf(stderr, "error code = %d\n", st->error);
}
*/

void run(const FunctionsTable* table, string name, Object* argv = nullptr)
{
    if (table == nullptr)
        return;

    state st;
    st.table = table;
    st.error = 0;
    st.csp = 0;
    st.dsp = 0;

    Command temp;
    temp.fn   = st.table->find(name.c_str());

    if (temp.fn != nullptr)
        call(&temp, &st);
    if (st.error)
    {
        fprintf(stderr, "st.error = %d", st.error);
    }
}

std::string ReadFile(const char* name)
{
    std::ifstream    fin;
    fin.open(name);

    if (!fin)
    {
        std::cout << "file not opened";
        return "";
    }

    std::string temp = "";
    std::string text = "";

    while(getline(fin, temp))
        text += temp + '\n';

    return text;
}

int main()
{
    Lexer lex(ReadFile("code.txt"));
    vector<Token> tokens = lex.Tokenize();

    for(auto token : tokens)
        fprintf(stdout, "%s\n", ToString(token).c_str());

    fprintf(stdout, "sizeof(Command) = %d\n", sizeof(Command));

    FunctionsTable table;

    Parser parser(tokens, table);
    parser.Parse();


    for(Function* it = table.start; it != table.current; ++it)
        PrintFunction(*it);
    //table->Print();
    run(&table, "main");
    return 0;
}
