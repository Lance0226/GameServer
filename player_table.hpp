#ifndef PLAYER_TABLE
#define PLAYER_TABLE
#include <stdio.h>
#include <mysql.h>
#include <vector>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include "test.pb.hpp"
using namespace boost;
using namespace std;
using namespace Protocol;
//------------------------------------------------------------------
class player_table
{
public:
           player_table();
    void   add_player(player_info person);
    
    player_info get_player(int index);
    
private:
    vector<player_info> player_list;
    
};

player_table::player_table()
{
    player_list=*new vector<player_info>;
}

void player_table::add_player(player_info person)
{
    player_list.push_back(person);
}

player_info player_table::get_player(int index)
{
    player_info person=player_list[index];
    return person;
}

//-------------------------------------------------------------------

class database
{
public:
    void init();
    
    void query();
    
    player_table get_player_table();
    
    
private:
    MYSQL *connection,mysql;
    int res;
    MYSQL_RES *result;
    MYSQL_FIELD *fd;
    char column[32][32];
    MYSQL_ROW sql_row;
    
    player_info cur_person;
    player_table playertable;
    
};

void database::init()
{
    mysql_init(&this->mysql);
    this->connection=mysql_real_connect(&this->mysql, "localhost", "root", "19880226", "player_information", 3306,0,0);
    if(this->connection==NULL)
    {
        std::cout<<mysql_error(&mysql)<<std::endl;
    }
    query();
}

void database::query()
{
    res=mysql_query(connection, "select * from player");
    if (!res) {
        result=mysql_store_result(connection);
        
        while((sql_row=mysql_fetch_row(result)))
        {
            int id=lexical_cast<int>(sql_row[0]);
            cur_person.set_id(id);
            cur_person.set_name(sql_row[1]);
            cur_person.set_email(sql_row[2]);
            
            playertable.add_player(cur_person);
        }
        
    }
}

player_table database::get_player_table()
{
    return playertable;
}
#endif
