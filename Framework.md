# 总体构思

## 存储

### 外存

#### 数据文件 

- trains.txt

- - 存储所有火车的信息，包括 (trainInfoID(MermoryRiver维护的)) trainID, stationNum, stations, seatNum, prices, startTime, travelTimes, stopoverTime, saleDate, type(这些是固定不变的)，resSeatNums(发车日期起每一天的每相邻两站剩余座位数，即resSeatNums[100][100], 即resSeatNums[i][i][j]指的是发售日期 + i天, 第j站到第j + 1站的剩余座位数目), state(是否已经被release)

- users.txt

- -  存储所有用户的信息，包括 (userInfoID(MermoryRiver维护的)) username, password, name, mailAddr, privilege

- orders.txt

- - 存储所有订单信息，每个订单信息包括 (orderInfoID(MermoryRiver维护的)) timestamp(订单创建的时间戳), username(归属用户), status(success or 
pending or refunded), trainID, d_start(对应列车始发站日期), from, leaving_time, to, arriving_time, price, num

#### 索引文件（帮助更好更快地找到对应数据）

- 说明

- - 当我们讲A到B的映射时，指A是互不相同的，而当我们讲维护二元组(A, B)时，往往是A可重复，但(A, B)整体不重复，本质上我们是在维护(A, B)到0的映射，不过第一种可以看作第二种的一个特例，所以如果B占的空间不大（往往B是一个数据文件的索引），我们可以只维护第二类BPT

- username-userInfo.txt

- - 用BPT维护从username到 userInfoID的映射, 再通过userInfoID取出对应userInfo

- username-orderInfoID.txt

- - 通过用BPT维护(username, orderInfoID)二元组，再通过orderID取出对应orderInfo

- trainID-trainInfoID.txt

- - 通过用BPT维护从trainID到trainInfoID的映射，再通过trainInfoID取出对应trainInfo

- station-trainInfoID.txt（为了解决query_ticket和query_transfer）(对于已经release的train)

- - 通过用BPT维护(station, trainInfoID)二元组，再通过trainInfoID取出对应trainInfo

- (trainID, date(始发站出发日期))-orderInfoID.txt(为了解决候补队列的问题)(对于已经release的train)

- - 通过用BPT维护((trainID, date), orderInfoID(候补的))二元组，再通过orderInfoID取出对应orderInfo

- (station1, station2)-trainInfoID(帮助解决query_ticket和query_transfer)(对于已经release的train)

- - 通过用BPT维护((station1, station2), trainInfoID)二元组，使得通过 station1 和 station2 可快速找出所有经过这两站（且 station1 在 station2 之前）的列车，再通过trainInfoID取出对应trainInfo



### 内存

### 登录池

- 存所有在线（已登录）的用户信息(username, privilege) (用BPT维护从username到privilege的映射)

## 模块

- UserManager模块

- - 说明

- - - 实现为全局单例UserManager

- - 功能：维护users.txt, username-userInfo.txt，并负责维护和存储登录池，支持对用户信息的查询，添加，修改，

- - 暴露接口：QueryUserInfo(), AddUserInfo(), ModifyUserInfo(), Login(), Logout(), IsLogin(), QueryPrivilege()

- - 接口调用模块：Parser

- TrainManager模块
  
- - 说明

- - - 实现为全局单例TrainManager

- - 功能：维护trains.txt, trainID-trainInfoID.txt, station-trainInfoID.txt，支持对火车的查询，添加，修改，删除（删除只要删掉有trainInfoID的映射，即删除所有三个映射中的trainInfoID映射，那么就相当于在train.txt里删去了此列车的信息，因为搜不到了）

- - 暴露接口：AddTrain(), QueryTrainInfo(), QueryTicket(), QueryTransfer(), ModifyTrainInfo()(由于买票和退票会导致火车剩余座位发生变化，需要修改火车信息), ReleaseTrain(), DeleteTrain()、

- - 接口调用模块：Parser

- OrderManager模块

- - 说明

- - - 实现为全局单例OrderManager

- - 功能：维护orders.txt, username-orderInfoID.txt, (trainID, date)-orderInfoID.txt, 支持对订单的创建，订单状态的修改，以及候补队列的维护(私有)

- - 暴露接口：BuyTicket(), RefundTicket(), QueryOrder()

- Lexer模块

- - 功能：将输入指令解析为sjtu::vector(tokens)，传给Parser

- - 暴露接口：TranslateIntoTokens()

- Parser模块

- - 功能：解析Lexer传入的sjtu::vector(tokens)，解析为指令，并执行

- - 暴露接口：Parse()

- Time模块

- - Date类

- - - 功能：维护Date类，做日期加减整数，日期-日期的运算

- - HourMinite类

- - - 功能：维护时间类，做小时和分钟得加减法和换算

- - AccurateTime类

- - - 功能：将Date模块和HourMinite模块结合起来