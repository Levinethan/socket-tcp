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
#include <vector>
#define INVALID_SOCKET  0

int _sock;
int _cSock;
enum CMD{
    CMD_LOGIN,
    CMD_LOGOUT,
    CMD_ERROR,
    CMD_LOGIN_RESULT,
    CMD_LOGOUT_RESULT
};
struct DataHeader{
    short dataLength;
    short cmd;
};
struct Login:public DataHeader{
    
    Login()
    {
        dataLength = sizeof(Login);
        cmd = CMD_LOGIN;
    }
    char userName[32];
    char PassWord[32];
};
struct LoginResult:public DataHeader{
    LoginResult()
    {
        dataLength = sizeof(Login);
        cmd = CMD_LOGIN_RESULT;
        result = 0;
    }
    int result;
    
};
struct Logout:public DataHeader{
    Logout()
    {
        dataLength = sizeof(Login);
        cmd = CMD_LOGOUT;
    }
    char userName[32];
};
struct LogoutResult:public DataHeader{
    LogoutResult()
    {
        dataLength = sizeof(Login);
        cmd = CMD_LOGOUT_RESULT;
        result = 0;
    }
    int result;
};

std::vector<socklen_t>g_clients;

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
    
    

    while (1) {
        fd_set fdRead;
        fd_set fdWrite;
        fd_set fdExp;
        __DARWIN_FD_ZERO(&fdRead);
        __DARWIN_FD_ZERO(&fdWrite);
        __DARWIN_FD_ZERO(&fdExp);
        __DARWIN_FD_SET(_sock, &fdRead);
        __DARWIN_FD_SET(_sock, &fdWrite);
        __DARWIN_FD_SET(_sock, &fdExp);
        
        for (size_t n = g_clients.size()-1; n >= 0; n--) {
            __DARWIN_FD_SET(g_clients[n], &fdRead);
        }
        int ret = select(_sock+1, &fdRead, &fdWrite, &fdExp, NULL);
        if (ret < 0){
            printf("select over\n");
            break;
        }
        
        if (__DARWIN_FD_ISSET(_sock, &fdRead)){
            __DARWIN_FD_CLR(_sock, &fdRead);
            _cSock = INVALID_SOCKET;
            _cSock = accept(_sock, (sockaddr *)NULL ,NULL);
            if (INVALID_SOCKET == _sock){
                printf("invalid client socket\n");
            }
            g_clients.push_back(_cSock);
            printf("success accept\n");
            
        }
        //缓冲区
        for (size_t n = 0; n< ; n++) {
            char szRecv[4096] = {};
            long nLen = recv(_cSock, szRecv, sizeof(DataHeader), 0);
            DataHeader* header = (DataHeader *)szRecv;
            
            if (nLen <= 0 ){
                printf("client log out");
                break;
            }
            switch (header->cmd) {
                case CMD_LOGIN:
                {
                    
                    recv(_cSock, szRecv+sizeof(DataHeader), header->dataLength-sizeof(DataHeader), 0);
                    //判断账号密码是否正确
                    Login *login = (Login *)szRecv;
                    printf("command:Login length:%d User:%s \n",login->dataLength,login->userName);
                    LoginResult ret;
                    send(_cSock, (char *)&ret, sizeof(LoginResult), 0);
                }
                    break;
                case CMD_LOGOUT:
                {
                    
                    recv(_cSock, szRecv+sizeof(DataHeader), header->dataLength-sizeof(DataHeader), 0);
                    Logout *logout = (Logout *)szRecv;
                    printf("command:Logout length:%d User:%s \n",logout->dataLength,logout->userName);
                    //判断账号密码是否正确
                    LogoutResult ret;
                    send(_cSock, (char *)&ret, sizeof(ret), 0);
                }
                default:
                {
                    DataHeader header = {0,CMD_ERROR};
                    send(_cSock, (char *)&header, sizeof(header), 0);
                }
                    break;
            }
        }
        
    }
    getchar();
    //关闭socket
    close(_sock);
    return 0;
}
