
#ifndef _udp_socket_imp_h
#define _udp_socket_imp_h

#include <udp/udp_def.h>

#include <thread>
#include <mutex>

// to log file by spdlog
#ifdef _use_spdlog_
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#endif // !_use_spdlog_






namespace lib_udp
{


	void thread_recv_data(void* param);
	void thread_get_data_from_recv_queue(void *param);

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	
	/*
	*	@brief:  
	*/
	class udp_socket_imp : public udpsocket
	{
	public:
		explicit udp_socket_imp();
		virtual ~udp_socket_imp();

		udp_socket_imp(const udp_socket_imp& instance) = delete;
		udp_socket_imp& operator = (const udp_socket_imp& instance) = delete;

		udp_socket_imp(udp_socket_imp&& instance) = delete;
		udp_socket_imp& operator=(udp_socket_imp&& instance) = delete;
		// -------------------------------------------------------------------------------

		/* @ brief: to check parameters
		*  @ upd_param & param - parameters of udp
		*  @ return - int
				0 - success
				1 - faluere, param._port_dst�� is zero.
				2 - faluere, param._pip4_dst��'s length is less than 7
		*/
		int init_ip4(udp_param &param) ;




		/* @ brief: initialize socket of udp
		*  @ const unsigned int time_out_send - sending time out. 
		*  @ return - int
				0 - success
				20086 - failure, the parameter's type of udp_cast_type gets wrong
				X - faluere, the error's id to call  setsockopt function
		*/
		int open(const unsigned int time_out_send, udpsocket_recv* pfunc_recv);



		/* @ brief: send data after initializing success
		*  @ const char * psend - the data to send
		*  @ const int len_send - the length of sending data
		*  @ return - int
				-20086 - failure, initialize socket failure
				X> 0 - success, X is length of sending success
				X < 0 - failure, X is error msg's id  of calling sendto
				-20087 - psend isnull  [or] len_send is larger than 65535-20-8 [or] strlen(psend) is larger than 65535-20-8
		*/
		int send(const char *psend, const unsigned int len_send);



		/* @ brief: close socket
		*  @ return - int
				0 - success 
				-20086 - failure, udp doesnt open
				X - failure, error msg's id
		*/
		int shutdown() ;

		int uninit();

		// -------------------------------------------------------------------------------
		// to service thread to define the following functions
		// -------------------------------------------------------------------------------
		udp_param_init& get_param_init() { return _udp_param_init; }

#ifdef _use_spdlog_
		//  		 [ 23-08-2020 ] 
		// -------------------------------------------------------------------------------
		template<typename FormatString, typename... Args>
		void log_info(const FormatString &fmt, const Args &... args)
		{
			if (_udp_param_init._param._is_log_debug)
				if (_plog_file)
					_plog_file->info(fmt, args...);
		}
#endif // !_use_spdlog_

		const int get_error_id();

		bool get_thread_recv_is_running() { return _thread_recv_is_running; }
		queue_udp_recv& get_queue_recv() { return _queue_recv; }
		std::mutex& get_mutex_queue_recv(){ return _mutex_queue_recv; }

		bool& get_thread_get_recv_is_running() { return _thread_get_recv_is_running; }
	protected:
		void init_params();





	private:
		udp_param_init			_udp_param_init;

#ifdef _use_spdlog_
		// logger 
		std::shared_ptr<spdlog::logger>	_plog_file = nullptr;
#endif //!
		
		// -------------------------------------------------------------------------------
		// this thread is to recv data 
		std::thread				_thread_recv;
		bool					_thread_recv_is_running = false;

		
		// -------------------------------------------------------------------------------
		//   23-08-2020   this thread is to pop data 
		std::thread				_thread_get_recv;
		bool					_thread_get_recv_is_running = false;


		// use queue to recv data, his size is 2;
		queue_udp_recv	_queue_recv;
		// to lock _queue_recv
		std::mutex				_mutex_queue_recv;

	};
}


#endif // !_udp_socket_imp_h
