#include <cstdio>
#include <cinttypes>
#include <fstream>
#include <iostream>

#include "Lexer/Lexer.hpp"
#include "Parser/Parser.hpp"
#include "Command/Command.hpp"
#include "Builtins/Builtins.hpp"

void PrintError(state* st)
{
    for(size_t i = 0; i < st->csp; ++i)
    {
        const string& name     = st->cstack[i].command->fn->name;
        const size_t  position = st->cstack[i].current - st->cstack[i].start;

        if (st->cstack[i].command->fn->type == 0)
            fprintf(stderr, "in builtin function \"%s\":\n"    , name.c_str());
        if (st->cstack[i].command->fn->type == 1)
            fprintf(stderr, "in function \"%s\", command %d:\n", name.c_str(), position);
    }
    fprintf(stderr, "error code = %d\n", st->error);
}

void run(FunctionsTable* table, string name, Object* argv = nullptr)
{
    if (table == nullptr)
        return;

    state st;
    st.table = table;
    st.error = 0;
    st.csp = 0;
    st.dsp = 0;

    Command temp;
    temp.fn   = st.table->find(name);

    if (temp.fn != nullptr)
        call(&temp, &st);
    if (st.error)
    {
        PrintError(&st);
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

    Parser parser(tokens);
    FunctionsTable* table = parser.Parse();

    //table->Print();
    run(table, "main");
    return 0;
}
