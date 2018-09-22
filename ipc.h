// Copyright (C) 2018, Since Inc.
// Author: huojiasen (huojiasen@since-inc.com)
//
// Description:

#ifndef IPC_HJS
#define IPC_HJS

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>


#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/shm.h>

#include <semaphore.h>
#include <mqueue.h>
#include <sys/mman.h>

using std::string;
class Sem1 {
private:
    int semid;
public:
    Sem1();
    ~Sem1();

    void create(string name);
    void get(string name);

    void P();
    void V();
};

class Sem2 {
private:
    sem_t *sem;
    string name;
public:
    Sem2();
    ~Sem2();

    void create();
    void get();

    void P();
    void V();
};

class Shm1 {
private:
    int shmid;
public:
    Shm1();
    ~Shm1();
    char* create(string name, int size);

    char* at(string name, int size);
    void dt(char *buf);
};

class Shm2 {
private:
    int shmfd;
    string name;
public:
    Shm2();
    ~Shm2();

    char* create(int size);
    char* at(int size);

    void dt(char *but size);
};

class Msg1 {
private:
    int msgid;
public:
    Msg1();
    ~Msg1();

    void create(string name);
    void get(string name);

    void send(char c);
    char recv();
};

class Msg2 {
private:
    mqd_t msgid;
    string name;
public:
    Msg2();
    ~Msg2();

    void create(int size);
    void get();

    void send(char* buf, int size);
    char* recv(int size);
};

#endif

