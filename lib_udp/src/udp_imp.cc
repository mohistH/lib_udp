#include "universe_def.h"
#include <udp_imp.h>

#ifdef _use_debug_
#include <iostream>
#endif /// !_use_debug_


/// ----------------------------------------------------------------------------
/// @brief: 数据接受线程
/// ----------------------------------------------------------------------------
void lib_udp::udp_socket_imp::ipv4_thread_recv_data_func_(void* param)
{
	/// -------------------------------------------------------------------------------
	/// 1. parameter is nullptr
	if (nullptr == param || NULL == param)
		return;

	/// -------------------------------------------------------------------------------
	/// 2. get param
	lib_udp::udp_socket_imp *pself		= static_cast<lib_udp::udp_socket_imp*>(param);

	/// -------------------------------------------------------------------------------
	/// 3. the preparation of to recv data 
	///bool	mutex_is_lock = false;

	char	arr_recv[lib_udp::len_buf_1024_3] = { 0 };
	const int len_recv					= lib_udp::len_buf_1024_3 + 1;
	lib_udp::udp_param_init &param_init = pself->get_param_init_();

	int addr_len						= sizeof(struct sockaddr);
	lib_udp::udp_param&param_base		= param_init.param_;
	lib_udp::udp_init_other& others		= param_init.others_;
	lib_udp::udp_init_address& address	= param_init.address_;
	



#if defined(os_is_linux)
	socklen_t addr_len_t				= sizeof(struct sockaddr);
#endif ///! __linux__

	///std::cout << "\n\n\n接收线程\n\n";


	auto log_to_debug					= [](const char *parr)
	{
#ifdef _use_debug_
		if (parr)
			std::cout << "\n" << parr << "\n";
#endif/// _use_debug_
	};

	/// -------------------------------------------------------------------------------
	/// 4. recv data;
	int ret_val							= 0;
	while ( pself->get_thread_recv_is_running_() )
	{
		//log_to_debug("接收线程: 111111111111111111111111111");

		/// -------------------------------------------------------------------------------
#if defined(os_is_win)
		ret_val							= recvfrom(address.socket_, arr_recv, len_recv, 0, (struct sockaddr *)&(address.ipv4_.dest_), &addr_len);
#elif defined(os_is_linux)
		ret_val							= recvfrom(address.socket_, arr_recv, len_recv, 0, (struct sockaddr *)&(address.ipv4_.dest_), &addr_len_t);
#endif /// 

		//log_to_debug("接收线程: 222222222222222222222222222");

		if (0 < ret_val )
		{
			//log_to_debug("接收线程: 3333333333333333333333333");

			/// 4.1 cannot copy the data;
			if ((len_recv) <= ret_val)
				;
			else
			{
				arr_recv[ret_val] = '\0';

				/// 4.2 call
				//log_to_debug("接收线程: 44444444444444444444444444444");
				if (others.pfunc_recv_data_)
					others.pfunc_recv_data_->on_recv_data_((const unsigned char*)arr_recv, ret_val);

				//log_to_debug("接收线程: 55555555555555555555555555555");
			}

			//std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
		else
		{
			//log_to_debug("接收线程: 77777777777777777777777777");
			pself->log_info_("recv_data_thread, error msg id = {0}", pself->get_error_id_());
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

}



/// ----------------------------------------------------------------
/// @brief: 接收数据函数
/// ----------------------------------------------------------------
void lib_udp::udp_socket_imp::ipv6_thread_recv_data_func_(void* param)
{
	/// 1. 参数检查
	if (nullptr == param					|| NULL == param)
		return;

	/// 2.
	lib_udp::udp_socket_imp *pself			= static_cast<lib_udp::udp_socket_imp*>(param);

	if ((NULL == pself)						|| (nullptr == pself))
		return;


	/// 3.准备接收buff
	char	arr_recv[lib_udp::len_buf_1024_3]	 = { 0 };
	const int arr_recv_len					= lib_udp::len_buf_1024_3 + 1;
	lib_udp::udp_param_init &param_init		= pself->get_param_init_();

	
	lib_udp::udp_param&param_base			= param_init.param_;
	lib_udp::udp_init_other& others			= param_init.others_;
	lib_udp::udp_init_address& address		= param_init.address_;
	int addr_len							= sizeof(address.ipv6_.dest_);

	while (true								== pself->get_thread_recv_is_running_())
	{




#if defined(_use_debug_)
		std::cout << "\n 11111111111111111111111111111\n";
#endif /// 
		int recv_len						= recvfrom(address.socket_, arr_recv, arr_recv_len, 0, (struct sockaddr *)&address.ipv6_.dest_, (socklen_t*)&addr_len);
		
		/// --------------------------------------------------------------
		/// 收到数据
		if (0								< recv_len)
		{
			if (recv_len					<= arr_recv_len)
			{
#if defined(_use_debug_)
				std::cout << "\n 3333333333333333333333333333333333333333333\n";
#endif /// 
				arr_recv[recv_len] = '\0';

				/// 提交给应用层
				if (others.pfunc_recv_data_)
				{
					others.pfunc_recv_data_->on_recv_data_((const unsigned char*)arr_recv, recv_len);
				}
				else
					;
			}
			else
				;

		}
		/// 没有收到数据，
		else
		{
#if defined(_use_debug_)
			std::cout << "\n 5555555555555555555555555555555555\n";
#endif /// 
		}



		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

/// ----------------------------------------------------------------------------
/// @brief: 构造函数
/// ----------------------------------------------------------------------------
lib_udp::udp_socket_imp::udp_socket_imp()
{
	
}

/// ----------------------------------------------------------------------------
/// @brief: 析构函数
/// ----------------------------------------------------------------------------
lib_udp::udp_socket_imp::~udp_socket_imp()
{
	/// to check socket status, if its openning, close it 
	shutdown_();
}


/// ----------------------------------------------------------------------------
/// @brief: 发送数据
/// ----------------------------------------------------------------------------
int lib_udp::udp_socket_imp::send_(const unsigned char *psend, const unsigned int len_send)
{
	switch (udp_param_init_.param_.socket_version_)
	{
	case kipv4:
		return send_ipv4_(psend, len_send);

	case kipv6:
		return send_ipv6_(psend, len_send);

	default:
		break;
	}

	return -1;
}

/// ----------------------------------------------------------------------------
/// @brief: 关闭套接字
/// ----------------------------------------------------------------------------
int lib_udp::udp_socket_imp::shutdown_()
{
	udp_init_other	&others			= udp_param_init_.others_;
	udp_init_address& address		= udp_param_init_.address_;


	/// to clear recv thread
	if ((nullptr != others.pfunc_recv_data_) && (NULL != others.pfunc_recv_data_))
	{
		auto thread_join = [](std::thread& thread_t, bool& is_running)
		{
			is_running = false;
			thread_t.join();
		};

		thread_join(thread_recv_, thread_recv_is_running_);

		/// 4.
		/// -------------------------------------------------------------------------------
		others.pfunc_recv_data_ = nullptr;
	}

	/// 2. close socket
	int ret_val = 0;

	if (0 < address.socket_)
	{
#if defined(os_is_win)
		ret_val = closesocket(address.socket_);
#elif defined(os_is_linux)
		ret_val = close(address.socket_);
#endif /// 

		/// an error occured, log it
		if (0 != ret_val)
			ret_val = get_error_id_();

		/// whether udp is openning, call this function release , because it was created at contructor
#if defined(os_is_win)
		WSACleanup();
#endif /// !
	}

	udp_param_init_.zero();

	return ret_val;
}



/// ----------------------------------------------------------------
/// @brief: 错误ID
/// ----------------------------------------------------------------
int lib_udp::udp_socket_imp::error_id_()
{
	return get_error_id_();
}

/// ----------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------
lib_udp::udp_param_init& lib_udp::udp_socket_imp::get_param_init_()
{
	return udp_param_init_;
}

/// ----------------------------------------------------------------------------
/// @brief: 返回错误代码
/// ----------------------------------------------------------------------------
const int lib_udp::udp_socket_imp::get_error_id_() 
{
	int ret_val = 0;

#if defined(os_is_win)
	ret_val = WSAGetLastError();
#elif defined(os_is_linux)
	ret_val = errno;
#endif /// !_WIN32

	return ret_val;
}



/// ----------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------
bool lib_udp::udp_socket_imp::get_thread_recv_is_running_()
{
	return thread_recv_is_running_;
}

///// ----------------------------------------------------------------------------
///// @brief: 初始化套接字
///// ----------------------------------------------------------------------------
//void lib_udp::udp_socket_imp::init_socket_()
//{
//
//	udp_init_other	&others		= udp_param_init_.others_;
//	udp_init_address& address	= udp_param_init_.address_;
//
//
//	/// 0. create socket
//	/// -------------------------------------------------------------------------------
//#if defined(os_is_win)
//	WSADATA wsadata;
//	int ret = WSAStartup(MAKEWORD(2, 2), &wsadata);
//	if (0 != ret)
//		return ;
//#endif /// !
//
//
//	/// 1. create socket
//	address.socket_ 						= socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
//
//	/// 2. set adress family
//	address.ipv4_.dest_.sin_family			= AF_INET;
//	address.ipv4_.local_.sin_family			= AF_INET;
//	address.ipv4_.local_any_.sin_family		= AF_INET;
//
//	/// 3. to initialize spdlog, a library to log file
//	log_info_("dynamic library constructed success");
//}



/// --------------------------------------------------------------------------------------------------------
///	@brief: 检查是IPv6还是IPV4
/// --------------------------------------------------------------------------------------------------------
int lib_udp::udp_socket_imp::check_socket_version_(const int& socket_ver)
{
	int af_inet_ver		= 0;
	switch ( socket_ver )
	{
		case kipv4:
			af_inet_ver = AF_INET;
			break;

		case kipv6:
			af_inet_ver = AF_INET6;
			break;

		default:
			return -1;
			break;
	}

	return af_inet_ver;
}

/// ----------------------------------------------------------------
/// @brief: 初始化IPV4
/// ----------------------------------------------------------------
int lib_udp::udp_socket_imp::init_ipv4_(const udp_param& param, irecv_data* pfunc_recv)
{
	int ret_val				= 0;

	/// 1.端口无效
	if (0					== param._port_dst)
		return 1;
	

	/// 2. 参数赋值
	udp_param_init_.param_ = param;

	/// 3. 创建socket
	init_socket_(AF_INET);

	/// SOCKET 创建失败
	if (0					>= udp_param_init_.address_.socket_)
		return 2;


	const int af_inet_ver						= AF_INET;
	
	/// --------------------------------------------------------------
	/// 5. initialize adresses 
	lib_udp::udp_param&param_base				= udp_param_init_.param_;
	lib_udp::udp_init_other& others				= udp_param_init_.others_;
	lib_udp::udp_init_address& address			= udp_param_init_.address_;
	bool& is_log_debug							= param_base._is_log_debug;

	address.ipv4_.dest_.sin_port				= htons(param_base._port_dst);
	address.ipv4_.local_.sin_port				= htons(param_base._port_dst);
	address.ipv4_.local_any_.sin_port			= htons(param_base._port_dst);


	address.ipv4_.local_.sin_family				= af_inet_ver;
	address.ipv4_.dest_.sin_family				= af_inet_ver;
	address.ipv4_.local_any_.sin_family			= af_inet_ver;

#if defined(os_is_win)
	inet_pton(af_inet_ver, param_base.dest_ip_.value_.c_str(), &address.ipv4_.dest_.sin_addr.S_un.S_addr);
	inet_pton(af_inet_ver, param_base.local_ip_.value_.c_str(), &address.ipv4_.local_.sin_addr.S_un.S_addr);
	address.ipv4_.local_any_.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
#elif defined(os_is_linux)
	// inet_pton(AF_INET, param_base.dest_ip_.value_.c_str(), &adress._address_dest.sin_addr.S_un.S_addr);

	inet_pton(af_inet_ver, param_base.dest_ip_.value_.c_str(), &address.ipv4_.dest_.sin_addr.s_addr);
	inet_pton(af_inet_ver, param_base.local_ip_.value_.c_str(), &address.ipv4_.local_.sin_addr.s_addr);
	address.ipv4_.local_any_.sin_addr.s_addr = htonl(INADDR_ANY);
#endif /// !_WIN32

	ret_val										= init_common_properties_(address.socket_, udp_param_init_.param_);
	if (0										!= ret_val)
		return ret_val;


	/// -------------------------------------------------------------------------------
	/// 10.绑定地址
	ret_val										= bind(address.socket_, (struct sockaddr *)(&address.ipv4_.local_), sizeof(address.ipv4_.local_));
	if (0										!= ret_val)
		return 9;


	/// -------------------------------------------------------------------------------
	/// 11. 检查UDp类型
	switch (param_base._cast_type)
	{
		/// multicast
	case lib_udp::udp_multi_cast:
	{
		/// 5.1. IP_MULTICAST_IF
		ret_val									= setsockopt(address.socket_, IPPROTO_IP, IP_MULTICAST_IF, (const char *)&(address.ipv4_.local_.sin_addr), sizeof(address.ipv4_.local_.sin_addr));

		/// failure
		if (0									!= ret_val)
			return 10;

		/// success
		/// 5.2.to join multcast group
		struct ip_mreq req_multi_cast;

		/// local adress
		req_multi_cast.imr_interface = address.ipv4_.local_.sin_addr;
		/// destination adress
		req_multi_cast.imr_multiaddr = address.ipv4_.dest_.sin_addr;

		ret_val									= setsockopt(address.socket_, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char*)&req_multi_cast, sizeof(req_multi_cast));
		/// failure
		if (0									!= ret_val)
			return 11;


		/// 5.3. whether receiving msg that you sent to the adress 
		const char is_recv_my_self				= param_base._recv_loop ? 1 : 0;
		ret_val									= setsockopt(address.socket_, IPPROTO_IP, IP_MULTICAST_LOOP, &is_recv_my_self, sizeof(is_recv_my_self));

		/// failure
		if (0									!= ret_val)
			return 12;
	}
	break;

	case lib_udp::udp_broad_cast:
	{
		const char use_broad_cast				= 1;
		ret_val									= setsockopt(address.socket_, SOL_SOCKET, SO_BROADCAST, &use_broad_cast, sizeof(use_broad_cast));
		if (0									!= ret_val)
			return 13;
	}
	break;

	/// do nothing
	case lib_udp::udp_uni_cast:
		break;

		/// other . error
	default:
		return 15;
	}


	if (0 == ret_val)
		address.socket_is_openned_				= true;

	log_info_("open: initialized success");


	/// -------------------------------------------------------------------------------
	/// 12.检查接收
	if ((NULL != pfunc_recv)					|| (nullptr != pfunc_recv))
	{
		others.pfunc_recv_data_					= pfunc_recv;

		/// 6.1 create a thead to recv data
		thread_recv_is_running_					= true;
		thread_recv_							= std::thread(ipv4_thread_recv_data_func_, (void*)this);
	}

	return 0;
}


/// ----------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------
int lib_udp::udp_socket_imp::init_ipv6_(const udp_param& param, irecv_data* pfunc_recv)
{

	/// --------------------------------------------------------------	
	/// 1. 端口为0
	if (0 == param._port_dst)
		return 1;

	/// 

	/// 2. 参数赋值
	udp_param_init_.param_		= param;

	/// 3. 创建socket
	init_socket_(AF_INET6);

	/// SOCKET 创建失败
	if (0 >= udp_param_init_.address_.socket_)
		return 2;


	const int af_inet_ver = AF_INET6;

	/// --------------------------------------------------------------
	/// 5.
		/// --------------------------------------------------------------
	/// 5. initialize adresses 
	lib_udp::udp_param&param_base					= udp_param_init_.param_;
	lib_udp::udp_init_other& others					= udp_param_init_.others_;
	lib_udp::udp_init_address& address				= udp_param_init_.address_;
	bool& is_log_debug								= param_base._is_log_debug;

	address.ipv6_.dest_.sin6_port					= htons(param_base._port_dst);
	address.ipv6_.local_.sin6_port					= htons(param_base._port_dst);
	address.ipv6_.local_any_.sin6_port				= htons(param_base._port_dst);

	address.ipv6_.local_.sin6_family				= af_inet_ver;
	address.ipv6_.dest_.sin6_family					= af_inet_ver;
	address.ipv6_.local_any_.sin6_family			= af_inet_ver;

	inet_pton(af_inet_ver, param_base.dest_ip_.value_.c_str(), &address.ipv6_.dest_.sin6_addr);
	inet_pton(af_inet_ver, param_base.local_ip_.value_.c_str(), &address.ipv6_.local_.sin6_addr);
	address.ipv6_.local_any_.sin6_addr				= in6addr_any;
	
	int ret_val										= init_common_properties_(address.socket_, udp_param_init_.param_);
	if (0											!= ret_val)
		return ret_val;

	/// -------------------------------------------------------------------------------
	/// 10.绑定地址
	ret_val											= bind(address.socket_, (struct sockaddr *)(&address.ipv6_.local_), sizeof(address.ipv6_.local_));
	if (0											!= ret_val)
		return 9;


	/// --------------------------------------------------------------
	/// 11. 检查套接字类型
	switch (param._cast_type)
	{
		/// 组播
	case udp_multi_cast:
	{


		//uint_t ifindex;

		//ifindex = if_nametoindex )"hme3");
		//setsockopt(sock, IPPROTO_IPV6, IPV6_MULTICAST_IF, &ifindex,
		//	sizeof(ifindex))

		int ifindex									= 0;
		/// ret_val										= setsockopt(address.socket_, IPPROTO_IPV6, IPV6_MULTICAST_IF, (const char *)&(address.ipv6_.local_.sin6_addr), sizeof(address.ipv6_.local_.sin6_addr));
		ret_val										= setsockopt(address.socket_, IPPROTO_IPV6, IPV6_MULTICAST_IF, (const char*)(&ifindex), sizeof(ifindex));
		
		if (0										!= ret_val)
			return 10;

		/// ipv6_mreq结构提供了用于IPv6地址的多播组的信息。
		struct ipv6_mreq group;
		/// 将接口索引指定为0，则使用默认的多播接口。
		group.ipv6mr_interface						= ifindex;/// htonl(INADDR_ANY);
		/// IPv6组播组的地址。
		inet_pton(AF_INET6,							udp_param_init_.param_.dest_ip_.value_.c_str(), &group.ipv6mr_multiaddr);
		/// 将套接字加入到指定接口上提供的多播组
		ret_val										= setsockopt(address.socket_, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, (const char*)&group, sizeof(ipv6_mreq));
		if (0										!= ret_val)
			return 11;

		const char is_recv_my_self					= param_base._recv_loop ? 1 : 0;
		ret_val										= setsockopt(address.socket_, IPPROTO_IPV6, IPV6_MULTICAST_LOOP, &is_recv_my_self, sizeof(is_recv_my_self));

		/// failure
		if (0										!= ret_val)
			return 12;
	}
	break;
		/// 单播
	case udp_uni_cast:
		break;

		/// 广播 , Ipv6没有
	case udp_broad_cast:
		return 13;

	default:
		return 15;
	}


	if (0 == ret_val)
		address.socket_is_openned_				= true;

	log_info_("open: initialized success");


	/// -------------------------------------------------------------------------------
	/// 12.检查接收
	if ((NULL != pfunc_recv)					|| (nullptr != pfunc_recv))
	{
		others.pfunc_recv_data_					= pfunc_recv;

		/// 6.1 create a thead to recv data
		thread_recv_is_running_					= true;
		thread_recv_							= std::thread(ipv6_thread_recv_data_func_, (void*)this);
	}

	return 0;
}

/// ----------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------
int lib_udp::udp_socket_imp::send_ipv4_(const unsigned char *psend, const unsigned int len_send)
{
	static const int len_max_send	= len_buf_send_recv_10240;

	/// --------------------------------------------------------------
	/// 1， 参数检查
	if (    (NULL 					== psend) || (nullptr 	== psend) || (len_max_send - 1 < len_send) || ( 0 == len_max_send) )
		return 1;

	int ret_val						= 0;
	udp_init_address& address		= udp_param_init_.address_;
	const bool&	is_success			= address.socket_is_openned_;
	static int len_addr				= sizeof(address.ipv4_.dest_);

	/// --------------------------------------------------------------
	/// 2.套接字创建失败
	if ( !is_success )
		return 2;
	
	/// --------------------------------------------------------------
	/// 3. 发送数据
	ret_val							= sendto(address.socket_, (char*)psend, len_send, 0, (struct sockaddr*)&(address.ipv4_.dest_), len_addr);
	
	/// sending failure
	if (-1							== ret_val)
		return 3;

	return 0;
}

/// ----------------------------------------------------------------
/// @brief: 发送数据
/// ----------------------------------------------------------------
int lib_udp::udp_socket_imp::send_ipv6_(const unsigned char *psend, const unsigned int len_send)
{

	static const int len_max_send	= len_buf_send_recv_10240;

	/// --------------------------------------------------------------
	/// 1， 参数检查
	if (    (NULL 					== psend) || (nullptr 	== psend) || (len_max_send - 1 < len_send) || ( 0 == len_max_send) )
		return 1;

	int ret_val						= 0;
	udp_init_address& address		= udp_param_init_.address_;
	const bool&	is_success			= address.socket_is_openned_;
	static int len_addr				= sizeof(address.ipv6_.dest_);

	/// --------------------------------------------------------------
	/// 2.套接字创建失败
	if ( !is_success )
		return 2;
	
	/// --------------------------------------------------------------
	/// 3. 发送数据
	ret_val							= sendto(address.socket_, (char*)psend, len_send, 0, (struct sockaddr*)&(address.ipv6_.dest_), len_addr);
	
	/// sending failure
	if (-1							== ret_val)
		return 3;

	return 0;
}

/// ----------------------------------------------------------------
/// @brief: 初始化共有属性
/// ----------------------------------------------------------------
int lib_udp::udp_socket_imp::init_common_properties_(const int& socket, const udp_param& param)
{
	int ret_val										= 0;
		/// -------------------------------------------------------------------------------
	/// 6.设置发送超时
	if (0										< param.send_timeout_)
	{
		unsigned int time_out_send				= param.send_timeout_;
#if defined(os_is_win)
		ret_val									= setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, (const char *)(&time_out_send), sizeof(time_out_send));
#elif defined(os_is_linux)
		struct timeval time_out;
		time_out.tv_sec							= time_out_send;
		time_out.tv_usec						= 0;
		ret_val									= setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, (const char *)(&time_out), sizeof(time_out));
#endif /// !_WIN32

		if (0									!= ret_val)
			return 3;
	}

	/// --------------------------------------------------------------
	/// 7.接收超时
	if (0										< param.recv_timeout_)
	{
		unsigned int timeout_recv				= param.recv_timeout_;
#if defined(os_is_win)
		ret_val									= setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (const char *)(&timeout_recv), sizeof(timeout_recv));
#elif defined(os_is_linux)
		struct timeval time_out;
		time_out.tv_sec							= timeout_recv;
		time_out.tv_usec						= 0;
		ret_val									= setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (const char *)(&time_out), sizeof(time_out));
#endif /// !_WIN32

		if (0									!= ret_val)
			return 5;
	}


	/// -------------------------------------------------------------------------------
	/// 8 指定接收和发送缓冲
	const int len_buf_recv_send					= len_buf_send_recv_10240;
	ret_val										= setsockopt(socket, SOL_SOCKET, SO_SNDBUF, (const char *)&len_buf_recv_send, sizeof(len_buf_recv_send));
	if (0										!= ret_val)
		return 6;

	ret_val										= setsockopt(socket, SOL_SOCKET, SO_RCVBUF, (const char *)&len_buf_recv_send, sizeof(len_buf_recv_send));
	if (0										!= ret_val)
		return 7;

	/// --------------------------------------------------------------
	/// 9. 设置地址重用
#if defined(os_is_win)	
	char is_resuse_address						= 1;
	ret_val										= setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &is_resuse_address, sizeof(is_resuse_address));
