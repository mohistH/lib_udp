
#include <udp/udp_socket_imp.h>

#ifdef _use_debug_
#include <iostream>
#endif // !_use_debug_




/* 
*  @ brief: the thread to recv data
*  @ void * param - the adress of udp_socket_imp object 
*  @ return - void
		
*/
void lib_udp::thread_recv_data(void* param)
{

	// -------------------------------------------------------------------------------
	// 1. parameter is nullptr
	if (nullptr == param || NULL == param)
		return;

	// -------------------------------------------------------------------------------
	// 2. get param
	lib_udp::udp_socket_imp *pself		= static_cast<lib_udp::udp_socket_imp*>(param);

	// -------------------------------------------------------------------------------
	// 3. the preparation of to recv data 
	//bool	mutex_is_lock = false;

	char	arr_recv[lib_udp::len_buf_1024_3] = { 0 };
	const int len_recv					= lib_udp::len_buf_1024_3 + 1;
	lib_udp::udp_param_init &param_init = pself->get_param_init();

	int addr_len						= sizeof(struct sockaddr);
	lib_udp::udp_param&param_base		= param_init._param;
	lib_udp::udp_init_other& others		= param_init._others;
	lib_udp::udp_init_adress& adress	= param_init._adress;
	
	// -------------------------------------------------------------------------------
	//   23-08-2020   
	lib_udp::queue_udp_recv& queue_recv = pself->get_queue_recv();
	lib_udp::udp_recv_data_buf queue_recv_item;
	const int queu_recv_len_2			= lib_udp::queue_recv_size_10;
	std::mutex& mutex_queue_recv		= pself->get_mutex_queue_recv();
	

#ifndef _WIN32
	socklen_t addr_len_t				= sizeof(struct sockaddr);
#endif //! __linux__

	//std::cout << "\n\n\n接收线程\n\n";


	auto log_to_debug					= [](const char *parr)
	{
#ifdef _use_debug_
		if (parr)
			std::cout << "\n" << parr << "\n";
#endif// _use_debug_
	};

	// -------------------------------------------------------------------------------
	// 4. recv data;
	int ret_val							= 0;
	while ( pself->get_thread_recv_is_running() )
	{
		log_to_debug("接收线程: 111111111111111111111111111");

		// -------------------------------------------------------------------------------
#ifdef _WIN32
		ret_val							= recvfrom(adress._socket, arr_recv, len_recv, 0, (struct sockaddr *)&(adress._address_dest), &addr_len);
#else
		ret_val							= recvfrom(adress._socket, arr_recv, len_recv, 0, (struct sockaddr *)&(adress._address_dest), &addr_len_t);
#endif // 

		log_to_debug("接收线程: 222222222222222222222222222");

		if (0 < ret_val )
		{
			log_to_debug("接收线程: 3333333333333333333333333");

			// 4.1 cannot copy the data;
			if ((len_recv) <= ret_val)
				continue;

			arr_recv[ret_val] = '\0';

			// 4.2 call
			log_to_debug("接收线程: 44444444444444444444444444444");
			// -------------------------------------------------------------------------------
			// to construct item of recv queue
			if (mutex_queue_recv.try_lock())
			{
				// 4.3 to prepare item of this queue
				queue_recv_item.zero();
				memcpy(queue_recv_item._buf, arr_recv, ret_val);
				queue_recv_item._buf_len_valid = ret_val;

				// 4.4 to check queue's size
				for (;;)
				{
					if (queu_recv_len_2 < queue_recv.size())
						queue_recv.pop();
					else
						break;
				}

				// put the item into the queue
				queue_recv.push(queue_recv_item);
		
				// 4.5. unlock queue_recv
				mutex_queue_recv.unlock();
			}
			else
			{
				// lock error
			}

			log_to_debug("接收线程: 55555555555555555555555555555");

			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		else
		{
			log_to_debug("接收线程: 77777777777777777777777777");
			pself->log_info("recv_data_thread, error msg id = {0}", pself->get_error_id());
		}
	}

}



/* @ brief: to get data from recv queue
*  @ void * param - udpsocket_imp
*  @ return - void
		
*/
void lib_udp::thread_get_data_from_recv_queue(void *param)
{
	// -------------------------------------------------------------------------------
	// 1. parameter is nullptr
	if (nullptr == param 
			|| 
		NULL	== param)
		return;

	// -------------------------------------------------------------------------------
	// 2. get param
	lib_udp::udp_socket_imp *pself		= static_cast<lib_udp::udp_socket_imp*>(param);
	lib_udp::udp_param_init &param_init = pself->get_param_init();
	lib_udp::udp_init_other& others		= param_init._others;
	std::mutex& mutex_queue_recv		= pself->get_mutex_queue_recv();
	lib_udp::queue_udp_recv&queue_recv	= pself->get_queue_recv();
	lib_udp::udp_recv_data_buf item;

	bool is_got_success					= false;
	// true-go on this while 
	while (pself->get_thread_get_recv_is_running())
	{
		if (mutex_queue_recv.try_lock())
		{
			if (0 < queue_recv.size())
			{
				item = queue_recv.front();
				queue_recv.pop();
				is_got_success = true;
			}

			mutex_queue_recv.unlock();

			// to call recv_data function
			if (is_got_success)
				if (others._pfunc_recv_data)
					others._pfunc_recv_data->recv_data(item._buf, item._buf_len_valid);
		}

		is_got_success = false;
		std::this_thread::sleep_for(std::chrono::milliseconds(2));
	}

}



//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/*
*	@brief: constructor
*/
lib_udp::udp_socket_imp::udp_socket_imp()
{
	init_params();
}

/*
*	@brief: to open socket of udp 
*/
int lib_udp::udp_socket_imp::open(const unsigned int time_out_send, udpsocket_recv* pfunc_recv)
{
	int ret_val											= 0;

	// -------------------------------------------------------------------------------
	// 1. initialize adresses 
	lib_udp::udp_param&param_base						= _udp_param_init._param;
	lib_udp::udp_init_other& others						= _udp_param_init._others;
	lib_udp::udp_init_adress& adress					= _udp_param_init._adress;
	bool& is_log_debug									= param_base._is_log_debug;
	
	adress._address_dest.sin_port						= htons(param_base._port_dst);
	adress._address_local.sin_port						= htons(param_base._port_dst);
	adress._address_local_any.sin_port					= htons(param_base._port_dst);

#ifdef _WIN32

	inet_pton(AF_INET, param_base._pip4_dst,			&adress._address_dest.sin_addr.S_un.S_addr);
	inet_pton(AF_INET, param_base._pip4_local,			&adress._address_local.sin_addr.S_un.S_addr);
	adress._address_local_any.sin_addr.S_un.S_addr		= htonl(INADDR_ANY);
#else
	inet_pton(AF_INET, param_base._pip4_dst,			&adress._address_dest.sin_addr.s_addr);
	inet_pton(AF_INET, param_base._pip4_local,			&adress._address_local.sin_addr.s_addr);
	adress._address_local_any.sin_addr.s_addr			= htonl(INADDR_ANY);
#endif // !_WIN32

	// -------------------------------------------------------------------------------
	// 2. set socket timeout, include receiving and sending
	if ( 0 < time_out_send )
	{
#ifdef _WIN32
		ret_val = setsockopt(adress._socket, SOL_SOCKET, SO_SNDTIMEO, (const char *)(&time_out_send), sizeof(time_out_send));
#else
		struct timeval time_out;
		time_out.tv_sec = time_out_send;
		time_out.tv_usec = 0;
		ret_val = setsockopt(adress._socket, SOL_SOCKET, SO_SNDTIMEO, (const char *)(&time_out), sizeof(time_out));
#endif // !_WIN32

		if ( 0 != ret_val)
		{
			ret_val = get_error_id();
			log_info(" open  : [set time out send failure], ret = {0}, error msg id = {1}", ret_val, ret_val);
			return ret_val;
		}
	}


	// -------------------------------------------------------------------------------
	// 3. set the length of receiving and sending adn  set reuse adress
	const int len_buf_recv_send = len_buf_send_recv_10240;
	ret_val = setsockopt(adress._socket, SOL_SOCKET, SO_SNDBUF,		(const char *)&len_buf_recv_send,	sizeof(len_buf_recv_send));
	if ( 0 != ret_val)
	{
		ret_val = get_error_id();
		log_info(" open : [SO_SNDBUF], ret = {0}, error msg id = {1}", ret_val, ret_val);
		return ret_val;
	}

	ret_val = setsockopt(adress._socket, SOL_SOCKET, SO_RCVBUF,		(const char *)&len_buf_recv_send,	sizeof(len_buf_recv_send));	
	if ( 0 != ret_val)
	{
		ret_val = get_error_id();
		log_info(" open : [ SO_RCVBUF], ret = {0}, error msg id = {1}", ret_val, ret_val);
		return ret_val;
	}

	// to set socket address resuse
#ifdef _WIN32	
	char is_resuse_address		= 1;
	ret_val = setsockopt(adress._socket, SOL_SOCKET, SO_REUSEADDR,	&is_resuse_address,	sizeof(is_resuse_address));
#else 
	int address_resuse = 1;
	ret_val = setsockopt(adress._socket, SOL_SOCKET, SO_REUSEADDR,	(void* )&address_resuse,	sizeof(address_resuse));
#endif 

	// an error occured, log it
	if ( 0 != ret_val)
	{
		ret_val = get_error_id();
		log_info(" open : [SO_SNDBUF | SO_RCVBUF | SO_REUSEADDR], ret = {0}, error msg id = {1}", ret_val, ret_val);
		return ret_val;
	}

	

	// -------------------------------------------------------------------------------
	// 3.5 set socket non-block 
	// 23-08-2020 do block,
// #ifdef _WIN32
// 	unsigned long is_block = 1;	// 0 - block
// 	ret_val = ioctlsocket(adress._socket, FIONBIO, &is_block);
// 	if (0 != ret_val)
// 	{
// 		ret_val = get_error_id();
// 		log_info(" open:[ioctlsocket], ret = {0}, error msg id = {1}", ret_val, ret_val);
// 		return ret_val;
// 	}
// #else 

// #endif // !_WIN32





	// -------------------------------------------------------------------------------
	// 4.bind local adress
	ret_val = bind(adress._socket, (struct sockaddr *)&(adress._address_local), sizeof(adress._address_local));
	if (0 != ret_val)
	{
		ret_val = get_error_id();
		log_info("open : [bind local adress], error msg id = {0}", ret_val);
		return ret_val;
	}
	

	// -------------------------------------------------------------------------------
	// 5.set udp type: unicast ? multiast ? 
	switch (param_base._cast_type)
	{
		// multicast
	case udp_multi_cast:
		{
			// 5.1. IP_MULTICAST_IF
			ret_val = setsockopt(adress._socket, IPPROTO_IP, IP_MULTICAST_IF, (const char *)&(adress._address_local.sin_addr), sizeof(adress._address_local.sin_addr));

			// failure
			if (0 != ret_val)
			{
				ret_val = get_error_id();
				log_info("open : [multiast], ret = {0}, error msg id = {1}", ret_val, ret_val);
				return ret_val;
			}

			// success
			// 5.2.to join multcast group
			struct ip_mreq req_multi_cast;

			// local adress
			req_multi_cast.imr_interface = adress._address_local.sin_addr;
			// destination adress
			req_multi_cast.imr_multiaddr = adress._address_dest.sin_addr;

			ret_val = setsockopt(adress._socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char* )&req_multi_cast, sizeof(req_multi_cast));
			// failure
			if (0 != ret_val)
			{
				ret_val = get_error_id();
				log_info(" open : [ IP_ADD_MEMBERSHIP ], ret = {0}, error msg id = {1}", ret_val, ret_val);

				return ret_val;
			}


			// 5.3. whether receiving msg that you sent to the adress 
			const char is_recv_my_self = param_base._recv_loop ? 1 : 0;
			ret_val = setsockopt(adress._socket, IPPROTO_IP, IP_MULTICAST_LOOP, &is_recv_my_self, sizeof(is_recv_my_self));

			// failure
			if (0 != ret_val)
			{
				ret_val = get_error_id();
				log_info("open : [ IP_MULTICAST_LOOP ], ret = {0}, error msg id = {1}", ret_val, ret_val);
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
				ret_val = get_error_id();
				log_info(" int open(const unsigned int time_out_send) (256) : [ udp_broad_cast ], ret = {0}, error msg id = {1}", ret_val, ret_val); 
				return ret_val;
			}
		}
		break;

		// do nothing
	case udp_uni_cast:
		break;

		// other . error
	default:
		ret_val = 20086;
		break;
	}


	if ( 0 == ret_val)
		adress._socket_is_open = true;

	log_info("open: initialized success");


	// -------------------------------------------------------------------------------
	// 6. to prepare params of recv data
	if (NULL != pfunc_recv || nullptr != pfunc_recv)
	{
		others._pfunc_recv_data		= pfunc_recv;

		// 6.1 create a thead to recv data
		_thread_recv_is_running		= true;
		_thread_recv				= std::thread(thread_recv_data, (void*)this);

		// 6.2 create a thread to get recv data
		_thread_get_recv_is_running = true;
		_thread_get_recv			= std::thread(thread_get_data_from_recv_queue, (void*)this);
	}

	return ret_val;
}

