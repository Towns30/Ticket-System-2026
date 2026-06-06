#include "../include/TrainManager.hpp"

TrainManager::TrainManager()
    : trainID_traininfoID_bpt_("trainID-traininfoID_bpt"),
      station_traininfoID_bpt_("station-traininfoID_bpt"),
      start_end_traininfoID_bpt_("start_end-traininfoID_bpt")
{
  train_info_mr_.Initialise("train_info_mr");
}
TrainManager::~TrainManager() {}

int TrainManager::WriteTrainInfo(TrainInfo train_info)
{
  int train_infoID = train_info_mr_.Write(train_info);
  return train_infoID;
}
void TrainManager::UpdateTrainInfo(TrainInfo train_info, int train_infoID)
{
  train_info_mr_.Update(train_info, train_infoID);
}
void TrainManager::AddFunctionStartEndID(char64 start, char64 end,
                                         int train_infoID)
{
  StartEndTrainInfoID start_end_traininfoID(
      Utils::Char64Key(start), Utils::Char64Key(end), train_infoID);
  start_end_traininfoID_bpt_.Insert(start_end_traininfoID, 0);
}
void TrainManager::AddFunctionTrainIDTrainInfoID(char64 trainID,
                                                 int train_infoID)
{
  Utils::Char64Key trainID_k(trainID);
  trainID_traininfoID_bpt_.Insert(trainID_k, train_infoID);
}
void TrainManager::AddFunctionStationID(char64 station, int train_infoID)
{
  StationTrainInfoID station_traininfoID(Utils::Char64Key(station),
                                         train_infoID);
  station_traininfoID_bpt_.Insert(station_traininfoID, 0);
}
bool TrainManager::GetTrainInfo(char64 trainID, TrainInfo &train_info,
                                int &train_infoID)
{
  Utils::Char64Key trainID_k(trainID);
  if (!trainID_traininfoID_bpt_.Find(trainID_k, train_infoID))
  {
    return false;
  }
  train_info_mr_.Read(train_info, train_infoID);
  return true;
}
sjtu::vector<int> TrainManager::QueryStartEndID(char64 start, char64 end)
{
  StartEndTrainInfoID low(Utils::Char64Key(start), Utils::Char64Key(end),
                          INT_MIN);
  StartEndTrainInfoID up(Utils::Char64Key(start), Utils::Char64Key(end),
                         INT_MAX);
  sjtu::vector<BPlusTree<StartEndTrainInfoID, int>::KeyValue>
      start_end_traininfoID_vec =
          start_end_traininfoID_bpt_.IntervalFind(low, up);
  sjtu::vector<int> traininfoID_vec;
  for (auto it : start_end_traininfoID_vec)
  {
    traininfoID_vec.push_back(it.key_.train_infoID_);
  }
  return traininfoID_vec;
}
sjtu::vector<int> TrainManager::QueryStationID(char64 station)
{
  StationTrainInfoID low(Utils::Char64Key(station), INT_MIN);
  StationTrainInfoID up(Utils::Char64Key(station), INT_MAX);
  sjtu::vector<BPlusTree<StationTrainInfoID, int>::KeyValue>
      station_traininfoID_vec = station_traininfoID_bpt_.IntervalFind(low, up);
  sjtu::vector<int> traininfoID_vec;
  for (auto it : station_traininfoID_vec)
  {
    traininfoID_vec.push_back(it.key_.train_infoID_);
  }
  return traininfoID_vec;
}
void TrainManager::DeleteFunctionTrainIDTrainInfoID(char64 trainID)
{
  Utils::Char64Key trainID_k(trainID);
  trainID_traininfoID_bpt_.Delete(trainID_k);
}
void TrainManager::PrintTrain(TrainInfo train_info, Date date)
{
  std::cout << train_info.trainID_ << ' ' << train_info.type_ << "\n";
  for (int i = 0; i < train_info.station_num_; i++)
  {
    if (i == 0) // 始发站
    {
      std::cout << train_info.stations_[0] << ' ' << "xx-xx xx:xx" << " -> "
                << AccurateTime(date, train_info.start_time_).ToString() << ' '
                << "0" << ' '
                << train_info.res_seat_nums[date.DaysBetween(
                       train_info.start_sale_date_)][0]
                << '\n';
    }
    else if (i == train_info.station_num_ - 1)
    {
      std::cout << train_info.stations_[i] << ' '
                << AccurateTime(date, train_info.start_time_)
                       .AddMinites(train_info.arrive_times_[i])
                       .ToString()
                << " -> " << "xx-xx xx:xx" << ' '
                << train_info.total_prices_[train_info.station_num_ - 1] << ' '
                << "x" << '\n';
    }
    else
    {
      std::cout << train_info.stations_[i] << ' '
                << AccurateTime(date, train_info.start_time_)
                       .AddMinites(train_info.arrive_times_[i])
                       .ToString()
                << " -> "
                << AccurateTime(date, train_info.start_time_)
                       .AddMinites(train_info.leaving_times_[i])
                       .ToString()
                << ' ' << train_info.total_prices_[i] << ' '
                << train_info.res_seat_nums[date.DaysBetween(
                       train_info.start_sale_date_)][i]
                << '\n';
    }
  }
}
void TrainManager::AddTrain(TrainInfo train_info)
{
  TrainInfo _train_info;
  int train_infoID;
  if (GetTrainInfo(train_info.trainID_, _train_info,
                   train_infoID)) // trainID已存在
  {
    std::cout << "-1\n";
    return;
  }
  train_infoID = WriteTrainInfo(train_info);
  // 添加trainID-traininfoID映射
  AddFunctionTrainIDTrainInfoID(train_info.trainID_, train_infoID);
  std::cout << "0\n";
}
void TrainManager::QueryTrainInfo(char64 trainID, Date date)
{
  TrainInfo train_info;
  int _train_infoID;
  if (!GetTrainInfo(trainID, train_info, _train_infoID)) // tarinID不存在
  {
    std::cout << "-1\n";
    return;
  }
  if (date < train_info.start_sale_date_ ||
      date > train_info.end_sale_date_) // 查询日期未发车
  {
    std::cout << "-1\n";
    return;
  }
  PrintTrain(train_info, date);
}
void TrainManager::QueryTicket(char64 from, char64 to, Date date,
                               SortBy sort_by)
{
  sjtu::vector<int> train_infoIDs_vec = QueryStartEndID(from, to);
  sjtu::priority_queue<TicketResult, CostCompare> pq_cost;
  sjtu::priority_queue<TicketResult, TimeCompare> pq_time;
  for (int train_infoID : train_infoIDs_vec)
  {
    TrainInfo train_info;
    train_info_mr_.Read(train_info, train_infoID);
    // 找起点和终点站的位置
    int pos_f = 0, pos_t = 0;
    bool flag_f = false, flag_t = false;
    for (int i = 0; i < train_info.station_num_; i++)
    {
      if (!std::strcmp(train_info.stations_[i], from))
      {
        flag_f = true;
        pos_f = i;
      }
      if (!std::strcmp(train_info.stations_[i], to))
      {
        flag_t = true;
        pos_t = i;
      }
      if (flag_f && flag_t)
      {
        break;
      }
    }
    AccurateTime start_leaving_time_f(
        train_info.start_sale_date_,
        train_info.start_time_.AddMinites(train_info.leaving_times_[pos_f]));
    AccurateTime start_arriving_time_t(
        train_info.start_sale_date_,
        train_info.start_time_.AddMinites(train_info.arrive_times_[pos_t]));
    int day_len = train_info.end_sale_date_.DaysBetween(
        train_info.start_sale_date_); // 列车共发车间隔多少天
    Date tmp_date = date;
    if (start_leaving_time_f >=
        AccurateTime(tmp_date.AddDays(1),
                     HourMinite())) // 最早一班至少在要求日期后一天
    {
      continue;
    }
    for (int i = 0; i <= day_len; i++)
    {
      AccurateTime tmp_start_leaving_time_f = start_leaving_time_f;
      if (tmp_start_leaving_time_f.AddDays(i) >=
          AccurateTime(date, HourMinite())) // 找到列车符合要求的那一班车
      {
        int total_price =
            train_info.total_prices_[pos_t] - train_info.total_prices_[pos_f];
        int duration =
            train_info.arrive_times_[pos_t] - train_info.leaving_times_[pos_f];
        int max_seat = 200000;
        for (int j = pos_f; j <= pos_t - 1; j++)
        {
          // std::cout << "DEBUG Query: i=" << i << " seg=" << j
          // << " seat=" << train_info.res_seat_nums[i][j] << "\n";
          if (train_info.res_seat_nums[i][j] < max_seat)
          {
            max_seat = train_info.res_seat_nums[i][j];
          }
        }
        AccurateTime leaving_time = start_leaving_time_f.AddDays(i);
        AccurateTime arriving_time = start_arriving_time_t.AddDays(i);
        TicketResult ticket_result;
        std::strcpy(ticket_result.trainID_, train_info.trainID_);
        std::strcpy(ticket_result.from_, from);
        std::strcpy(ticket_result.to_, to);
        ticket_result.leaving_time_ = leaving_time;
        ticket_result.arriving_time_ = arriving_time;
        ticket_result.duration_ = duration;
        ticket_result.total_price_ = total_price;
        ticket_result.max_seat_ = max_seat;
        if (sort_by == SortBy::COST)
        {
          pq_cost.push(ticket_result);
        }
        else
        {
          pq_time.push(ticket_result);
        }
        break;
      }
    }
  }
  if (sort_by == SortBy::COST)
  {
    std::cout << pq_cost.size() << '\n';
    while (!pq_cost.empty())
    {
      std::cout << pq_cost.top();
      pq_cost.pop();
    }
  }
  else
  {
    std::cout << pq_time.size() << '\n';
    while (!pq_time.empty())
    {
      std::cout << pq_time.top();
      pq_time.pop();
    }
  }
}
void TrainManager::QueryTransfer(char64 from, char64 to, Date date,
                                 SortBy sort_by)
{
  // std::cerr << "from = " << from << '\n';
  sjtu::priority_queue<TransferResult, CostCompare> pq_cost_whole;
  sjtu::priority_queue<TransferResult, TimeCompare> pq_time_whole;
  sjtu::vector<int> train_infoIDs_vec = QueryStationID(from);
  if (train_infoIDs_vec.empty())
  {
    std::cout << "0\n";
    return;
  }
  for (int train_infoID : train_infoIDs_vec)
  {
    TrainInfo train_info_1;
    train_info_mr_.Read(train_info_1, train_infoID);
    int pos_f = 0;
    // for (int i = 0; i <= train_info_1.station_num_ - 1; i++)
    // {
    //   std::cerr << train_info_1.stations_[i] << ' ';
    // }
    // std::cerr << '\n';
    for (int i = 0; i <= train_info_1.station_num_ - 1; i++)
    {
      if (!std::strcmp(train_info_1.stations_[i], from))
      {
        // std::cerr << "pos_f = " << i << '\n';
        pos_f = i;
        break;
      }
    }
    // std::cout << pos_f << "\n";
    AccurateTime start_leaving_time_f = AccurateTime(
        train_info_1.start_sale_date_, train_info_1.start_time_.AddMinites(
                                           train_info_1.leaving_times_[pos_f]));
    AccurateTime end_leaving_time_f(train_info_1.end_sale_date_,
                                    train_info_1.start_time_.AddMinites(
                                        train_info_1.leaving_times_[pos_f]));
    int day_len = train_info_1.end_sale_date_.DaysBetween(
        train_info_1.start_sale_date_); // 列车共发车间隔多少天
    if (start_leaving_time_f >= AccurateTime(date.AddDays(1), HourMinite()) ||
        end_leaving_time_f <
            AccurateTime(
                date,
                HourMinite())) // 最早一班至少在要求日期后一天，或者最晚的发车一天在要求日期之前
    {
      continue;
    }
    AccurateTime leaving_time_f;
    int day_pos_1 = 0; // 第一辆列车是第几班
    for (int day_offset_1 = 0; day_offset_1 <= day_len; day_offset_1++)
    {
      AccurateTime tmp_start_leaving_time_f = start_leaving_time_f;
      if (tmp_start_leaving_time_f.AddDays(day_offset_1) >=
          AccurateTime(date, HourMinite()))
      {
        day_pos_1 = day_offset_1;
        leaving_time_f = start_leaving_time_f.AddDays(day_offset_1);
        break;
      }
    }
    // 遍历中转站
    for (int pos_trans_1 = pos_f + 1;
         pos_trans_1 <= train_info_1.station_num_ - 1; pos_trans_1++)
    {
      sjtu::priority_queue<TransferResult, CostCompare> pq_cost;
      sjtu::priority_queue<TransferResult, TimeCompare> pq_time;
      AccurateTime arriving_time_trans =
          leaving_time_f.AddMinites(train_info_1.arrive_times_[pos_trans_1] -
                                    train_info_1.leaving_times_[pos_f]);
      int max_seat_1 = 200005;
      for (int j = pos_f; j <= pos_trans_1 - 1; j++)
      {
        if (train_info_1.res_seat_nums[day_pos_1][j] < max_seat_1)
        {
          max_seat_1 = train_info_1.res_seat_nums[day_pos_1][j];
        }
      }
      // 第一部分赋值
      int total_price_1 = train_info_1.total_prices_[pos_trans_1] -
                          train_info_1.total_prices_[pos_f];
      int duartion_1 = train_info_1.arrive_times_[pos_trans_1] -
                       train_info_1.leaving_times_[pos_f];
      TransferResult transfer_result;
      std::strcpy(transfer_result.trainID_1_, train_info_1.trainID_);
      transfer_result.leaving_time_1_ = leaving_time_f;
      transfer_result.arriving_time_1_ = arriving_time_trans;
      std::strcpy(transfer_result.from_, from);
      std::strcpy(transfer_result.trans_, train_info_1.stations_[pos_trans_1]);
      transfer_result.duration_1_ = duartion_1;
      transfer_result.total_price_1_ = total_price_1;
      transfer_result.max_seat_1_ = max_seat_1;
      // 找第二个车次
      char64 trans;
      std::strcpy(trans, train_info_1.stations_[pos_trans_1]);
      sjtu::vector<int> train_infoIDs_2_vec =
          QueryStartEndID(train_info_1.stations_[pos_trans_1], to);
      if (train_infoIDs_2_vec.empty()) // 没有符合的
      {
        continue;
      }
      for (int train_infoID_2 : train_infoIDs_2_vec) // 遍历第二个列车车次
      {

        TrainInfo train_info_2;
        train_info_mr_.Read(train_info_2, train_infoID_2);
        if (!std::strcmp(train_info_1.trainID_,
                         train_info_2.trainID_)) // 同一列车不算换乘
        {
          continue;
        }
        // 找两下标pos_trans_2, pos_to
        int pos_trans_2 = 0;
        int pos_to = 0;
        bool flag_trans = false, flag_to = false;
        for (int i = 0; i < train_info_2.station_num_; i++)
        {
          if (!std::strcmp(train_info_2.stations_[i], trans))
          {
            flag_trans = true;
            pos_trans_2 = i;
          }
          if (!std::strcmp(train_info_2.stations_[i], to))
          {
            flag_to = true;
            pos_to = i;
          }
          if (flag_trans && flag_to)
          {
            break;
          }
        }
        // 找day_pos_2
        AccurateTime start_leaving_time_trans =
            AccurateTime(train_info_2.start_sale_date_,
                         train_info_2.start_time_.AddMinites(
                             train_info_2.leaving_times_[pos_trans_2]));
        AccurateTime end_leaving_time_trans =
            AccurateTime(train_info_2.end_sale_date_,
                         train_info_2.start_time_.AddMinites(
                             train_info_2.leaving_times_[pos_trans_2]));
        if (end_leaving_time_trans <
            arriving_time_trans) // 最后一班车的发车都早于到达中转站的时间
        {
          continue;
        }
        int day_len = train_info_2.end_sale_date_.DaysBetween(
            train_info_2.start_sale_date_);
        int day_pos_2 = 0;
        for (int day_offset = 0; day_offset <= day_len; day_offset++)
        {
          AccurateTime tmp_start_leaving_time_trans = start_leaving_time_trans;
          if (tmp_start_leaving_time_trans.AddDays(day_offset) >=
              arriving_time_trans) // 找到第一班来得及换乘的班次
          {
            day_pos_2 = day_offset;
            break;
          }
        } // 第二辆列车赋值
        AccurateTime leaving_time_trans =
            start_leaving_time_trans.AddDays(day_pos_2);
        AccurateTime arriving_time_to =
            start_leaving_time_trans.AddDays(day_pos_2).AddMinites(
                train_info_2.arrive_times_[pos_to] -
                train_info_2.leaving_times_[pos_trans_2]);
        int duartion_2 = train_info_2.arrive_times_[pos_to] -
                         train_info_2.leaving_times_[pos_trans_2];
        int total_price_2 = train_info_2.total_prices_[pos_to] -
                            train_info_2.total_prices_[pos_trans_2];
        int max_seat_2 = 200005;
        for (int i = pos_trans_2; i < pos_to; i++)
        {
          if (train_info_2.res_seat_nums[day_pos_2][i] < max_seat_2)
          {
            max_seat_2 = train_info_2.res_seat_nums[day_pos_2][i];
          }
        }
        transfer_result.duration_2_ = duartion_2;
        transfer_result.total_price_2_ = total_price_2;
        transfer_result.max_seat_2_ = max_seat_2;
        transfer_result.leaving_time_2_ = leaving_time_trans;
        transfer_result.arriving_time_2_ = arriving_time_to;
        std::strcpy(transfer_result.to_, to);
        std::strcpy(transfer_result.trainID_2_, train_info_2.trainID_);
        if (sort_by == SortBy::COST)
        {
          pq_cost.push(transfer_result);
        }
        else
        {
          pq_time.push(transfer_result);
        }
      }
      // 得出在此中转站下最优解
      if (sort_by == SortBy::COST)
      {
        if (!pq_cost.empty())
        {
          pq_cost_whole.push(pq_cost.top());
        }
      }
      else
      {
        if (!pq_time.empty())
        {
          pq_time_whole.push(pq_time.top());
        }
      }
    }
  }
  if (sort_by == SortBy::COST)
  {
    if (pq_cost_whole.empty())
    {
      std::cout << "0\n";
    }
    else
    {
      std::cout << pq_cost_whole.top();
    }
  }
  else
  {
    if (pq_time_whole.empty())
    {
      std::cout << "0\n";
    }
    else
    {
      std::cout << pq_time_whole.top();
    }
  }
}
void TrainManager::ModifyTrainInfo(char64 trainID, AccurateTime leaving_time,
                                   int pos_f, int pos_t, int seat_num, int dir)
{
  TrainInfo train_info;
  int train_infoID;
  GetTrainInfo(trainID, train_info, train_infoID);
  AccurateTime start_time =
      leaving_time.AddMinites(-train_info.leaving_times_[pos_f]);
  Date start_date = start_time.GetDate();
  int day_pos = start_date.DaysBetween(train_info.start_sale_date_);
  for (int i = pos_f; i <= pos_t - 1; i++)
  {
    train_info.res_seat_nums[day_pos][i] += dir * seat_num;
  }
  UpdateTrainInfo(train_info, train_infoID);
}
void TrainManager::ReleaseTrain(char64 trainID)
{
  TrainInfo train_info;
  int train_infoID;
  if (!GetTrainInfo(trainID, train_info, train_infoID)) // trainID不存在
  {
    std::cout << "-1\n";
    return;
  }
  if (train_info.is_released_) // train已发布
  {
    std::cout << "-1\n";
    return;
  }
  train_info.is_released_ = true;
  // 添加两映射
  for (int i = 0; i < train_info.station_num_; i++)
  {
    AddFunctionStationID(train_info.stations_[i], train_infoID);
  }
  for (int i = 0; i < train_info.station_num_ - 1; i++)
  {
    for (int j = i + 1; j < train_info.station_num_; j++)
    {
      AddFunctionStartEndID(train_info.stations_[i], train_info.stations_[j],
                            train_infoID);
    }
  }
  // train_info写回
  train_info_mr_.Update(train_info, train_infoID);
  std::cout << "0\n";
}
void TrainManager::DeleteTrain(char64 trainID)
{
  TrainInfo train_info;
  int train_infoID;
  if (!GetTrainInfo(trainID, train_info, train_infoID)) // trainID不存在
  {
    std::cout << "-1\n";
    return;
  }
  if (train_info.is_released_) // train已发布，不可删除
  {
    std::cout << "-1\n";
    return;
  }
  DeleteFunctionTrainIDTrainInfoID(
      trainID); // 删除索引就相当于删除了这辆列车，因为找不到了
  std::cout << "0\n";
}
void TrainManager::Clean()
{
  train_info_mr_.Clean();
  trainID_traininfoID_bpt_.Clean();
  station_traininfoID_bpt_.Clean();
  start_end_traininfoID_bpt_.Clean();
}