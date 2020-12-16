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

	void win_thread_recv_data_(void *param);
	void win_thread_get_recv_data_(void *param);

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
		irecv_data_interface *_pfunc_recv;

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
	class udp_wsa_imp : public udp_socket_interface
	{
	public:
		explicit udp_wsa_imp();
		virtual ~udp_wsa_imp();

		udp_wsa_imp(const udp_wsa_imp& instance) = delete;
		udp_wsa_imp& operator = (const udp_wsa_imp& instance) = delete;
		udp_wsa_imp(const udp_wsa_imp&& instance) = delete;
		udp_wsa_imp& operator = (const udp_wsa_imp&& instance) = delete;
		// -------------------------------------------------------------------------------
		
		int init_ip4_(udp_param& param);
		int open_(const unsigned int time_out_send, irecv_data_interface* pfunc_recv = nullptr);
		int send_(const char *psend, const unsigned int len_send) ;
		int shutdown_();

		const bool get_thread_recv_is_running_() const { return thread_recv_is_running_; }
		udp_wsa_init& get_udp_wsa_init_() { return udp_init_; };

		const bool get_thread_get_recv_data_is_running_() const { return thread_get_recv_data_is_running_; }

		queue_udp_recv& get_wsa_queue_recv_() { return queue_recv_; }
		std::mutex& get_mutex_wsa_queue_recv_() { return mutex_wsa_queue_recv_; }

	protected:
		void init_env();

	private:
		udp_wsa_init	udp_init_;

		bool		thread_recv_is_running_ = false;
		std::thread	_thread_recv;

		bool		thread_get_recv_data_is_running_ = false;
		std::thread	thread_get_recv_data_;

		// use queue to recv data, his size is 2;
		queue_udp_recv	queue_recv_;
		// to lock _queue_recv
		std::mutex				mutex_wsa_queue_recv_;
	};

}

#endif // 


#endif // ! 