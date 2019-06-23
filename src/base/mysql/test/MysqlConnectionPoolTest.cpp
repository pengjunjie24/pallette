/*************************************************************************
  > File Name: MysqlConnectionPoolTest.cpp
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2019年06月22日 星期六 16时50分48秒
 ************************************************************************/

#include "../MysqlConnectionPool.h"

#include <cppconn/statement.h>
#include <cppconn/resultset.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

using namespace pallette;
/*
+---------+----------------+-----------------+-------------+------------+----------+--------------+
| vend_id | vend_name      | vend_address    | vend_city   | vend_state | vend_zip | vend_country |
+---------+----------------+-----------------+-------------+------------+----------+--------------+
|    1001 | Anvils R Us    | 123 Main Street | Southfield  | MI         | 48075    | USA          |
|    1002 | LT Supplies    | 500 Park Street | Anytown     | OH         | 44333    | USA          |
|    1003 | ACME           | 555 High Street | Los Angeles | CA         | 90046    | USA          |
|    1004 | Furball Inc.   | 1000 5th Avenue | New York    | NY         | 11111    | USA          |
|    1005 | Jet Set        | 42 Galaxy Road  | London      | NULL       | N16 6PS  | England      |
|    1006 | Jouets Et Ours | 1 Rue Amusement | Paris       | NULL       | 45678    | France       |
+---------+----------------+-----------------+-------------+------------+----------+--------------+
*/

int main()
{
    MysqlConnectionPool connPool("tcp://127.0.0.1:3306", "root", "root");

    std::cout << "first PoolSize: " << connPool.getConnectionPoolSize() << std::endl;
    {
        std::unique_ptr<sql::Connection, DeleteFunc> conn =
            connPool.getConnection();//获得一个连接
        std::cout << "second PoolSize: " << connPool.getConnectionPoolSize() << std::endl;

        std::unique_ptr<sql::Statement> state(conn->createStatement());//获得一个数据库连接对象
        state->execute("use test");//使用test这个数据库
        std::unique_ptr<sql::ResultSet> result(state->executeQuery("select * from Vendors;"));//查询语句
        while (result->next())
        {
            int id = result->getInt("vend_id");
            std::string name = result->getString("vend_name");
            std::cout << "idvend_id:" << id << "  name:" << name << std::endl;
        }
    }
    //自动将连接放入连接池中
    std::cout << "third PoolSize: " << connPool.getConnectionPoolSize() << std::endl;

    return EXIT_SUCCESS;
}
