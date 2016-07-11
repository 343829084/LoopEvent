#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h> //for bzero
#include <iostream>
#include <sys/types.h>
#include <unistd.h>


#define MAX_LINE 100
#define MAX_EVENTS 500
#define MAX_LISTENFD 5

class TcpServer
{
public:

  void start();
  void handleRead(int efd, int fd);
  void handleWrite(int efd, int fd);
  void handleAccept(int efd, int fd);

  void setNonBlock(int fd);
  void updateEvent(int efd, int fd, int events, int op);

private:
  int createAndListen();


private:
  int m_listenfd, m_sockfd;
  struct epoll_event m_events[MAX_EVENTS];
};
