#include "MsgProcesssor.hpp"



MsgProcesssor::MsgProcesssor(boost::lockfree::queue<Request *, boost::lockfree::capacity<1000>> *income_queue, boost::atomic<bool> *done):
    income_queue_(income_queue), done_(done)
{

}

std::string MsgProcesssor::currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
    return buf;
}
                                


void MsgProcesssor::Run()
{
    boost::system::error_code ignored_ec;
    std::string MyAnswer;

    while(! done_->load(boost::memory_order_release))
    {
        // Request *req = this->income_queue_->pop();
         //std::cout << req->raw_data_len << std::endl;
        ;
    }
    // обрабатываем запрос

    /*DnsMessage NS(rcv_data_,rcv_data_len_);
    // формируем ответ
    if (NS.Error==true)
    {
        socket_->send_to(boost::asio::buffer(NS.AnswerError()), sender_endpoint_, 0, ignored_ec);
    }
    else
    {
        std::string phone = std::to_string(NS.GetRequestedNumber());



    }
    */

}
