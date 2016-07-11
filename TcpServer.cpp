#include "TcpServer.h"
using namespace std;


int TcpServer::createAndListen()
{
    int on = 1;
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in servaddr;
    fcntl(listenfd, F_SETFL, O_NONBLOCK); //no-block io
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(11111);

    if(-1 == bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)))
    {
        cout << "bind error, errno:" << errno << endl;
    }

    if(-1 == listen(listenfd, MAX_LISTENFD))
    {
        cout << "listen error, errno:" << errno << endl;
    }
    return listenfd;
}

void TcpServer::start()
{
  int epollfd = epoll_create(1);
  if (epollfd< 0)
  {
      cout << "epoll_create error, error:" << epollfd << endl;
      return;
  }
  m_listenfd = createAndListen();
  struct epoll_event ev;
  ev.data.fd = m_listenfd;
  ev.events = EPOLLIN;
  epoll_ctl(epollfd, EPOLL_CTL_ADD, m_listenfd, &ev);

  for(;;)
  {
      int fds = epoll_wait(epollfd, m_events, MAX_EVENTS, -1);
      if(fds == -1)
      {
          cout << "epoll_wait error, errno:" << errno << endl;
          break;
      }
      int sockfd;
      for(int i = 0; i < fds; i++)
      {
          if(m_events[i].events & EPOLLIN)
          {
            if((sockfd = m_events[i].data.fd) < 0)
            {
                cout << "EPOLLIN sockfd < 0 error " << endl;
                continue;
            }
            if(m_events[i].data.fd == m_listenfd)
            {
              handleAccept(epollfd, m_listenfd);

            } else {
              handleRead(epollfd, sockfd);
            }
          } else if (m_events[i].events & EPOLLOUT){
             handleWrite(epollfd, sockfd);
          } else {
              // do some error handle.
          }
      }
  }
  return;
}


void TcpServer::handleRead(int efd, int fd)
{
  char buf[MAX_LINE];
  bzero(buf, MAX_LINE);
  int readlength;
  if((readlength = read(fd, buf, MAX_LINE)) < 0)
  {
      if(errno == ECONNRESET)
      {
          cout << "ECONNREST closed socket fd:" << fd << endl;
          close(fd);
      }
  }
  else if( readlength == 0)
  {
      cout << "read 0 closed socket fd:" << fd << endl;
      close(fd);
  }


  return;
}

void TcpServer::handleWrite(int efd, int fd)
{
  char buf[MAX_LINE] = "hello ,world!";
  int sendLen = sizeof buf;
  if(write(fd, buf, sendLen) != sendLen)
            cout << "error: not finished one time" << endl;

}
void TcpServer::handleAccept(int efd, int fd)
{
  struct sockaddr_in cliaddr;
  socklen_t clilen = sizeof(cliaddr);
  struct epoll_event ev;
  int connfd = accept(fd, (sockaddr*)&cliaddr, (socklen_t*)&clilen);
  if(connfd > 0)
  {
      cout << "new connection from "
           << "[" << inet_ntoa(cliaddr.sin_addr)
           << ":" << ntohs(cliaddr.sin_port) << "]"
           << " accept socket fd:" << connfd
           << endl;
  }
  else
  {
      cout << "accept error, connfd:" << connfd
           << " errno:" << errno << endl;
  }
  fcntl(connfd, F_SETFL, O_NONBLOCK); //no-block io
  ev.data.fd = connfd;
  ev.events = EPOLLIN;
  if( -1 == epoll_ctl(efd, EPOLL_CTL_ADD, connfd, &ev))
      cout << "epoll_ctrl error, errno:" << errno << endl;

  return;
}

void TcpServer::setNonBlock(int fd)
{
  return;
}


void TcpServer::updateEvent(int efd, int fd, int events, int op)
{

  return;
}
