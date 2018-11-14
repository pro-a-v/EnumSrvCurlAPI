#include "IOServer.h"


IOServer::IOServer(boost::asio::io_service& io_service, short port)
    : socket_(io_service, udp::endpoint(udp::v4(), port))
{
    done.store(false, boost::memory_order_release);

    if (!income_queue.is_lock_free())
        {
            std::cout << "lockfree is not supported on this hardware " << std::endl;
            return;
        }

    opl = new OperatorsList();


    // worker threads
    unsigned concurentThreadsSupported = std::thread::hardware_concurrency();
    if (concurentThreadsSupported == 0)
        concurentThreadsSupported = 1;

    for (unsigned i=0;i<concurentThreadsSupported;i++)
    {
        boost::thread worker(&IOServer::RequestConsumerWorker, this);
    }



    socket_.async_receive_from( boost::asio::buffer(data_, max_length), sender_endpoint_, [this](boost::system::error_code ec, std::size_t bytes_recvd)
        {
          if (!ec && bytes_recvd > 0)
          {
             Request *request = new Request(data_,bytes_recvd, &socket_ , sender_endpoint_);
             income_queue.push(request);
          }
            do_receive();

    });
}

IOServer::~IOServer()
{
   done.store(true, boost::memory_order_release);
   delete opl;
}

void IOServer::do_receive()
{
  socket_.async_receive_from( boost::asio::buffer(data_, max_length), sender_endpoint_, [this](boost::system::error_code ec, std::size_t bytes_recvd)
      {
        if (!ec && bytes_recvd > 0)
        {
          Request *request = new Request(data_,bytes_recvd, &socket_ , sender_endpoint_);
          income_queue.push(request);
        }
          do_receive();

      });
}


std::string IOServer::currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
    return buf;
}

void IOServer::RequestConsumerWorker()
{
    while(! done.load(boost::memory_order_relaxed))
    {
        Request *req;
        if (income_queue.pop(req))
        {
            // обрабатываем запрос

            DnsMessage NS(req->raw_data,req->raw_data_len);
            // формируем ответ
            if (NS.Error==true)
            {
                boost::system::error_code ignored_ec;
                req->socket_->send_to(boost::asio::buffer(NS.AnswerError()), req->sender_endpoint_, 0, ignored_ec);
            }
            else
            {
                std::string phone = std::to_string(NS.GetRequestedNumber());
                std::cout << phone << std::endl;
                boost::system::error_code ignored_ec;
                if (
                        (phone.size() != 11) ||
                        (phone.substr(0,3) != std::string("372"))
                    )
                {
                    req->socket_->send_to(boost::asio::buffer(NS.AnswerError()), req->sender_endpoint_, 0, ignored_ec);
                }
                else
                {
                    // http://numbribaas.elisa.ee/rest/api/operator/81941907
                    const std::string host = std::string("numbribaas.elisa.ee");
                    const std::string target = std::string("/rest/api/operator/") + phone.substr(3);
                    try
                       {
                           // The io_context is required for all I/O
                           boost::asio::io_context ioc;

                           // These objects perform our I/O
                           boost::asio::ip::tcp::resolver resolver{ioc};
                           boost::asio::ip::tcp::socket socket{ioc};

                           // Look up the domain name
                           tcp::resolver::query query(host, "http");
                           auto const results = resolver.resolve(query);

                           // Make the connection on the IP address we get from a lookup
                           boost::asio::connect(socket, results.begin(), results.end());

                           // Set up an HTTP GET request message
                           boost::beast::http::request<boost::beast::http::string_body> http_request{boost::beast::http::verb::get, target, 10};
                           http_request.set(boost::beast::http::field::host, host);
                           http_request.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);

                           // Send the HTTP request to the remote host
                           boost::beast::http::write(socket, http_request);

                           // This buffer is used for reading and must be persisted
                           boost::beast::flat_buffer buffer;

                           // Declare a container to hold the response
                           boost::beast::http::response<boost::beast::http::dynamic_body> res;

                           // Receive the HTTP response
                           boost::beast::http::read(socket, buffer, res);

                           // Write the body xml message to string
                           std::string xml_output= boost::beast::buffers_to_string(res.body().data());
                           std::string::size_type pos = 0;
                           while ((pos = xml_output.find("\n", pos)) != std::string::npos)  xml_output.replace(pos, 1, " ");
                           while ((pos = xml_output.find("\t", pos)) != std::string::npos)  xml_output.replace(pos, 1, " ");
                           while ((pos = xml_output.find("\r", pos)) != std::string::npos)  xml_output.replace(pos, 1, " ");

                           xml_output = xml_output.substr(xml_output.find("<name>")+6);
                           xml_output = xml_output.substr(0,xml_output.find("</name>"));


                           std::string return_data = opl->GetOperatorMnp(xml_output);
                           if (return_data.size()>4)
                               req->socket_->send_to(boost::asio::buffer(NS.Answer(return_data.substr(0,3),return_data.substr(3))), req->sender_endpoint_, 0, ignored_ec);
                           else
                               req->socket_->send_to(boost::asio::buffer(NS.AnswerError()), req->sender_endpoint_, 0, ignored_ec);

                           // Gracefully close the socket
                           boost::system::error_code ec;
                           socket.shutdown(tcp::socket::shutdown_both, ec);

                           // not_connected happens sometimes
                           // so don't bother reporting it.
                           //
                           if(ec && ec != boost::system::errc::not_connected)
                               throw boost::system::system_error{ec};


                           // If we get here then the connection is closed gracefully
                       }
                       catch(std::exception const& e)
                       {
                           std::cerr << "Error: " << e.what() << std::endl;
                           req->socket_->send_to(boost::asio::buffer(NS.AnswerError()), req->sender_endpoint_, 0, ignored_ec);
                       }

                }

            }

        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

    }
}


