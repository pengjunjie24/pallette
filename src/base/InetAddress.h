/*************************************************************************
  > File Name: InetAddress.h
  > Author: pengjunjie
  > Mail: 1002398145@qq.com 
  > Created Time: 2019年01月29日 星期二 16时00分49秒
 ************************************************************************/

#ifndef PALLETTE_INET_ADDRESS_H
#define PALLETTE_INET_ADDRESS_H

#include "SocketsOperations.h"

#include <netinet/in.h>
#include <string>

namespace pallette
{
	class InetAddress
	{
	public:
		explicit InetAddress(uint16_t port = 0, bool loopbackOnly = false, bool ipv6 = false);
		InetAddress(std::string ip, uint16_t port, bool ipv6 = false);
		explicit InetAddress(const struct sockaddr_in& addr)
			: addr_(addr)
		{
		}
		explicit InetAddress(const struct sockaddr_in6& addr)
			: addr6_(addr)
		{
		}

		sa_family_t family() const { return addr_.sin_family; }
		std::string toIp() const;
		std::string toIpPort() const;
		uint16_t toPort() const;

		const struct sockaddr* getSockAddr() const { return sockets::sockaddr_cast(&addr6_); }
		void setSockAddrInet6(const struct sockaddr_in6& addr6) { addr6_ = addr6; }

		uint32_t ipNetEndian() const;
		uint16_t portNetEndian() const { return addr_.sin_port; }

		static bool resolve(std::string hostname, InetAddress* result);

	private:
		union
		{
			struct sockaddr_in addr_;
			struct sockaddr_in6 addr6_;
		};
	};
}


#endif