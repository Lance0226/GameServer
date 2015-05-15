#ifndef SERVER
#define SERVER

#include <stdio.h>
#include <vector>
#include "PlayerTable.hpp"
#include <map>

#include <boost/asio.hpp>
#include <boost/any.hpp>
#include <boost/assign.hpp>
#include <string>

#include "test.pb.h"
#include "google/protobuf/io/coded_stream.h"
#include "google/protobuf/io/zero_copy_stream_impl.h"

using namespace boost::asio;
using namespace boost;
using namespace boost::assign;



class Byte
{
public:
    unsigned char bytes[4];
};



class decode_map
{
    
    
public:
    decode_map()
    {   tutorial::Person person;
        Byte byte;
        boost::any a=person;
        boost::any b=byte;
        m.insert(pair<int, boost::any>(1,a));
        m.insert(pair<int, boost::any>(2,b));
    }
    static tutorial::Person person;
    static Byte byte;
    map<int,boost::any> m;
};


class server
{
public:
    server(io_service& ioser,ip::tcp::endpoint& ep)
    :io_service_(ioser)
    ,acceptor_(ioser,ep)
    ,socket_(ioser)
    {
        io_service_.run();
        acceptor_.accept(socket_);
        while (do_accept()) {
            
        }
    }
    
    
    int do_accept()
    {
        if(socket_.available()<4)
        {
            return 1;
        }
        else
        {
            int uid=get_uid();
            int num=decode_header();
            tutorial::Person t;
            t=decode_body(t,num);
            for(int i=0;i<3;i++)
            {tutorial::Person curPerson=GetPlayer(i);
            do_send(curPerson);
            }
            
            return 1;
        }
    }
    
    
    
    int get_uid()
    {
        unsigned char code_buf[4];
        socket_.read_some(buffer(code_buf));
        int code_num=BytesToInt(code_buf);
        return code_num;
    }
    
    boost::any decode_uid(int uid)
    {
        tutorial::Person person;
        Byte byte;
        
        switch (uid) {
            case 1:return boost::any(person);break;
            default:return boost::any(byte);break;
        }
    }
    
    
    
    int decode_header()
    {
        unsigned char buf[4];
        socket_.read_some(buffer(buf));
        int num=BytesToInt(buf);
        return num;
    }
    
     template <class Type>
     Type decode_body(Type t,int length)
    {
        unsigned char buf_[length];
        boost::asio::read(socket_, buffer(buf_,length));
        ::google::protobuf::io::ArrayInputStream input(buf_,length);
        ::google::protobuf::io::CodedInputStream coded_input_stream(&input);
        t.ParseFromCodedStream(&coded_input_stream);
        return t;
    }
    
    int BytesToInt(unsigned char buf[4])
    {
        int num=0;
        for (int i=0; i<4; i++) {
            num<<=8;
            num|=(buf[i]&0xff);
        }
        return num;
    }
    
    template<class Type>
    Type IntToBytes(Type t,int n)
    {
        t.bytes[0] = (n >> 24) & 0xFF;
        t.bytes[1] = (n >> 16) & 0xFF;
        t.bytes[2] = (n >> 8) & 0xFF;
        t.bytes[3] = n & 0xFF;
        return t;
    }
    
    
    void read_header(const boost::system::error_code& error)
    {
    }
    
    template<class Type>
    void do_send(Type t)
    {
        boost::asio::streambuf stream_buffer;
        std::ostream os(&stream_buffer);
        t.SerializeToOstream(&os);
        int num=t.ByteSize();
        Byte byte;
        byte=IntToBytes(byte,num);
        socket_.send(buffer(byte.bytes));
        socket_.send(stream_buffer.data());
    }
    
    tutorial::Person GetPlayer(int index)
    {
        Database db;
        db.init();
        PlayerTable pt=db.GetPlayerTable();
        tutorial::Person person=pt.GetPlayer(index);
        return person;
    }
    
private:
    ip::tcp::acceptor acceptor_;
    io_service& io_service_;
    ip::tcp::socket socket_;
    string body;
    system::error_code ec;
};
#endif