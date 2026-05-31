#ifndef USER_MANAGER_HPP
#define USER_MANAGER_HPP
#include "Utils.hpp"
#include "BPlusTree.hpp"

struct UserInfo
{
  char64 username_;
  char64 password_;
  char64 name_;
  char64 mail_addr_;
  int privilege_;
};

struct UserModifyInfo
{
  char64 password_ = "";
  char64 name_ = "";
  char64 mail_addr_ = "";
  int privilege_ = -1;
};

class UserManager
{
private:
  UserManager();
  ~UserManager();
  BPlusTree<char64, int> login_info_bpt_;
  
public:
  static UserManager &getInstance()
  {
    static UserManager instance;
    return instance;
  }
  UserManager(const UserManager &) = delete;
  UserManager &operator=(const UserManager &) = delete;
  void QueryUserInfo(int timestamp, char64 cur_username, char64 username);
  void AddUserInfo(int timestamp, char64 cur_username, UserInfo user_info);
  void ModifyUserInfo(int timestamp, char64 cur_username, char64 username, UserModifyInfo modify_info);
  void Login(int timestamp, char64 username, char64 password);
  void Logout(int timestamp, char64 username);
  bool IsLogin(char64 username);
  int QueryPrivilege(char64 username);
  void Clean(int timestamp);
};

#endif