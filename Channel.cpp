#include "Channel.h"

Channel::Channel(int epollfd, int sockfd):m_epollfd(epollfd), m_sockfd(sockfd),
  m_event(0), m_revent(0), m_recvMsgCallBack(NULL), m_sendMsgCallBack(NULL)
{
}

Channel::~Channel()
{
}

void Channel::setRecvCallBack(OnInFunc recvMsgFun) {
  m_recvMsgCallBack = recvMsgFun;
}

void Channel::setSendCallBack(OnInFunc sendMsgFun) {
  m_sendMsgCallBack = sendMsgFun;
}

void Channel::handleEvent(){
  if (m_revent && EPOLLIN) {
    m_recvMsgCallBack(m_sockfd);
  } else if(m_revent && EPOLLOUT) {
    m_sendMsgCallBack(m_sockfd);
  }
  return;
}

void Channel::setRevents(int revent) {
  m_revent = revent;
}

int Channel::getSocketfd(){
  return m_sockfd;
}

void Channel::setReadEvent(){
  m_event |= EPOLLIN;
  struct epoll_event ev;
  ev.data.ptr = this;
  ev.events = m_event;
  epoll_ctl(m_epollfd, EPOLL_CTL_ADD, m_sockfd, &ev);
}

void Channel::setWriteEvent(){
  m_event |= EPOLLOUT;
  struct epoll_event ev;
  ev.data.ptr = this;
  ev.events = m_event;
  epoll_ctl(m_epollfd, EPOLL_CTL_ADD, m_sockfd, &ev);
}