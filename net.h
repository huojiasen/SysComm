// Copyright (C) 2018, Since Inc.
// Author: huojiasen (huojiasen@since-inc.com)
//
// Description:
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

int TcpListen(int port) {

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    struct linger tmp = {1,0};
    setsockopt(sock, SOL_SOCKET, SO_LINGER, &tmp, sizeof(tmp));

    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(struct sockaddr_in));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(port);

    bind(sock, (struct sockaddr*)&sin, sizeof(sin));

    listen(sock, 10);
    return sock;
}

int TcpConnect(const char*host, int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(struct sockaddr_in));
    sin.sin_addr.s_addr = inet_addr(host);
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);

    connect(sock, (struct sockaddr*)&sin, sizeof(sin));
    return sock;
}

int TcpAccept(int sock) {
    int confd;
    struct sockaddr addr;
    socklen_t addrlen;
    while ((confd = accept(sock, &addr, &addrlen)) == -1) {
    }
    return confd;
}

int TcpRecv(int sock, void *buf, int buf_size) {
    int read_num;
    while (true) {
        read_num = read(sock, buf, buf_size);
        if (read_num >= 0) {
            break;
        }
    }
    return read_num;
}

int TcpSend(int sock, const void *buffer, int size) {
    int left_bytes = size;
    int read_num;

    const char *pbuf = reinterpret_cast<const char *>(buffer);
    while (left_bytes > 0) {
        read_num = write(sock, pbuf, left_bytes);
        left_bytes -= read_num;
        pbuf += read_num;
    }
    return size;
}
