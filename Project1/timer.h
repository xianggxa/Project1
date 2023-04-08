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
    time_t expire;                     //��ʱ������ʱ��
    int connfd;
    bool validFlag;                    //��Ч��־ 
};

/* ������ʱ�ıȽϺ��� */
bool cmp_heap_timer_node(heap_timer_node* node1,
    heap_timer_node* node2);

/* ʱ����� */
class timer_heap {
public:
    timer_heap(int cap);

    void add_timer(heap_timer_node* timer); 
    void del_timer(heap_timer_node* timer); //�ӳ�ɾ��ָ����ʱ��
    heap_timer_node* get_top();             
    void del_top();                         
    //void tick();                                                    
    bool is_empty();                        

    ~timer_heap();                          
private:
    void resize();                          //��������

private:
    std::vector<heap_timer_node*> heap_array;        
    int capacity;                           
    int cur_size;                       //��ǰԪ�ظ���                          
    int deleteNum;                      //ɾ����Ԫ�ظ���                        
};

#endif // TIMER