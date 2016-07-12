/*
TcpConnection的作用很关键，在当前版本上只是用来简单处理socket的读写事件，后续会将TcpConnection传递给库的用户，将读写权利转交给用户

*/

#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h> //for bzero
#include <iostream>
#include <sys/types.h>
#include <unistd.h>


#include "Channel.h"

#include <boost/make_shared.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "Define.h"


class TcpConnection:
  public boost::enable_shared_from_this<TcpConnection>
{
public:
  typedef boost::shared_ptr<Channel>  ChannelPtr;


public:
  TcpConnection(int epollfd);
  ~TcpConnection();

  void newConnection(int sockfd);


private:
  void handleRead(int fd);
  void handleWrite(int fd);

private:
  int m_epollfd;

};

#endif