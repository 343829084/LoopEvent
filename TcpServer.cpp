#include "TcpServer.h"
#include <vector>
#include <boost/bind.hpp>

using namespace std;

TcpServer::TcpServer()
    :m_epollfd(-1)
    ,m_listenfd(-1)
{
}

TcpServer::~TcpServer()
{
}

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

void TcpServer::onHandleMsg(int sockfd) {
  std::cout << "onHandleMsg socket=" << sockfd << std::endl;
  BOOST_ASSERT(sockfd != m_listenfd);
  if (sockfd == m_listenfd) {
    int newSockFd = handleAccept(m_epollfd, sockfd);
    if (0 > newSockFd) {
      return;
    }
    ChannelPtr newChannel = boost::make_shared<Channel>(m_epollfd, newSockFd);
    newChannel->setReadEvent();
    newChannel->setWriteEvent();
    newChannel->setRecvCallBack(boost::bind(&TcpServer::handleWrite, shared_from_this(), newSockFd ));
    newChannel->setSendCallBack(boost::bind(&TcpServer::handleRead, shared_from_this(), newSockFd ));
  } else {
    std::cout << "error : " << std::endl;
  }
}

void TcpServer::start()
{
  m_epollfd = epoll_create(1);
  if (m_epollfd< 0)
  {
      cout << "epoll_create error, error:" << m_epollfd << endl;
      return;
  }
  m_listenfd = createAndListen();
  ChannelPtr listenChannel = boost::make_shared<Channel>(m_epollfd, m_listenfd);
  BOOST_ASSERT(listenChannel);

  listenChannel->setReadEvent();
  listenChannel->setRecvCallBack(boost::bind(&TcpServer::onHandleMsg, shared_from_this(), m_listenfd));

  for(;;)
  {
      int fds = epoll_wait(m_epollfd, m_events, MAX_EVENTS, -1);
      if(fds == -1)
      {
          cout << "epoll_wait error, errno:" << errno << endl;
          break;
      }
      vector<ChannelPtr> channels;
      for(int i = 0; i < fds; i++)
      {
          ChannelPtr newChannel = boost::make_shared<Channel>(m_epollfd, m_events[i].data.fd);
          newChannel->setRevents(m_events[i].events);
          channels.push_back(newChannel);
      }
      std::vector<ChannelPtr>::iterator iter = channels.begin();
      for(; iter != channels.end(); ++iter) {
        (*iter)->handleEvent();
      }
  }
  return;
}


void TcpServer::handleRead(int fd)
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

void TcpServer::handleWrite(int fd)
{
  char buf[MAX_LINE] = "hello ,world!";
  int sendLen = sizeof buf;
  if(write(fd, buf, sendLen) != sendLen)
            cout << "error: not finished one time" << endl;

}


int TcpServer::handleAccept(int efd, int fd)
{
  struct sockaddr_in cliaddr;
  socklen_t clilen = sizeof(cliaddr);
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
      return -1;
  }
  fcntl(connfd, F_SETFL, O_NONBLOCK); //no-block io
  return connfd;
}

