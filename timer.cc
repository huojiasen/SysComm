// Copyright (C) 2018, Since Inc.
// Author: huojiasen (huojiasen@since-inc.com)
//
// Description:

#include <iostream>
#include <queue>
#include <string>

#include <sys/time.h>
#include <netinet/in.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#include "base_thread.hpp"

using namespace std;

struct heap_timer {
    heap_timer(int delay,string t) {
        expire = time(NULL) + delay;
        name = t;
    }
    void cb_func(){
        cout<<name<<endl;
    }
    long expire;
    string name;
};
struct cmp {
    bool operator()(heap_timer* a, heap_timer* b) {
        return a->expire > b->expire;
    }
};
class time_heap : public Thread{
public:
    time_heap(){
        pthread_mutex_init(&m_mutex, NULL);
        pthread_cond_init(&m_cond, NULL);
    }

    ~time_heap() {
        pthread_mutex_destroy( &m_mutex );
        pthread_cond_destroy( &m_cond );
    }

    void add_timer(heap_timer* timer) {
        if (array.empty()) {
            array.push(timer);
            pthread_cond_signal( &m_cond );
        } else {
            if(timer->expire < array.top()->expire){
                array.push(timer);
                pthread_cond_signal( &m_cond );
            } else {
                array.push(timer);
            }
        }
    }
    void Run() {
        tick();
    }
    void tick() {
        while(1) {
            if (array.empty()) {
                pthread_cond_wait(&m_cond, &m_mutex);
            }

            heap_timer* tmp = array.top();
            if (tmp->expire>time(NULL)) {
                timespec ts;
                ts.tv_sec = tmp->expire;
                ts.tv_nsec = 0;
                pthread_cond_timedwait(&m_cond, &m_mutex, &ts);
            }
            if (time(NULL)>=tmp->expire){
                tmp->cb_func();
                array.pop();
            }
        }
    }

private:
    std::priority_queue<heap_timer*, vector<heap_timer*>, cmp> array;
    pthread_mutex_t m_mutex;
    pthread_cond_t m_cond;

};

int main() {
    time_heap th;
    th.Start();
    th.add_timer(new heap_timer(2,"2s"));
    th.add_timer(new heap_timer(6,"6s"));
    th.add_timer(new heap_timer(8,"8s"));
    th.add_timer(new heap_timer(4,"4s"));
    th.add_timer(new heap_timer(20,"10s"));
    th.Join();
    return 0;
}
