#include <iostream>
#include "../include/Lexer.hpp"
#include "../include/Parser.hpp"

int main()
{
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);
  Lexer lexer;
  Parser parser;
  std::string line;
  while(std::getline(std::cin, line))
  {
    if(line.empty())
    {
      continue;
    }
    parser.ParseLine(lexer.Tokenize(line));
  }
  return 0;
}