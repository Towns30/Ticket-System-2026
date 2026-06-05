#include "../include/TrainManager.hpp"


TrainManager::TrainManager() {}
TrainManager::~TrainManager() {}
void TrainManager::AddTrain(TrainInfo train_info) {}
void TrainManager::QueryTrainInfo(char64 trainID, Date date) {}
void TrainManager::QueryTicket(char64 from, char64 to, Date date,
                               SortBy sort_by)
{
}
void TrainManager::QueryTransfer(char64 from, char64 to, Date date,
                                 SortBy sort_by)
{
}
void TrainManager::ModifyTrainInfo(char64 trainID, Date date, int from_idx,
                                   int to_idx, int seat_num, int dir)
{
}
void TrainManager::ReleaseTrain(char64 trainID) {}
void TrainManager::DeleteTrain(char64 trainID) {}
void TrainManager::Clean() {}