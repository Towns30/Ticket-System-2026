#include "../include/Lexer.hpp"
#include <iostream>

int main()
{
  std::string s;
  int n;
  Lexer lexer;
  std::cin >> n;
  std::cin.ignore();
  for (int i = 0; i <= n - 1; i++)
  {
    std::getline(std::cin, s);
    sjtu::vector<Token> tokens = lexer.Tokenize(s);
    for (int j = 0; j <= tokens.size() - 1; j++)
    {
      for (int k = 0; k <= tokens[j].value_.size() - 1; k++)
      {
        std::cout << tokens[j].value_[k] << ' ';
      }
    }
    std::cout << '\n';
  }
}