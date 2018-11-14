#include "OperatorsList.hpp"

OperatorsList::OperatorsList()
{
   LoadData();
   is_working = true;
   UpdaterWorkerThread = std::thread(&OperatorsList::UpdaterWorker, this);
}

OperatorsList::~OperatorsList()
{
   is_working = false;
   UpdaterWorkerThread.join();
}

void OperatorsList::LoadData()
{
    MYSQL mysql;
    MYSQL_RES *result;

    if(mysql_init(&mysql) == nullptr)
    {
        std::cout << "mysql_init() failed " << std::endl;
        return;
    }

    if  (!mysql_real_connect(&mysql,"127.0.0.1","login","pass","db_name",0,nullptr,0))
    {
         std::cout <<  "Failed to connect to MySQL: Error: " <<   mysql_error(&mysql) << std::endl;
         return;
    }

    if(mysql_select_db(&mysql,"enum")==0)         ;
    else
    {
         std::cout <<  "Failed to connect to Database: Error: " <<   mysql_error(&mysql) << std::endl;
         return;
    }

    char query_def[1000];

    strcpy(query_def,"SELECT Name,MccMnc from l11p372operators ");
    if(mysql_query(&mysql,query_def))
    {
        std::cout <<  "Failed SELECT Name,MccMnc from l11p372operators : Error: " <<   mysql_error(&mysql) << std::endl;
        return;
    }

    result = mysql_use_result(&mysql);

    if (result)  // there are rows
    {
        MYSQL_ROW row;
        db_data.clear();
        while((row = mysql_fetch_row(result)))
        {
                op_data tmp_data;
                tmp_data.OperatorName = std::string(row[0]);
                tmp_data.OperatorMccMnc = std::string(row[1]);
                db_data.push_back(tmp_data);
        }
        mysql_free_result(result);
    }

    mysql_close(&mysql);

    // All seems ok - update data
    UpdateData();


}

void OperatorsList::UpdateData()
{
    std::lock_guard<std::mutex> guard(data_mutex);
        working_data.clear();
        copy(db_data.begin(), db_data.end(), back_inserter(working_data));
}

void OperatorsList::UpdaterWorker()
{

    int i = 0;

    while(is_working)
    {
        if (i < 60)
        {
           i++;
           std::chrono::milliseconds dura( 1000 );
           std::this_thread::sleep_for(dura);
        }
        else
        {
            i=0;
            LoadData();
        }

    }

}

std::string OperatorsList::GetOperatorMnp(std::string OpName)
{
        std::lock_guard<std::mutex> guard(data_mutex);

        for(data_it = working_data.begin(); data_it != working_data.end(); data_it++ )    {

            if( (*data_it).OperatorName == OpName)
            {
                return (*data_it).OperatorMccMnc;
            }
        }
        return std::string("");
}
