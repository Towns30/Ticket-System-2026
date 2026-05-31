#ifndef TRAIN_MANAGER_HPP
#define TRAIN_MANAGER_HPP
#include "BPlusTree.hpp"
#include "Time.hpp"
#include "Utils.hpp"
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
  int prices_[100];
  HourMinite start_time_;
  int travel_times_[100];
  int stopover_times_[100];
  Date start_sale_date_;
  Date end_sale_date_;
  char type_;
  int res_seat_nums[100][100];
  bool is_released_;
};

class TrainManager
{
private:
  TrainManager();
  ~TrainManager();

public:
  static TrainManager &getInstance()
  {
    static TrainManager instance;
    return instance;
  }
  TrainManager(const TrainManager &) = delete;
  TrainManager &operator=(const TrainManager &) = delete;
  void AddTrain(int timestamp, TrainInfo train_info);
  void QueryTrainInfo(int timestamp, char64 trainID, Date date);
  void QueryTicket(int timestamp, char64 from, char64 to, Date date,
                   SortBy sort_by);
  void QueryTransfer(int timestamp, char64 from, char64 to, Date date,
                     SortBy sort_by);
  void
  ModifyTrainInfo(char64 trainID, Date date, int from_idx, int to_idx,
                  int seat_num,
                  int dir); // date represents the start day of the train,
                               // dir = 1 means plus, dir = -1 means minus
  void ReleaseTrain(int timestamp, char64 trainID);
  void DeleteTrain(int timestamp, char64 trainID);
  void Clean(int timestamp);
};

#endif