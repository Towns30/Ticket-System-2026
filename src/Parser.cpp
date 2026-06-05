#include "../include/Parser.hpp"
#include "../include/Utils.hpp"

sjtu::vector<std::string>
Parser::GetPrefixValues(const sjtu::vector<Token> &tokens,
                        const std::string &key)
{
  for (int i = 2; i < tokens.size(); i += 2)
  {
    if (tokens[i].value_[0] == key)
    {
      return tokens[i + 1].value_;
    }
  }
  return {};
}

std::string Parser::GetPrefixValue(const sjtu::vector<Token> &tokens,
                                   const std::string &key)
{
  for (int i = 2; i < tokens.size(); i += 2)
  {
    if (tokens[i].value_[0] == key)
    {
      return tokens[i + 1].value_[0];
    }
  }
  return "";
}

void Parser::ParseLine(const sjtu::vector<Token> &tokens)
{
  int timestamp = Utils::StringToInt(tokens[0].value_[0]);
  std::string opt = tokens[1].value_[0];
  std::cout << '[' << timestamp << "] ";
  if (opt == "add_user")
  {
    char64 cur_username;
    Utils::StringToChar64(GetPrefixValue(tokens, "-c"), cur_username);
    UserInfo user_info;
    Utils::StringToChar64(GetPrefixValue(tokens, "-u"), user_info.username_);
    Utils::StringToChar64(GetPrefixValue(tokens, "-p"), user_info.password_);
    Utils::StringToChar64(GetPrefixValue(tokens, "-n"), user_info.name_);
    Utils::StringToChar64(GetPrefixValue(tokens, "-m"), user_info.mail_addr_);
    user_info.privilege_ = Utils::StringToInt(GetPrefixValue(tokens, "-g"));
    UserManager::getInstance().AddUserInfo(cur_username, user_info);
  }
  else if (opt == "login")
  {
    char64 username;
    Utils::StringToChar64(GetPrefixValue(tokens, "-u"), username);
    char64 password;
    Utils::StringToChar64(GetPrefixValue(tokens, "-p"), password);
    UserManager::getInstance().Login(username, password);
  }
  else if (opt == "logout")
  {
    char64 username;
    Utils::StringToChar64(GetPrefixValue(tokens, "-u"), username);
    UserManager::getInstance().Logout(username);
  }
  else if (opt == "query_profile")
  {
    char64 cur_username;
    Utils::StringToChar64(GetPrefixValue(tokens, "-c"), cur_username);
    char64 username;
    Utils::StringToChar64(GetPrefixValue(tokens, "-u"), username);
    UserManager::getInstance().QueryUserInfo(cur_username, username);
  }
  else if (opt == "modify_profile")
  {
    char64 cur_username;
    Utils::StringToChar64(GetPrefixValue(tokens, "-c"), cur_username);
    char64 username;
    Utils::StringToChar64(GetPrefixValue(tokens, "-u"), username);
    UserModifyInfo modify_info;
    for (int i = 2; i <= tokens.size() - 1; i += 2)
    {
      if (tokens[i].value_[0] == "-p")
      {
        Utils::StringToChar64(tokens[i + 1].value_[0], modify_info.password_);
      }
      if (tokens[i].value_[0] == "-n")
      {
        Utils::StringToChar64(tokens[i + 1].value_[0], modify_info.name_);
      }
      if (tokens[i].value_[0] == "-m")
      {
        Utils::StringToChar64(tokens[i + 1].value_[0], modify_info.mail_addr_);
      }
      if (tokens[i].value_[0] == "-g")
      {
        modify_info.privilege_ = Utils::StringToInt(tokens[i + 1].value_[0]);
      }
    }
    UserManager::getInstance().ModifyUserInfo(cur_username, username,
                                              modify_info);
  }
  else if (opt == "add_train")
  {
    TrainInfo train_info;
    Utils::StringToChar64(GetPrefixValue(tokens, "-i"), train_info.trainID_);
    train_info.station_num_ = Utils::StringToInt(GetPrefixValue(tokens, "-n"));
    train_info.seat_num_ = Utils::StringToInt(GetPrefixValue(tokens, "-m"));
    sjtu::vector<std::string> tmp = GetPrefixValues(tokens, "-s");
    for (int i = 0; i <= tmp.size() - 1; i++)
    {
      Utils::StringToChar64(tmp[i], train_info.stations_[i]);
    }
    tmp = GetPrefixValues(tokens, "-p");
    for (int i = 0; i <= tmp.size() - 1; i++)
    {
      train_info.prices_[i] = Utils::StringToInt(tmp[i]);
    }
    train_info.start_time_ = HourMinite(GetPrefixValue(tokens, "-x"));
    tmp = GetPrefixValues(tokens, "-t");
    for (int i = 0; i <= tmp.size() - 1; i++)
    {
      train_info.travel_times_[i] = Utils::StringToInt(tmp[i]);
    }
    tmp = GetPrefixValues(tokens, "-o");
    for (int i = 0; i <= tmp.size() - 1; i++)
    {
      train_info.stopover_times_[i] = Utils::StringToInt(tmp[i]);
    }
    tmp = GetPrefixValues(tokens, "-d");
    train_info.start_sale_date_ = Date(tmp[0]);
    train_info.end_sale_date_ = Date(tmp[1]);
    train_info.type_ = GetPrefixValue(tokens, "-y")[0];
    train_info.is_released_ = false;
    for (int i = 0; i <= train_info.end_sale_date_.DaysBetween(
                             train_info.start_sale_date_);
         i++)
    {
      for (int j = 0; j <= train_info.station_num_ - 1; j++)
      {
        train_info.res_seat_nums[i][j] = train_info.seat_num_;
      }
    }
    TrainManager::getInstance().AddTrain(train_info);
  }
  else if (opt == "delete_train")
  {
    char64 trainID;
    Utils::StringToChar64(GetPrefixValue(tokens, "-i"), trainID);
    TrainManager::getInstance().DeleteTrain(trainID);
  }
  else if (opt == "release_train")
  {
    char64 trainID;
    Utils::StringToChar64(GetPrefixValue(tokens, "-i"), trainID);
    TrainManager::getInstance().ReleaseTrain(trainID);
  }
  else if (opt == "query_train")
  {
    char64 trainID;
    Utils::StringToChar64(GetPrefixValue(tokens, "-i"), trainID);
    Date date(GetPrefixValue(tokens, "-d"));
    TrainManager::getInstance().QueryTrainInfo(trainID, date);
  }
  else if (opt == "query_ticket")
  {
    char64 from;
    Utils::StringToChar64(GetPrefixValue(tokens, "-s"), from);
    char64 to;
    Utils::StringToChar64(GetPrefixValue(tokens, "-t"), to);
    Date date(GetPrefixValue(tokens, "-d"));
    SortBy sort_by;
    if (tokens.size() == 10 && GetPrefixValue(tokens, "-p") == "cost")
    {
      sort_by = SortBy::COST;
    }
    else
    {
      sort_by = SortBy::TIME;
    }
    TrainManager::getInstance().QueryTicket(from, to, date, sort_by);
  }
  else if (opt == "query_transfer")
  {
    char64 from;
    Utils::StringToChar64(GetPrefixValue(tokens, "-s"), from);
    char64 to;
    Utils::StringToChar64(GetPrefixValue(tokens, "-t"), to);
    Date date(GetPrefixValue(tokens, "-d"));
    SortBy sort_by;
    if (tokens.size() == 10 && GetPrefixValue(tokens, "-p") == "cost")
    {
      sort_by = SortBy::COST;
    }
    else
    {
      sort_by = SortBy::TIME;
    }
    TrainManager::getInstance().QueryTransfer(from, to, date,
                                              sort_by);
  }
  else if (opt == "buy_ticket")
  {
    char64 username;
    Utils::StringToChar64(GetPrefixValue(tokens, "-u"), username);
    char64 trainID;
    Utils::StringToChar64(GetPrefixValue(tokens, "-i"), trainID);
    Date date(GetPrefixValue(tokens, "-d"));
    int num = Utils::StringToInt(GetPrefixValue(tokens, "-n"));
    char64 from;
    Utils::StringToChar64(GetPrefixValue(tokens, "-f"), from);
    char64 to;
    Utils::StringToChar64(GetPrefixValue(tokens, "-t"), to);
    bool allow_pending;
    if (tokens.size() == 16 && GetPrefixValue(tokens, "-q") == "true")
    {
      allow_pending = true;
    }
    else
    {
      allow_pending = false;
    }
    OrderManager::getInstance().BuyTicket(username, trainID, date,
                                          num, from, to, allow_pending);
  }
  else if (opt == "query_order")
  {
    char64 username;
    Utils::StringToChar64(GetPrefixValue(tokens, "-u"), username);
    OrderManager::getInstance().QueryOrder(username);
  }
  else if (opt == "refund_ticket")
  {
    char64 username;
    Utils::StringToChar64(GetPrefixValue(tokens, "-u"), username);
    int order_pos = 1;
    if (tokens.size() == 6 && GetPrefixValue(tokens, "-n") != "1")
    {
      order_pos = Utils::StringToInt(GetPrefixValue(tokens, "-n"));
    }
    OrderManager::getInstance().RefundTicket(username, order_pos);
  }
  else if (opt == "clean")
  {
    UserManager::getInstance().Clean();
    TrainManager::getInstance().Clean();
    OrderManager::getInstance().Clean();
    std::cout << "0\n";
  }
  else if (opt == "exit")
  {
    UserManager::getInstance().CleanLogin();
    std::cout << "bye\n";
    exit(0);
  }
}