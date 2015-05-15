#ifndef PLAYER_TABLE
#define PLAYER_TABLE
#include <stdio.h>
#include <mysql.h>
#include <vector>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include "test.pb.h"
using namespace boost;
using namespace std;

class PlayerTable
{
public:
    PlayerTable()
    {
        PlayerList=*new vector<tutorial::Person>;
    }
    void Add_Player(tutorial::Person person)
    {
        PlayerList.push_back(person);
    }
    
    tutorial::Person GetPlayer(int index)
    {
        tutorial::Person person=PlayerList[index];
        return person;
    }
    
private:
    vector<tutorial::Person> PlayerList;
    
};


class Database
{
public:
    void init()
    {
        mysql_init(&this->mysql);
        this->connection=mysql_real_connect(&this->mysql, "localhost", "root", "19880226", "player_information", 3306,0,0);
        if(this->connection==NULL)
        {
            std::cout<<mysql_error(&mysql)<<std::endl;
        }
        query();
    }
    
    void query()
    {
        res=mysql_query(connection, "select * from player");
        if (!res) {
            result=mysql_store_result(connection);
            
            while((sql_row=mysql_fetch_row(result)))
            {
                int id=lexical_cast<int>(sql_row[0]);
                curPerson.set_id(id);
                curPerson.set_name(sql_row[1]);
                curPerson.set_email(sql_row[2]);
                
                playertable.Add_Player(curPerson);
            }
            
        }
    }
    
    PlayerTable GetPlayerTable()
    {
        return playertable;
    }
    
    
private:
    MYSQL *connection,mysql;
    int res;
    MYSQL_RES *result;
    MYSQL_FIELD *fd;
    char column[32][32];
    MYSQL_ROW sql_row;
    
    tutorial::Person curPerson;
    PlayerTable playertable;
    
};
#endif
