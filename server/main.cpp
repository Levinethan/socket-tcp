//
//  main.cpp
//  server
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

#define SOCKET int
#define INVALID_SOCKET (SOCKET)(~0)
#define SOCKET_ERROR (-1)

enum CMD
{
    CMD_LOGIN,
    CMD_LOGIN_RESULT,
    CMD_LOGOUT,
    CMD_LOGOUT_RESULT,
    CMD_ERROR
};

struct DataHeader
{
    short dataLength;
    short cmd;
};

//DataPackage
struct Login: public DataHeader
{
    Login()
    {
        dataLength = sizeof(Login);
        cmd = CMD_LOGIN;
    }
    char userName[32];
    char PassWord[32];
};

struct LoginResult : public DataHeader
{
    LoginResult()
    {
        dataLength = sizeof(LoginResult);
        cmd = CMD_LOGIN_RESULT;
        result = 0;
    }
    int result;
};

struct Logout : public DataHeader
{
    Logout()
    {
        dataLength = sizeof(Logout);
        cmd = CMD_LOGOUT;
    }
    char userName[32];
};

struct LogoutResult : public DataHeader
{
    LogoutResult()
    {
        dataLength = sizeof(LogoutResult);
        cmd = CMD_LOGIN_RESULT;
        result = 0;
    }
    int result;
};

std::vector<SOCKET> g_clients;

int processor(SOCKET _cSock)
{
    char szRecv[4096] = {};
    int nLen = (int)recv(_cSock, szRecv, sizeof(DataHeader), 0);
    DataHeader* header = (DataHeader*)szRecv;
    if (nLen <= 0)
    {
        printf("client log out");
        return -1;
    }
    switch (header->cmd)
    {
    case CMD_LOGIN:
    {
        recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
        Login* login = (Login*)szRecv;
        printf("command:Login length:%d User:%s \n",login->dataLength,login->userName);
        LoginResult ret;
        send(_cSock, (char*)&ret, sizeof(LoginResult), 0);
    }
    break;
    case CMD_LOGOUT:
    {
        recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
        Logout* logout = (Logout*)szRecv;
        printf("command:Logout length:%d User:%s \n",logout->dataLength,logout->userName);
        LogoutResult ret;
        send(_cSock, (char*)&ret, sizeof(ret), 0);
    }
    break;
    default:
    {
        DataHeader header = { 0,CMD_ERROR };
        send(_cSock, (char*)&header, sizeof(header), 0);
    }
    break;
    }
    return 1;
}

int main()
{

    //------------


    SOCKET _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    sockaddr_in _sin = {};
    //绑定client连接的网络接口
    _sin.sin_family = AF_INET;
    _sin.sin_port = htons(4500);
    _sin.sin_addr.s_addr = htonl(INADDR_ANY);
    if (SOCKET_ERROR == bind(_sock, (sockaddr*)&_sin, sizeof(_sin)))
    {
        perror("bind error");
        
    }
    else {
        printf("success bind\n");
    }

    if (SOCKET_ERROR == listen(_sock, 5))
    {
        perror("listen error\n");
        
    }
    else {
        printf("success listen\n");
    }

    while (true)
    {
       
        fd_set fdRead;
        fd_set fdWrite;
        fd_set fdExp;

        FD_ZERO(&fdRead);
        FD_ZERO(&fdWrite);
        FD_ZERO(&fdExp);

        FD_SET(_sock, &fdRead);
        FD_SET(_sock, &fdWrite);
        FD_SET(_sock, &fdExp);

        for (int n = (int)g_clients.size()-1; n >= 0 ; n--)
        {
            FD_SET(g_clients[n], &fdRead);
        }

        timeval t = {0,0};
        int ret = select(_sock + 1, &fdRead, &fdWrite, &fdExp, &t);
        if (ret < 0)
        {
            printf("select over\n");
            break;
        }
        if (FD_ISSET(_sock, &fdRead))
        {
            FD_CLR(_sock, &fdRead);
          
            sockaddr_in clientAddr = {};
           
            SOCKET _cSock = INVALID_SOCKET;
            _cSock = accept(_sock, (sockaddr*)NULL, NULL);
            if (INVALID_SOCKET == _cSock)
            {
                printf("invalid client socket\n");
            }
            g_clients.push_back(_cSock);
            printf("success acceptsocket = %d,IP = %s \n", (int)_cSock, inet_ntoa(clientAddr.sin_addr));
        }
        for (size_t n = 0; n < fdRead.fd_count; n++)
        {
            if (-1 == processor(fdRead.fd_array[n]))
            {
                auto iter = find(g_clients.begin(), g_clients.end(), fdRead.fd_array[n]);
                if (iter != g_clients.end())
                {
                    g_clients.erase(iter);
                }
            }
        }
    }

    for (size_t n = g_clients.size() - 1; n >= 0; n--)
    {
        close(g_clients[n]);
    }
    getchar();
    close(_sock);
    return 0;
}
