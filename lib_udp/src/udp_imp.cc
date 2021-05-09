

#include "udp_imp.h"
#include "universe_def.h"
#include "udp_def.h"

#ifdef _use_debug_
#include <iostream>
#endif // !_use_debug_

#include <iostream>
#include <string>




namespace lib_udp
{


	///  --------------------------------------------------------------------------------
	///  @brief: 	线程接收数据函数
	///  @param:	void * param - 
	///  @return:	void	
	/// 			
	///  --------------------------------------------------------------------------------
	void lib_udp::udp_imp::thread_recv_data_(void* param)
	{
		// -------------------------------------------------------------------------------
		// 1. parameter is nullptr
		if (nullptr == param || NULL == param)
			return;

		// -------------------------------------------------------------------------------
		// 2. get param
		lib_udp::udp_imp *pself = static_cast<lib_udp::udp_imp*>(param);

		// -------------------------------------------------------------------------------
		// 3. the preparation of to recv data 
		//bool	mutex_is_lock = false;

		char	arr_recv[lib_udp::len_buf_1024_3] = { 0 };
		const int len_recv = lib_udp::len_buf_1024_3 + 1;
		lib_udp::udp_param_config &param_init = pself->get_udp_config_();

		int addr_len = sizeof(struct sockaddr);
		lib_udp::st_udp_init&param_base = param_init.init_;
		lib_udp::udp_init_other& others = param_init.others_;
		lib_udp::udp_init_adress& adress = param_init.address_;

		// -------------------------------------------------------------------------------
		//   23-08-2020   
		//lib_udp::queue_udp_recv& queue_recv = pself->get_queue_recv_();
		//lib_udp::udp_recv_data_buf queue_recv_item;
		//const int queu_recv_len_2 = lib_udp::queue_recv_size_10;
		//std::mutex& mutex_queue_recv = pself->get_mutex_queue_recv_();


#if defined(os_is_linux)
		socklen_t addr_len_t = sizeof(struct sockaddr);
#endif //! __linux__


		auto log_to_debug = [](const char *parr)
		{
#ifdef _use_debug_
			if (parr)
				std::cout << "\n" << parr << "\n";
#endif// _use_debug_
		};

		// -------------------------------------------------------------------------------
		// 4. recv data;
		int ret_val = 0;
		while (pself->get_thread_recv_is_running_())
		{
			log_to_debug("接收线程: 111111111111111111111111111");

			// -------------------------------------------------------------------------------
#if defined(os_is_win)
			ret_val = recvfrom(adress._socket, arr_recv, len_recv, 0, (struct sockaddr *)&(adress._address_dest), &addr_len);
#elif defined(os_is_linux)
			ret_val = recvfrom(adress._socket, arr_recv, len_recv, 0, (struct sockaddr *)&(adress._address_dest), &addr_len_t);
#endif // 
			/// 超时， 继续执行接收
			if (ret_val == EWOULDBLOCK || ret_val == EAGAIN)
				continue;

			log_to_debug("接收线程: 222222222222222222222222222");

			if (0 < ret_val)
			{
				log_to_debug("接收线程: 3333333333333333333333333");

				// 4.1 cannot copy the data;
				if ((len_recv) <= ret_val)
					continue;

				arr_recv[ret_val] = '\0';

				// 4.2 call
				log_to_debug("接收线程: 44444444444444444444444444444");
				// -------------------------------------------------------------------------------
				if (others.precv_data_)
					others.precv_data_->on_recv_data_((const unsigned char*)arr_recv, ret_val);
				else
					;/// 创建失败

				log_to_debug("接收线程: 55555555555555555555555555555");

				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
			else
			{
				log_to_debug("接收线程: 77777777777777777777777777");
				pself->log_info_("recv_data_thread, error msg id = {0}", pself->get_error_id_());
			}
		}

	}

	/// ---------------------------------------------------------------------------
	/// @brief: 构造函数
	/// ---------------------------------------------------------------------------
	lib_udp::udp_imp::udp_imp()
	{
		//init_params();
	}



	/// ---------------------------------------------------------------------------
	/// @brief: 关闭套接字
	/// ---------------------------------------------------------------------------
	int udp_imp::shutdown_()
	{
		udp_init_other_	&others				= udp_config_.others_;
		udp_init_adress& address			= udp_config_.address_;

		/// -----------------------------------------------------------------
		/// to clear recv thread
		if ((nullptr != others.precv_data_)	|| (NULL != others.precv_data_))
		{
			thread_recv_is_running_			= false;
			thread_recv_.join();
			others.precv_data_				= nullptr;
		}

		/// -----------------------------------------------------------------
		/// 2. close socket
		if (0								< address._socket)
		{
			int ret_val = 0;
#if defined(os_is_win)
			ret_val							= closesocket(address._socket);
			WSACleanup();
#elif defined(os_is_linux)
			ret_val							= close(address._socket);
#endif // 
		}
		/// 套接字创建失败
		else
			;

		udp_config_.zero_();

		return 0;
	}



	/// --------------------------------------------------------------------------------------------------------
	///	@brief: 
	/// --------------------------------------------------------------------------------------------------------
	lib_udp::st_udp_config& lib_udp::udp_imp::get_udp_config_()
	{
		return udp_config_;
	}


	/// ---------------------------------------------------------------------------
	/// @brief: 返回错误代码
	/// ---------------------------------------------------------------------------
	const int lib_udp::udp_imp::get_error_id_()
	{
#if defined(os_is_win)
		return WSAGetLastError();
#elif defined(os_is_linux)
		return errno;
#endif // !_WIN32
	}



	/// ---------------------------------------------------------------------------
	/// @brief: 初始化套接字
	/// ---------------------------------------------------------------------------
	bool lib_udp::udp_imp::init_socket_()
	{

		udp_init_other_	&others				= udp_config_.others_;
		udp_init_adress& address			= udp_config_.address_;


		// 0. create socket
		// -------------------------------------------------------------------------------
#if defined(os_is_win)
		WSADATA wsadata;
		int ret									= WSAStartup(MAKEWORD(2, 2), &wsadata);
		if (0									!= ret)
			return false ;
#endif // !


		// 1. create socket
		address._socket							= socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		// 2. set adress family
		address._address_dest.sin_family		= AF_INET;
		address._address_local.sin_family		= AF_INET;
		address._address_local_any.sin_family	= AF_INET;

		if (0 > address._socket)
			return false;

		return true;
	}



	/// ---------------------------------------------------------------------------
	/// @brief: deconstructor
	/// ---------------------------------------------------------------------------
	lib_udp::udp_imp::~udp_imp()
	{
		// to check socket status, if its openning, close it 
		shutdown_();
	}



	/// --------------------------------------------------------------------------------------------------------
	///	@brief: 初始化
	/// --------------------------------------------------------------------------------------------------------
	int lib_udp::udp_imp::init_(const st_udp_init& udp_init, irecv_data* precv_data)
	{
		/// 防止多次初始化
		shutdown_();

		/// 初始化套接字
		bool init_result			= init_socket_();
		if (false					== init_result)
			return 13;

		/// 拷贝参数
		memcpy(&udp_config_.init_, &udp_init, sizeof(udp_init));

		
		// 1. initialize addresses 
		lib_udp::st_udp_init&param_base					= udp_config_.init_;
		lib_udp::udp_init_other& others					= udp_config_.others_;
		lib_udp::udp_init_adress& address				= udp_config_.address_;
		bool& is_log_debug								= param_base.is_log_debug_;

		address._address_dest.sin_port					= htons(param_base.dest_port_);
		address._address_local.sin_port					= htons(param_base.dest_port_);
		address._address_local_any.sin_port				= htons(param_base.dest_port_);

#if defined(os_is_win)

		inet_pton(AF_INET, param_base.dest_ip_.str_ip_.c_str(), &address._address_dest.sin_addr.S_un.S_addr);
		inet_pton(AF_INET, param_base.local_ip_.str_ip_.c_str(), &address._address_local.sin_addr.S_un.S_addr);
		address._address_local_any.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
#elif defined(os_is_linux)
		inet_pton(AF_INET, param_base.dest_ip_.str_ip_.c_str(), &address._address_dest.sin_addr.s_addr);
		inet_pton(AF_INET, param_base.local_ip_.str_ip_.c_str(), &address._address_local.sin_addr.s_addr);
		address._address_local_any.sin_addr.s_addr = htonl(INADDR_ANY);
#endif ///


		int ret_val										= 0;
		
		/// -----------------------------------------------------------------
		// 2. set socket timeout, include receiving and sending
		unsigned int time_out_send						= udp_config_.init_.time_out;
		if (0											< time_out_send)
		{
	
#if defined(os_is_win)
			ret_val										= setsockopt(address._socket, SOL_SOCKET, SO_SNDTIMEO, (const char *)(&time_out_send), sizeof(time_out_send));
#elif defined(os_is_linux)
			struct timeval time_out;
			time_out.tv_sec								= time_out_send;
			time_out.tv_usec							= 0;
			ret_val										= setsockopt(address._socket, SOL_SOCKET, SO_SNDTIMEO, (const char *)(&time_out), sizeof(time_out));
#endif 
			if (0										!= ret_val)
				return 1;


#if defined(os_is_win)
			ret_val = setsockopt(address._socket, SOL_SOCKET, SO_RCVTIMEO, (const char *)(&time_out_send), sizeof(time_out_send));
#elif defined(os_is_linux)
			struct timeval time_out;
			time_out.tv_sec = time_out_send;
			time_out.tv_usec = 0;
			ret_val = setsockopt(address._socket, SOL_SOCKET, SO_RCVTIMEO, (const char *)(&time_out), sizeof(time_out));
#endif 
			if (0 != ret_val)
				return 13;

		}



		// 3. set the length of receiving and sending adn  set reuse adress
		const int len_buf_recv_send						= len_buf_send_recv_10240;
		ret_val											= setsockopt(address._socket, SOL_SOCKET, SO_SNDBUF, (const char *)&len_buf_recv_send, sizeof(len_buf_recv_send));
		if (0											!= ret_val)
			return 2;

		ret_val											= setsockopt(address._socket, SOL_SOCKET, SO_RCVBUF, (const char *)&len_buf_recv_send, sizeof(len_buf_recv_send));
		if (0											!= ret_val)
			return 3;

		// to set socket address resuse
#if defined(os_is_win)	
		char is_resuse_address							= 1;
		ret_val											= setsockopt(address._socket, SOL_SOCKET, SO_REUSEADDR, &is_resuse_address, sizeof(is_resuse_address));
#elif defined(os_is_linux) 
		int address_resuse								= 1;
		ret_val											= setsockopt(address._socket, SOL_SOCKET, SO_REUSEADDR, (void*)&address_resuse, sizeof(address_resuse));
#endif 

		// an error occured, log it
		if (0											!= ret_val)
			return 5;

		/// -------------------------------------------------------------------------------
		/// 4.bind local adress
		ret_val											= bind(address._socket, (struct sockaddr *)&(address._address_local), sizeof(address._address_local));
		if (0											!= ret_val)
			return 7;


		/// -------------------------------------------------------------------------------
		// 5.set udp type: unicast ? multiast ? 
		switch (param_base.cast_type_)
		{
			// multicast
		case lib_udp::kmulti_cast:
		{
			// 5.1. IP_MULTICAST_IF
			ret_val										= setsockopt(address._socket, IPPROTO_IP, IP_MULTICAST_IF, (const char *)&(address._address_local.sin_addr), sizeof(address._address_local.sin_addr));

			// failure
			if (0										!= ret_val)
				return 8;

			// success
			// 5.2.to join multcast group
			struct ip_mreq req_multi_cast;

			// local adress
			req_multi_cast.imr_interface				= address._address_local.sin_addr;
			// destination adress
			req_multi_cast.imr_multiaddr				= address._address_dest.sin_addr;

			ret_val										= setsockopt(address._socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char*)&req_multi_cast, sizeof(req_multi_cast));
			// failure
			if (0 != ret_val)
				return 9;


			// 5.3. whether receiving msg that you sent to the adress 
			const char is_recv_my_self					= param_base.recv_loop_ ? 1 : 0;
			ret_val										= setsockopt(address._socket, IPPROTO_IP, IP_MULTICAST_LOOP, &is_recv_my_self, sizeof(is_recv_my_self));

			// failure
			if (0										!= ret_val)
				return 10;
		}
		break;

		case lib_udp::kbroad_cast:
		{
			const char use_broad_cast					= 1;
			ret_val										= setsockopt(address._socket, SOL_SOCKET, SO_BROADCAST, &use_broad_cast, sizeof(use_broad_cast));
			if (0										!= ret_val)
				return 11;
		}
		break;

		// do nothing
		case lib_udp::kuni_cast:
			break;

			// other . error
		default:
			ret_val										= 12;
			break;
		}



		// -------------------------------------------------------------------------------
		// 6. to prepare params of recv data
		if ((NULL != precv_data)						&& (nullptr != precv_data))
		{
			others.precv_data_							= precv_data;

			// 6.1 create a thead to recv data
			thread_recv_is_running_						= true;
			thread_recv_								= std::thread(thread_recv_data_, (void*)this);
		}

		return ret_val;
	}


	/// --------------------------------------------------------------------------------------------------------
	///	@brief: 发送数据
	/// --------------------------------------------------------------------------------------------------------
	int udp_imp::send_(const unsigned char *pdata, const unsigned int pdata_len)
	{
		/// 参数check
		if ((NULL == pdata) || (nullptr == pdata) || (0 == pdata_len))
			return 1;


		static const int max_send_length		= len_buf_send_recv_10240;
		/// 发送长度过于长
		if (max_send_length						< pdata_len)
			return 2;

		/// 通道没有初始化成功
		if (0 >= udp_config_.address_._socket)
			return 3;

		static const  int len_addr				= sizeof(udp_config_.address_._address_dest);
		/// to send data
		int	ret_val								= sendto(udp_config_.address_._socket, (const char*)pdata, pdata_len, 0, (struct sockaddr*)&(udp_config_.address_._address_dest), len_addr);

		// sending failure
		if (-1									== ret_val)
			return 5;

		return 0;
	}

}