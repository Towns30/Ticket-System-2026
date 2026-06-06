#ifndef TRAIN_MANAGER_HPP
#define TRAIN_MANAGER_HPP
#include "BPlusTree.hpp"
#include "Time.hpp"
#include "Utils.hpp"
#include "priority_queue.hpp"
#include <iostream>
enum class SortBy
{
  TIME,
  COST,
};

struct TrainInfo
{
  char64 trainID_;
  int station_num_;
  char64 stations_[100];
  int seat_num_;
  int total_prices_[100]; // 从始发站坐到每一站的总价
  HourMinite start_time_;
  int arrive_times_[100]; // 从始发站出发至到达每个站所用分钟数
  int leaving_times_[100]; // 从始发站出发至从每个站出发所用分钟数
  Date start_sale_date_;
  Date end_sale_date_;
  char type_;
  int res_seat_nums
      [100]
      [100]; // res[i][j]表示从发售日期开始的第i天，列车在下标i站出发时剩余座位数
  bool is_released_;
  friend std::ostream &operator<<(std::ostream &os, TrainInfo &t)
  {
    os << "trainID: " << t.trainID_ << '\n'
       << "station_num: " << t.station_num_ << '\n'
       << "seat_num: " << t.seat_num_ << '\n'
       << "stations: ";
    for (int i = 0; i < t.station_num_; i++)
      os << t.stations_[i] << ' ';
    os << '\n' << "total_prices: ";
    for (int i = 0; i < t.station_num_; i++)
      os << t.total_prices_[i] << ' ';
    os << '\n'
       << "start_time: " << t.start_time_.ToString() << '\n'
       << "arrive_times: ";
    for (int i = 0; i < t.station_num_; i++)
      os << t.arrive_times_[i] << ' ';
    os << '\n' << "leaving_times: ";
    for (int i = 0; i < t.station_num_; i++)
      os << t.leaving_times_[i] << ' ';
    os << '\n'
       << "sale: " << t.start_sale_date_.ToString() << " ~ "
       << t.end_sale_date_.ToString() << '\n'
       << "type: " << t.type_ << '\n'
       << "released: " << t.is_released_ << '\n';
    return os;
  }
};
struct TicketResult
{
  char64 trainID_;
  AccurateTime leaving_time_;
  AccurateTime arriving_time_;
  char64 from_;
  char64 to_;
  int duration_;
  int total_price_;
  int max_seat_;
  friend std::ostream &operator<<(std::ostream &os, TicketResult tic)
  {
    os << tic.trainID_ << ' ' << tic.from_ << ' '
       << tic.leaving_time_.ToString() << " -> " << tic.to_ << ' '
       << tic.arriving_time_.ToString() << ' ' << tic.total_price_ << ' '
       << tic.max_seat_ << '\n';
    return os;
  }
};
struct TransferResult
{
  char64 trainID_1_;
  AccurateTime leaving_time_1_;
  AccurateTime arriving_time_1_;
  char64 from_;
  char64 trans_;
  char64 to_;
  int duration_1_;
  int total_price_1_;
  int max_seat_1_;
  char64 trainID_2_;
  AccurateTime leaving_time_2_;
  AccurateTime arriving_time_2_;
  int duration_2_;
  int total_price_2_;
  int max_seat_2_;
  friend std::ostream &operator<<(std::ostream &os, TransferResult trans)
  {
    os << trans.trainID_1_ << ' ' << trans.from_ << ' '
       << trans.leaving_time_1_.ToString() << " -> " << trans.trans_ << ' '
       << trans.arriving_time_1_.ToString() << ' ' << trans.total_price_1_
       << ' ' << trans.max_seat_1_ << '\n';
    os << trans.trainID_2_ << ' ' << trans.trans_ << ' '
       << trans.leaving_time_2_.ToString() << " -> " << trans.to_ << ' '
       << trans.arriving_time_2_.ToString() << ' ' << trans.total_price_2_
       << ' ' << trans.max_seat_2_ << '\n';
    return os;
  }
};
class TimeCompare
{
public:
  bool operator()(TicketResult t1, TicketResult t2)
  {
    if (t1.duration_ != t2.duration_)
    {
      return t1.duration_ > t2.duration_;
    }
    return std::strcmp(t1.trainID_, t2.trainID_) > 0;
  }
  bool operator()(TransferResult t1, TransferResult t2)
  {
    int min1 = t1.arriving_time_2_.ToTotalMinites() -
               t1.leaving_time_1_.ToTotalMinites();
    int min2 = t2.arriving_time_2_.ToTotalMinites() -
               t2.leaving_time_1_.ToTotalMinites();
    if (min1 != min2)
    {
      return min1 > min2;
    }
    int cost1 = t1.total_price_1_ + t1.total_price_2_;
    int cost2 = t2.total_price_1_ + t2.total_price_2_;
    if (cost1 != cost2)
    {
      return cost1 > cost2;
    }
    int cmp1 = std::strcmp(t1.trainID_1_, t2.trainID_1_);
    if (cmp1)
    {
      return cmp1 > 0;
    }
    return std::strcmp(t1.trainID_2_, t2.trainID_2_) > 0;
  }
};

