# include "RequestClass.hpp"

Request::Request(char *data, int len, boost::asio::ip::udp::socket *socket,  boost::asio::ip::udp::endpoint sender_endpoint): socket_(socket), sender_endpoint_(sender_endpoint)
{
    raw_data = new char [len+1];
    memcpy(raw_data, data, len);
    raw_data_len = len;
}

Request::~Request()
{
     delete []raw_data;
}
