#ifndef PARSER_HPP
#define PARSER_HPP
#include "Lexer.hpp"
#include "OrderManager.hpp"
#include "TrainManager.hpp"
#include "UserManager.hpp"

class Parser
{
public:
  void ParseLine(const sjtu::vector<Token> &);

private:
  std::string GetPrefixValue(const sjtu::vector<Token> &tokens,
                             const std::string &key);
  sjtu::vector<std::string> GetPrefixValues(const sjtu::vector<Token> &tokens,
                                            const std::string &key);
};

#endif