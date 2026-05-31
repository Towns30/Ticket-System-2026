#include "../include/Lexer.hpp"

sjtu::vector<Token> Lexer::Tokenize(const std::string &s)
{
  int cur = 0;
  sjtu::vector<Token> tokens;
  tokens.push_back(GetTimestamp(cur, s));
  SkipBlank(cur, s);
  tokens.push_back(GetBasic(cur, s));
  bool is_prefix = true;
  while (cur <= s.size() - 1)
  {
    SkipBlank(cur, s);
    if (cur == s.size())
    {
      break;
    }
    if (is_prefix)
    {
      tokens.push_back(GetPrefix(cur, s));
      is_prefix = false;
    }
    else
    {
      tokens.push_back(GetBasic(cur, s));
      is_prefix = true;
    }
    if (cur == s.size())
    {
      break;
    }
  }
  return tokens;
}

Token Lexer::GetTimestamp(int &cur, std::string s)
{
  Token token;
  token.value_.push_back("");
  if (s[cur] == '[')
  {
    cur++;
  }
  while (cur <= s.size() - 1 && s[cur] != ']')
  {
    token.value_[0].push_back(s[cur]);
    cur++;
  }
  cur++;
  token.type_ = TokenType::TIMEESTAMP;
  return token;
}

Token Lexer::GetPrefix(int &cur, std::string s)
{
  Token token;
  token.value_.push_back("");
  while (cur <= s.size() - 1 && s[cur] != ' ')
  {
    token.value_[0].push_back(s[cur]);
    cur++;
  }
  token.type_ = TokenType::PREFIX;
  return token;
}

Token Lexer::GetBasic(int &cur, std::string s)
{
  Token token;
  token.value_.push_back("");
  int count = 0;
  while (cur <= s.size() - 1 && s[cur] != ' ')
  {
    if (s[cur] == '|')
    {
      count++;
      token.value_.push_back("");
    }
    else
    {
      token.value_[count].push_back(s[cur]);
    }
    cur++;
  }
  token.type_ = TokenType::BASIC;
  return token;
}

void Lexer::SkipBlank(int &cur, std::string s)
{
  while (cur <= s.size() - 1 && s[cur] == ' ')
  {
    cur++;
  }
}