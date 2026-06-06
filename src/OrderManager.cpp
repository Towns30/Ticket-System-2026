#include "../include/OrderManager.hpp"

sjtu::vector<int> OrderManager::GetUserOrderInfoIDs(char64 username)
{
  Utils::Char64Key username_k(username);
  UsernameOrderInfoID low(username_k, INT_MIN);
  UsernameOrderInfoID up(username_k, INT_MAX);
  sjtu::vector<BPlusTree<UsernameOrderInfoID, int>::KeyValue>
      username_order_infoIDs = username_orderinfoID_bpt_.IntervalFind(low, up);
  sjtu::vector<int> order_infoIDs;
  for (auto it : username_order_infoIDs)
  {
    order_infoIDs.push_back(it.key_.order_infoID_);
  }
  return order_infoIDs;
}
void OrderManager::AddFunctionUsernameOrderInfoIDs(char64 username,
                                                   int order_infoID)
{
  Utils::Char64Key username_k(username);
  username_orderinfoID_bpt_.Insert({username_k, order_infoID}, 0);
}
sjtu::vector<int> OrderManager::GetTrainIDDateOrderInfoIDs(char64 trainID,
                                                           Date date)
{
  Utils::Char64Key trainID_k(trainID);
  TrainDateOrderInfoID low(trainID_k, date, INT_MIN);
  TrainDateOrderInfoID up(trainID_k, date, INT_MAX);
  sjtu::vector<BPlusTree<TrainDateOrderInfoID, int>::KeyValue>
      trainID_date_order_infoIDs =
          train_date_order_infoID_bpt_.IntervalFind(low, up);
  sjtu::vector<int> order_infoIDs;
  for (auto it : trainID_date_order_infoIDs)
  {
    order_infoIDs.push_back(it.key_.order_infoID_);
  }
  return order_infoIDs;
}
void OrderManager::AddFunctionTrainIDDateOrderInfoIDs(char64 trainID, Date date,
                                                      int order_infoIDs)
{
  Utils::Char64Key trainID_k(trainID);
  train_date_order_infoID_bpt_.Insert({trainID_k, date, order_infoIDs}, 0);
}
void OrderManager::DeleteFunctionTrainIDDateOrderInfoIDs(char64 trainID,
                                                         Date date,
                                                         int order_infoIDs)
{
  Utils::Char64Key trainID_k(trainID);
  train_date_order_infoID_bpt_.Delete({trainID_k, date, order_infoIDs});
}
void OrderManager::DeleteFunctionUsernameOrderInfoIDs(char64 username,
                                                      int order_infoID)
{
  Utils::Char64Key username_k(username);
  username_orderinfoID_bpt_.Delete({username_k, order_infoID});
}
void OrderManager::PrintOrder(OrderInfo order_info)
{
  if (order_info.status_ == OrderStatus::PENDING)
  {
    std::cout << "[pending] ";
  }
  else if (order_info.status_ == OrderStatus::SUCCESS)
  {
    std::cout << "[success] ";
  }
  else if (order_info.status_ == OrderStatus::REFUNDED)
  {
    std::cout << "[refunded] ";
  }
  std::cout << order_info.trainID_ << ' ' << order_info.from_ << ' '
            << order_info.leaving_time_.ToString() << " -> " << order_info.to_
            << ' ' << order_info.arriving_time_.ToString() << ' '
            << order_info.price_ << ' ' << order_info.num_ << '\n';
}
void OrderManager::DealWaitingList(Date date, TrainInfo train_info,
                                   int train_infoID) // date为这班列车的发车日期
{
  sjtu::vector<int> order_infoIDs_vec =
      GetTrainIDDateOrderInfoIDs(train_info.trainID_, date);
  for (int order_infoID : order_infoIDs_vec)
  {
    OrderInfo order_info;
    order_info_mr_.Read(order_info, order_infoID);
    // std::cerr << "[DEAL] order=" << order_infoID << " num=" <<
    // order_info.num_
    //           << " day=" << order_info.day_pos_ << " seg4seat="
    //           << train_info.res_seat_nums[order_info.day_pos_][4] << '\n';
    if (order_info.status_ != OrderStatus::PENDING)
    {
      continue;
    }
    // 检查座位是否够
    bool is_enough = true;
    for (int pos = order_info.pos_f_; pos <= order_info.pos_t_ - 1; pos++)
    {
      if (train_info.res_seat_nums[order_info.day_pos_][pos] < order_info.num_)
      {
        is_enough = false;
        break;
      }
    }
    if (!is_enough)
    {

      continue;
    }
    // 可以买
    // 先更新座位
    for (int pos = order_info.pos_f_; pos <= order_info.pos_t_ - 1; pos++)
    {
      train_info.res_seat_nums[order_info.day_pos_][pos] -= order_info.num_;
    }
    // 再更新order状态
    order_info.status_ = OrderStatus::SUCCESS;
    order_info_mr_.Update(order_info, order_infoID);
    // 删除候补队列中此条
    DeleteFunctionTrainIDDateOrderInfoIDs(train_info.trainID_, date,
                                          order_infoID);

  } // 更新train_info
  TrainManager::getInstance().UpdateTrainInfo(train_info, train_infoID);
}

