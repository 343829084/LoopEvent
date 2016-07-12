#include "Acceptor.h"


using namespace std;

Acceptor::Acceptor(int epollfd):
  m_epollfd(epollfd),
  m_listenfd(-1){
}

Acceptor::~Acceptor()
{
}

void Acceptor::start()
{
  createAndListen();
  m_channel = boost::make_shared<Channel>(m_epollfd, m_listenfd);
  m_channel->setReadEvent();
  m_channel->setRecvCallBack(boost::bind(&Acceptor::handleAccept, shared_from_this(), -1));
}

void Acceptor::createAndListen()
{
    int on = 1;
    m_listenfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in servaddr;
    fcntl(m_listenfd, F_SETFL, O_NONBLOCK); //no-block io
    setsockopt(m_listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(11111);

    if(-1 == bind(m_listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)))
    {
        cout << "bind error, errno:" << errno << endl;
    }

    if(-1 == listen(m_listenfd, MAX_LISTENFD))
    {
        cout << "listen error, errno:" << errno << endl;
    }
}

void Acceptor::setConnectCallBack(NewConnectFunc newConnectFun)
{
  m_newConnectCallBack = newConnectFun;
}


void Acceptor::handleAccept(int efd)
{
  struct sockaddr_in cliaddr;
  socklen_t clilen = sizeof(cliaddr);
  int connfd = accept(m_listenfd, (sockaddr*)&cliaddr, (socklen_t*)&clilen);
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
      return;
  }
  fcntl(connfd, F_SETFL, O_NONBLOCK); //no-block io
  m_newConnectCallBack(connfd);
}