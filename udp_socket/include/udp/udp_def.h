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

#endif // !_WIN32



#include <memory>
#include <queue>	// to recvdata
#include <udp/udp_interface.h>

// -------------------------------------------------------------------------------
//   23-08-2020   to use spdlog. 
#define _use_spdlog_

// to output debug information
#define _use_debug_

namespace lib_udp
{
	//
	// -------------------------------------------------------------------------------
	enum 
	{
		// recv_buf_len
		len_buf_send_recv_10240 = 1024 * 10,

		// 
		len_buf_1024_3 			= 1024 * 3,

		// 10k , buffer to recv data
		queue_recv_size_10 		= 10,

		max_events_10			= 10,
	};


	// -------------------------------------------------------------------------------
	// recv buffer
	struct udp_recv_buf_
	{
	public:
		//
		char _arr_data[len_buf_1024_3] = { 0 };

		void zero()
		{
			memset(_arr_data, 0, len_buf_1024_3);
		}

		udp_recv_buf_()
		{
			zero();
		}
	};
	using udp_recv_buf = udp_recv_buf_;



	// 2. socket adress
	// -------------------------------------------------------------------------------
	struct udp_init_adress_
	{
		// the socket
		int						_socket = 0;

		bool					_socket_is_open = false;

		// the destination adress 
		struct sockaddr_in		_address_dest;

		// local adress
		struct sockaddr_in		_address_local;

		// any of local adresses
		struct sockaddr_in		_address_local_any;

		void zero()
		{
			memset(this, 0, sizeof(udp_init_adress_));
		}

		udp_init_adress_()
		{
			zero();
		}

	};
	using udp_init_adress = udp_init_adress_;



	// 3.
	// -------------------------------------------------------------------------------
	struct udp_init_other_
	{
		udpsocket_recv*		_pfunc_recv_data;

		void zero()
		{
			_pfunc_recv_data = nullptr;
		}

		udp_init_other_()
		{
			zero();
		}
	};
	using udp_init_other = udp_init_other_;


	// 4. to initialize udp params 
	// -------------------------------------------------------------------------------
	struct udp_param_init_
	{
		// adress 
		udp_init_adress_		_adress;

		// other parameters
		udp_param			_param;

		// others
		udp_init_other_		_others;


		void zero()
		{
			_adress.zero();
			_param.zero();
			_others.zero();
		}

		udp_param_init_()
		{
			zero();
		}
	};
	using udp_param_init = udp_param_init_;


	// to recv data, use this struct to save data 
	struct udp_recv_data_buf_
	{
		// data to save
		char	_buf[len_buf_1024_3 + 1] = { 0 };
		// the valid length _buf
		int		_buf_len_valid = 0;

		void zero()
		{
			memset(this, 0, sizeof(udp_recv_data_buf_));
		}

		udp_recv_data_buf_()
		{
			zero();
		}
	};
	using udp_recv_data_buf = udp_recv_data_buf_;
	using queue_udp_recv = std::queue<udp_recv_data_buf>;

	// epoll pramaters of epoll
	// -------------------------------------------------------------
#ifdef __linux__
	struct udp_epoll_event_
	{
	public:
		int _epoll_fd;
		struct epoll_event _event;
    	struct epoll_event _event_wait[max_events_10];

		void zero()
		{
			bzero(this, sizeof(udp_epoll_event_));
		}

		udp_epoll_event_ ()
		{
			zero();
		}
	};
	using udp_epoll_event = udp_epoll_event_;
#endif //!__linux__


}



#endif //!_typedef_h