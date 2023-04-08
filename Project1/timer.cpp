#include "timer.h"

timer_heap::timer_heap(int cap)
{
    this->capacity = cap;
    this->heap_array.resize(cap);
    this->cur_size = 0;
    this->deleteNum = 0;
    for (int i = 0; i < cap; i++) {
        this->heap_array[i] = nullptr;
    }
    make_heap(heap_array.begin(), heap_array.begin() + cur_size, cmp_heap_timer_node);
   
}

void timer_heap::add_timer(heap_timer_node* timer)
{
    if (timer == nullptr) {
        return;
    }
    if (cur_size >= capacity) {     /* 无空间，则将array扩大一倍 */
        resize();
    }

    heap_array[cur_size] = timer;
    cur_size++;

    push_heap(heap_array.begin(), heap_array.begin() + cur_size, cmp_heap_timer_node);//调整堆

}

void timer_heap::del_timer(heap_timer_node* timer)
{
    if (timer == nullptr) {
        return;
    }
    timer->validFlag = 0;
    deleteNum++;

    if (deleteNum >= cur_size / 2) {//当累计的删除数达到总实际容量的一般时进行删除重构
        int n_index = 0;
        for (int i = 0; i < cur_size; i++) {
            if (heap_array[i]->validFlag&&i!=n_index) {
                heap_array[n_index++] = heap_array[i];
                heap_array[i] = nullptr;

            }
            else {
                delete heap_array[i];
                heap_array[i] = nullptr;

            }
        }
        cur_size = n_index;
        make_heap(heap_array.begin(), heap_array.begin() + cur_size, cmp_heap_timer_node);

    }

}

heap_timer_node* timer_heap::get_top()
{
    if (is_empty()) {
        return nullptr;
    }
    else return heap_array.front();
    
}

void timer_heap::del_top()
{
    pop_heap(heap_array.begin(), heap_array.begin() + cur_size, cmp_heap_timer_node);
    delete heap_array[cur_size - 1];
    heap_array[cur_size - 1] = nullptr;
    cur_size--;
}

bool timer_heap::is_empty()
{
    return cur_size==0;
}

timer_heap::~timer_heap()
{
    for (int i = 0; i < cur_size; i++) {
        delete heap_array[i];
        heap_array[i] = nullptr;
    }
}

void timer_heap::resize()
{
    heap_array.resize(capacity * 2);
    capacity *= 2;
}

bool cmp_heap_timer_node(heap_timer_node* node1, heap_timer_node* node2)
{
    
        return  node1->expire > node2->expire;

}
