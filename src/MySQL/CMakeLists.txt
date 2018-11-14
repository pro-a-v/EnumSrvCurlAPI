project(EnumSrvCurlAPI)
cmake_minimum_required(VERSION 3.11)

execute_process(COMMAND mysql_config --cflags OUTPUT_VARIABLE MYSQL_CFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND mysql_config --libs OUTPUT_VARIABLE MYSQL_LIBS OUTPUT_STRIP_TRAILING_WHITESPACE)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MYSQL_CFLAGS} -std=c++11 -Wno-narrowing -fpermissive -L/usr/lib64/mysql")

find_package( Boost COMPONENTS system thread filesystem regex system signals date_time REQUIRED )
link_directories( ${Boost_LIBRARY_DIRS} )
include_directories( ${Boost_INCLUDE_DIRS} )
include_directories( ./ )
include_directories( /usr/local/include )


set(SOURCE_FILES
        main.cpp
        RequestClass.cpp
        DnsMessage.cpp
        zbxsender.cpp
        IOServer.cpp
        OperatorsList.cpp
     )



add_executable(EnumSrvCurlAPI ${SOURCE_FILES})
#target_link_libraries(EnumSrvCurlAPI ${Boost_LIBRARIES} ${OpenSSL_LIBRARIES} ${CPPNETLIB_LIBRARIES} pthread m dl)
target_link_libraries(EnumSrvCurlAPI ${Boost_LIBRARIES} ${MYSQL_LIBS} pthread m dl)
