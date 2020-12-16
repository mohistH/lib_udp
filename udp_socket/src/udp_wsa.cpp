
#ifdef _WIN32


#include <udp\\udp_wsa.h>
//#include <iostream>


void lib_udp::win_thread_recv_data_(void *param)
{
	// 1. check param
	// -------------------------------------------------------------------------------
	if (NULL == param || nullptr == param)
		return;

	// 2.prepare params
	// -------------------------------------------------------------------------------
	lib_udp::udp_wsa_imp* pself					= static_cast<lib_udp::udp_wsa_imp*>(param);
	lib_udp::udp_init_adress &adress		= pself->get_udp_wsa_init_()._address;
	lib_udp::udp_wsa_init_other& others		= pself->get_udp_wsa_init_()._others;
	std::mutex&	mutex_recv_queue			= pself->get_mutex_wsa_queue_recv_();
	lib_udp::queue_udp_recv & queue_recv	= pself->get_wsa_queue_recv_();

	int addr_len							= sizeof(struct sockaddr);
	ULONG data_recv_len						= 0;
	ULONG	flag							= 0;
	int ret_val								= 0;
	char *pdata_recv						= nullptr;
	
	//std::cout << "\n----------------------2222222222222222222222---------------------\n";

	udp_recv_data_buf item;

	while (pself->get_thread_recv_is_running_())
	{
		//std::cout << "\n----------------------33333333333333333333333---------------------\n";
		ret_val							= WSARecvFrom(adress._socket, 
														&others._buf_recv, 
														1, 
														&data_recv_len, 
														&flag, 
														(SOCKADDR *)&adress._address_dest, 
														&addr_len, 
														NULL,
														NULL);
		//std::cout << "\n----------------------4444444444444444444444444---------------------\n";
		if (0							!= ret_val)
		{
			//std::cout << "\n----------------------5555555555555555555555555555---------------------\n";
			ret_val = GetLastError();
			// std::cout << "\nerror msg id = " << ret_val << "\n";
			//std::cout << "\n----------------------6666666666666666666666666666---------------------\n";
		}
		else
		{
			// its too long, passed
			if (len_buf_1024_3 < data_recv_len)
				continue;

			if (mutex_recv_queue.try_lock())
			{
				// 
				item.zero();
				memcpy(item._buf, others._buf_recv.buf, data_recv_len);
				item._buf_len_valid = data_recv_len;

				for (;;)
				{
					if (queue_recv_size_10 < queue_recv.size())
						queue_recv.pop();
					else
						break;
				}

				queue_recv.push(item);
				mutex_recv_queue.unlock();
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}




/*
*	@brief:
*/
void lib_udp::win_thread_get_recv_data_(void *param)
{
	if (NULL == param || nullptr == param)
		return;

	// -------------------------------------------------------------------------------
	// 2. get param
	lib_udp::udp_wsa_imp *pself				= static_cast<lib_udp::udp_wsa_imp*>(param);
	lib_udp::udp_wsa_init &param_init	= pself->get_udp_wsa_init_();

	lib_udp::udp_wsa_init_other& others	= param_init._others;
	std::mutex& mutex_queue_recv		= pself->get_mutex_wsa_queue_recv_();
	lib_udp::queue_udp_recv&queue_wsa_recv	= pself->get_wsa_queue_recv_();
	lib_udp::udp_recv_data_buf item;

	bool is_got_success = false;
	// true-go on this while 
	while (pself->get_thread_get_recv_data_is_running_())
	{
		if (mutex_queue_recv.try_lock())
		{
			if (0 < queue_wsa_recv.size())
			{
				item = queue_wsa_recv.front();
				queue_wsa_recv.pop();
				is_got_success = true;
			}

			mutex_queue_recv.unlock();

			// to call recv_data function
			if (is_got_success)
				if (others._pfunc_recv)
					others._pfunc_recv->on_recv_data_(item._buf, item._buf_len_valid);
		}

		is_got_success = false;
		std::this_thread::sleep_for(std::chrono::milliseconds(2));
	}


}

/*
*	@brief:
*/
lib_udp::udp_wsa_imp::udp_wsa_imp()
{
	init_env();
}

/*
*	@brief:
*/
int lib_udp::udp_wsa_imp::init_ip4_(udp_param& param)
{
	int ret_val = 0;

	// 1 port of destination
	if (0 == param._port_dst)
	{
		ret_val = 1;

		return ret_val;
	}

	// 2�� _pip4_dst    the length must be grater than 7
	if (7 > strlen(param._pip4_dst))
	{
		ret_val = 2;

		return ret_val;
	}

	// 3��save the param to initialize
	memcpy(&udp_init_._param, &param, sizeof(param));

	return ret_val;
}

/*
*	@brief:
*/
int lib_udp::udp_wsa_imp::open_(const unsigned int time_out_send, irecv_data_interface* pfunc_recv /*= nullptr*/)
{
	int ret_val = 0;

	// -------------------------------------------------------------------------------
	// 1. initialize adresses 
	udp_param& param						= udp_init_._param;
	udp_init_adress& adress					= udp_init_._address;

	adress._address_dest.sin_port			= htons(param._port_dst);
	adress._address_local.sin_port			= htons(param._port_dst);
	adress._address_local_any.sin_port		= htons(param._port_dst);

#ifdef _WIN32

	inet_pton(AF_INET, param._pip4_dst,		&adress._address_dest.sin_addr.S_un.S_addr);
	inet_pton(AF_INET, param._pip4_local,	&adress._address_local.sin_addr.S_un.S_addr);

	//param_init._address_dest.sin_addr.S_un.S_addr			= inet_addr(param._pip4_dst);
	//param_init._address_local.sin_addr.S_un.S_addr		= inet_addr(param._pip4_local);
	adress._address_local_any.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
#else
	inet_pton(AF_INET, param._pip4_dst, &param_init._address_dest.sin_addr.s_addr);
	inet_pton(AF_INET, param._pip4_local, &param_init._address_local.sin_addr.s_addr);
	param_init._address_local_any.sin_addr.s_addr = htonl(INADDR_ANY);
#endif // !_WIN32

	// -------------------------------------------------------------------------------
	// 2��set time out
	if (0 < time_out_send)
	{
#ifdef _WIN32
		ret_val = setsockopt(adress._socket, SOL_SOCKET, SO_SNDTIMEO, (const char *)(&time_out_send), sizeof(time_out_send));
#else
		struct timeval time_out;
		time_out.tv_sec = time_out_send;
		time_out.tv_usec = 0;
		ret_val = setsockopt(param_init._socket, SOL_SOCKET, SO_SNDTIMEO, (const char *)(&time_out), sizeof(time_out));
#endif // 
		// 0 is success
		if (0 != ret_val)
		{
			ret_val = WSAGetLastError();
			return ret_val;
		}
	}


	// -------------------------------------------------------------------------------
	// 3�� set the length of receiving and sending adn  set reuse adress
	const int len_buf_recv_send = 1024 * 10;
	char is_resuse_address = 1;
	ret_val = setsockopt(adress._socket, SOL_SOCKET, SO_SNDBUF, (const char *)&len_buf_recv_send, sizeof(len_buf_recv_send));
	if (0 != ret_val)
	{
		ret_val = WSAGetLastError();
		return ret_val;
	}

	ret_val = setsockopt(adress._socket, SOL_SOCKET, SO_RCVBUF, (const char *)&len_buf_recv_send, sizeof(len_buf_recv_send));
	if (0 != ret_val)
	{
		ret_val = WSAGetLastError();
		return ret_val;
	}

#ifdef _WIN32	
	ret_val = setsockopt(adress._socket, SOL_SOCKET, SO_REUSEADDR, &is_resuse_address, sizeof(is_resuse_address));
#else
	int address_resuse = 1;
	ret_val = setsockopt(param_init._socket, SOL_SOCKET, SO_REUSEADDR, (void*)&address_resuse, sizeof(address_resuse));
#endif 
	if (0 != ret_val)
	{
		ret_val = WSAGetLastError();
		return ret_val;
	}



	// -------------------------------------------------------------------------------
	// 3.5 set socket non-block
#ifndef _WIN32
	unsigned long is_block = 1;	// 0 - block
	ret_val = ioctlsocket(adress._socket, FIONBIO, &is_block);
	if (0 != ret_val)
	{
		ret_val = get_error_id();
		int error_msg_id = ret_val;

		return ret_val;
	}
#endif // !_WIN32





	// -------------------------------------------------------------------------------
	// 4��bind local adress
	ret_val = bind(adress._socket, (struct sockaddr *)&(adress._address_local), sizeof(adress._address_local));
	if (0 != ret_val)
	{
		ret_val = WSAGetLastError();
		return ret_val;
	}


	// -------------------------------------------------------------------------------
	// 5��set udp type: unicast ? multiast ? 
	switch (param._cast_type)
	{
		// multicast
	case udp_multi_cast:
	{
		// 5.1��IP_MULTICAST_IF
		ret_val = setsockopt(adress._socket, IPPROTO_IP, IP_MULTICAST_IF, (const char *)&(adress._address_local.sin_addr), sizeof(adress._address_local.sin_addr));

		// failure
		if (0 != ret_val)
		{
			ret_val = WSAGetLastError();
			return ret_val;
		}

		// success
		// 5.2��to join multcast group
		struct ip_mreq req_multi_cast;

		// local adress
		req_multi_cast.imr_interface = adress._address_local.sin_addr;// adress._address_local.sin_addr;
		// destination adress
		req_multi_cast.imr_multiaddr = adress._address_dest.sin_addr;

		ret_val = setsockopt(adress._socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char*)&req_multi_cast, sizeof(req_multi_cast));
		// failure
		if (0 != ret_val)
		{
			ret_val = WSAGetLastError();
			return ret_val;
		}


		// 5.3�� whether receiving msg that you sent to the adress 
		const char is_recv_my_self = param._recv_loop ? 1 : 0;
		ret_val = setsockopt(adress._socket, IPPROTO_IP, IP_MULTICAST_LOOP, &is_recv_my_self, sizeof(is_recv_my_self));

		// failure
		if (0 != ret_val)
		{
			ret_val = WSAGetLastError();
			return ret_val;
		}
	}
	break;

	case udp_broad_cast:
	{
		const char use_broad_cast = 1;
		ret_val = setsockopt(adress._socket, SOL_SOCKET, SO_BROADCAST, &use_broad_cast, sizeof(use_broad_cast));
		if (0 != ret_val)
		{
			ret_val = WSAGetLastError();
			return ret_val;
		}
	}
	break;

	// do nothing
	case udp_uni_cast:
		break;

		// other  error
	default:
		ret_val = 20086;
		break;
	}


	if (0 == ret_val)
		adress._socket_is_open = true;

	// -------------------------------------------------------------------------------
	// 6. to prepare params of recv data
	if (NULL != pfunc_recv || nullptr != pfunc_recv)
	{
		udp_wsa_init_other &others = udp_init_._others;
		others ._pfunc_recv= pfunc_recv;

		thread_recv_is_running_ = true;
		_thread_recv = std::thread(win_thread_recv_data_, (void*)this);

		thread_get_recv_data_is_running_ = true;
		thread_get_recv_data_ = std::thread(win_thread_get_recv_data_, (void*)this);
	}

	return ret_val;
}

/*
*	@brief:
*/
int lib_udp::udp_wsa_imp::send_(const char *psend, const unsigned int len_send)
{

	// 1. initialize params
	// -------------------------------------------------------------------------------
	int ret_val = 0;
	const bool&	is_success		= udp_init_._address._socket_is_open;
	udp_init_adress &adress		= udp_init_._address;
	udp_wsa_init_other& others	= udp_init_._others;
	static int address_len		= sizeof(adress._address_dest);
	ULONG len_send_data			= static_cast<ULONG>(len_send);

	if (!is_success)
	{
		ret_val = -20086;
		return ret_val;
	}

	//  2.send data
	others._buf_send.buf = (CHAR*)psend;
	others._buf_send.len = len_send_data;

	ULONG has_send_len = 0;
	ret_val = WSASendTo(adress._socket, &others._buf_send, 1, &has_send_len, 0, (SOCKADDR*)&adress._address_dest, address_len, NULL, NULL);
	
	// sending failure
	if ( 0 != ret_val)
		ret_val = WSAGetLastError();

	return ret_val;
}

/*
*	@brief:
*/
int lib_udp::udp_wsa_imp::shutdown_()
{
	// 1. socket doesnt open
	if (!udp_init_._address._socket_is_open)
	{
		WSACleanup();
		return -20086;
	}

	queue_recv_.empty();

	// 2. set it false
	udp_init_._address._socket_is_open = false;

	// 3.
	int ret_val = 0;
	ret_val = closesocket(udp_init_._address._socket);

	// 4.
	if (0 != ret_val)
		ret_val = WSAGetLastError();

	WSACleanup();

	if (nullptr != udp_init_._others._pfunc_recv || NULL != udp_init_._others._pfunc_recv)
	{
		// to end the thread
		auto thread_join					= [](std::thread& thread_t, bool& is_running )
		{
			is_running						= false;
			if (thread_t.joinable())
				thread_t.detach();
			else
				thread_t.join();
		};

		// 2.
		// -------------------------------------------------------------------------------
		//_thread_recv_is_running			= false;
		//_thread_recv.join();

		//_thread_get_recv_data_is_running	= false;
		//_thread_get_recv_data.join();

		thread_join(_thread_recv,			 thread_recv_is_running_);
		thread_join(thread_get_recv_data_,	thread_get_recv_data_is_running_);

		udp_init_._others._pfunc_recv		= nullptr;
	}

	return ret_val;
}

/*
*	@brief:
*/
void lib_udp::udp_wsa_imp::init_env()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	udp_init_._address._socket = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, WSA_FLAG_OVERLAPPED);

	// 2. set adress family
	udp_init_._address._address_dest.sin_family			= AF_INET;
	udp_init_._address._address_local.sin_family		= AF_INET;
	udp_init_._address._address_local_any.sin_family	= AF_INET;

	queue_recv_.empty();

}

/*
*	@brief:
*/
lib_udp::udp_wsa_imp::~udp_wsa_imp()
{
	shutdown_();
}




#endif // ! _WIN32


