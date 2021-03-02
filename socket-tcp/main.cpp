//
//  main.cpp
//  socket-tcp
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
#define INVALID_SOCKET  0
int _sock;
int _cSock;
struct DataPackage{
    int age;
    char name[32];
};
int main(int argc, const char * argv[]) {
    //建立一个socket IPV4 创建面向数据流 tcp/udp
    _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    sockaddr_in _sin = {};
    //绑定client连接的网络接口
    _sin.sin_family = AF_INET;
    _sin.sin_port = htons(4500);
    _sin.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(_sock, (sockaddr *)&_sin, sizeof(_sin)) < 0){
        perror("bind error");
        exit(1);
    }else{
        printf("success bind\n");
    }
    //监听端口
    if (listen(_sock,5) < 0){
        perror("listen error\n");
        exit(1);
    }else{
        printf("success listen\n");
    }
    
    //accept 等待client连接
    
    _cSock = INVALID_SOCKET;
    char msgBuf[] = "I`m Server";
    _cSock = accept(_sock, (sockaddr *)NULL ,NULL);
    if (INVALID_SOCKET == _sock){
        printf("invalid client socket\n");
    }
    printf("success accept\n");
    char recvBuf[128] = {};
    while (1) {
        long nLen = recv(_cSock, recvBuf, strlen(msgBuf)+1, 0);
        if (nLen <= 0 ){
            printf("client log out");
            break;
        }
        if (0 == strcmp(recvBuf, "getInfo")) {
            DataPackage dp = {80,"zc"};
            
            send(_cSock, (const char *)&dp, sizeof(DataPackage), 0);
        }
        else{
            char msgBuf[] = "what can i help you?";
            send(_cSock, msgBuf, strlen(msgBuf)+1, 0);
        }
    }
    getchar();
    //关闭socket
    close(_sock);
    return 0;
}
