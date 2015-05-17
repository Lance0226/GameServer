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

    
    
    int                  do_accept();
    
    void                 decode_uid(int uid,int num);
    
    Message*             create_message(const std::string& type_name);
    
    player_info*         player_info_channel(google::protobuf::Message* msg);
    player_transform*    player_transform_channel(google::protobuf::Message* msg);
    
    int                  get_uid();
    
    int                  decode_header();
    
    template<class Type>
    Type*  decode_body(Type* t,int length);
    
    template<class Type>
    void     do_send(Type t);
    
    player_info   get_player(int index);
    
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
        {player_info cur_person=get_player(i);
            do_send(cur_person);
        }
        
        return 1;
    }
}

void server::decode_uid(int uid,int num)
{   string type_name;
    google::protobuf::Message* msg_ptr;
    if(uid==1)
    {
        type_name="Protocol.player_info";
        msg_ptr=create_message(type_name);
        player_info* p=player_info_channel(msg_ptr);
        player_info person;
        p=&person;
        p=decode_body(p,num);
        cout<<p->email()<<endl;
    }
    if(uid==2)
    {
        type_name="Protocol.player_transform";
        msg_ptr=create_message(type_name);
        player_transform* p=player_transform_channel(msg_ptr);
        player_transform transform;
        p=&transform;
        p=decode_body(p,num);
        cout<<p->rot_z()<<endl;

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

player_info* server::player_info_channel(google::protobuf::Message* msg)
{
    player_info* p=dynamic_cast<player_info*>(msg);
    return p;
    
}

player_transform*   server::player_transform_channel(google::protobuf::Message* msg)
{
    player_transform* p=dynamic_cast<player_transform*>(msg);
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
template<class Type>
Type* server::decode_body(Type* t,int length)
{
    unsigned char buf_[length];
    boost::asio::read(socket_, buffer(buf_,length));
    ::google::protobuf::io::ArrayInputStream input(buf_,length);
    ::google::protobuf::io::CodedInputStream coded_input_stream(&input);
    t->ParseFromCodedStream(&coded_input_stream);
    return t;
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

player_info server::get_player(int index)
{
    database db;
    db.init();
    player_table pt=db.get_player_table();
    player_info person=pt.get_player(index);
    return person;
}
//----------------------------------------------------------------------------


#endif