#ifndef _win_udp_h
#define _win_udp_h


#ifdef _WIN32

#pragma once
#include <udp/udp_def.h>
#include <thread>
#include <chrono>
#include <mutex>

#ifdef _use_spdlog_
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#endif // !_use_spdlog_


namespace lib_udp
{

	void win_thread_recv_data(void *param);
	void win_thread_get_recv_data(void *param);

#ifdef _use_old_
	// -------------------------------------------------------------------------------
	// 1. udp adress struct 
	struct udp_wsa_adress_
	{
		// socket
		int						_socket;

		// true: opened, false: closed
		bool					_socket_is_open;

		// the destination adress 
		struct sockaddr_in		_address_dest;

		// local adress
		struct sockaddr_in		_address_local;

		// any of local adresses
		struct sockaddr_in		_address_local_any;

		void zero()
		{
			memset(this, 0, sizeof(udp_wsa_adress_));
		}

		udp_wsa_adress_()
		{
			zero();
		}
	};
	using udp_wsa_adress = udp_wsa_adress_;
#endif // _use_old_


	// -------------------------------------------------------------------------------
	// 2. other parameters
	struct udp_wsa_init_other_
	{
	private:
		enum
		{
			len_arr_recv_1024_3							= 1024 * 10,
		};
		char			_arr_recv[len_arr_recv_1024_3]	= { 0 };

	public:
		// recv_interface
		udpsocket_recv *_pfunc_recv;

		// send buf
		WSABUF			_buf_send;

		// recv buf
		WSABUF			_buf_recv;

		// 
		WSAOVERLAPPED	_wsa_overlapped;

	

		void zero()
		{
			memset(this, 0, sizeof(udp_wsa_init_other_));

			_buf_recv.len = len_arr_recv_1024_3;
			_buf_recv.buf = (char *)_arr_recv;
		}

		udp_wsa_init_other_()
		{
			zero();
		}
	};
	using udp_wsa_init_other = udp_wsa_init_other_;



	// -------------------------------------------------------------------------------
	// 3. the class need parameters
	struct udp_wsa_init_
	{
		// 1. udp_param
		udp_param	_param;

		// 2. adress
		//udp_wsa_adress	_adress;
		udp_init_adress		_address;


		// 3. others 
		udp_wsa_init_other	_others;

		void zero()
		{
			_address.zero();
			_param.zero();
			_others.zero();
		}

		udp_wsa_init_()
		{
			zero();
		}
	};
	using udp_wsa_init = udp_wsa_init_;


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	
	/*
	*	@ brief: using wsaoverlapped model to create udp socket
	*/
	class udp_wsa : public udpsocket
	{
	public:
		explicit udp_wsa();
		virtual ~udp_wsa();

		udp_wsa(const udp_wsa& instance) = delete;
		udp_wsa& operator = (const udp_wsa& instance) = delete;
		udp_wsa(const udp_wsa&& instance) = delete;
		udp_wsa&& operator = (const udp_wsa&& instance) = delete;
		// -------------------------------------------------------------------------------
		
		int init_ip4(udp_param& param);
		int open(const unsigned int time_out_send, udpsocket_recv* pfunc_recv = nullptr);
		int send(const char *psend, const unsigned int len_send) ;
		int shutdown();

		const bool get_thread_recv_is_running() const { return _thread_recv_is_running; }
		udp_wsa_init& get_udp_wsa_init() { return _udp_init; };

		const bool get_thread_get_recv_data_is_running() const { return _thread_get_recv_data_is_running; }

		queue_udp_recv& get_wsa_queue_recv() { return _queue_recv; }
		std::mutex& get_mutex_wsa_queue_recv() { return _mutex_wsa_queue_recv; }

	protected:
		void init_env();

	private:
		udp_wsa_init	_udp_init;

		bool		_thread_recv_is_running = false;
		std::thread	_thread_recv;

		bool		_thread_get_recv_data_is_running = false;
		std::thread	_thread_get_recv_data;

		// use queue to recv data, his size is 2;
		queue_udp_recv	_queue_recv;
		// to lock _queue_recv
		std::mutex				_mutex_wsa_queue_recv;
	};

}

#endif // 


#endif // ! 