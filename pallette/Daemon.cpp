/*************************************************************************
	> File Name: Dameon.cpp
	> Author: pengjunjie
	> Mail: 1002398145@qq.com
	> Created Time: 2019年03月19日 星期二 19时08分43秒
 ************************************************************************/

#include <pallette/Daemon.h>

#include <sys/stat.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <netdb.h>

int pallette::daemonInit(std::string workdir)
{
    //1. 创建子进程，父进程退出 fork，所有工作在子进程中进行形式上脱离了控制终端，并且进程组ID和进程ID是不同的，这样才能执行下一个步骤
    //2. 在子进程中创建新会话，setsid()函数，使子进程完全独立出来，脱离于控制终端的所有关联关系
    //3. 改变当前目录为根目录，chdir()函数
    //4. 重设文件权限掩码，umask()函数，防止继承的文件创建屏蔽字拒绝某些权限
    //5. 关闭文件描述符，继承的打开文件不会用到,将文件描述符0,1,2重定向到/dev/null中，目的是确保daemon调用了IO库函数不会执行失败
    //6. 开始执行守护进程核心工作
    const int kMaxClose = 8192;

    switch (fork())
    {
    case -1:
        return -1;
    case 0:
        break;
    default:
        _exit(EXIT_SUCCESS);
    }

    if (setsid() == -1)
    {
        return -1;
    }

    switch (fork())//确保进程不是会话首进程
    {
    case -1:
        return -1;
    case 0:
        break;
    default:
        _exit(EXIT_SUCCESS);
    }

    umask(0);
    if (mkdir(workdir.c_str(), 0775))
    {
        if (errno != EEXIST)
        {
            return -1;
        }
    }

    if (chdir(workdir.c_str()))
    {
        return -1;
    }

    int maxfd = sysconf(_SC_OPEN_MAX);
    if (maxfd == -1)
    {
        maxfd = kMaxClose;
    }
    int fd;
    for (fd = 0; fd < maxfd; ++fd)
    {
        close(fd);
    }

    fd = open("/dev/null", O_RDWR);
    if (fd != STDIN_FILENO)
    {
        return -1;
    }
    if (dup2(STDIN_FILENO, STDOUT_FILENO) != STDOUT_FILENO)
    {
        return -1;
    }
    if (dup2(STDIN_FILENO, STDERR_FILENO) != STDERR_FILENO)
    {
        return -1;
    }

    return 0;
}