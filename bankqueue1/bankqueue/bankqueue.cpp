#include<stdio.h>
#include<windows.h>
#include<stdlib.h>
#include<conio.h>
#include<time.h>
#include <string>
using namespace std;

#define OK 1
#define ERROR 0
#define TRUE 1
#define FALSE 0

typedef struct Time {
    int Hour;
    int Minute;
}Time;

typedef struct Event {   //事件类型
    int OccurTime;  //事件发生时刻
    int NType;      //事件类型，0表示到达事件，1至4表示四个窗口的离开事件
    int DealAmount;   //办理业务类型,正数为存款，负数为取款
    struct Event* next;
}Event;

typedef struct { //单向链表结构
    Event* head;//头指针
    Event* tail;//尾指针
    int len;    //长度
}LinkList;
typedef LinkList EventList; //事件链表

typedef struct CustNode {
    int BeginTime;                      //到达时间   
    int Duration;                       //办理业务时间
    int Amount;                         //正数为存款，负数为取款
    struct CustNode* next;              //指针域  
}CustNode;

typedef struct LinkQueue {//队列结构
    CustNode* front;//头指针
    CustNode* rear;//尾指针
}LinkQueue;

int Open = 540;                         //银行开门时间
int BankAmount = 10000000;              //初始时银行现存资金总额  
int CloseTime = 400;                    //营业结束时间-开门时间  
int ClientArriveMaxTime;                //两个到达事件之间的间隔上限  
int ClientArriveMinTime;                //两个到达事件之间的间隔下限  
int DealMaxTime;                        //客户之间交易的时间上限  
int DealMinTime;                        //客户之间交易的时间下限  
int MaxAmount;                          //交易额上限  
int NeedIn = 0;                         //需要存款的人数  
int NeedOut = 0;                        //需要取款的人数  
int SuccessIn = 0;                      //成功存款的人数  
int SuccessOut = 0;                     //成功取款的人数  
int CurrentTime = 0;                    //当前时间  
int TotalTime = 0;                      //客户逗留总时间  
int CustomerNum = 0;                    //客户总数
int DealTime = 0;                       //交易时间  
int MaxTime = 0;                        //最大到达时间
int InAmount = 0;                       //存款总额
int OutAmount = 0;                      //贷款总额
EventList ev;                           //事件链表
Event en;                               //事件
LinkQueue q[5];                         //队列数组存放队列
CustNode customer;                      //客户节点


void OpenForDay();
Time TransTime(int minute);
Event NewEvent(int occurtime, int nType, int money);
int InitList(LinkList* L);
int ListEmpty(LinkList* L);
int OrderInsert(LinkList* L, Event e);
int DelFirst(LinkList* L, Event* e);
int ListTraverse(LinkList* L);
void PrintEventList();
int InitQueue(LinkQueue* Q);
int EmptyQueue(LinkQueue* Q);
int EnQueue(LinkQueue* Q, CustNode e);
int DelQueue(LinkQueue* Q, CustNode* e);
int QueueLength(LinkQueue Q);
int Min(int a[], int n);
int ShortestQueue();
int GetHead(LinkQueue* Q, CustNode* e);
int QueueTraverse(LinkQueue* Q);
void PrintQueue();
void CustomerArrived();
void CustomerDepature();
void Bank_Simulation();


int main()
{
    Bank_Simulation();
    return 0;
}


//初始化操作
void OpenForDay() {
    InitList(&ev);//初始化事件队列
    en.OccurTime = 0;
    en.NType = 0;
    en.DealAmount = rand() % 30000 - 15000;
    OrderInsert(&ev, en);
    for (int i = 1; i <= 4; i++)
        InitQueue(&q[i]);//初始化四个窗口队列
}

Time TransTime(int minute) {
    int timeh, timem;
    timeh = (Open + minute) / 60;
    timem = (Open + minute) % 60;
    Time time;
    time.Hour = timeh;
    time.Minute = timem;
    return time;
}

//根据OccurTime和NType值，创建新事件
Event NewEvent(int occurT, int nType, int money) {
    Event e;
    e.OccurTime = occurT;
    e.NType = nType;
    e.DealAmount = money;
    return e;
}

