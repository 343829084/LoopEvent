#include "TcpServer.h"
#include <boost/bind.hpp>
#include <vector>
#include <iostream>

using namespace std;

TcpServer::TcpServer()
    :m_epollfd(-1)
    ,m_listenfd(-1)
{
}

TcpServer::~TcpServer()
{
}


void TcpServer::newConnection(int sockfd) {
  TcpConnectionPtr newConnection = boost::make_shared<TcpConnection>(sockfd);
  m_tcpConnections[sockfd] = newConnection;

}

void TcpServer::start()
{
  m_epollfd = epoll_create(1);
  if (m_epollfd< 0)
  {
      cout << "epoll_create error, error:" << m_epollfd << endl;
      return;
  }

  m_acceptor = boost::make_shared<Acceptor>(m_epollfd);
  m_acceptor->setConnectCallBack(boost::bind(&TcpServer::newConnection, shared_from_this(), _1));
  m_acceptor->start();

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




