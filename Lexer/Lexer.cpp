#include "Lexer.hpp"
#include "../Object/ObjectType.hpp"

#include <string>

bool Lexer::Match(const std::string& word)
{
    std::string next = input.substr(position, word.length());
    if (next != word)
        return false;
    char ws = input[position + word.length()];
    if ((ws == ' ') || (ws == '\t') || (ws == '\n'))
    {
        position += word.length();
        return true;
    }
    else
    {
        return false;
    }
}

bool Lexer::GetNumber(std::string& result)
{
    result = "";

    char current = input[position];
    if (!isdigit(current)) return false;

    while(true)
    {
        current = input[position];
        if (!isdigit(current)) break;
        result += current;
        ++position;
    }
    return true;
}

bool Lexer::GetString(std::string& result)
{
    result = "";

    char current = input[position];
    if (current != '"') return false;

    ++position;
    while(true)
    {
        current = input[position];
        if (current == '"') break;
        result += current;
        ++position;
    }

    return true;
}

bool Lexer::GetID(std::string& result)
{
    size_t from;
    from   = position;
    result = "";

    char current = input[position];
    switch(current)
    {
        case 'a': case 'A': case 'b': case 'B': case 'c': case 'C':
        case 'd': case 'D': case 'e': case 'E': case 'f': case 'F':
        case 'g': case 'G': case 'h': case 'H': case 'i': case 'I':
        case 'j': case 'J': case 'k': case 'K': case 'l': case 'L':
        case 'm': case 'M': case 'n': case 'N': case 'o': case 'O':
        case 'p': case 'P': case 'q': case 'Q': case 'r': case 'R':
        case 's': case 'S': case 't': case 'T': case 'u': case 'U':
        case 'v': case 'V': case 'w': case 'W': case 'x': case 'X':
        case 'y': case 'Y': case 'z': case 'Z': case '_':
            break;
        default:
            return false;
    }

    while(true)
    {
        current = input[position];
        switch(current)
        {
            case 'a': case 'A': case 'b': case 'B': case 'c': case 'C':
            case 'd': case 'D': case 'e': case 'E': case 'f': case 'F':
            case 'g': case 'G': case 'h': case 'H': case 'i': case 'I':
            case 'j': case 'J': case 'k': case 'K': case 'l': case 'L':
            case 'm': case 'M': case 'n': case 'N': case 'o': case 'O':
            case 'p': case 'P': case 'q': case 'Q': case 'r': case 'R':
            case 's': case 'S': case 't': case 'T': case 'u': case 'U':
            case 'v': case 'V': case 'w': case 'W': case 'x': case 'X':
            case 'y': case 'Y': case 'z': case 'Z': case '_':
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                ++position;
                break;
            default:
                result = input.substr(from, position - from);
                return true;
        }
    }

    return true;
}

bool Lexer::IsKeyword(std::string text)
{
    if (text == "function")
        return true;
    if (text == "begin")
        return true;
    if (text == "end")
        return true;
    return false;
}

void Lexer::AddTokenKeyword(std::string text, size_t line, size_t column)
{
    if (text == "function")
    {
        AddToken(TokenType::KeywordFunction, line, column);
        return;
    }
    if (text == "begin")
    {
        AddToken(TokenType::KeywordBegin, line, column);
        return;
    }
    if (text == "end")
    {
        AddToken(TokenType::KeywordEnd, line, column);
        return;
    }
    return;
}