//初始化事件链表
int InitList(LinkList* L) {
    L->head = L->tail = (Event*)malloc(sizeof(Event));
    if (!L->head) {
        printf("申请空间失败！\n");
        exit(0);
    }
    L->head->next = NULL;
    return OK;
}

//判断链表L是否为空
int ListEmpty(LinkList* L) {
    if ((L->head == L->tail) && (L->head != NULL))
        return TRUE;
    else
        return FALSE;
}

//将事件e按发生时间顺序插入有序链表L中
int OrderInsert(LinkList* L, Event e) {
    Event* p, * q, * newptr;
    newptr = (Event*)malloc(sizeof(Event));
    if (!newptr) {
        printf("申请空间失败！\n");
        exit(0);
    }
    *newptr = e;
    if (TRUE == ListEmpty(L)) {//链表为空
        L->head->next = newptr;
        L->tail = newptr;
        L->tail->next = NULL;
        return OK;
    }
    q = L->head;
    p = L->head->next;
    while (p) {//遍历整个链表
        if (p->OccurTime >= newptr->OccurTime)
            break;
        q = p;
        p = p->next;
    }
    q->next = newptr;
    newptr->next = p;
    if (!p)//插入位置为链表尾部
        L->tail = newptr;
    return OK;
}

//链表L不为空，删除其首结点，用e返回
int DelFirst(LinkList* L, Event* e) {
    Event* p = L->head->next;
    if (!p)
        return ERROR;
    L->head->next = p->next;
    *e = *p;
    free(p);
    if (L->head->next == NULL)
        L->tail = L->head;
    return OK;
}

//遍历链表
int ListTraverse(LinkList* L) {
    Event* p = L->head->next;
    string type, deal;
    Time T;
    if (!p) {
        printf("链表为空！\n");
        return ERROR;
    }
    while (p != NULL) {
        if (p->NType == 0) type = "到达";
        else  type = "离开" + to_string(p->NType) + "号窗口";
        if (p->DealAmount >= 0)  deal = "存款" + to_string(p->DealAmount) + "元";
        else  deal = "贷款" + to_string(-1 * p->DealAmount) + "元";
        T = TransTime(p->OccurTime);
        printf("事件发生时间：%d点%d分，事件类型：%s，交易类型：%s\n", T.Hour, T.Minute, type.c_str(), deal.c_str());
        p = p->next;
    }
    printf("\n");
    return OK;
}

//输出事件链表
void PrintEventList() {

    printf("事件链表:\n");
    ListTraverse(&ev);
}

//初始化队列Q
int InitQueue(LinkQueue* Q) {
    Q->front = Q->rear = (CustNode*)malloc(sizeof(CustNode));
    if (!Q->front) {
        printf("申请空间失败！\n");
        exit(0);
    }
    Q->front->next = NULL;
    return OK;
}

//若队列Q为空，返回TRUE，否则返回FALSE
int EmptyQueue(LinkQueue* Q) {
    if (Q->front == Q->rear && Q->front != NULL)
        return TRUE;
    else
        return FALSE;
}

//结点e入队Q
int EnQueue(LinkQueue* Q, CustNode e) {
    CustNode* p = (CustNode*)malloc(sizeof(CustNode));
    if (!p) {
        printf("申请空间失败！\n");
        exit(0);
    }
    *p = e;
    p->next = NULL;
    Q->rear->next = p;//插入队尾
    Q->rear = p;//修改队尾指针
    return OK;
}

//若队列Q不为空，首结点出队，用e返回
int DelQueue(LinkQueue* Q, CustNode* e) {
    CustNode* p = Q->front->next;
    if (!p)
        return ERROR;
    *e = *p;
    Q->front->next = p->next;//修正队首指针
    free(p);
    if (!Q->front->next)//队空
        Q->rear = Q->front;
    return OK;
}


//返回队列Q的长度，即元素个数
int QueueLength(LinkQueue Q) {
    int count = 0;
    CustNode* p = Q.front->next;
    while (p) {
        p = p->next;
        count++;
    }
    return count;
}

//返回长度为n的数组a第一个最小值的下标，从0开始
int Min(int a[], int n) {
    int i, tmp, ind = 0;
    tmp = a[0];
    for (i = 1; i < n; i++) {
        if (a[i] < tmp) {
            tmp = a[i];
            ind = i;
        }
    }
    return ind;
}