/*
*	@brief: send data
*/
int lib_udp::udp_socket_imp::send(const char *psend, const unsigned int len_send)
{
	static const int len_max_send	= 65535 - 20 - 8; 
	const int psend_len				= strlen(psend);
	// psend is null or send data is too long
	if (    (NULL 		== psend) ||
			(nullptr 	== psend) ||
			(len_max_send - 1 < len_send) || 
			(len_max_send < psend_len)  )
		return -20087;

	int ret_val = 0;
	udp_init_adress& adress			= _udp_param_init._adress;
	const bool&	is_success			= adress._socket_is_open;
	static int len_addr				= sizeof(adress._address_dest);

	if ( !is_success )
	{
		ret_val = -20086;

		return ret_val;
	}
	
	// send data
	ret_val = sendto(adress._socket, psend, len_send, 0, (struct sockaddr*)&(adress._address_dest), len_addr);
	
	// sending failure
	if (-1 == ret_val)
		ret_val = get_error_id();

	return ret_val;
}

/*
*	@brief: to close socket
*/
int lib_udp::udp_socket_imp::shutdown()
{
	udp_init_other_	&others	= _udp_param_init._others;
	udp_init_adress& adress = _udp_param_init._adress;
	bool&	is_success		= adress._socket_is_open;

	// 0. emepty queue of receiving
	_queue_recv.empty();
	
	
	if (!is_success)
	{
#ifdef _WIN32
		WSACleanup();
#endif // !_WIN32

		return -20086;
	}
	

	//---------------------------------------------------------------------
	// 1. set socket flag closed
	is_success = false;

	// 2. close socket
	int ret_val = 0;
#ifdef _WIN32
	ret_val = closesocket(adress._socket);
#else
	ret_val = close(adress._socket);
#endif // 

	// an error occured, log it
	if ( 0 != ret_val)
		ret_val = get_error_id();
	
	// whether udp is open , call this function release , because it was created at contructor
#ifdef _WIN32
	WSACleanup();
#endif // !


	// to clear recv thread
	if (nullptr != others._pfunc_recv_data || NULL != others._pfunc_recv_data)
	{
		auto thread_join = [](std::thread& thread_t, bool& is_running)
		{
			is_running = false;
			thread_t.join();

			//if (thread_t.joinable())
			//	thread_t.detach();
			//else
			//	thread_t.join();
		};

		// 2.
		// -------------------------------------------------------------------------------
		//_thread_recv_is_running = false;
		//_thread_recv.join();

		//_thread_get_recv_is_running = false;
		//_thread_get_recv.join();

		thread_join(_thread_recv, _thread_recv_is_running);
		thread_join(_thread_get_recv,_thread_get_recv_is_running);

		// 4.
		// -------------------------------------------------------------------------------
		others._pfunc_recv_data = nullptr;
	}

	return ret_val;
}

