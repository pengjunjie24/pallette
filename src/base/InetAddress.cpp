/*************************************************************************
  > File Name: InetAddress.cpp
  > Author: pengjunjie
  > Mail: 1002398145@qq.com 
  > Created Time: 2019年01月29日 星期二 16时00分56秒
 ************************************************************************/

#include "InetAddress.h"

#include "Logging.h"
#include "Endian.h"
#include "SocketsOperations.h"

#include <netdb.h>
#include <string.h>
#include <assert.h>

using namespace pallette;

InetAddress::InetAddress(uint16_t port, bool loopbackOnly, bool ipv6)
{
	if (ipv6)
	{
		bzero(&addr6_, sizeof addr6_);
		addr6_.sin6_family = AF_INET6;
		in6_addr ip = loopbackOnly ? in6addr_loopback : in6addr_any;
		addr6_.sin6_addr = ip;
		addr6_.sin6_port = sockets::hostToNetwork16(port);
	}
	else
	{
		bzero(&addr_, sizeof addr_);
		addr_.sin_family = AF_INET;
		in_addr_t ip = loopbackOnly ? INADDR_LOOPBACK : INADDR_ANY;
		addr_.sin_addr.s_addr = sockets::hostToNetwork32(ip);
		addr_.sin_port = sockets::hostToNetwork16(port);
	}
}

InetAddress::InetAddress(std::string ip, uint16_t port, bool ipv6)
{
	if (ipv6)
	{
		bzero(&addr6_, sizeof addr6_);
		sockets::fromIpPort(ip.c_str(), port, &addr6_);
	}
	else
	{
		bzero(&addr_, sizeof addr_);
		sockets::fromIpPort(ip.c_str(), port, &addr_);
	}
}

std::string InetAddress::toIp() const
{
	char buf[64] = "";
	sockets::toIp(buf, sizeof buf, getSockAddr());
	return buf;
}

std::string InetAddress::toIpPort() const
{
	char buf[64] = "";
	sockets::toIpPort(buf, sizeof buf, getSockAddr());
	return buf;
}

uint16_t InetAddress::toPort() const
{
	return sockets::networkToHost16(portNetEndian());
}

uint32_t InetAddress::ipNetEndian() const
{
	assert(family() == AF_INET);
	return addr_.sin_addr.s_addr;
}

bool InetAddress::resolve(std::string hostname, InetAddress* out)
{
	assert(out != NULL);
	struct hostent hent;
	struct hostent* he = NULL;
	int herrno = 0;
	bzero(&hent, sizeof(hent));
	char resolveBuffer[8 * 1024] = { 0 };

	int ret = gethostbyname_r(hostname.c_str(), &hent, resolveBuffer, sizeof resolveBuffer, &he, &herrno);
	if (ret == 0 && he != NULL)
	{
		assert(he->h_addrtype == AF_INET && he->h_length == sizeof(uint32_t));
		out->addr_.sin_addr = *reinterpret_cast<struct in_addr*>(he->h_addr);
		return true;
	}
	else
	{
		if (ret)
		{
			LOG_SYSERR << "InetAddress::resolve";
		}
		return false;
	}
}