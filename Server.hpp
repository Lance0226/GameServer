#ifndef SERVER
#define SERVER

#include <stdio.h>
#include <vector>
#include "player_table.hpp"
#include <map>

#include <boost/asio.hpp>
#include <boost/any.hpp>
#include <boost/assign.hpp>
#include <string>

#include "test.pb.hpp"
#include "google/protobuf/io/coded_stream.h"
#include "google/protobuf/io/zero_copy_stream_impl.h"
#include <google/protobuf/dynamic_message.h>

#include "server_util.hpp"

using namespace boost::asio;
using namespace boost;
using namespace google::protobuf;
using namespace Protocol;
using namespace server_util;



//-----------------------------------------------------------------------
//Declare class server
class server
{
public:
    server(io_service& ioser,ip::tcp::endpoint& ep):io_service_(ioser),acceptor_(ioser,ep),socket_(ioser)
    {   io_service_.run();
        acceptor_.accept(socket_);
        while (do_accept()) {
            
        }
    }

    
    
    int      do_accept();
    
    void     decode_uid(int uid,int num);
    
    Message* create_message(const std::string& type_name);
    
    Person*  deal_with_person(google::protobuf::Message* msg);
    
    int      get_uid();
    
    int      decode_header();
    
    Person*  decode_body(Person* person,int length);
    
    template<class Type>
    void     do_send(Type t);
    
    Person   get_player(int index);
    
private:
    ip::tcp::acceptor acceptor_;
    io_service& io_service_;
    ip::tcp::socket socket_;
    string body;
    system::error_code ec;
};
//-----------------------------------------------------------------------
//Implement class server
int server::do_accept()
{
    if(socket_.available()<4)
    {
        return 1;
    }
    else
    {
        int uid=get_uid();
        int num=decode_header();
        decode_uid(uid, num);
        for(int i=0;i<3;i++)
        {Person cur_person=get_player(i);
            do_send(cur_person);
        }
        
        return 1;
    }
}

void server::decode_uid(int uid,int num)
{   string type_name;
    google::protobuf::Message* msg_ptr;
    if(uid==1)
    {type_name="Protocol.Person";
        msg_ptr=create_message(type_name);
        Person* p=deal_with_person(msg_ptr);
        Person person;
        p=&person;
        p=decode_body(p,num);
        cout<<p->name()<<endl;
    }
    else
    {
    }
}

Message* server::create_message(const std::string& type_name)
{
    google::protobuf::Message* message = NULL;
    const google::protobuf::Descriptor* descriptor = \
    google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(type_name);
    if(descriptor)
    {
        const google::protobuf::Message* prototype =\
        google::protobuf::MessageFactory::generated_factory()->GetPrototype(descriptor);
        if(prototype)
        {message =prototype->New();}
    }
    return message;
}

Person* server::deal_with_person(google::protobuf::Message* msg)
{
    Person* p=dynamic_cast<Person*>(msg);
    return p;
    
}

int server::get_uid()
{
    unsigned char code_buf[4];
    socket_.read_some(buffer(code_buf));
    int code_num=byte_util::bytes2int(code_buf);
    return code_num;
}

int server::decode_header()
{
    unsigned char buf[4];
    socket_.read_some(buffer(buf));
    int num=byte_util::bytes2int(buf);
    return num;
}

Person* server::decode_body(Person* person,int length)
{
    unsigned char buf_[length];
    boost::asio::read(socket_, buffer(buf_,length));
    ::google::protobuf::io::ArrayInputStream input(buf_,length);
    ::google::protobuf::io::CodedInputStream coded_input_stream(&input);
    person->ParseFromCodedStream(&coded_input_stream);
    return person;
}

template<class Type>void server::do_send(Type t)
{
    boost::asio::streambuf stream_buffer;
    std::ostream os(&stream_buffer);
    t.SerializeToOstream(&os);
    int num=t.ByteSize();
    bytes byte_;
    byte_=byte_util::int2bytes(byte_,num);
    socket_.send(buffer(byte_.bytes));
    socket_.send(stream_buffer.data());
}

Person server::get_player(int index)
{
    database db;
    db.init();
    player_table pt=db.get_player_table();
    Person person=pt.get_player(index);
    return person;
}
//----------------------------------------------------------------------------


#endif