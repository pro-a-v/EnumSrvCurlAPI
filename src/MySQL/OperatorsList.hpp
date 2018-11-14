#ifndef OPERATORSLIST_HPP
#define OPERATORSLIST_HPP



#include <mysql/mysql.h>

#include <iostream>
#include <string.h>

#include <vector>
#include<algorithm> // for copy() and assign()
#include<iterator> // for back_inserter
#include <chrono>
#include <thread>
#include <mutex>

class OperatorsList
{
public:
    OperatorsList();
    ~OperatorsList();
    std::string GetOperatorMnp(std::string OpName);
private:
    void LoadData();
    void UpdateData();
    void UpdaterWorker();


    std::string FileName;


    struct op_data
    {
        std::string OperatorName;
        std::string OperatorMccMnc;
    } ;

    std::vector<op_data> db_data;
    std::vector<op_data> working_data;
    std::mutex data_mutex;
    std::vector<op_data>::iterator data_it;

    bool is_working;
    std::thread UpdaterWorkerThread;

};

#endif // OPERATORSLIST_HPP
