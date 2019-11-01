#ifndef _PARSER_HPP_
#define _PARSER_HPP_

#include <vector>
#include <map>
#include "../Token/Token.hpp"
//#include "Object.hpp"

#include "../Command/Command.hpp"
#include "../Builtins/Builtins.hpp"
#include "../FunctionsTable/FunctionsTable.hpp"

using std::map;
using std::string;
using std::vector;

/*
struct Function
{
    string             name;
    vector<ObjectType> input;
    vector<ObjectType> output;

    vector<Object> consts;
    vector<Object> args;

    size_t type;
    CFunction       fn;
    vector<Command> code;

    map<string, size_t> __ConstListMapper;
    map<string, size_t> __MetkaListMapper;
};
*/

class Parser
{
public:
    void import(const char* name, Builtins::il input, Builtins::il output, Builtins::il args);
    void ImportAll();
private:
    FunctionsTable& output;
    vector<Token>   input;
    size_t position;
    size_t length;
private:
    mutable string error;
/* checked */
private:
    bool CheckError(const char* retval) const;
    void PrintError(const char* retval) const;
private:
    auto Get (size_t n, Token const *& t) const -> const char*;
    auto Skip(size_t n) -> const char*;
    auto Next() -> const char*;
private:
    //difference between Compare and Match is that Compare does not increment position
    auto Compare(TokenType type)              -> const char*;
    auto Compare(TokenType type, string data) -> const char*;
    auto Match(TokenType type)              -> const char*;
    auto Match(TokenType type, string data) -> const char*;
private:
    auto ParseFunction(Function*  fn)   -> const char*;
    auto ParseCode    (LFunction* code) -> const char*;
    auto ParseCommand (Command*   cmd, const map<string, size_t>& MetkaList) -> const char*;
/* not checked */
private:
    auto ParseArgn(Array<Object>* args, size_t n, const map<string, size_t>& MetkaList) -> const char*;
private:
    auto ReadSignature(FunctionSignature& fn) -> const char*;
    //auto ParseArumentString(Function* fn) -> const char*;
private:
    auto Command1(const map<string, size_t>& MetkaList) -> const char*;
    auto Command2(Command&, const map<string, size_t>& Metkalist) -> const char*;
private:
    size_t ParseList_ObjectType(Array<ObjectType>* dest);
private:
    size_t ParseArgs(Array<Object>* args, const map<string, size_t>& MetkaList);

private:
    bool IsMetka() const;
public:
    Parser(std::vector<Token> input, FunctionsTable& __output):
        output(__output)
    {
        this->input    = input;
        this->position = 0;
        this->length   = input.size();

        error = "";
    }
    int Parse();
};

#endif // _PARSER_HPP_
