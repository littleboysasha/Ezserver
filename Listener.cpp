//
// Created by steve on 2019/10/11.
//

#include "Listener.h"
#include <cstdio>
#include <cstdlib>
#include <inaddr.h>


unsigned WINAPI ClntHandler(void *arg) {
    printf("This is thread : %d \n", threadNum);
    threadNum++;
    Listener *pListener = (Listener *)arg;
    SOCKET clntSock = pListener->clntSock;
    char buf[BUFSIZE];
    recv(clntSock, buf, BUFSIZE, 0);
    printf("Request from client : %s:%d \n  Http request : %s \n", inet_ntoa(pListener->clntAddr.sin_addr), ntohs(pListener->clntAddr.sin_port), buf);
    fflush(stdout);
    if(strstr(buf, "HTTP/") == NULL) {
        pListener->SendError(clntSock);
        closesocket(clntSock);
        return 1;
    }
    char method[SBUFSIZE];
    strcpy(method, strtok(buf, " /"));
    if(strcmp(method, "GET")) {
     //   printf("OK \n");
     //   fflush(stdout);
        pListener->SendError(clntSock);
        closesocket(clntSock);
        return 1;
    }
   // printf("OK \n");
   // fflush(stdout);
    char fileName[SBUFSIZE];

    strcpy(fileName, strtok(NULL, " /"));

    if(fileName == NULL || strstr(fileName, ".") == NULL) {
        pListener->SendError(clntSock);
        closesocket(clntSock);
        return 1;
    }

    /*
    if(fileName == "") {
        pListener->SendError(clntSock);
        closesocket(clntSock);
        return 1;
    }
*/
  //  printf("%s \n", fileName);

    char recvType[SBUFSIZE];
    strcpy(recvType, pListener->GetType(fileName));

  //  printf("%s \n", recvType);

    pListener->SendMsg(clntSock, recvType, fileName);

    closesocket(clntSock);
    return 0;
}


void Listener::init() {
    WSADATA wsadata;

    if(WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
        printf("WSASTART FAIL ! \n");
        fflush(stdout);
        exit(-1);
    } else {
        printf("WSASTART SUCCESSFULLY ! \n");
        fflush(stdout);
    }

    SOCKADDR_IN servAddr;
    servAddr.sin_port = htons(port);
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.S_un.S_addr = inet_addr(listenIp);

    servSock = socket(AF_INET, SOCK_STREAM, 0);

    if(bind(servSock, (SOCKADDR *)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) {
        printf("BINDED FAIL !\n");
        fflush(stdout);
        exit(-1);
    } else {
        printf("BINDED SUCCESSFULLY !\n");
        fflush(stdout);
    }

    if(listen(servSock, 10) == SOCKET_ERROR) {
        printf("LISTENING FAIL !\n");
        fflush(stdout);
        exit(-1);
    } else {
        printf("LISTENING SUCCESSFULLY !\n");
        fflush(stdout);
    }
}


void Listener::start() {
    int len;
    while(1) {
        len = sizeof(clntAddr);
        clntSock = accept(servSock, (SOCKADDR *)&clntAddr, &len);
        hThread = (HANDLE)_beginthreadex(NULL, 0, ClntHandler, (void *)this, 0, (unsigned *)&threadID);
    }
    closesocket(servSock);
    WSACleanup();
}



char* Listener::GetType(char *fName) {
    char exts[SBUFSIZE];
    char name[SBUFSIZE];
    strcpy(name, fName);
    strtok(name, ".");
    strcpy(exts, strtok(NULL, "."));
    if(!strcmp(exts, "html") || !strcmp(exts, "htm")) {
        return (char *)"text/html";
    } else {
        return (char *)"text/plain";
    }
}

void Listener::SendMsg(SOCKET sock, char *rcType, char *fName) {
    char* servHeader = "HTTP/1.0 200 OK\r\n"
                       "Server:Ezserver\r\n"
                       "Content-length:%d\r\n"
                       "Content-type:%s\r\n\r\n";
    char header[BUFSIZE];

/*
    char protocol[] = "HTTP/1.0 200 OK\r\n";
    char serv[] = "Server:Ezserver\r\n";
    char cntSize[] = "Content-length:40960\r\n";
    char cntType[SBUFSIZE];
    */
    char buf[BUFSIZE];
    FILE* opFile;
    char fileName[SBUFSIZE];
    strcpy(fileName, rootPath);
    strcat(fileName, fName);
    //sprintf(cntType, "Content-type:%s\r\n\r\n", rcType);

    //printf("%s \n", cntType);

    opFile = fopen(fileName, "rb");
    //test
    //printf("OK \n");
    //fflush(stdout);
    //test
    if(opFile == NULL) {
        SendError(sock);
        return;
    }
    int fileSize;
    int headSize;
    fseek(opFile, 0, SEEK_END);
    fileSize = ftell(opFile);
    fseek(opFile, 0, SEEK_SET);

    headSize = sprintf(header, servHeader, fileSize, rcType);

    send(sock, header, headSize, 0);

    int readLen;
    while((readLen = fread(buf, 1, 1024, opFile)) != 0 && fileSize > 0) {
        send(sock, buf, readLen, 0);
        fileSize -= readLen;
    }


/*
    send(sock, protocol, strlen(protocol), 0);
    send(sock, serv, strlen(serv), 0);
    send(sock, cntSize, strlen(cntSize), 0);
    send(sock, cntType, strlen(cntType), 0);

    while(fgets(buf, BUFSIZE, opFile) != NULL) {
        //printf("OK \n`
        // ");
       // fflush(stdout);
        send(sock, buf, strlen(buf), 0);
    }
    */
    fclose(opFile);
    closesocket(sock);
    printf("Satisfy the request successfully! \n");
    fflush(stdout);
}

void Listener::SendError(SOCKET sock) {
    printf("Fail to satisfy the request! \n");
    fflush(stdout);
    char protocol[] = "HTTP/1.1 400 Bad Request\r\n";
    char serv[] = "Server:Ezserver\r\n";
    char cntSize[] = "Content-length:4096\r\n";
    char cntType[] = "Content-Type:text/html\r\n\r\n";
    char content[] = "<html><head><title>NETWORK ERROR</title></head>"
                     "<body><font size=+4><br> FILE NOT FOUND! "
                     "</font></body></html>";

    send(sock, protocol, strlen(protocol), 0);
    send(sock, serv, strlen(serv), 0);
    send(sock, cntSize, strlen(cntSize), 0);
    send(sock, cntType, strlen(cntType), 0);
    send(sock, content, strlen(content), 0);

    closesocket(sock);
}

int Listener::ReadConfig() {
    FILE *fp = fopen(configName, "r");
    if(fp == NULL) {
        printf("no config file \n");
        fflush(stdout);
        return -1;
    }
    char buf[50];
    int i = 0;
    while(fgets(buf, 50, fp)) {
        if(i == 0) {
            strcpy(listenIp, buf);
        } else if(i == 1) {
            port = atoi(buf);
        } else if(i == 2) {
            strcpy(rootPath, buf);
        }
        i++;
    }

    printf("config file is loaded successfull\n");
    fflush(stdout);
    return 1;

}