std::vector<Token> Lexer::Tokenize()
{
    size_t line   = 1;
    size_t column = 1;

    std::string text = "";
    bool ok;
    while(position < length)
    {
        char current = input[position];
        switch(current)
        {
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                ok = GetNumber(text);
                if (ok)
                    AddToken(TokenType::Number, text, line, column);
                column += text.length();
                break;
            case 'a': case 'A': case 'b': case 'B': case 'c': case 'C':
            case 'd': case 'D': case 'e': case 'E': case 'f': case 'F':
            case 'g': case 'G': case 'h': case 'H': case 'i': case 'I':
            case 'j': case 'J': case 'k': case 'K': case 'l': case 'L':
            case 'm': case 'M': case 'n': case 'N': case 'o': case 'O':
            case 'p': case 'P': case 'q': case 'Q': case 'r': case 'R':
            case 's': case 'S': case 't': case 'T': case 'u': case 'U':
            case 'v': case 'V': case 'w': case 'W': case 'x': case 'X':
            case 'y': case 'Y': case 'z': case 'Z': case '_':
                ok = GetID(text);
                if (ok)
                {
                    if (IsKeyword(text))
                    {
                        AddTokenKeyword(text, line, column);
                        break;
                    }
                    if (ObjectType::find(text) != ObjectType::Exception)
                    {
                        AddToken(TokenType::ObjectType, text, line, column);
                        break;
                    }
                    AddToken(TokenType::ID, text, line, column);
                    break;
                }
                column += text.length();
                break;
            case '"':
                ok = GetString(text);
                if (ok)
                    AddToken(TokenType::String, text, line, column);
                column += text.length() + 2;
                break;
            case '+':
                AddToken(TokenType::Plus, line, column);
                ++position;
                ++column;
                break;
            case '-':
                if (Match("->"))
                {
                    AddToken(TokenType::Arrow, line, column);
                    //position += 2;
                    column   += 2;
                    break;
                }
                AddToken(TokenType::Minus, line, column);
                ++position;
                ++column;
                break;
            case '*':
                AddToken(TokenType::Star, line, column);
                ++position;
                ++column;
                break;
            case '/':
                AddToken(TokenType::Slash, line, column);
                ++position;
                ++column;
                break;
            case '(':
                AddToken(TokenType::LParen, line, column);
                ++position;
                ++column;
                break;
            case ')':
                AddToken(TokenType::RParen, line, column);
                ++position;
                ++column;
                break;
            case '[':
                AddToken(TokenType::LBracket, line, column);
                ++position;
                ++column;
                break;
            case ']':
                AddToken(TokenType::RBracket, line, column);
                ++position;
                ++column;
                break;
            case '{':
                AddToken(TokenType::LBrace, line, column);
                ++position;
                ++column;
                break;
            case '}':
                AddToken(TokenType::RBrace, line, column);
                ++position;
                ++column;
                break;
            case ':':
                if (Match(":="))
                {
                    AddToken(TokenType::Assign, line, column);
                    //position += 2;
                    column += 2;
                    break;
                }
                AddToken(TokenType::Colon, line, column);
                ++position;
                ++column;
                break;
            case ';':
                AddToken(TokenType::Semicolon, line, column);
                ++position;
                ++column;
                break;
            case ',':
                AddToken(TokenType::Comma, line, column);
                ++position;
                ++column;
                break;
            case ' ': case '\t':
                ++position;
                ++column;
                break;
            case '\n':
                ++line;
                ++position;
                column = 1;
                break;
            default:
                AddToken(TokenType::None, std::string(&current, 1), line, column);
                ++position;
                ++column;
                break;
        }
    }
    return output;
}

/*
int LexerFile::read(const char* filename)
{
    size_t length = 0;
    size_t count  = 0;
    char   buffer[80];
    FILE*  file;


    file = fopen(filename, "r");
    while(fgets(buffer, 80, file))
        ++count;
    fclose(file);

    start  = (char**) calloc(count, sizeof(char*));
    if (start == nullptr)
        return 1;
    finish = start + count;

    file = fopen(filename, "r");
    while(fgets(buffer, 80, file))
        length += strlen(buffer) + 1;
    fclose(file);

    *start = (char*) calloc(length, sizeof(char));
    if (*start == nullptr)
        return 2;

    file = fopen(filename, "r");
    count  = 0;
    length = 0;
    while(true)
    {
        if (!fgets(*start + length, 80, file))
            break;
        *(start + count) = *start + length;
        length += strlen(*start + length) + 1;
        ++count;
    }
    fclose(file);

    return 0;
}

int LexerFile::write(const char* filename)
{
    FILE* file;

    file = fopen(filename, "w");
    for(char** it = start; it != finish; ++it)
        fputs(*it, file);
    fclose(file);

    return 0;
}
*/
