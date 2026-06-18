# 火车票管理系统

SJTU CS1951 课程大作业

## 概况

### 作业安排

本作业分为两个部分。

在第一部分中，需要实现一个基于文件的 B+ 树。

在第二部分中，需要实现一个火车票管理系统。此部分要求使用 Git 开发，维持良好的项目管理习惯。此部分的中期检查等检查方式均会通过查看登记的 Git 仓库链接，因此如果想更换仓库的链接请及时联系助教。

### 作业周期

- B+ 树: 2026-04-27（第 9 周周一）~ 2025-05-25（第 13 周周一）

## 评分标准

本作业占本课程总成绩 15%，其中 B+ 树占 7%，管理系统占 8%。

- B+ 树: 7%
  - OJ 测试（仅题 [3091](https://acm.sjtu.edu.cn/OnlineJudge/problem/3091)，不含压力测试）: 80%
  - Code Review: 20%

bonus 另外计算，计入平时分总分，且不超过总分的 1%。

## 系统架构

### 存储设计

#### 外存

##### 数据文件

- **trains.txt**：存储所有火车的信息，包括 (trainInfoID(MemoryRiver 维护的)) trainID, stationNum, stations, seatNum, prices, startTime, travelTimes, stopoverTime, saleDate, type(这些是固定不变的)，resSeatNums(发车日期起每一天的每相邻两站剩余座位数)，state(是否已经被 release)

- **users.txt**：存储所有用户的信息，包括 (userInfoID(MemoryRiver 维护的)) username, password, name, mailAddr, privilege

- **orders.txt**：存储所有订单信息，每个订单信息包括 (orderInfoID(MemoryRiver 维护的)) username(归属用户), status(success / pending / refunded), trainID, d_start(对应列车始发站日期), from, leaving_time, to, arriving_time, price, num

##### 索引文件（基于 B+ 树）

使用 BPT 维护以下映射，以加速数据检索：

- **username → userInfoID**：通过 username 快速找到 userInfoID，再取出对应 userInfo
- **(username, orderInfoID)**：通过 username 和 orderInfoID 二元组定位订单，再取出对应 orderInfo
- **trainID → trainInfoID**：通过 trainID 快速找到 trainInfoID，再取出对应 trainInfo
- **(station, trainInfoID)**：维护 station 与 trainInfoID 的二元组，用于 query_ticket 和 query_transfer（仅对已 release 的 train）
- **((trainID, date), orderInfoID)**：维护候补队列，按 (trainID, 始发站出发日期) 和候补 orderInfoID 二元组存储（仅对已 release 的 train）
- **((station1, station2), trainInfoID)**：维护经过 station1 和 station2（station1 在 station2 之前）的列车，用于 query_ticket 和 query_transfer（仅对已 release 的 train）

#### 内存

##### 登录池

- 存所有在线（已登录）的用户信息 (username, privilege)，用 BPT 维护从 username 到 privilege 的映射

### 模块设计

- **UserManager 模块**
  - 实现为全局单例 UserManager
  - 功能：维护 users.txt、username-userInfoID 索引，并负责维护和存储登录池，支持对用户信息的查询、添加、修改
  - 暴露接口：`QueryUserInfo()`, `AddUserInfo()`, `ModifyUserInfo()`, `Login()`, `Logout()`, `IsLogin()`, `QueryPrivilege()`

- **TrainManager 模块**
  - 实现为全局单例 TrainManager
  - 功能：维护 trains.txt、trainID-trainInfoID 索引、station-trainInfoID 索引、(station1, station2)-trainInfoID 索引，支持对火车的查询、添加、修改、删除
  - 暴露接口：`AddTrain()`, `QueryTrainInfo()`, `QueryTicket()`, `QueryTransfer()`, `ModifyTrainInfo()`, `ReleaseTrain()`, `DeleteTrain()`

- **OrderManager 模块**
  - 实现为全局单例 OrderManager
  - 功能：维护 orders.txt、username-orderInfoID 索引、(trainID, date)-orderInfoID 索引，支持对订单的创建、订单状态的修改，以及候补队列的维护
  - 暴露接口：`BuyTicket()`, `RefundTicket()`, `QueryOrder()`

- **Lexer 模块**
  - 功能：将输入指令解析为 Token 序列（支持 `|` 分隔的多值参数），传给 Parser
  - 暴露接口：`Tokenize()`

- **Parser 模块**
  - 功能：解析 Lexer 传入的 Token 序列，根据指令类型调用对应 Manager 接口，输出结果
  - 暴露接口：`ParseLine()`

- **Time 模块**
  - **Date 类**：日期加减、日期差运算
  - **HourMinute 类**：时刻的加减和换算
  - **AccurateTime 类**：将 Date 和 HourMinute 结合，表示绝对时间点

### B+ 树 & 工具

- **BPlusTree**：基于文件系统的 B+ 树模板，支持 Insert、Delete、Find、IntervalFind 操作
- **MemoryRiver**：基于 fstream 的定长记录文件读写器，为 BPT 提供底层存储支持
- **vector**：自行实现的动态数组，替代 std::vector
- **priority_queue**：左偏堆实现的优先队列，用于 query_ticket 和 query_transfer 的排序
