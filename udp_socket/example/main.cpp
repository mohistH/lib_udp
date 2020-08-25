#ifdef _WIN32
#pragma once
#endif // 


#include <udp/udp_interface.h>
#include <iostream>
#include <memory>
#include <thread>
#include <chrono>

using namespace lib_udp;


// to recv data, you must inherit udpsocket_recv class
class my_udp : public udpsocket_recv
{
public:

	// constructor
	explicit my_udp()
	{
		// pudp = std::unique_ptr<udpsocket>(lib_udp::udp_wsa_create());
		pudp = std::unique_ptr<udpsocket>(lib_udp::udp_create());
	}


	// deconstructor
	virtual ~my_udp()
	{
		//udp_release(pudp);
	}


	// you must override this function to recv data
	void recv_data(char *pdata_recv, unsigned int recv_data_len)
	{
		// -------------------------------------------------------------------------------
		// 1. recv data
		std::cout << "\ndata length = " << recv_data_len << "\n";
		for (int i = 0; i < recv_data_len; i++)
		{
			if (i == 10)
				std:: cout << "\n";

			std::cout << pdata_recv[i] << ", ";
		}
		
		std::cout << "\n";
	}

	// -------------------------------------------------------------------------------
	int init_ip4(udp_param& param)
	{
		if (pudp)
			return pudp->init_ip4(param);

		return -20000;
	}

	// 
	int open(const unsigned int time_out_send)
	{
		if (pudp)
			return pudp->open(time_out_send, this);

		return -20000;
	}

	// 
	int send(const char *psend, const int len_send)
	{
		if (pudp)
			return pudp->send(psend, len_send);

		return -20000;
	}

	// 
	int shutdown()
	{
		if (pudp)
			return pudp->shutdown();

		return -20000;
	}

private:
	udpsocket* pudp = nullptr;
};




int main(int argc, char *argv[])
{

	// -------------------------------------------------------------------------------
	// 1. to prepare params to initialize
	udp_param param;

	param._is_log_debug = true;
	param._cast_type 	= lib_udp::udp_multi_cast;
	param._port_dst 	= 10086;
	param._recv_loop 	= true;

#ifdef _WIN32
	char arr_ipv4[] = "10.1.1.5";
#elif __linux__
	char arr_ipv4[] = "192.168.244.128";
#else 
	char arr_ipv4[] = "10.1.1.3";
#endif//

	std::cout << "local IP = " << arr_ipv4 << std::endl;
	char arr_dst[] = "233.0.0.11";
	memcpy(param._pip4_dst, arr_dst, strlen(arr_dst));
	memcpy(param._pip4_local, arr_ipv4, strlen(arr_ipv4));
	
	
	// -------------------------------------------------------------------------------
	// 1. to create 
	std::unique_ptr<my_udp> pmy_udp(new(std::nothrow) my_udp);
	// failure
	if (!pmy_udp)
	{
		std::cout << "\n my_udp crated failure\n";

#ifdef _WIN32
		system("pause");
#endif //_WIN32
		return 0;
	}
	

	// -------------------------------------------------------------------------------
	// 2. to initialize udp
	int ret = pmy_udp->init_ip4(param);
	if (0 != ret)
	{
		std::cout << "\ninit error , id = " << ret << "\n";
		ret = pmy_udp->shutdown();
		if (0 != ret)
				std::cout << "\nshutdown error , id = " << ret << "\n";
#ifdef _WIN32
		system("pause");
#endif //_WIN32

		return 0;
	}

	std::cout << "\n init_ip4 success\n";


	// -------------------------------------------------------------------------------
	// 3. to open udp
	ret = pmy_udp->open(10);
	if (0 != ret)
	{
		std::cout << "\nopen error, error msg id = " << ret << "\n";
		pmy_udp->shutdown();
#ifdef _WIN32
		system("pause");
#endif //_WIN32
	}

	std::cout << "\nopen success\n";


	// -------------------------------------------------------------------------------
	// 4. to send data
	char arr[] = "1234567890";
	for (int i = 0; i < 1; i++)
	{
		int send_len = pmy_udp->send(arr, strlen(arr));
		// to output the result 
		std::cout << "udp send, send length = " << send_len << "\n";
		std::this_thread::sleep_for(std::chrono::milliseconds(1000 * 1));
	}



	// to recv data, it needs to rest
	std::cout << "\n---------------------------main 1111----------------------------\n";
	std::this_thread::sleep_for(std::chrono::milliseconds(1000 * 10));
	std::cout << "\n---------------------------main 2222----------------------------\n";
	pmy_udp->shutdown();
	std::cout << "\n---------------------------main 3333----------------------------\n";
	std::cout << "\nudp has closed\n";





#ifdef _WIN32
	system("pause");
#endif // 
	return 0;
}


