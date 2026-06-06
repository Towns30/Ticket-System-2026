#include "../include/Time.hpp"
#include "../include/Utils.hpp"
#include <cassert>

AccurateTime::AccurateTime() : month_(0), day_(0), hour_(0), minite_(0) {}
AccurateTime::AccurateTime(const MonthType &month, const DayType &day,
                           const HourType &hour, const MiniteType &minite)
    : month_(month), day_(day), hour_(hour), minite_(minite)
{
  Normalize();
}
AccurateTime::AccurateTime(const Date &date, const HourMinite &hour_minite)
{
  month_ = date.month_;
  day_ = date.day_;
  hour_ = hour_minite.hour_;
  minite_ = hour_minite.minite_;
  Normalize();
}
AccurateTime::AccurateTime(const std::string &s)
{
  int tmp = 0;
  for (int i = 0; i <= s.size() - 1; i++)
  {
    if (s[i] >= '0' && s[i] <= '9')
    {
      tmp = tmp * 10 + (s[i] - '0');
    }
    else if (s[i] == '-')
    {
      month_ = tmp;
      tmp = 0;
    }
    else if (s[i] == ' ')
    {
      day_ = tmp;
      tmp = 0;
    }
    else if (s[i] == ':')
    {
      hour_ = tmp;
      tmp = 0;
    }
  }
  minite_ = tmp;
  Normalize();
}
void AccurateTime::Normalize()
{
  hour_ += Utils::FloorDiv(minite_, 60);
  minite_ = Utils::FloorRes(minite_, 60);
  day_ += Utils::FloorDiv(hour_, 24);
  hour_ = Utils::FloorRes(hour_, 24);
  assert(month_ >= 6);
  while (day_ <= 0)
  {
    month_ -= 1;
    day_ += totalDays[month_];
  }
  while (day_ > totalDays[month_])
  {
    day_ -= totalDays[month_];
    month_ += 1;
  }
}
AccurateTime AccurateTime::AddMinites(const int &min)
{
  AccurateTime at = *this;
  at.minite_ += min;
  at.Normalize();
  return at;
}
AccurateTime AccurateTime::AddDays(const int &day)
{
  AccurateTime at = *this;
  at.day_ += day;
  at.Normalize();
  return at;
}
Date AccurateTime::GetDate() { return Date{month_, day_}; }
HourMinite AccurateTime::GetHourMinite() { return HourMinite{hour_, minite_}; }
std::string AccurateTime::ToString()
{
  return Utils::IntToString(month_) + '-' + Utils::IntToString(day_) + ' ' +
         Utils::IntToString(hour_) + ':' + Utils::IntToString(minite_);
}
int AccurateTime::ToTotalMinites()
{
  return GetDate().ToTotalDays() * 24 * 60 + GetHourMinite().ToMinites();
}
Date::Date() : month_(0), day_(0) {}
Date::Date(const MonthType &month, const DayType &day)
    : month_(month), day_(day)
{
  Normalize();
}
Date::Date(const std::string &s)
{
  int tmp = 0;
  for (int i = 0; i <= s.size() - 1; i++)
  {
    if (s[i] >= '0' && s[i] <= '9')
    {
      tmp = tmp * 10 + (s[i] - '0');
    }
    else if (s[i] == '-')
    {
      month_ = tmp;
      tmp = 0;
    }
  }
  day_ = tmp;
  Normalize();
}

int Date::DaysBetween(Date other)
{
  return this->ToTotalDays() - other.ToTotalDays();
}

int Date::ToTotalDays() // start from 06-01
{
  int tmp_d = day_ - 1;
  int tmp_m = month_;
  while(tmp_m > 6)
  {
    tmp_m--;
    tmp_d += totalDays[tmp_m];
  }
  return tmp_d;
}

Date Date::AddDays(const int &day)
{
  Date da = *this;
  da.day_ += day;
  da.Normalize();
  return da;
}

std::string Date::ToString()
{
  return Utils::IntToString(month_) + '-' + Utils::IntToString(day_);
}

void Date::Normalize()
{
  while (day_ <= 0)
  {
    month_ -= 1;
    day_ += totalDays[month_];
  }
  while (day_ > totalDays[month_])
  {
    day_ -= totalDays[month_];
    month_ += 1;
  }
}

void HourMinite::Normalize()
{
  hour_ += Utils::FloorDiv(minite_, 60);
  minite_ = Utils::FloorRes(minite_, 60);
}

HourMinite::HourMinite() : hour_(0), minite_(0) {}

HourMinite::HourMinite(const HourType &hour, const MiniteType &minite)
    : hour_(hour), minite_(minite)
{
  Normalize();
}

HourMinite::HourMinite(const std::string &s)
{
  int tmp = 0;
  for (int i = 0; i <= s.size() - 1; i++)
  {
    if (s[i] >= '0' && s[i] <= '9')
    {
      tmp = tmp * 10 + (s[i] - '0');
    }
    else if (s[i] == ':')
    {
      hour_ = tmp;
      tmp = 0;
    }
  }
  minite_ = tmp;
  Normalize();
}

HourMinite HourMinite::AddMinites(const int &minite)
{
  HourMinite hm = *this;
  hm.minite_ += minite;
  hm.Normalize();
  return hm;
}

int HourMinite::ToMinites() { return hour_ * 60 + minite_; }

std::string HourMinite::ToString()
{
  return Utils::IntToString(hour_) + ':' + Utils::IntToString(minite_);
}
