#include <iostream>
#include <stdio.h>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <csignal>

/*
    Created by lyj on 2022/7/1.
    p8 socket编程
    实现点对点的连接
 */

#define ERR_EXIT(m) \
    do \
    { \
        perror(m); \
        exit(EXIT_FAILURE); \
    }while(0)

void handler(int sig)
{
    printf("recv a sig = %d\n",sig);
    exit(EXIT_SUCCESS);
}

int main(void)
{
    /*
     * 创建一个套接字类型
     * */
    int sock;
    if((sock = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP)) < 0)// 三个参数表示 网际协议，流式套接字，TCP协议
        ERR_EXIT("socket");//创建成功返回0 失败返回-1


    /*
     * 创建并初始化一个ipv4地址结构
     * */
    struct sockaddr_in servaddr; //创建一个ipv4的地址结构
    memset(&servaddr,0,sizeof servaddr);//初始化地址
    servaddr.sin_family = PF_INET;//初始化地址族为PF_INET 表示IPV4的套接字类型
    servaddr.sin_port = htons(30000);//初始化端口号 需要网络字节序类型
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    /*
     * 连接至服务器端
     * 连接成功 则sock这个套接字处于通信状态
     * */
    if(connect(sock,(struct sockaddr*)&servaddr,sizeof(servaddr)) < 0)
        ERR_EXIT("connect");

    pid_t pid;
    pid = fork();
    if(pid == -1)
        ERR_EXIT("fork");

    if(pid == 0){
        char recvbuf[1024];
        while(1){
            memset(recvbuf,0,sizeof recvbuf);
            int ret = read(sock,recvbuf,sizeof recvbuf);
            if(ret == -1)
                ERR_EXIT("read");
            if(ret == 0) {
                printf("peer close\n");
                break;
            }
            fputs(recvbuf,stdout);
        }
        close(sock);
        kill(getppid(),SIGUSR1);
    } else {
        signal(SIGUSR1,handler);
        char sendbuf[1024] = {0};
        while (fgets(sendbuf,sizeof(sendbuf),stdin) != NULL){
            write(sock,sendbuf,strlen(sendbuf));
            memset(sendbuf,0,sizeof sendbuf);
        }
        close(sock);
    }
    return 0;
}
