// Copyright (C) 2018, Since Inc.
// Author: huojiasen (huojiasen@since-inc.com)
//
// Description:
#ifndef BASE_THREAD_H_
#define BASE_THREAD_H_

#include <pthread.h>
#include <iostream>
using namespace std;
class Thread {
public:
    Thread(){}
    virtual ~Thread() {
        Join();
    }

    pthread_t tid() const {
        return tid_;
    }

    void Start() {
        int result = pthread_create(&tid_, NULL, &ThreadRunner, this);
    }

    void Join(){
        int result = pthread_join(tid_, NULL);
    }

protected:
    virtual void Run() = 0;
    static void* ThreadRunner(void* arg) {
        cout<<"create thread"<<endl;
        Thread *t = reinterpret_cast<Thread*>(arg);
        t->Run();
        return NULL;
    }
    pthread_t tid_;
};

#endif
