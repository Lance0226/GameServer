#ifndef SERVER
#define SERVER

#include <stdio.h>
#include <vector>
#include "player_table.hpp"

#include <boost/asio.hpp>
#include <boost/any.hpp>
#include <boost/bind.hpp>
#include <string>

#include "test.pb.hpp"
#include "google/protobuf/io/coded_stream.h"
#include "google/protobuf/io/zero_copy_stream_impl.h"
#include <google/protobuf/dynamic_message.h>

#include "server_util.hpp"
#include "tcp_connection.hpp"

using namespace boost::asio;
using namespace boost;




//-----------------------------------------------------------------------
//Declare class server
class server
{
public:
    server(io_service& ioser,ip::tcp::endpoint& ep)
      :acceptor_(ioser,ep),
       socket_(ioser)
    {
        start_accept();
    }
    
    void                 start_accept();
    
    void                 handle_accept(tcp_connection::pointer new_connection,const boost::system::error_code& error)
    {
        if (!error)
        {
            new_connection->start();
        }
        
        start_accept();
    }
        
private:
    ip::tcp::acceptor acceptor_;
    ip::tcp::socket socket_;
    string body;
    system::error_code ec;
};
//-----------------------------------------------------------------------
//Implement class server

void server::start_accept()
{
    tcp_connection::pointer new_connection =
    tcp_connection::create(acceptor_.get_io_service());
    
    acceptor_.async_accept(new_connection->socket(),
                           boost::bind(&server::handle_accept, this, new_connection,
                                       boost::asio::placeholders::error));
}


//----------------------------------------------------------------------------


#endif