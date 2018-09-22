// Copyright (C) 2018, Since Inc.
// Author: huojiasen (huojiasen@since-inc.com)
//
// Description:

#include "ipc.h"

Sem1::Sem1(){
    semid = 0;
}
Sem1::~Sem1(){
    if (semid>0){
        semctl(semid, 1, IPC_RMID);
    }
}
void Sem1::create(string name) {
    key_t key = ftok(name.c_str(), 'R');
    semid = semget(key, 1, IPC_CREAT|0666);
}
void Sem1::get(string name) {
    key_t key = ftok(name.c_str(), 'R');
    semid = semget(key, 1, 0666);
}

void Sem1::P() {
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_op = -1;
    sb.sem_flg = 0;
    semop(semid, &sb, 1);
}

void Sem1::V() {
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_op = 1;
    sb.sem_flg = 0;
    semop(semid, &sb, 1);
}


Sem2::Sem2(){
    sem = NULL;
    name = "/semname";
}
Sem2::~Sem2(){
    if (sem!=NULL) {
        sem_close(sem);
        sem_unlink(name.c_str());
    }
}
void Sem2::create() {
    sem = sem_open(name.c_str(), O_RDWR | O_CREAT,
                   S_IRWXU | S_IRWXG | S_IRWXO, 0);
}
void Sem2::get() {
    sem = sem_open(name.c_str(), O_RDWR);
}

void Sem2::P() {
    sem_wait(sem);
}

void Sem2::V() {
    sem_post(sem);
}

Shm1::Shm1() {
    shmid = 0;
}
Shm1::~Shm1() {
    if (shmid>0){
        shmctl(shmid, IPC_RMID,NULL);
    }
}
char* Shm1::create(string name, int size) {
    key_t key = ftok(name.c_str(), 'R');
    shmid = shmget(key, size, IPC_CREAT|0666);
    char *buffer = (char*)shmat(shmid, NULL, 0);
    return buffer;
}

char* Shm1::at(string name, int size) {
    key_t key = ftok(name.c_str(), 'R');
    shmid = shmget(key, size, 0666);
    char *buffer = (char*)shmat(shmid, NULL,0);
}
void Shm1::dt(char *buf) {
    if (buf!=NULL)
        shmdt(buf);
}


Shm2::Shm2() {
    shmfd = 0;
    name = "/shmname";
}
Shm2::~Shm2() {
    if (shmfd>0){
        close(shmfd);
        shm_unlink(name.c_str());
    }
}
char* Shm2::create(int size) {

    shmfd = shm_open(name.c_str(), O_RDWR | O_CREAT,
                     S_IRWXU | S_IRWXG | S_IRWXO);
    ftruncate(shmfd, size);
    char* buffer =(char*) mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, shmfd, 0);
    return buffer;
}

char* Shm2::at(int size) {
    shmfd = shm_open(name.c_str(), O_RDWR,
                     S_IRWXU | S_IRWXG | S_IRWXO);

    char* buffer =(char*) mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, shmfd, 0);
    return buffer;
}
void Shm2::dt(char *but size) {
    if (buf!=NULL)
        munmap(buf,size);
}



Msg1::Msg1() {
    msgid = 0;
}
Msg1::~Msg1() {
    if (msgid > 0)
        msgctl(msgid, IPC_RMID, NULL);
}

void Msg1::create(string name) {
    key_t key = ftok(name.c_str(),'R');
    msgid = msgget(key, IPC_CREAT|0666);
}

void Msg1::get(string name) {
    key_t key = ftok(name.c_str(), 'R');
    msgid = msgget(key, 0666);
}

void Msg1::send(char c) {
    struct msgbuf buf;
    buf.mtype = 10;
    buf.mtext[0] = c;
    msgsnd(msgid, &buf, sizeof(buf), 0);
}

char Msg1::recv() {
    struct msgbuf buf;
    msgrcv(msgid, &buf, 4,10,0);
    return buf.mtext[0];
}


Msg2::Msg2() {
    msgid = 0;
    name = "/msgname";
}
Msg2::~Msg2() {
    if (msgid > 0) {
        mq_close(msgid);
        mq_unlink(name.c_str());
    }
}

void Msg2::create(int size) {
    struct mq_attr attr;
    attr.mq_flags = O_RDWR;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = size;
    attr.mq_curmsgs = 0;
    msgid = mq_open(name.c_str(),
                    O_RDWR | O_CREAT,
                    S_IRWXU | S_IRWXG | S_IRWXO,
                    &attr);
}

void Msg2::get() {
    msgid = mq_open(name.c_str(), O_RDONLY);
}

void Msg2::send(char* buf, int size) {
    mq_send(msgid, buf, size, 0);
}

char* Msg2::recv(int size) {
    char* buf = new char[size];
    mq_receive(msgid, buf, size,NULL);
    return buf;
}
