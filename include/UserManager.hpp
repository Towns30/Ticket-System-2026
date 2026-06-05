#ifndef USER_MANAGER_HPP
#define USER_MANAGER_HPP
#include "BPlusTree.hpp"
#include "Utils.hpp"

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
  char64 password_;
  char64 name_;
  char64 mail_addr_;
  int privilege_;

  UserModifyInfo() : privilege_(-1)
  {
    password_[0] = '\0';
    name_[0] = '\0';
    mail_addr_[0] = '\0';
  }
};

class UserManager
{
private:
  UserManager();
  ~UserManager();
  BPlusTree<Utils::Char64Key, int> login_info_bpt_; // 登录池中维护username到privilege的映射
  BPlusTree<Utils::Char64Key, int>
      username_userinfoID_bpt_; // 维护username到userinfoID的映射
  MemoryRiver<UserInfo, 3> user_info_mr_;
  bool GetPrivilegeFromLogin(char64 username,
                             int &privilege); // 返回false若没有登录
  void LoginUser(char64 username, int privilege); // 用户登录
  void LogoutUser(char64 username); // 用户退出登录
  bool GetUserInfo(char64 username, UserInfo &user_info,
                   int &userinfoID); // 返回false若不存在
  int WriteUserInfo(
      UserInfo user_info); // 返回写入用户数据文件的位置，即userinfoID
  void UpdateUserInfo(
      UserInfo user_info,
      int userinfoID); // 用user_info更新用户数据文件的userinfoID位置数据
  void AddFunction(char64 username, int userinfoID);

public:
  static UserManager &getInstance()
  {
    static UserManager instance;
    return instance;
  }
  UserManager(const UserManager &) = delete;
  UserManager &operator=(const UserManager &) = delete;
  void QueryUserInfo(char64 cur_username, char64 username);
  void AddUserInfo(char64 cur_username, UserInfo user_info);
  void ModifyUserInfo(char64 cur_username, char64 username,
                      UserModifyInfo modify_info);
  void Login(char64 username, char64 password);
  void Logout(char64 username);
  bool IsLogin(char64 username);
  int QueryPrivilege(char64 username); // 用户不存在则返回-1
  void Clean();
  void CleanLogin();
};

#endif