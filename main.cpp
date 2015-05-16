#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include "server.hpp"



using namespace std;
using namespace boost::asio;
using namespace boost;


int main()
{   try
    {
    io_service io_service_;
    ip::tcp::endpoint endpoint_(ip::tcp::v4(),8888);
    server ser(io_service_,endpoint_);
    }
    
    catch(std::exception& e)
    {
        e.what();
    }
    return 0;
    
}