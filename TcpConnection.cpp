#include "TcpConnection.h"
#include <iostream>
#include <boost/bind.hpp>


using namespace std;


TcpConnection::TcpConnection(int epollfd):m_epollfd(epollfd)
{

}

TcpConnection::~TcpConnection()
{

}

void TcpConnection::newConnection(int sockfd)
{
  ChannelPtr channel = boost::make_shared<Channel>(m_epollfd, sockfd);
  channel->setReadEvent();
  channel->setWriteEvent();
  channel->setRecvCallBack(boost::bind(&TcpConnection::handleRead, shared_from_this(), sockfd));
  channel->setSendCallBack(boost::bind(&TcpConnection::handleWrite, shared_from_this(), sockfd));
}



void TcpConnection::handleRead(int fd)
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

void TcpConnection::handleWrite(int fd)
{
  char buf[MAX_LINE] = "hello ,world!";
  int sendLen = sizeof buf;
  if(write(fd, buf, sendLen) != sendLen)
            cout << "error: not finished one time" << endl;

}