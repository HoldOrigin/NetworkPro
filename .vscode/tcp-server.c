#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>



int keep_alive(int nSockFd, int nKeepAlive, int nKeepIdle, int nKeepInterval, int nKeepCount)
{
    int nRet = 0;
    if(nSockFd < 0){
        return -1;
    }
    //开启keepalive探测
    nRet = setsockopt(nSockFd, SOL_SOCKET, SO_KEEPALIVE, (void *)&nKeepAlive, sizeof(nKeepAlive));
    if(nRet < 0)
    {
        perror("set socket keepalived");
        return nRet;
    }
    //设置第一次探活的时间间隔
    nRet = setsockopt(nSockFd, SOL_TCP, TCP_KEEPIDLE, (void *)&nKeepIdle, sizeof(nKeepIdle));
    if(nRet < 0)
    {
        perror("set socket nKeepIdle");
        return nRet;
    }
    //每次探测的时间检测
     nRet = setsockopt(nSockFd, SOL_TCP, TCP_KEEPINTVL, (void *)&nKeepInterval, sizeof(nKeepInterval));
    if(nRet < 0)
    {
        perror("set socket nKeepIntvl");
        return nRet;
    }
    //尝试探测的次数
     nRet = setsockopt(nSockFd, SOL_SOCKET, TCP_KEEPCNT, (void *)&nKeepCount, sizeof(nKeepCount));
    if(nRet < 0)
    {
        perror("set socket nKeepCnt");
        return nRet;
    }
    return nRet;

}
int set_tcpserver(char *strIp, unsigned int nPort, int nMaxClient)
{
    int nSockfd = 0;
    int nRet = -1;
    int nVal = 0;
    int nEnable = 1;
    int nTcpTimeOut = 60 * 60;
    int nTcpTimeIntvl = 60;
    int nTcpTryCnt = 3;
    struct sockaddr_in addr_in;

    if(NULL == strIp || 0 == nPort || nMaxClient <= 0)
    {
        return nRet;
    }
    nSockfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(nSockfd < 0)
    {
        perror("Create socket!");
        return nSockfd;
    }
    //设置套接字的属性
    nVal = 1;
    //开启端口复用，避免bind绑定失败
    nRet = setsockopt(nSockfd, SOL_SOCKET, SO_REUSEADDR, (void *)&nVal,sizeof(nVal));
    if( nRet < 0){
        perror("set socketfd ReuseAddr!")
        close(nSockfd);
        return nRet;
    }
    //禁止Nagle算法，连接上一有数据，接发送。不缓存
    nRet = setsockopt(nSockfd, IPPROTO_TCP, TCP_NODELAY, (void *)&nVal, sizeof(nVal));
    if( nRet < 0){
        perror("set socketfd NoDelay!")
        close(nSockfd);
        return nRet;
    }
    nRet = keep_alive(nSockfd, nEnable, nTcpTimeOut, nTcpTimeIntvl, nTcpTryCnt );
    if(nRet != 0)
    {
        perror("set socket keep!");
        close(nSockfd);
        return nRet;
    }
    //绑定地址端口
    memset(&addr_in, 0, sizeof(struct sockaddr_in));
    addr_in.sin_family = AF_INET;
    addr_in.sin_addr = htons(nPort);
    addr_in.in_addr =  strIp ? inet_addr(strIp) : INADDR_ANY; 

    nRet = bind(nSockfd, (const struct  sockaddr *)&addr_in, sizeof(struct sockaddr));
    if( nRet < 0)
    {
        perror("bind socket!")
        close(nSockfd);
        return nRet;
    }
//监听套接字
    nRet = listen(nSockfd, nMaxClient);
    if(nRet < 0)
    {
        perror("socket listen");
        close(nSockfd);
        return nRet;
    }

}
int select_clientfd(int nFd, int ntimeout)
{
    int nRet = 0;
    int nMaxFd = 0;
    struct timeval tv;
    fd_set rFdSet;
    if(nFd < 0 ){ return -1;}

    memset(&tv, 0, sizeof(tv));
    tv.tv_sec = ntimeout;
    FD_ZERO(rFdSet);
    FD_SET(nFd, &rFdSet);
    nMaxFd = nFd + 1;
    nRet = select(nMaxFd, &nFdSet, NULL, NULL, &tv);
    if (nRet < 0)
    {
        printf("Select Error:%d\r\n",nRet);
        return nRet;
    }else( nRet == 0){
        printf("select TimeOut\n");
    }
    return nRet;
}


