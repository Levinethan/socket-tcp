//
//  main.cpp
//  client
//
//  Created by mac on 2021/3/2.
//

#include <iostream>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>


int main(int argc, const char * argv[]) {
    int _sock;
    char recvBuf[256] = {};
    if ((_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
            fprintf(stderr,"Socket error:%s\n\a", strerror(errno));
            exit(1);
        }
    sockaddr_in _sin = {};
    //绑定client连接的网络接口
    _sin.sin_family = AF_INET;
    _sin.sin_port = htons(4500);
    _sin.sin_addr.s_addr = htonl(INADDR_ANY);
    int ret = connect(_sock, (sockaddr *)&_sin, sizeof(sockaddr));
    if (ret == -1) {
        printf("error\n");
    }else{
        printf("success connect server\n");
    }
    
    while (true) {
        char cmdBuf[128] = {};
        scanf("%s",cmdBuf);
        if (0 == strcmp(cmdBuf, "exit")) {
            break;
        }else{
            send(_sock, cmdBuf, strlen(cmdBuf)+1, 0);
        }
        long nlen = recv(_sock, recvBuf, 256, 0);
        if (nlen > 0){
            printf("receive data :%s \n",recvBuf);
        }
    }
    

    close(_sock);
    getchar();
    return 0;
}

