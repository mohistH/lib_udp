#ifdef _WIN32
#pragma once
#endif // 


#include "iudp.h"
#include <iostream>
#include <memory>
#include <thread>
#include <chrono>

using namespace lib_udp;




// to recv data, you must inherit udpsocket_recv class
class my_udp : public irecv_data
{
public:

	// constructor
	my_udp()
		: pudp_(NULL)
	{

	}


	// deconstructor
	virtual ~my_udp()
	{
		pudp_ = release_(pudp_);
	}


	// you must override this function to recv data
	virtual void on_recv_data_(const unsigned char *pdata, const unsigned int pdata_len)
	{
		// -------------------------------------------------------------------------------
		// 1. recv data
		std::cout << "\ndata length = " << pdata_len << "\n";
		for (unsigned int i = 0; i < pdata_len; i++)
		{
			if (i == 10)
				std:: cout << "\n";

			std::cout << pdata[i] << ", ";
		}
		
		std::cout << "\n";
	}

	// -------------------------------------------------------------------------------
	int init_(const st_udp_init& param)
	{
		pudp_ = create_();

		if (pudp_)
		{
			int ret = pudp_->init_(param, this);
			return ret;
		}

		return -20000;
	}

	// 
	int send_(const char *psend, const int len_send)
	{
		if (pudp_)
			return pudp_->send_((const unsigned char*)psend, len_send);

		return -20000;
	}

	// 
	int shutdown_()
	{
		if (pudp_)
			return pudp_->shutdown_();

		return -20000;
	}

private:
	iudp* pudp_ = NULL;
};



int call_udp()
{

	// -------------------------------------------------------------------------------
	// 1. to prepare params to initialize
	st_udp_init param;

	param.is_log_debug_ = false;
	param.dest_port_ = 10086;
	param.recv_loop_ = true;

	param.dest_ip_.str_ip_ = std::string("225.0.0.22");
	param.local_ip_.str_ip_ = std::string("10.0.0.5");
	param.time_out = 1000;

	// -------------------------------------------------------------------------------
	// 1. to create 
	my_udp* pmy_udp = new(std::nothrow) my_udp;

	// failure
	if (NULL == pmy_udp)
	{
		std::cout << "\n my_udp crated failure\n";

#ifdef _WIN32
		system("pause");
#endif //_WIN32
		return 0;
	}







	// -------------------------------------------------------------------------------
	int ret = pmy_udp->init_(param);
	if (0 != ret)
	{
		std::cout << "\ninit error , id = " << ret << "\n";
		//ret = pmy_udp->shutdown_();
		if (0 != ret)
			std::cout << "\nshutdown error , id = " << ret << "\n";
#ifdef _WIN32
		system("pause");
#endif //_WIN32

		return 0;
	}




	std::cout << "\n init_ip4 success\n";




	// -------------------------------------------------------------------------------
	// 4. to send data
	char arr[] = "1234567890";
	for (int i = 0; i < 1; i++)
	{
		int send_len = pmy_udp->send_(arr, strlen(arr));
		// to output the result 
		std::cout << "udp send, send length = " << send_len << "\n";
		std::this_thread::sleep_for(std::chrono::milliseconds(1000 * 1));
	}


	int xxxx = 0;


	//// to recv data, it needs to rest
	//std::cout << "\n---------------------------main 1111----------------------------\n";
	//std::this_thread::sleep_for(std::chrono::milliseconds(1000 * 3));
	//std::cout << "\n---------------------------main 2222----------------------------\n";
	pmy_udp->shutdown_();
	//std::cout << "\n---------------------------main 3333----------------------------\n";
	//std::cout << "\nudp has closed\n";


	delete pmy_udp;
	pmy_udp = NULL;

	return 0;
}


int main(int argc, char *argv[])
{
	int ret = call_udp();
	if (0 != ret)
	{
		std::cout << "\nAAA ret=" << ret;
	}

#ifdef _WIN32
	system("pause");
#endif // 
	return 0;
}


