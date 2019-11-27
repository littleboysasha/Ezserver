//
// Created by steve on 2019/10/11.
//

#ifndef EZSERVER_LISTENER_H
#define EZSERVER_LISTENER_H

#include <WinSock2.h>
#include <process.h>

static int threadNum = 0;

const int BUFSIZE = 4096;
const int SBUFSIZE = 200;

unsigned WINAPI ClntHandler(void *arg);

class Listener {
public:
    friend unsigned WINAPI ClntHandler(void *arg);
    Listener(char* configFile):configName(configFile) {
        listenIp = new char[SBUFSIZE];
        rootPath = new char[SBUFSIZE];
    }
    void SendError(SOCKET sock);
    void SendMsg(SOCKET sock, char *rcType, char *fName);
    char* GetType(char* fName);
    void init();
    void start();
    int ReadConfig();
private:
    SOCKET servSock;
    SOCKET clntSock;
    SOCKADDR_IN clntAddr;
    int port;
    HANDLE hThread;
    DWORD threadID;
    char* configName;
    char* listenIp;
    char* rootPath;
};



#endif //EZSERVER_LISTENER_H
