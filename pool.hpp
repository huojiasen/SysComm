// Copyright (C) 2018, Since Inc.
// Author: huojiasen (huojiasen@since-inc.com)
//
// Description:

#ifndef pool_h
#define pool_h
#include <list>
#include <cstdio>
#include <exception>
#include <pthread.h>
#include <unistd.h>

#include "locker.h"
#include <iostream>
using namespace std;

template<typename T> class threadpool {
private:
    int m_thread_number;
    int m_max_request;
    pthread_t* m_threads;
    std::list<T*> m_workqueue;
    locker m_queuelocker;
    sem m_queuestat;
    bool m_stop;
public:
    threadpool(int thread_number=8, int max_requests = 10000):
        m_thread_number(thread_number),
        m_max_request(max_requests),
        m_stop(false),
        m_threads(NULL) {
            m_threads = new pthread_t[m_thread_number];

            for (int i=0;i<m_thread_number;++i) {
                if(pthread_create(m_threads + i, NULL, worker, this)!=0) {
                    printf("failed\n");
                    delete [] m_threads;
                }
                if (pthread_detach(m_threads[i])) {
                    printf("failed\n");
                    delete [] m_threads;
                }
            }

        }
    static void* worker(void* arg) {
        threadpool* pool = (threadpool*)arg;
        pool->run();
        return pool;
    }
    void run() {
        while (!m_stop) {
            m_queuestat.wait();
            m_queuelocker.lock();
            if (m_workqueue.empty()) {
                m_queuelocker.unlock();
                continue;
            }

            T* request = m_workqueue.front();
            m_workqueue.pop_front();

            m_queuelocker.unlock();

            if (!request) {
                continue;
            }

            cout<<"start worker"<<endl;
            request->process();
        }
    }

    ~threadpool() {
        cout<<"start destory"<<endl;
        delete []m_threads;
        m_stop = true;
    }

    bool append(T* request) {
        m_queuelocker.lock();
        if (m_workqueue.size() > m_max_request) {
            m_queuelocker.unlock();
            return false;
        }
        cout<<"append one"<<endl;
        m_workqueue.push_back( request );
        m_queuelocker.unlock();
        m_queuestat.post();
        return true;
    }

};
#endif