#elif defined(os_is_linux) 
	int address_resuse							= 1;
	ret_val = setsockopt(address.socket_, SOL_SOCKET, SO_REUSEADDR, (void*)&address_resuse, sizeof(address_resuse));
#endif 

	if (0										!= ret_val)
		return 8;

	return 0;
}

/// --------------------------------------------------------------------------------------------------------
///	@brief: 创建套接字
/// --------------------------------------------------------------------------------------------------------
int lib_udp::udp_socket_imp::init_socket_(const int& ip_ver)
{
	udp_init_other	&others				= udp_param_init_.others_;
	udp_init_address& address			= udp_param_init_.address_;


	/// 0. create socket
	/// -------------------------------------------------------------------------------
#if defined(os_is_win)
	WSADATA wsadata;
	int ret = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (0 != ret)
		return 1;
#endif /// !

	/// -----------------------------------------------------------------
	///  check the ip version , if its wrong, this function will return not zero.
	/// -----------------------------------------------------------------


	/// 1. create socket
	address.socket_ 						= socket(ip_ver, SOCK_DGRAM, IPPROTO_UDP);

	/// 2. set adress family
	address.ipv4_.dest_.sin_family			= ip_ver;
	address.ipv4_.local_.sin_family			= ip_ver;
	address.ipv4_.local_any_.sin_family		= ip_ver;

	/// 3. to initialize spdlog, a library to log file
	log_info_("dynamic library constructed success");

	return 0;
}

/// --------------------------------------------------------------------------------------------------------
///	@brief: udp 实现初始化
/// --------------------------------------------------------------------------------------------------------
int lib_udp::udp_socket_imp::init_(const udp_param& param, irecv_data* pfunc_recv)
{
	shutdown_();
	/// --------------------------------------------------------------
	/// 1、检查是IPv6还是ipv4
	int ret = 0;
	switch (param.socket_version_)
	{
	case kipv4:
		/// ret = init_ipv4_(param, pfunc_recv);
		break;

	case kipv6:
		ret= init_ipv6_(param, pfunc_recv);
		break;

	default:
		return -1;
	}

	return ret;
}


