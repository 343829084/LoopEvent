/*
这个Acceptor主要是处理新连接,一个Acceptor里的Channel关注着用于listen的socket，一旦有连接到达，就将连接accept下来
新socket建立后就和之前用于listen的socket没有任何关系
*/
#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h> //for bzero
#include <sys/types.h>
#include <unistd.h>

#include <boost/make_shared.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>

#include <iostream>

#include "Channel.h"
#include "Define.h"


class Acceptor:
  public boost::enable_shared_from_this<Acceptor>
{
public:
  typedef boost::function<
    void (int newSockfd)> NewConnectFunc;

    typedef boost::shared_ptr<Channel>  ChannelPtr;

public:
  Acceptor(int epollfd);
  ~Acceptor();

  void createAndListen();
  void setConnectCallBack(NewConnectFunc newConnectFun);
  void start();

private:
  void handleAccept(int efd);

private:
  int m_epollfd;
  int m_listenfd;

  NewConnectFunc m_newConnectCallBack;
  ChannelPtr m_channel;

};

#endif