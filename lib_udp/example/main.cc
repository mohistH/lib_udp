#ifdef _WIN32
#pragma once
#endif /// 


#include <iostream>
#include <memory>
#include <thread>
#include <chrono>

#include "iudp.h"
using namespace lib_udp;
#include <list>
#include <string>
#include <WinSock2.h>
#include <wtypes.h>
#include <Windows.h>

#include <ws2tcpip.h>


template<typename ... Args>
static std::string str_format(const std::string &format, Args ... args)
{
	auto size_buf = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1;
	std::unique_ptr<char[]> buf(new(std::nothrow) char[size_buf]);

	if (!buf)
		return std::string("");

	std::snprintf(buf.get(), size_buf, format.c_str(), args ...);
	return std::string(buf.get(), buf.get() + size_buf - 1);
}

#pragma  comment(lib, "ws2_32.lib")
int get_ipv6_win(std::list<std::string>& out_list_ip6)
{

	int error_id = 0;
	WORD sock_ver;
	WSADATA wsaData;
	int ret_val = 0;
	sock_ver = MAKEWORD(2, 2);
	ret_val = WSAStartup(sock_ver, &wsaData);
	if (0 != ret_val)
	{
		error_id = GetLastError();
		return error_id;
	}

	if (2 != LOBYTE(wsaData.wVersion) ||
		2 != HIBYTE(wsaData.wVersion))
	{
		WSACleanup();
		error_id = GetLastError();
		return error_id;
	}


	// -------------------------------------------------------------------------------


	struct addrinfo hint;

	hint.ai_family = AF_INET6;
	hint.ai_socktype = SOCK_STREAM;
	hint.ai_flags = AI_PASSIVE;
	hint.ai_protocol = 0;
	hint.ai_addrlen = 0;
	hint.ai_canonname = NULL;
	hint.ai_addr = NULL;
	hint.ai_next = NULL;

	const int len_256 = 256;
	char name_host[len_256] = { 0 };

	struct addrinfo *pailist = nullptr;
	struct addrinfo *paip = nullptr;
	const char port_str[] = { "10086" };

	ret_val = getaddrinfo(name_host, port_str, &hint, &pailist);

	// 1.
	if (0 > ret_val || 0 > pailist)
	{
		ret_val = GetLastError();

		WSACleanup();
		return ret_val;
	}

	// 2.
	struct sockaddr_in6 *psinp6 = nullptr;
	for (paip = pailist; NULL != paip; paip = paip->ai_next)
	{
		paip->ai_family = AF_INET6;
		psinp6 = (struct sockaddr_in6 *)paip->ai_addr;
		if (nullptr != psinp6 && NULL != psinp6)
		{
			std::string str_ipv6;
			for (int i = 0; i < 16; i++)
			{
				if (((i - 1) % 2) && (i > 0))
					str_ipv6 += std::string(":");

				str_ipv6 += str_format("%02X", psinp6->sin6_addr.u.Byte[i]);
			}

			out_list_ip6.push_back(str_ipv6);
		}
	}

	WSACleanup();

	return ret_val;
}



/// to recv data, you must inherit udpsocket_recv class
class my_udp : public irecv_data
{
public:

	/// constructor
	my_udp()
	{
		if (NULL == pudp_)
			pudp_ = udp_create_();//// std::unique_ptr<iudp>(lib_udp::udp_create_()).get();
	}


	/// deconstructor
	virtual ~my_udp()
	{
		pudp_ = udp_release_(pudp_);
	}


	/// you must override this function to recv data
	void on_recv_data_(const unsigned char *pdata_recv, const unsigned int recv_data_len)
	{
		/// -------------------------------------------------------------------------------
		/// 1. recv data
		std::cout << "\n --------------AAAAAA data length = " << recv_data_len << "\n";
		for (unsigned int i = 0; i < recv_data_len; i++)
		{
			if (i == 10)
				std:: cout << "\n";

			std::cout << pdata_recv[i] << ", ";
		}
		
		std::cout << "\n";
	}

	/// -------------------------------------------------------------------------------
	int init_(udp_param& param)
	{
		if (pudp_)
			return pudp_->init_(param, this);

		return -20000;
	}

