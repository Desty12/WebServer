#include "util.h"
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <string.h>

ssize_t readn(int fd, void *buff, size_t n)
{
    size_t nleft = n;
    ssize_t nread = 0;
    ssize_t readSum = 0;
    char *ptr = (char*)buff;
    while (nleft > 0)
    {
        if ((nread = read(fd, ptr, nleft)) < 0)
        {
            if (errno == EINTR)
                nread = 0;
            else if (errno == EAGAIN)
            {
                return readSum;
            }
            else
            {
                return -1;
            }  
        }
        else if (nread == 0)
            break;
        readSum += nread;
        nleft -= nread;
        ptr += nread;
    }
    return readSum;
}

ssize_t writen(int fd, void *buff, size_t n)
{
    size_t nleft = n;
    ssize_t nwritten = 0;
    ssize_t writeSum = 0;
    char *ptr = (char*)buff;
    while (nleft > 0)
    {
        if ((nwritten = write(fd, ptr, nleft)) <= 0)
        {
            if (nwritten < 0)
            {
                if (errno == EINTR || errno == EAGAIN)
                {
                    nwritten = 0;
                    continue;
                }
                else
                    return -1;
            }
        }
        writeSum += nwritten;
        nleft -= nwritten;
        ptr += nwritten;
    }
    return writeSum;
}

void handle_for_sigpipe()
{
    struct sigaction sa;  // 结构体是 Linux 系统编程中用于信号处理的关键组件
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = SIG_IGN;  // 这是一个函数指针，指向信号处理函数，该函数在接收到信号时被调用,此处忽略信号
    sa.sa_flags = 0;          // 用于修改信号处理行为的一组标志
    /* 原型：int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact); */
    if(sigaction(SIGPIPE, &sa, NULL)) // 语句的作用为：收到管道信号后对信号执行信号忽略SIG_IGN
        return;
}

int setSocketNonBlocking(int fd)
{
    // fcntl 是一个用于操作文件描述符的系统调用函数
    int flag = fcntl(fd, F_GETFL, 0);
    if(flag == -1)
        return -1;
    // 设置非阻塞模式
    flag |= O_NONBLOCK;
    /* // 设置新的文件状态标志，启用非阻塞模式 */
    if(fcntl(fd, F_SETFL, flag) == -1)
        return -1;
    return 0;
}