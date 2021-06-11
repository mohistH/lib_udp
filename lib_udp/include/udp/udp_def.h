#ifndef _typedef_h
#define _typedef_h


#ifdef _WIN32
#include <WinSock2.h>
#include <Ws2tcpip.h>


#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

#endif /// !_WIN32



#include <memory>
#include <queue>	/// to recvdata
#include <iudp.h>

/// -------------------------------------------------------------------------------
///   23-08-2020   to use spdlog. 
///#define _use_spdlog_

/// to output debug information
#define _use_debug_

namespace lib_udp
{
	///
	/// -------------------------------------------------------------------------------
	enum 
	{
		/// recv_buf_len
		len_buf_send_recv_10240 = 1024 * 10,

		/// 
		len_buf_1024_3 			= 1024 * 3,

		/// 10k , buffer to recv data
		queue_recv_size_10 		= 10,

		max_events_10			= 10,
	};


	/// -------------------------------------------------------------------------------
	/// recv buffer
	struct st_udp_recv_buf_
	{
	public:
		///
		char _arr_data[len_buf_1024_3] = { 0 };

		void zero()
		{
			memset(_arr_data, 0, len_buf_1024_3);
		}

		st_udp_recv_buf_()
		{
			zero();
		}
	};
	using udp_recv_buf = st_udp_recv_buf_;


	/// ----------------------------------------------------------------------------
	/// @brief: 本机Ipv4地址
	/// ----------------------------------------------------------------------------
	struct st_address_ipv4_
	{
		/// the destination adress 
		struct sockaddr_in		dest_;

		/// local adress
		struct sockaddr_in		local_;

		/// any of local adresses
		struct sockaddr_in		local_any_;

		void zero_()
		{
			memset(this, 0, sizeof(st_address_ipv4_));
		}

		st_address_ipv4_()
		{
			zero_();
		}
	};

	using address_ipv4 = st_address_ipv4_;

	/// ----------------------------------------------------------------------------
	/// @brief: 本机IPv6
	/// ----------------------------------------------------------------------------
	struct st_address_ipv6_
	{
		/// the destination adress 
		struct sockaddr_in6		dest_;

		/// local adress
		struct sockaddr_in6		local_;

		/// any of local adresses
		struct sockaddr_in6		local_any_;

		void zero_()
		{
			memset(this, 0, sizeof(st_address_ipv6_));
			dest_.sin6_family			= AF_INET6;
			local_.sin6_family			= AF_INET6;
			local_any_.sin6_family		= AF_INET6;
		}

		st_address_ipv6_()
		{
			zero_();
		}
	};

	using address_ipv6 = st_address_ipv6_;


	/// 2. socket adress
	/// -------------------------------------------------------------------------------
	struct st_udp_init_adress_
	{
		/// the socket
		int						socket_ = 0;

		bool					socket_is_openned_ = false;

		union 
		{
			address_ipv4	ipv4_;
			address_ipv6	ipv6_;
		};

#ifdef use_old_address
		/// the destination adress 
		struct sockaddr_in		address_dest_;

		/// local adress
		struct sockaddr_in		address_local_;

		/// any of local adresses
		struct sockaddr_in		address_local_any_;
#endif /// 

		void zero()
		{
			memset(this, 0, sizeof(st_udp_init_adress_));
		}

		st_udp_init_adress_()
		{
			zero();
		}

	};
	using udp_init_address = st_udp_init_adress_;



	/// 3.
	/// -------------------------------------------------------------------------------
	struct st_udp_init_other_
	{
		irecv_data*		pfunc_recv_data_;

		void zero()
		{
			pfunc_recv_data_ = nullptr;
		}

		st_udp_init_other_()
		{
			zero();
		}
	};
	using udp_init_other = st_udp_init_other_;


	/// 4. to initialize udp params 
	/// -------------------------------------------------------------------------------
	struct st_udp_param_init_
	{
		/// adress 
		st_udp_init_adress_		address_;

		/// other parameters
		udp_param				param_;

		/// others
		st_udp_init_other_		others_;


		void zero()
		{
			address_.zero();
			param_.zero();
			others_.zero();
		}

		st_udp_param_init_()
		{
			zero();
		}
	};
	using udp_param_init = st_udp_param_init_;


	/// to recv data, use this struct to save data 
	struct st_udp_recv_data_buf_
	{
		/// data to save
		char	buf_[len_buf_1024_3 + 1] = { 0 };
		/// the valid length _buf
		int		buf_len_valid_ = 0;

		void zero()
		{
			memset(this, 0, sizeof(st_udp_recv_data_buf_));
		}

		st_udp_recv_data_buf_()
		{
			zero();
		}
	};
	using udp_recv_data_buf = st_udp_recv_data_buf_;
	using queue_udp_recv = std::queue<udp_recv_data_buf>;

	/// epoll pramaters of epoll
	/// -------------------------------------------------------------
#ifdef __linux__
	// struct udp_epoll_event_
	// {
	// public:
	// 	int _epoll_fd;
	// 	struct epoll_event _event;
    // 	struct epoll_event _event_wait[max_events_10];

	// 	void zero()
	// 	{
	// 		bzero(this, sizeof(udp_epoll_event_));
	// 	}

	// 	udp_epoll_event_ ()
	// 	{
	// 		zero();
	// 	}
	// };
	// using udp_epoll_event = udp_epoll_event_;
#endif ///!__linux__


}



#endif ///!_typedef_h