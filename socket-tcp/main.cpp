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
enum CMD{
    CMD_LOGIN,
    CMD_LOGOUT,
    CMD_ERROR
};
struct DataHeader{
    short dataLength;
    short cmd;
};
struct Login{
    char userName[32];
    char PassWord[32];
};
struct LoginResult{
    int result;
    
};
struct Logout{
    char userName[32];
};
struct LogoutResult{
    int result;
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
    
    _cSock = accept(_sock, (sockaddr *)NULL ,NULL);
    if (INVALID_SOCKET == _sock){
        printf("invalid client socket\n");
    }
    printf("success accept\n");

    while (1) {
        DataHeader header = {};
        
        long nLen = recv(_cSock, (char *)&header, sizeof(DataHeader), 0);
        if (nLen <= 0 ){
            printf("client log out");
            break;
        }
        printf("command:%d length:%d \n",header.cmd,header.dataLength);
        switch (header.cmd) {
            case CMD_LOGIN:
            {
                Login login = {};
                recv(_cSock, (char *)&login, sizeof(login), 0);
                //判断账号密码是否正确
                LoginResult ret = {0};
                
                send(_cSock, (char *)&header, sizeof(DataHeader), 0);
                send(_cSock, (char *)&ret, sizeof(LoginResult), 0);
            }
                break;
            case CMD_LOGOUT:
            {
                Logout logout = {};
                recv(_cSock, (char *)&logout, sizeof(logout), 0);
                //判断账号密码是否正确
                LogoutResult ret = {1};
                
                send(_cSock, (char *)&header, sizeof(header), 0);
                send(_cSock, (char *)&ret, sizeof(LogoutResult), 0);
            }
            default:
                header.cmd = CMD_ERROR;
                header.dataLength = 0;
                send(_cSock, (char *)&header, sizeof(header), 0);
                
                break;
        }
        
    }
    getchar();
    //关闭socket
    close(_sock);
    return 0;
}
