#ifndef UTILS_HPP
#define UTILS_HPP
#include <string>

#ifndef TYPES_HPP
#define TYPES_HPP
using char64 = char[64];
#endif

namespace Utils
{
inline int StringToInt(const std::string &s)
{
  int ans = 0;
  for (int i = 0; i <= s.size() - 1; i++)
  {
    ans = ans * 10 + (s[i] - '0');
  }
  return ans;
}

inline int FloorDiv(const int &a, const int &b) // guarantee b > 0
{
  int r = (a % b + b) % b;
  int q = (a - r) / b;
  return q;
}

inline int FloorRes(const int &a, const int &b) // guarantee b > 0
{
  return (a % b + b) % b;
}

inline void StringToChar64(const std::string &s, char64 &a)
{
  int len = 63;
  if(s.size() < 63)
  {
    len = s.size();
  }
  for(int i = 0; i <= len - 1; i++)
  {
    a[i] = s[i];
  }
  a[len] = '\0';
}
template <int N> std::string ArrayToString(const std::array<char, N> &a)
{
  std::string result;
  for (int i = 0; i <= N - 1; i++)
  {
    if (a[i] != '\0')
    {
      result += a[i];
    }
    else
    {
      return result;
    }
  }
  return result;
}

std::string IntToString(int a)
{
  std::string ans;
  while (a > 0)
  {
    ans = char(a % 10 + '0') + ans;
    a /= 10;
  }
  return ans;
}
} // namespace Utils
#endif