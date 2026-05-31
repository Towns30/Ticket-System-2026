#ifndef UTILS_HPP
#define UTILS_HPP
#include <string>

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

template <int N> std::array<char, N> StringToArray(const std::string &a)
{
  std::array<char, N> result;
  for (int j = 0; j <= N - 1; j++)
  {
    if (j < a.size())
    {
      result[j] = a[j];
    }
    else
    {
      result[j] = '\0';
    }
  }
  return result;
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
  while(a > 0)
  {
    ans = char(a % 10 + '0') + ans;
    a /= 10; 
  }
  return ans;
}
} // namespace Utils
#endif