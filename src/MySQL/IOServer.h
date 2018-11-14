#ifndef IOSERVER_H
#define IOSERVER_H


#include <cstdlib>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/lockfree/queue.hpp>
#include <boost/atomic.hpp>
#include <memory>

#include "udpmessage.hpp"
#include "RequestClass.hpp"
#include "MsgProcesssor.hpp"
#include "HttpSyncClient.hpp"

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>


#include "pugiconfig.hpp"
#include "pugixml.hpp"

#include "OperatorsList.hpp"

#define UDP_MSG_SIZE 512

using boost::asio::ip::udp;

class IOServer
{
public:
  IOServer(boost::asio::io_service& io_service, short port);
  ~IOServer();
  void do_receive();


private:
  udp::socket socket_;
  udp::endpoint sender_endpoint_;
  enum { max_length = UDP_MSG_SIZE };
  char data_[UDP_MSG_SIZE];

  std::string currentDateTime();
  boost::lockfree::queue<Request *, boost::lockfree::fixed_sized<true> , boost::lockfree::capacity<64000>> income_queue;
  //boost::lockfree::queue<Request *, boost::lockfree::capacity<1000>> income_queue;

  boost::atomic<bool> done;

  void RequestConsumerWorker();

  OperatorsList *opl;


};

#endif // IOSERVER_H