//获取最短队列的编号
int ShortestQueue() {
    int i, a[4];
    for (i = 1; i <= 4; i++) {
        a[i - 1] = QueueLength(q[i]);
        //printf("队%d的长度为%d\n",i,QueueLength(q[i]));
    }
    return Min(a, 4) + 1;//队列从1开始编号
}

//若队列Q不为空，用e返回其首结点
int GetHead(LinkQueue* Q, CustNode* e) {
    if (EmptyQueue(Q))
        return ERROR;
    *e = *(Q->front->next);
    return OK;
}

//遍历队列Q
int QueueTraverse(LinkQueue* Q) {
    CustNode* p = Q->front->next;
    string deal;
    Time T;
    if (!p) {
        printf("队列为空！\n");
        return ERROR;
    }
    while (p) {
        if (p->Amount >= 0)  deal = "存款" + to_string(p->Amount) + "元";
        else  deal = "贷款" + to_string(-1 * p->Amount) + "元";
        T = TransTime(p->BeginTime);
        printf("\n\t\t(到达时间：%d点%d分，业务办理时间：%d，办理业务：%s) ", T.Hour, T.Minute, p->Duration, deal.c_str());
        p = p->next;
    }
    printf("\n");
    return OK;
}

//打印当前队列
void PrintQueue() {
    int i;
    for (i = 1; i <= 4; i++) {
        printf("队列 %d 有 %d 个客户:", i, QueueLength(q[i]));
        QueueTraverse(&q[i]);
    }
    printf("\n");
}

//顾客达到事件
void CustomerArrived() {
    int durtime, intertime, money, i, t;
    CustNode e;
    Time T1, T2;
    CustomerNum++;
    intertime = rand() % 5 + 1;//间隔时间
    durtime = rand() % 10 + 10;//办理业务时间
    money = rand() % 30000 - 15000;//客户要办理的金额
    t = en.OccurTime + intertime;
    if (en.OccurTime < CloseTime) {//银行尚未关门
        T1 = TransTime(en.OccurTime);
        T2 = TransTime(t);
        printf("客户到达时间:%d点%d分,业务办理时间:%d分钟,下一位顾客达到时间:%d点%d分\n", T1.Hour, T1.Minute, durtime, T2.Hour, T2.Minute);//下一位顾客达到时间
        OrderInsert(&ev, NewEvent(t, 0, money));
        i = ShortestQueue();//最短队列
        e.BeginTime = en.OccurTime;
        e.Duration = durtime;
        e.Amount = en.DealAmount;
        EnQueue(&q[i], e);
        if (QueueLength(q[i]) == 1)
            OrderInsert(&ev, NewEvent(en.OccurTime + durtime, i, en.DealAmount));
    }
    else {
        printf("银行关门了！");
    }
}

//顾客离开事件
void CustomerDepature() {
    int i = en.NType;
    Time T;
    T = TransTime(en.OccurTime);
    DelQueue(&q[i], &customer);
    printf("客户离开时间:%d点%d分\n", T.Hour, T.Minute);//输出顾客离开时间
    TotalTime += en.OccurTime - customer.BeginTime;
    BankAmount += en.DealAmount;
    if (en.DealAmount >= 0) InAmount += en.DealAmount;
    else OutAmount += -1 * en.DealAmount;
    if (!EmptyQueue(&q[i])) {
        GetHead(&q[i], &customer);
        OrderInsert(&ev, NewEvent(en.OccurTime + customer.Duration, i, customer.Amount));
    }
}



//银行排队模拟
void Bank_Simulation() {
    srand((unsigned)time(NULL));//随机数发生器的初始化函数
    OpenForDay();
    while (!ListEmpty(&ev)) {
        DelFirst(&ev, &en);
        printf("----------------开始------------------\n");
        if (en.NType == 0)
            CustomerArrived();
        else
            CustomerDepature();
        PrintQueue();
        PrintEventList();
    }
    printf("\n总逗留时间: %d min\n客户总人数：%d 人\n平均逗留时间: %g min\n银行资金总额：%d元\n存款总额:%d 元\n贷款总额:%d 元\n", TotalTime, CustomerNum, (float)TotalTime / CustomerNum, BankAmount, InAmount, OutAmount);
}