OrderManager::OrderManager()
    : username_orderinfoID_bpt_("username-orderinfoID_bpt"),
      train_date_order_infoID_bpt_("train-date-orderinfoID_bpt")
{
  order_info_mr_.Initialise("order_info_mr");
}
OrderManager::~OrderManager() {}
void OrderManager::BuyTicket(char64 username, char64 trainID, Date date,
                             int num, char64 from, char64 to,
                             bool allow_pending)
{
  if (!UserManager::getInstance().IsLogin(username)) // -u未登录
  {
    std::cout << "-1\n";
    return;
  }
  TrainInfo train_info;
  int train_infoID;
  if (!TrainManager::getInstance().GetTrainInfo(trainID, train_info,
                                                train_infoID)) // trainID不存在
  {
    // std::cout << "u\n";
    std::cout << "-1\n";
    return;
  }
  if (train_info.is_released_ == false) // 车次未发布
  {
    // std::cout << "r\n";
    std::cout << "-1\n";
    return;
  }
  // 找pos_f, pos_t
  int pos_f = 0, pos_t = 0;
  bool flag_f = false, flag_t = false;
  for (int pos = 0; pos <= train_info.station_num_ - 1; pos++)
  {
    if (!std::strcmp(train_info.stations_[pos], from))
    {
      flag_f = true;
      pos_f = pos;
    }
    if (!std::strcmp(train_info.stations_[pos], to))
    {
      flag_t = true;
      pos_t = pos;
    }
    if (flag_f && flag_t)
    {
      break;
    }
  }
  if (!flag_f || !flag_t) // 无from或tos
  {
    // std::cout << "y\n";
    std::cout << "-1\n";
    return;
  }
  // 找是第几班车
  AccurateTime start_leaving_time_f(
      train_info.start_sale_date_,
      train_info.start_time_.AddMinites(train_info.leaving_times_[pos_f]));
  AccurateTime end_leaving_time_f(
      train_info.end_sale_date_,
      train_info.start_time_.AddMinites(train_info.leaving_times_[pos_f]));
  if (start_leaving_time_f >= AccurateTime(date.AddDays(1), HourMinite()) ||
      end_leaving_time_f <
          AccurateTime(
              date,
              HourMinite())) // 最早的发车一天在要求日期之后，或者最晚的发车一天在要求日期之前
  {
    // std::cout << "x\n";
    std::cout << "-1\n";
    return;
  }
  int day_len =
      train_info.end_sale_date_.DaysBetween(train_info.start_sale_date_);
  int day_pos = -1;
  for (int i = 0; i <= day_len; i++)
  {
    AccurateTime tmp_start_leaving_time_f = start_leaving_time_f;
    // std::cout << "DEBUG Buy: day_pos=" << day_pos << " date=" <<
    // date.ToString()
    //       << " start_sale=" << train_info.start_sale_date_.ToString()
    //       << " res_seat_nums[" << day_pos << "][...] deducted\n";
    if (tmp_start_leaving_time_f.AddDays(i) >= AccurateTime(date, HourMinite()))
    {
      day_pos = i;
      break;
    }
  }
  AccurateTime leaving_time_f = start_leaving_time_f.AddDays(day_pos);
  AccurateTime arriving_time_t =
      start_leaving_time_f.AddDays(day_pos).AddMinites(
          train_info.arrive_times_[pos_t] - train_info.leaving_times_[pos_f]);
  // 检查最多能买多少票
  int max_seat = 200005;
  for (int pos = pos_f; pos <= pos_t - 1; pos++)
  {
    if (train_info.res_seat_nums[day_pos][pos] < max_seat)
    {
      max_seat = train_info.res_seat_nums[day_pos][pos];
    }
  }
  // std::cout << "max_seat = " << max_seat << '\n';
  if (max_seat >= num) // 可以买
  {
    // 更新train_info
    for (int pos = pos_f; pos <= pos_t - 1; pos++)
    {
      train_info.res_seat_nums[day_pos][pos] -= num;
    }
    TrainManager::getInstance().UpdateTrainInfo(train_info, train_infoID);
    // 更新订单信息
    OrderInfo order_info;
    std::strcpy(order_info.username_, username);
    order_info.status_ = OrderStatus::SUCCESS;
    std::strcpy(order_info.trainID_, trainID);
    order_info.d_start_ = date;
    order_info.leaving_time_ = leaving_time_f;
    order_info.arriving_time_ = arriving_time_t;
    order_info.price_ =
        train_info.total_prices_[pos_t] - train_info.total_prices_[pos_f];
    order_info.train_start_date_ = train_info.start_sale_date_.AddDays(day_pos);
    std::strcpy(order_info.from_, from);
    std::strcpy(order_info.to_, to);
    order_info.pos_f_ = pos_f;
    order_info.pos_t_ = pos_t;
    order_info.day_pos_ = day_pos;
    order_info.num_ = num;
    int order_infoID = order_info_mr_.Write(order_info);
    // 更新用户订单映射
    AddFunctionUsernameOrderInfoIDs(username, order_infoID);
    // std::cout << "price: " << order_info.price_ << "  num: " <<
    // order_info.num_ << '\n';
    std::cout << order_info.price_ * order_info.num_ << '\n';
  }
  else
  {
    if (allow_pending == false) // 不支持候补
    {
      // std::cout << "al\n";
      std::cout << "-1\n";
      return;
    }
    // 支持候补
    // 创建并写入订单
    OrderInfo order_info;
    std::strcpy(order_info.username_, username);
    order_info.status_ = OrderStatus::PENDING;
    std::strcpy(order_info.trainID_, trainID);
    order_info.d_start_ = date;
    order_info.leaving_time_ = leaving_time_f;
    order_info.arriving_time_ = arriving_time_t;
    order_info.price_ =
        train_info.total_prices_[pos_t] - train_info.total_prices_[pos_f];
    order_info.train_start_date_ = train_info.start_sale_date_.AddDays(day_pos);
    std::strcpy(order_info.from_, from);
    std::strcpy(order_info.to_, to);
    order_info.pos_f_ = pos_f;
    order_info.pos_t_ = pos_t;
    order_info.day_pos_ = day_pos;
    order_info.num_ = num;
    int order_infoID = order_info_mr_.Write(order_info);
    // 更新用户订单映射
    AddFunctionUsernameOrderInfoIDs(username, order_infoID);
    // 更新候补队列映射
    AddFunctionTrainIDDateOrderInfoIDs(
        trainID, order_info.train_start_date_,
        order_infoID); // 注意此时插入的日期是这班列车的出发日期
    std::cout << "queue\n";
  }
}
void OrderManager::RefundTicket(char64 username,
                                int order_pos) // order_pos 1-based
{
  if (!UserManager::getInstance().IsLogin(username)) // -u未登录
  {
    std::cout << "-1\n";
    return;
  }
  sjtu::vector<int> order_infoIDs_vec = GetUserOrderInfoIDs(username);
  if (order_pos > order_infoIDs_vec.size() || order_pos <= 0) // 超出订单范围
  {
    std::cout << "-1\n";
    return;
  }
  int order_infoID = order_infoIDs_vec[order_infoIDs_vec.size() - order_pos];
  OrderInfo order_info;
  order_info_mr_.Read(order_info, order_infoID);
  if (order_info.status_ == OrderStatus::REFUNDED) // 已经退订
  {
    std::cout << "-1\n";
    return;
  }
  if (order_info.status_ ==
      OrderStatus::PENDING) // 还没买上，只需删除订单的映射
  {
    DeleteFunctionTrainIDDateOrderInfoIDs(order_info.trainID_,
                                          order_info.train_start_date_,
                                          order_infoID); // 将其从候补队列删除
    order_info.status_ = OrderStatus::REFUNDED;
    order_info_mr_.Update(order_info, order_infoID);
    std::cout << "0\n";
  }
  else if (order_info.status_ == OrderStatus::SUCCESS)
  {
    // 更新列车剩余座位信息
    TrainInfo train_info;
    int train_infoID;
    TrainManager::getInstance().GetTrainInfo(order_info.trainID_, train_info,
                                             train_infoID);
    // std::cerr << "[REFUND] order=" << order_infoID << " num=" << order_info.num_
    //           << " day=" << order_info.day_pos_ << " seg4seat="
    //           << train_info.res_seat_nums[order_info.day_pos_][4] << '\n';

    for (int i = order_info.pos_f_; i <= order_info.pos_t_ - 1; i++)
    {
      train_info.res_seat_nums[order_info.day_pos_][i] += order_info.num_;
    }
    // 更新订单状态
    order_info.status_ = OrderStatus::REFUNDED;
    order_info_mr_.Update(order_info, order_infoID);
    // 更新候补队列
    DealWaitingList(order_info.train_start_date_, train_info, train_infoID);
    std::cout << "0\n";
  }
}
void OrderManager::QueryOrder(char64 username)
{
  if (!UserManager::getInstance().IsLogin(username)) // -u未登录
  {
    std::cout << "-1\n";
    return;
  }
  sjtu::vector<int> order_infoIDs_vec = GetUserOrderInfoIDs(username);
  std::cout << order_infoIDs_vec.size() << '\n';
  for (int i = order_infoIDs_vec.size() - 1; i >= 0; i--)
  {
    int order_infoID = order_infoIDs_vec[i];
    OrderInfo order_info;
    order_info_mr_.Read(order_info, order_infoID);
    PrintOrder(order_info);
  }
}
void OrderManager::Clean()
{
  username_orderinfoID_bpt_.Clean();
  train_date_order_infoID_bpt_.Clean();
  order_info_mr_.Clean();
}