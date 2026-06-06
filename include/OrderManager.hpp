#ifndef ORDER_MANAGER_HPP
#define ORDER_MANAGER_HPP
#include "BPlusTree.hpp"
#include "Time.hpp"
#include "TrainManager.hpp"
#include "UserManager.hpp"
#include "Utils.hpp"
#include "vector"

enum class OrderStatus
{
  SUCCESS,
  PENDING,
  REFUNDED,
};

struct OrderInfo
{
  char64 username_;
  OrderStatus status_;
  char64 trainID_;
  Date d_start_;
  Date train_start_date_;
  char64 from_;
  int pos_f_;   // from在列车站的下标
  int day_pos_; // 列车是第几班
  AccurateTime leaving_time_;
  char64 to_;
  int pos_t_; // to在列车站的下标
  AccurateTime arriving_time_;
  int price_; // 单价
  int num_;
};

struct UsernameOrderInfoID
{
  Utils::Char64Key username_;
  int order_infoID_;

  bool operator<(const UsernameOrderInfoID &other) const
  {
    if (username_ < other.username_)
    {
      return true;
    }
    if (other.username_ < username_)
    {
      return false;
    }
    return order_infoID_ < other.order_infoID_;
  }
  bool operator==(const UsernameOrderInfoID &other) const
  {
    return username_ == other.username_ && order_infoID_ == other.order_infoID_;
  }
  bool operator!=(const UsernameOrderInfoID &other) const
  {
    return !(*this == other);
  }
  bool operator>(const UsernameOrderInfoID &other) const
  {
    return other < *this;
  }
  bool operator<=(const UsernameOrderInfoID &other) const
  {
    return !(other < *this);
  }
  bool operator>=(const UsernameOrderInfoID &other) const
  {
    return !(*this < other);
  }
};

struct TrainDateOrderInfoID
{
  Utils::Char64Key trainID_;
  Date date_;
  int order_infoID_;

  bool operator<(const TrainDateOrderInfoID &other) const
  {
    if (trainID_ < other.trainID_)
    {
      return true;
    }
    if (other.trainID_ < trainID_)
    {
      return false;
    }
    if (date_ < other.date_)
    {
      return true;
    }
    if (other.date_ < date_)
    {
      return false;
    }
    return order_infoID_ < other.order_infoID_;
  }
  bool operator==(const TrainDateOrderInfoID &other) const
  {
    return trainID_ == other.trainID_ && date_ == other.date_ &&
           order_infoID_ == other.order_infoID_;
  }
  bool operator!=(const TrainDateOrderInfoID &other) const
  {
    return !(*this == other);
  }
  bool operator>(const TrainDateOrderInfoID &other) const
  {
    return other < *this;
  }
  bool operator<=(const TrainDateOrderInfoID &other) const
  {
    return !(other < *this);
  }
  bool operator>=(const TrainDateOrderInfoID &other) const
  {
    return !(*this < other);
  }
};

class OrderManager
{
private:
  OrderManager();
  ~OrderManager();
  MemoryRiver<OrderInfo, 3> order_info_mr_;
  BPlusTree<UsernameOrderInfoID, int> username_orderinfoID_bpt_; // value不使用
  BPlusTree<TrainDateOrderInfoID, int>
      train_date_order_infoID_bpt_; // value不使用，候补队列
  sjtu::vector<int> GetUserOrderInfoIDs(char64 username);
  void AddFunctionUsernameOrderInfoIDs(char64 username, int order_infoID);
  sjtu::vector<int> GetTrainIDDateOrderInfoIDs(char64 trainID, Date date);
  void AddFunctionTrainIDDateOrderInfoIDs(char64 trainID, Date date,
                                          int order_infoIDs);
  void DeleteFunctionTrainIDDateOrderInfoIDs(char64 trainID, Date date,
                                             int order_infoIDs);
  void DeleteFunctionUsernameOrderInfoIDs(char64 username, int order_infoID);
  void PrintOrder(OrderInfo order_info);
  void DealWaitingList(Date date, TrainInfo train_info,
                       int train_infoID); // date为这班列车的发车日期

public:
  static OrderManager &getInstance()
  {
    static OrderManager instance;
    return instance;
  }
  OrderManager(const OrderManager &) = delete;
  OrderManager &operator=(const OrderManager &) = delete;
  void BuyTicket(char64 username, char64 trainID, Date date, int num,
                 char64 from, char64 to, bool allow_pending = false);
  void RefundTicket(char64 username, int order_pos = 1);
  void QueryOrder(char64 username);
  void Clean();
};

#endif