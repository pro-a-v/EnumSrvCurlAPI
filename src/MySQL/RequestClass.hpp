#ifndef REQUESTDATACLASS_HPP
#define REQUESTDATACLASS_HPP

#include <boost/asio.hpp>
#include <string.h>

class Request
{
public:
    Request(char *data, int len, boost::asio::ip::udp::socket *socket,  boost::asio::ip::udp::endpoint sender_endpoint);
    ~Request();
    char *raw_data;
    int raw_data_len;
    boost::asio::ip::udp::socket *socket_;
    boost::asio::ip::udp::endpoint sender_endpoint_;
};


#endif // REQUESTDATACLASS_HPP