	/// 
	int send_(const char *psend, const int len_send)
	{
		if (pudp_)
			return pudp_->send_((const unsigned char*)psend, len_send);

		return -20000;
	}

	/// 
	int shutdown_()
	{
		if (pudp_)
			return pudp_->shutdown_();

		return -20000;
	}

	int error_id_()
	{
		if (pudp_)
			return pudp_->error_id_();

		return -20000;
	}

private:
	iudp* pudp_ = NULL;
};




int main(int argc, char *argv[])
{

	using namespace  std;
	std::list <std::string> ip6_list;
	int retttt =  get_ipv6_win(ip6_list);
	if ( 0 != retttt)
	{
		std::cout << "ipv6 gets error\n";
		system("pause");
		return 0;
	}

	int index = 0;
	for (auto item : ip6_list)
	{
		cout << ++index << "ip6 = " << item.c_str() << endl;
	}

	//system("pause");
	//return 0;








	/// -------------------------------------------------------------------------------
	/// 1. to prepare params to initialize
	udp_param param;

	param._is_log_debug		= false;
	param._cast_type 		= lib_udp::udp_multi_cast;
	param._port_dst 		= 10086;
	param._recv_loop 		= true;
	param.socket_version_	= kipv6;

#ifdef _WIN32
	char arr_ipv4[] = "10.0.0.5";
#elif __linux__
	char arr_ipv4[] = "192.168.15.129";
#else 
	char arr_ipv4[] = "10.1.1.3";
#endif///

	std::cout << "local IP = " << arr_ipv4 << std::endl;
	char arr_dst[] = "233.0.0.11";

	//param.dest_ip_.value_ = std::string(arr_dst);
	//param.local_ip_.value_ = std::string(arr_ipv4);

	std::string str_ipv6;
	int indexxxx = 0;
	for (auto list_item : ip6_list)
	{
		str_ipv6 = list_item;
		if (3 == indexxxx)
			break;
		++indexxxx;
	}

	param.dest_ip_.value_	= std::string("FF02::1");
	param.local_ip_.value_ = str_ipv6;/// ip6_list.front();


	
	
	/// -------------------------------------------------------------------------------
	/// 1. to create 
	std::unique_ptr<my_udp> pmy_udp(new(std::nothrow) my_udp);

	//my_udp* pmy_udp = new(std::nothrow) my_udp;

	/// failure
	if (!pmy_udp)
	{
		std::cout << "\n my_udp crated failure\n";

#ifdef _WIN32
		system("pause");
#endif ///_WIN32
		return 0;
	}
	

	/// -------------------------------------------------------------------------------
	/// 2. to initialize udp
	int ret = 0;
	ret = pmy_udp->init_(param);
	if (0 != ret)
	{
		std::cout << "\ninit error , id = " << ret << ", error id=" << pmy_udp->error_id_() << "\n\n";
		ret = pmy_udp->shutdown_();
		if (0 != ret)
				std::cout << "\nshutdown error , id = " << ret << "\n";
#ifdef _WIN32
		system("pause");
#endif ///_WIN32

		return 0;
	}

	//pmy_udp->shutdown_();

	////delete pmy_udp;
	////pmy_udp = NULL;

	//return 0;


	std::cout << "\n init_ip4 success\n";

	/// -------------------------------------------------------------------------------
	/// 4. to send data
	char arr[] = "1234567890";
	for (int i = 0; i < 1; i++)
	{
		int send_len = pmy_udp->send_(arr, strlen(arr));
		/// to output the result 
		std::cout << "udp send, send length = " << send_len << "\n";
		std::this_thread::sleep_for(std::chrono::milliseconds(1000 * 1));
	}



	/// to recv data, it needs to rest
	std::cout << "\n---------------------------main 1111----------------------------\n";
	std::this_thread::sleep_for(std::chrono::milliseconds(1000 * 1));
	std::cout << "\n---------------------------main 2222----------------------------\n";
	pmy_udp->shutdown_();
	std::cout << "\n---------------------------main 3333----------------------------\n";
	std::cout << "\nudp has closed\n";


	//delete pmy_udp;
	//pmy_udp = nullptr;

#ifdef _WIN32
	system("pause");
#endif ///_WIN32
	return 0;
}


