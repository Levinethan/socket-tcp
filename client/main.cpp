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
    int _sock;
    char recvBuf[128] = {};
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
        }else if (0 == strcmp(cmdBuf, "login")){
            Login login = {"cooci","qwer1234"};
            DataHeader dh = {sizeof(login),CMD_LOGIN};
            

            send(_sock, (char *)&dh, sizeof(dh), 0);
            send(_sock, (char *)&login, sizeof(login), 0);
            
            DataHeader retHeader = {};
            LoginResult loginRet = {};
            
            recv(_sock, (char *)&retHeader, sizeof(retHeader), 0);
            recv(_sock, (char *)&loginRet, sizeof(loginRet), 0);
            
            printf("Login Result %d\n",loginRet.result);
        }else if (0 == strcmp(cmdBuf, "logout")){
            Logout logout = {"cooci"};
            DataHeader dh = {sizeof(Logout),CMD_LOGOUT};
            send(_sock, (char *)&dh, sizeof(dh), 0);
            send(_sock, (char *)&logout, sizeof(logout), 0);
            
            DataHeader retHeader = {};
            LogoutResult logoutRet = {};
            
            recv(_sock, (char *)&retHeader, sizeof(retHeader), 0);
            recv(_sock, (char *)&logoutRet, sizeof(logoutRet), 0);
            
            printf("Logout Result %d\n",logoutRet.result);
            
            
        }else{
            printf("unsupport command,please try again\n");
        }
    }
    

    close(_sock);
    getchar();
    return 0;
}

