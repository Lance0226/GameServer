#ifndef TCP_CONNECTION
#define TCP_CONNECTION

#include<boost/enable_shared_from_this.hpp>
#include<boost/asio.hpp>
#include"server.hpp"

using namespace boost::asio;
using namespace boost;
using boost::asio::ip::tcp;
using namespace google::protobuf;
using namespace Protocol;
using namespace server_util;

class tcp_connection
: public boost::enable_shared_from_this<tcp_connection>
{
public:
    typedef boost::shared_ptr<tcp_connection> pointer;
    
    static pointer create(boost::asio::io_service& io_service);
    
    tcp::socket& socket()
    {
        return socket_;
    }
    
    template<class Type>
    void                 do_send(Type t);
    
    int                  get_uid();
    
    void                 decode_uid(int uid,int num);
    
    int                  decode_header();
    
    template<class Type>
    Type*                decode_body(Type* t,int length);
    
    Message*             create_message(const std::string& type_name);
    
    player_info*         player_info_channel(google::protobuf::Message* msg);
    
    player_transform*    player_transform_channel(google::protobuf::Message* msg);
    
    player_info          get_player(int index);

    
    void start();
private:
    tcp_connection(boost::asio::io_service& io_service):socket_(io_service){}
    
    void handle_accept(tcp_connection::pointer new_connection,const boost::system::error_code& error);
    
    tcp::socket socket_;
    
    
};

void tcp_connection::start()
{
    cout<<"aa"<<endl;
    int uid=get_uid();
    int num=decode_header();
    decode_uid(uid, num);
    for(int i=0;i<3;i++)
    {player_info cur_person=get_player(i);
        do_send(cur_person);
    }
}

 tcp_connection::pointer tcp_connection::create(boost::asio::io_service& io_service)
{
    return pointer(new tcp_connection(io_service));
}

template<class Type>void tcp_connection::do_send(Type t)
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

int tcp_connection::get_uid()
{
    unsigned char code_buf[4];
    socket_.read_some(buffer(code_buf));
    int code_num=byte_util::bytes2int(code_buf);
    return code_num;
}

void tcp_connection::decode_uid(int uid,int num)
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

int tcp_connection::decode_header()
{
    unsigned char buf[4];
    socket_.read_some(buffer(buf));
    int num=byte_util::bytes2int(buf);
    return num;
}

template<class Type>Type* tcp_connection::decode_body(Type* t,int length)
{
    unsigned char buf_[length];
    boost::asio::read(socket_, buffer(buf_,length));
    ::google::protobuf::io::ArrayInputStream input(buf_,length);
    ::google::protobuf::io::CodedInputStream coded_input_stream(&input);
    t->ParseFromCodedStream(&coded_input_stream);
    return t;
}


Message* tcp_connection::create_message(const std::string& type_name)
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

player_info* tcp_connection::player_info_channel(google::protobuf::Message* msg)
{
    player_info* p=dynamic_cast<player_info*>(msg);
    return p;
    
}

player_transform*   tcp_connection::player_transform_channel(google::protobuf::Message* msg)
{
    player_transform* p=dynamic_cast<player_transform*>(msg);
    return p;
}



player_info tcp_connection::get_player(int index)
{
    database db;
    db.init();
    player_table pt=db.get_player_table();
    player_info person=pt.get_player(index);
    return person;
}


#endif
