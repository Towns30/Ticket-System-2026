#include "../include/UserManager.hpp"

UserManager::UserManager()
    : username_userinfoID_bpt_("username-userinfoID_bpt"),
      login_info_bpt_("login_info_bpt"), user_info_mr_("user_info_mr")
{
  user_info_mr_.Initialise("user_info_mr");
}

UserManager::~UserManager() {}

bool UserManager::GetPrivilegeFromLogin(char64 username, int &privilege)
{
  Utils::Char64Key username_k(username);
  if (login_info_bpt_.Find(username_k, privilege))
  {
    return true;
  }
  return false;
}

bool UserManager::GetUserInfo(char64 username, UserInfo &user_info,
                              int &userinfoID)
{
  Utils::Char64Key username_k(username);
  // username_userinfoID_bpt_.PrintAllValues();
  if (!username_userinfoID_bpt_.Find(username_k, userinfoID))
  {
    return false;
  }
  user_info_mr_.Read(user_info, userinfoID);
  return true;
}
void UserManager::LoginUser(char64 username, int privilege)
{
  Utils::Char64Key username_k(username);
  login_info_bpt_.Insert(username_k, privilege);
}

void UserManager::LogoutUser(char64 username)
{
  Utils::Char64Key username_k(username);
  login_info_bpt_.Delete(username_k);
}

int UserManager::WriteUserInfo(UserInfo user_info)
{
  int user_infoID = user_info_mr_.Write(user_info);
  // std::cerr << "return: user_infoID:" << user_infoID << '\n';
  return user_infoID;
}

void UserManager::UpdateUserInfo(UserInfo user_info, int userinfoID)
{
  user_info_mr_.Update(user_info, userinfoID);
}

void UserManager::AddFunction(char64 username, int userinfoID)
{
  Utils::Char64Key username_k(username);
  username_userinfoID_bpt_.Insert(username_k, userinfoID);
}

void UserManager::QueryUserInfo(char64 cur_username, char64 username)
{
  int cur_privilege = -1;
  if (!GetPrivilegeFromLogin(cur_username, cur_privilege))
  {
    std::cout << "-1" << '\n';
    return;
  }
  UserInfo user_info;
  int _userinfoID;
  if (!GetUserInfo(username, user_info, _userinfoID))
  {
    std::cout << "-1" << '\n';
    return;
  }
  if (std::strcmp(cur_username, username) &&
      cur_privilege <= user_info.privilege_)
  {
    std::cout << "-1" << '\n';
    return;
  }
  std::cout << user_info.username_ << ' ' << user_info.name_ << ' '
            << user_info.mail_addr_ << ' ' << user_info.privilege_ << '\n';
}

void UserManager::AddUserInfo(char64 cur_username, UserInfo user_info)
{
  if (user_info_mr_.Empty()) // 添加第一个用户，忽略cur参数
  {
    user_info.privilege_ = 10;
    int user_infoID = WriteUserInfo(user_info);
    AddFunction(user_info.username_, user_infoID);
    std::cout << "0" << '\n';
    return;
  }
  int cur_privilege;
  if (!GetPrivilegeFromLogin(cur_username, cur_privilege)) // cur未登录
  {
    std::cout << "-1" << '\n';
    return;
  }
  UserInfo _user_info;
  int _userinfoID;
  if (GetUserInfo(user_info.username_, _user_info,
                  _userinfoID)) // username已存在
  {
    std::cout << "-1" << '\n';
    return;
  }
  if (user_info.privilege_ >= cur_privilege) // 新注册用户权限不低于操作用户权限
  {
    std::cout << "-1" << '\n';
    return;
  }
  int user_infoID = WriteUserInfo(user_info);
  AddFunction(user_info.username_, user_infoID);
  std::cout << "0" << '\n';
  ;
}

