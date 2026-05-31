#ifndef LEXER_HPP
#define LEXER_HPP

#include "Token.hpp"
#include "vector.hpp"

class Lexer
{
private:
  Token GetTimestamp(int &, std::string);
  Token GetPrefix(int &, std::string);
  Token GetBasic(int &, std::string);
  void SkipBlank(int &, std::string);

public:
  sjtu::vector<Token> Tokenize(const std::string &);
};

#endif