class CostCompare
{
public:
  bool operator()(TicketResult t1, TicketResult t2)
  {
    if (t1.total_price_ != t2.total_price_)
    {
      return t1.total_price_ > t2.total_price_;
    }
    return std::strcmp(t1.trainID_, t2.trainID_) > 0;
  }
  bool operator()(TransferResult t1, TransferResult t2)
  {
    int cost1 = t1.total_price_1_ + t1.total_price_2_;
    int cost2 = t2.total_price_1_ + t2.total_price_2_;
    if (cost1 != cost2)
    {
      return cost1 > cost2;
    }
    int min1 = t1.arriving_time_2_.ToTotalMinites() -
               t1.leaving_time_1_.ToTotalMinites();
    int min2 = t2.arriving_time_2_.ToTotalMinites() -
               t2.leaving_time_1_.ToTotalMinites();
    if (min1 != min2)
    {
      return min1 > min2;
    }

    int cmp1 = std::strcmp(t1.trainID_1_, t2.trainID_1_);
    if (cmp1)
    {
      return cmp1 > 0;
    }
    return std::strcmp(t1.trainID_2_, t2.trainID_2_) > 0;
  }
};

struct StationTrainInfoID
{
  Utils::Char64Key station_;
  int train_infoID_;

  bool operator<(const StationTrainInfoID &other) const
  {
    if (station_ < other.station_)
    {
      return true;
    }
    if (other.station_ < station_)
    {
      return false;
    }
    return train_infoID_ < other.train_infoID_;
  }
  bool operator==(const StationTrainInfoID &other) const
  {
    return station_ == other.station_ && train_infoID_ == other.train_infoID_;
  }
  bool operator!=(const StationTrainInfoID &other) const
  {
    return !(*this == other);
  }
  bool operator>(const StationTrainInfoID &other) const
  {
    return other < *this;
  }
  bool operator<=(const StationTrainInfoID &other) const
  {
    return !(other < *this);
  }
  bool operator>=(const StationTrainInfoID &other) const
  {
    return !(*this < other);
  }
};

struct StartEndTrainInfoID
{
  Utils::Char64Key start_;
  Utils::Char64Key end_;
  int train_infoID_;

  bool operator<(const StartEndTrainInfoID &other) const
  {
    if (start_ < other.start_)
    {
      return true;
    }
    if (other.start_ < start_)
    {
      return false;
    }
    if (end_ < other.end_)
    {
      return true;
    }
    if (other.end_ < end_)
    {
      return false;
    }
    return train_infoID_ < other.train_infoID_;
  }
  bool operator==(const StartEndTrainInfoID &other) const
  {
    return start_ == other.start_ && end_ == other.end_ &&
           train_infoID_ == other.train_infoID_;
  }
  bool operator!=(const StartEndTrainInfoID &other) const
  {
    return !(*this == other);
  }
  bool operator>(const StartEndTrainInfoID &other) const
  {
    return other < *this;
  }
  bool operator<=(const StartEndTrainInfoID &other) const
  {
    return !(other < *this);
  }
  bool operator>=(const StartEndTrainInfoID &other) const
  {
    return !(*this < other);
  }
};

class TrainManager
{
private:
  TrainManager();
  ~TrainManager();
  MemoryRiver<TrainInfo, 3> train_info_mr_;
  BPlusTree<Utils::Char64Key, int> trainID_traininfoID_bpt_;
  BPlusTree<StationTrainInfoID, int> station_traininfoID_bpt_; // value不使用
  BPlusTree<StartEndTrainInfoID, int>
      start_end_traininfoID_bpt_; // value 不使用
  int WriteTrainInfo(
      TrainInfo train_info); // 返回写入列车数据文件位置，即train_infoID

  void AddFunctionStartEndID(char64 start, char64 end,
                             int train_infID); // 添加映射
  void AddFunctionTrainIDTrainInfoID(char64 trainID,
                                     int train_infoID);        // 添加映射
  void AddFunctionStationID(char64 station, int tarin_infoID); // 添加映射
  void DeleteFunctionTrainIDTrainInfoID(
      char64 trainID); //  删除索引也就相当于删除了列车（对于未发表的列车）
  sjtu::vector<int> QueryStartEndID(char64 start, char64 end);
  sjtu::vector<int> QueryStationID(char64 station);
  void PrintTrain(TrainInfo train_info, Date date); // 格式为query_train返回格式

public:
  static TrainManager &getInstance()
  {
    static TrainManager instance;
    return instance;
  }
  bool GetTrainInfo(char64 trainID, TrainInfo &train_info,
                    int &train_infoID); // 返回false若不存在
  TrainManager(const TrainManager &) = delete;
  TrainManager &operator=(const TrainManager &) = delete;
  void AddTrain(TrainInfo train_info);
  void QueryTrainInfo(char64 trainID, Date date);
  void QueryTicket(char64 from, char64 to, Date date, SortBy sort_by);
  void QueryTransfer(char64 from, char64 to, Date date, SortBy sort_by);
  void UpdateTrainInfo(
      TrainInfo train_info,
      int train_infoID); // 用train_infoID更新列车数据文件train_infoID位置的数据
  void ModifyTrainInfo(char64 trainID, AccurateTime leaving_time, int from_idx,
                       int to_idx, int seat_num,
                       int dir); // leaving_time代表列车从from出发的时间
  void ReleaseTrain(char64 trainID);
  void DeleteTrain(char64 trainID);
  void Clean();
};

#endif