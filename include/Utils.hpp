#ifndef UTILS_HPP
#define UTILS_HPP
#include <cstring>
#include <iostream>
#include <string>

#ifndef TYPES_HPP
#define TYPES_HPP
using char64 = char[64];
#endif

namespace Utils
{
struct Char64Key
{
  char data_[64];

  Char64Key()
  {
    for (int i = 0; i <= 63; i++)
    {
      data_[i] = '\0';
    }
  }
  Char64Key(const std::string &s)
  {
    for (int i = 0; i <= 63; i++)
    {
      data_[i] = '\0';
    }
    for (int i = 0; i <= s.size() - 1; i++)
    {
      data_[i] = s[i];
    }
  }
  Char64Key(const char64 &array) { std::memcpy(data_, array, 64); }

  bool operator<(const Char64Key &other) const
  {
    return std::strcmp(data_, other.data_) < 0;
  }
  bool operator==(const Char64Key &other) const
  {
    return std::strcmp(data_, other.data_) == 0;
  }
  bool operator>(const Char64Key &other) const { return other < *this; }
  bool operator<=(const Char64Key &other) const { return !(other < *this); }
  bool operator>=(const Char64Key &other) const { return !(*this < other); }
  bool operator!=(const Char64Key &other) const { return !(*this == other); }

  friend std::ostream &operator<<(std::ostream &out, const Char64Key &other)
  {
    out << other.data_;
    return out;
  }

};

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
  std::memset(a, 0, 64);
  int len = 63;
  if (s.size() < 63)
  {
    len = s.size();
  }
  for (int i = 0; i <= len - 1; i++)
  {
    a[i] = s[i];
  }
  a[len] = '\0';
}

inline std::string IntToString(int a)
{
  if (a == 0)
  {
    return "0";
  }
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