void UserManager::ModifyUserInfo(char64 cur_username, char64 username,
                                 UserModifyInfo modify_info)
{
  int cur_privilege;
  if (!GetPrivilegeFromLogin(cur_username, cur_privilege)) // cur未登录
  {
    // if (!std::strcmp(cur_username, "EeQkrciqcUmFzgx"))
    // {
    //   std::cerr << "cur未登录\n";
    // }
    std::cout << "-1" << '\n';
    return;
  }
  UserInfo user_info;
  int userinfoID;
  if (!GetUserInfo(username, user_info, userinfoID)) // username不存在，没法修改
  {
    // if (!std::strcmp(cur_username, "EeQkrciqcUmFzgx"))
    // {
    //   std::cerr << "username不存在，没法修改\n";
    // }
    std::cout << "-1" << '\n';
    return;
  }
  if (user_info.privilege_ >= cur_privilege &&
      std::strcmp(cur_username,username)) // -u权限>=-c权限且不是“自己改自己”
  {
    // if (!std::strcmp(cur_username, "EeQkrciqcUmFzgx"))
    // {
    //   std::cerr << "-u权限>=-c权限且不是“自己改自己”\n";
    // }
    std::cout << "-1" << '\n';
    return;
  }
  if (modify_info.privilege_ != -1 &&
      modify_info.privilege_ >= cur_privilege) // -g权限>=-c权限
  {
    // if (!std::strcmp(cur_username, "EeQkrciqcUmFzgx"))
    // {
    //   std::cerr << "-g权限>=-c权限\n";
    // }
    std::cout << "-1" << '\n';
    return;
  }
  if (std::strcmp(modify_info.password_, "") != 0)
  {
    std::strcpy(user_info.password_, modify_info.password_);
  }
  if (std::strcmp(modify_info.name_, "") != 0)
  {
    std::strcpy(user_info.name_, modify_info.name_);
  }
  if (std::strcmp(modify_info.mail_addr_, "") != 0)
  {
    std::strcpy(user_info.mail_addr_, modify_info.mail_addr_);
  }
  if (modify_info.privilege_ != -1)
  {
    user_info.privilege_ = modify_info.privilege_;
  }
  UpdateUserInfo(user_info, userinfoID);
  std::cout << user_info.username_ << ' ' << user_info.name_ << ' '
            << user_info.mail_addr_ << ' ' << user_info.privilege_ << '\n';
}

void UserManager::Login(char64 username, char64 password)
{
  UserInfo user_info;
  int _userinfoID;
  if (!GetUserInfo(username, user_info, _userinfoID)) // username不存在
  {
    std::cout << "-1" << '\n';
    return;
  }
  int _privilege;
  if (GetPrivilegeFromLogin(username, _privilege)) // 用户已经登陆
  {
    std::cout << "-1" << '\n';
    return;
  }
  if (std::strcmp(user_info.password_, password) != 0) // 密码不对
  {
    std::cout << "-1" << '\n';
    return;
  }
  LoginUser(username, user_info.privilege_);
  std::cout << "0" << '\n';
}

void UserManager::Logout(char64 username)
{
  int _privilege;
  if (!GetPrivilegeFromLogin(username, _privilege)) // username未登录
  {
    std::cout << "-1" << '\n';
    return;
  }
  LogoutUser(username);
  std::cout << "0" << '\n';
}

bool UserManager::IsLogin(char64 username)
{
  int _privilege;
  return GetPrivilegeFromLogin(username, _privilege);
}

int UserManager::QueryPrivilege(char64 username)
{
  UserInfo user_info;
  int _userinfoID;
  if (!GetUserInfo(username, user_info, _userinfoID)) // 用户不存在
  {
    return -1;
  }
  return user_info.privilege_;
}

void UserManager::Clean()
{
  login_info_bpt_.Clean();
  username_userinfoID_bpt_.Clean();
  user_info_mr_.Clean();
}

void UserManager::CleanLogin() { login_info_bpt_.Clean(); }