#include "../include/OrderManager.hpp"


OrderManager::OrderManager() {}
OrderManager::~OrderManager() {}
void OrderManager::BuyTicket(char64 username, char64 trainID, Date date,
                             int num, char64 from, char64 to,
                             bool allow_pending)
{
}
void OrderManager::RefundTicket(char64 username, int order_pos) {}
void OrderManager::QueryOrder(char64 username) {}
void OrderManager::Clean() {}