// Copyright (C) 2018, Since Inc.
// Author: huojiasen (huojiasen@since-inc.com)
//
// Description:

#include "pool.hpp"
#include "net.h"

#include <fcntl.h>
#include <sys/epoll.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
using namespace std;
struct A {
    int _fd;
    A(int fd) {
        _fd = fd;
    }
    void process() {
        char buf[100];
        read(_fd, buf, 100);
        printf("%s\n", buf);
    }
};

int setnonblocking(int fd) {
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

void addfd(int epollfd, int fd, bool oneshot) {
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN|EPOLLET;
    if (oneshot) {
        event.events |= EPOLLONESHOT;
    }
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    setnonblocking(fd);
}

int main() {
    signal(SIGPIPE, SIG_IGN);
    threadpool<A> *tp  = new threadpool<A>(10,100);

    int listenfd = TcpListen(1999);

    epoll_event events[1024];
    int epollfd = epoll_create(5);
    addfd(epollfd, listenfd, false);

    while(1) {
        int number = epoll_wait(epollfd, events, 1024, -1);
        for (int i=0;i<number;i++) {
            int sockfd = events[i].data.fd;
            if (sockfd == listenfd) {
                int connfd = TcpAccept(sockfd);
                addfd(epollfd, connfd, true);
            } else if (events[i].events&EPOLLIN) {
                tp->append(new A(sockfd));
            }
        }
    }
    close(epollfd);
    close(listenfd);
    sleep(10);
    delete tp;
    return 0;
}