/*
*	@brief:
*/
int lib_udp::udp_socket_imp::uninit()
{
	
	// to avoid occurring an error while running deconstructor
	// -------------------------------------------------------------------------------
	// 2.
#ifdef _use_spdlog_
	try
	{
		if (_udp_param_init._param._is_log_debug)
			spdlog::shutdown();
	}
	catch (const spdlog::spdlog_ex &ex)
	{
		// an error occured, but do anything
	}

#endif // _use_spdlog_


	return 0;
}



/*
*	@brief: get the id of error msg
*/
const int lib_udp::udp_socket_imp::get_error_id() 
{
	int ret_val = 0;

#ifdef _WIN32
	ret_val = WSAGetLastError();
#else
	ret_val = errno;
#endif // !_WIN32

	return ret_val;
}

/*
*	@brief: to initialize parameters
*/
void lib_udp::udp_socket_imp::init_params()
{

	udp_init_other_	&others = _udp_param_init._others;
	udp_init_adress& adress = _udp_param_init._adress;


	// 0. create socket
	// -------------------------------------------------------------------------------
#ifdef _WIN32
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);
#endif // !


	// 1. create socket
	adress._socket 							= socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	// 2. set adress family
	adress._address_dest.sin_family			= AF_INET;
	adress._address_local.sin_family		= AF_INET;
	adress._address_local_any.sin_family	= AF_INET;

	// 3. to initialize spdlog, a library to log file
	log_info("dynamic library constructed success");

	
	// -------------------------------------------------------------------------------
	//  
	//   23-08-2020   to initialze queue of  receiving
	_queue_recv.empty();
}

/*
*	@brief: to check parameters 
*/
int lib_udp::udp_socket_imp::init_ip4(udp_param& param)
{
	int ret_val = 0;

	// 1.port of destination
	if (0 == param._port_dst)
	{
		ret_val = 1;

		return ret_val;
	}

	// 2. _pip4_dst    the length must be grater than 7
	if ( 7 > strlen(param._pip4_dst) )
	{
		ret_val = 2;

		return ret_val;
	}

	// 3. save the param to initialize
	memcpy(&_udp_param_init._param, &param, sizeof(param));

	log_info("init_ip4: initialized success");

	return ret_val;
}


/*
*	@brief: deconstructor
*/
lib_udp::udp_socket_imp::~udp_socket_imp()
{
	// to check socket status, if its openning, close it 
	shutdown();
	uninit();
}
