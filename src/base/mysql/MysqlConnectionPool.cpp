/*************************************************************************
  > File Name: MysqlConnectionPool.cpp
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2019年06月22日 星期六 14时33分23秒
 ************************************************************************/

#include "MysqlConnectionPool.h"

#include <cppconn/driver.h>

using namespace sql;
using namespace pallette;

MysqlConnectionPool::MysqlConnectionPool(std::string url,
    std::string username, std::string passwd)
    : url_(url)
    , username_(username)
    , passwd_(passwd)
{
    driver_ = get_driver_instance();
    initConnectionPool();
}

MysqlConnectionPool::~MysqlConnectionPool()
{
    destoryConnectionPool();
}

std::unique_ptr<Connection, DeleteFunc> MysqlConnectionPool::getConnection()
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (connectionList_.empty())
    {
        addConnection(kInitSize_);
    }

    std::unique_ptr<Connection, DeleteFunc> conn(connectionList_.back().release(), [this](Connection* tConn)
    {
        connectionList_.push_back(std::unique_ptr<Connection>(tConn));
    });
    connectionList_.pop_back();
    return std::move(conn);
}

void MysqlConnectionPool::initConnectionPool()
{
    std::unique_lock<std::mutex> lock(mutex_);
    addConnection(kInitSize_);
}

void MysqlConnectionPool::destoryConnectionPool()
{
    for (auto &conn : connectionList_)
    {
        conn->close();
        conn.release();
    }
    connectionList_.clear();
}

void MysqlConnectionPool::addConnection(int size)
{
    for (int i = 0; i < size; ++i)
    {
        std::unique_ptr<Connection> conn(driver_->connect(url_, username_, passwd_));
        connectionList_.push_back(std::move(conn));
    }
}