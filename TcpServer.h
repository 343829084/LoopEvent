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
#include <iostream>
#include "Channel.h"
#include "Define.h"




class TcpServer :
  public boost::enable_shared_from_this<TcpServer>
{

public:
  typedef boost::shared_ptr<Channel>  ChannelPtr;


public:
  TcpServer();
  ~TcpServer();

  void start();
  void handleRead(int fd);
  void handleWrite(int fd);
  int handleAccept(int efd, int fd);

private:
  int createAndListen();
  void onHandleMsg(int sockfd);


private:
  int m_epollfd;
  int m_listenfd;
  struct epoll_event m_events[MAX_EVENTS];
};

#endif