#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>
#include "vector.hpp"

enum class TokenType
{
  TIMEESTAMP,
  BASIC,
  PREFIX,
};

class Token
{
public:
  TokenType type_;
  std::string timestamp_;
  sjtu::vector<std::string> value_;
  std::string prefix_;
};

#endif