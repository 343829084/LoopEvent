/*
这个channel主要是一个socket的封装
每个Channel对象自始至终只负责一个文件描述符的IO事件分发
events和_revents，前者是要关注的事件，后者是发生的事件
*/
#ifndef CHANNEL_H
#define CHANNEL_H

#include <boost/function.hpp>
#include <sys/epoll.h>


class Channel
{

public:
  // 响应消息的回调函数
  typedef boost::function<
    void (int sockfd)>  OnInFunc;



public:
  Channel(int epollfd, int sockfd);
  ~Channel();

  void setRecvCallBack(OnInFunc recvMsgFun);
  void setSendCallBack(OnInFunc sendMsgFun);

  void handleEvent();
  void setRevents(int revent); // 发生的事件

  int getSocketfd();

  void setReadEvent();
  void setWriteEvent();

private:
  int m_epollfd;
  int m_sockfd;
  int m_event;
  int m_revent;
  OnInFunc m_recvMsgCallBack;
  OnInFunc m_sendMsgCallBack;

};

#endif