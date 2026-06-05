#ifndef ORDER_MANAGER_HPP
#define ORDER_MANAGER_HPP
#include "BPlusTree.hpp"
#include "Time.hpp"
#include "Utils.hpp"

enum class OrderStatus
{
  SUCCESS,
  PENDING,
  REFUNDED,
};

struct OrderInfo
{
  int timestamp_;
  char64 username_;
  OrderStatus status_;
  char64 trainID_;
  Date d_start_;
  char64 from_;
  AccurateTime leaving_time_;
  char64 to_;
  AccurateTime arriving_time_;
  int price_;
  int num_;
};

class OrderManager
{
private:
  OrderManager();
  ~OrderManager();

public:
  static OrderManager &getInstance()
  {
    static OrderManager instance;
    return instance;
  }
  OrderManager(const OrderManager &) = delete;
  OrderManager &operator=(const OrderManager &) = delete;
  void BuyTicket(char64 username, char64 trainID, Date date,
                 int num, char64 from, char64 to, bool allow_pending = false);
  void RefundTicket(char64 username, int order_pos = 1);
  void QueryOrder(char64 username);
  void Clean();
};

#endif