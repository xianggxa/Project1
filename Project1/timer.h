#pragma once
#ifndef TIMER
#define TIMER

#define MAX_TIME_LIMIT 300

#include<time.h>
#include<algorithm>
#include<queue>

class heap_timer_node {
public:
    heap_timer_node(int connfd) {
        expire = time(NULL);
        validFlag = true;
        this->connfd = connfd;
    }
    int isTimeout() {
        return time(NULL) - expire >= MAX_TIME_LIMIT;
    }


public:
    time_t expire;                     //定时器连接时间
    int connfd;
    bool validFlag;                    //有效标志 
};

/* 构建堆时的比较函数 */
bool cmp_heap_timer_node(heap_timer_node* node1,
    heap_timer_node* node2);

/* 时间堆类 */
class timer_heap {
public:
    timer_heap(int cap);

    void add_timer(heap_timer_node* timer); 
    void del_timer(heap_timer_node* timer); //延迟删除指定计时器
    heap_timer_node* get_top();             
    void del_top();                         
    //void tick();                                                    
    bool is_empty();                        

    ~timer_heap();                          
private:
    void resize();                          //容量翻倍

private:
    std::vector<heap_timer_node*> heap_array;        
    int capacity;                           
    int cur_size;                       //当前元素个数                          
    int deleteNum;                      //删除的元素个数                        
};

#endif // TIMER