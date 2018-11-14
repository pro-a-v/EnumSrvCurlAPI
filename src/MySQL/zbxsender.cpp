#include "zbxsender.hpp"

ZbxSender::ZbxSender()
{
    ready = true;
    std::thread worker(&ZbxSender::run, this);
    worker.detach();
}

void ZbxSender::setServer(std::string ZabbixHost_, int ZabbixPort_)
{
    ZabbixHost = ZabbixHost_;
    ZabbixPort = ZabbixPort_;
}

void ZbxSender::setHostKey(std::string MonitoringHost_, std::string MonitoringKey_)
{
    MonitoringHost = MonitoringHost_;
    MonitoringKey = MonitoringKey_;
}

void ZbxSender::send(std::string data)
{
    queue.push(data);
}

void ZbxSender::run()
{
    while(ready)
    {
        if (!queue.empty())
        {
            std::string data = queue.front();
            queue.pop();
            tcp_send(data);
        }

        if (queue.size()>100)
        {
            queue.empty();
        }
        std::this_thread::sleep_for (std::chrono::seconds(5));
    }
}

ZbxSender::~ZbxSender()
{
    ready = false;
}




namespace asio = boost::asio;

bool ZbxSender::tcp_send(std::string value) {
  asio::io_service io_service;
  asio::ip::tcp::socket socket(io_service);

  socket.connect( asio::ip::tcp::endpoint( asio::ip::address::from_string(ZabbixHost), ZabbixPort ) );

  char msg[1024];
  memset(msg, 0x00, 1024);
  strcpy(msg, "ZBXD");
  msg[4] = 0x01;

  std::string DATA = std::string("{ \"request\":\"sender data\", \"data\":[ {\"host\":\"")
          + MonitoringHost
          + std::string("\",\"key\":\"")
          + MonitoringKey
          + std::string("\",\"value\":\"")
          + value.substr(0,900)
          + std::string("\" } ]}");

  uint8_t data_len=DATA.length();
  memcpy(msg+5,&data_len,sizeof(uint8_t));
  memcpy(msg+13,DATA.c_str(),DATA.length());

  boost::system::error_code error;
  asio::write( socket, asio::buffer(msg, data_len+13), error );

  if( error ) {
    TextError =  "send failed: " + error.message();
  }

  asio::streambuf receive_buffer;
  asio::read( socket, receive_buffer, asio::transfer_all(), error );

  if( error && error != asio::error::eof ) {
    std::cout << "receive failed: " << error.message() << std::endl;
  }
  else {
    const char* data = asio::buffer_cast<const char*>(receive_buffer.data());
    // TODO проанализировать
    //std::cout << data << std::endl;
  }

  return 0;
}
