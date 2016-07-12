#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h> //for bzero
#include <iostream>
#include <sys/types.h>
#include <unistd.h>



#include <boost/make_shared.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>

#include <iostream>
#include <map>

#include "Channel.h"
#include "Acceptor.h"
#include "TcpConnection.h"
#include "Define.h"


class TcpServer :
  public boost::enable_shared_from_this<TcpServer>
{

public:
  typedef boost::shared_ptr<Channel>  ChannelPtr;

  typedef boost::shared_ptr<Acceptor> AcceptorPtr;

  typedef boost::shared_ptr<TcpConnection> TcpConnectionPtr;

public:
  TcpServer();
  ~TcpServer();

  void start();

private:
  void newConnection(int sockfd);


private:
  int m_epollfd;
  int m_listenfd;

  AcceptorPtr m_acceptor;
  std::map<int, TcpConnectionPtr> m_tcpConnections;
  struct epoll_event m_events[MAX_EVENTS];
};

#endif