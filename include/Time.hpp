#ifndef TIME_HPP
#define TIME_HPP

#include <string>

using MonthType = int;
using DayType = int;
using HourType = int;
using MiniteType = int;

class Date;
class HourMinite;
class AccurateTime;

class AccurateTime
{
private:
  static constexpr int totalDays[13] = {0,  31, 28, 31, 30, 31, 30,
                                    31, 31, 30, 31, 30, 31};
  MonthType month_;
  DayType day_;
  HourType hour_;
  MiniteType minite_;
  void Normalize();

public:
  friend class Date;
  friend class HourMinite;
  AccurateTime();
  AccurateTime(const MonthType &, const DayType &, const HourType &,
               const MiniteType &);
  AccurateTime(const Date &, const HourMinite &);
  AccurateTime(const std::string &);
  void AddMinites(const int &);
  void AddDays(const int &);
  Date GetDate();
  HourMinite GetHourMinite();
  std::string ToString();

  bool operator<=>(const AccurateTime &) const = default;
};

class Date
{
private:
  static constexpr int totalDays[13] = {0,  31, 28, 31, 30, 31, 30,
                                    31, 31, 30, 31, 30, 31};
  MonthType month_;
  DayType day_;
  void Normalize();

public:
  friend class AccurateTime;
  friend class HourMinite;
  Date();
  Date(const MonthType &, const DayType &);
  Date(const std::string &);
  int DaysBetween(Date);
  void AddDays(const int &);
  std::string ToString();
  int ToTotalDays();
  bool operator<=>(const Date &) const = default;
};

class HourMinite
{
private:
  HourType hour_;
  MiniteType minite_;
  void Normalize();

public:
  friend class Date;
  friend class AccurateTime;
  HourMinite();
  HourMinite(const HourType &, const MiniteType &);
  HourMinite(const std::string &);
  void AddMinites(const int &);
  int ToMinites();
  std::string ToString();
  bool operator<=>(const HourMinite &) const = default;
};

#endif