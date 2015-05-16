#ifndef SERVER_UTIL
#define SERVER_UTIL

namespace  server_util{
    

//Class that stored the package header in bytes
class bytes
{
public:
    unsigned char bytes[4];
};
//---------------------------------------------------------
//Class that convert byte to int and convert int to byte
//Declaration of  byte_util
class byte_util
{
public:
    static int bytes2int(unsigned char buf[4]);
    template<class Type>static Type int2bytes(Type t,int n);
};
//----------------------------------------------------------
//Implemation of byte_util
int byte_util::bytes2int(unsigned char buf[4])
{
    int num=0;
    for (int i=0; i<4; i++) {
        num<<=8;
        num|=(buf[i]&0xff);
        }
        return num;
}
    
template<class Type> Type byte_util::int2bytes(Type t,int n)
{
    t.bytes[0] = (n >> 24) & 0xFF;
    t.bytes[1] = (n >> 16) & 0xFF;
    t.bytes[2] = (n >> 8) & 0xFF;
    t.bytes[3] = n & 0xFF;
    return t;
    }
//----------------------------------------------------------


}//end server_util namespace
#endif