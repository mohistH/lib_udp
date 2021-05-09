#ifndef _typedef_h
#define _typedef_h

#include "universe_def.h"

#if defined(os_is_win)
#include <WinSock2.h>
#include <Ws2tcpip.h>

#elif defined (os_is_linux)
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

#endif // 

#include <memory>
#include "iudp.h"


namespace lib_udp
{
	//
	// -------------------------------------------------------------------------------
	enum
	{
		// recv_buf_len
		len_buf_send_recv_10240 = 1024 * 10,

		// 
		len_buf_1024_3 = 1024 * 3,

		// 10k , buffer to recv data
		queue_recv_size_10 = 10,

		max_events_10 = 10,
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
		irecv_data*		precv_data_;

		void zero()
		{
			precv_data_ = nullptr;
		}

		udp_init_other_()
		{
			zero();
		}
	};
	using udp_init_other = udp_init_other_;


	// 4. to initialize udp params 
	// -------------------------------------------------------------------------------
	struct st_udp_config
	{
		// adress 
		udp_init_adress_			address_;

		// other parameters
		st_udp_init					init_;

		// others
		udp_init_other_				others_;


		void zero_()
		{
			address_.zero();
			init_.zero();
			others_.zero();
		}

		st_udp_config()
		{
			zero_();
		}
	};
	using udp_param_config = st_udp_config;

}



#endif //!_typedef_h