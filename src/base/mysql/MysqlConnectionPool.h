/*************************************************************************
  > File Name: MysqlConnectionPool.h
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2019年06月22日 星期六 14时33分16秒
 ************************************************************************/

#ifndef PALLETTE_MYSQL_CONNECTION_POOL_H
#define PALLETTE_MYSQL_CONNECTION_POOL_H

#include <mysql_connection.h>

#include <string>
#include <mutex>
#include <list>
#include <memory>
#include <functional>

namespace pallette
{
    typedef std::function<void(sql::Connection*)> DeleteFunc;

    class MysqlConnectionPool
    {
    public:
        MysqlConnectionPool(std::string url,
            std::string username, std::string passwd);
        ~MysqlConnectionPool();

        std::unique_ptr<sql::Connection, DeleteFunc> getConnection();
        int getConnectionPoolSize(){ return static_cast<int>(connectionList_.size()); }

    private:
        void initConnectionPool();//初始化连接池
        void destoryConnectionPool();//销毁连接池
        void addConnection(int size);//添加size条连接

        std::string url_;
        std::string username_;
        std::string passwd_;
        std::list<std::unique_ptr<sql::Connection>> connectionList_;
        std::mutex mutex_;
        sql::Driver* driver_;

        const int kInitSize_ = 5;
    };
}

#endif
