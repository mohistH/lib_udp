
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




//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	
	/*
	*	@brief:  
	*/
	class udp_socket_imp : public udp_socket_interface
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
		int init_ip4_(udp_param &param) ;




		/* @ brief: initialize socket of udp
		*  @ const unsigned int time_out_send - sending time out. 
		*  @ return - int
				0 - success
				20086 - failure, the parameter's type of udp_cast_type gets wrong
				X - faluere, the error's id to call  setsockopt function
		*/
		int open_(const unsigned int time_out_send, irecv_data_interface* pfunc_recv);



		/* @ brief: send data after initializing success
		*  @ const char * psend - the data to send
		*  @ const int len_send - the length of sending data
		*  @ return - int
				-20086 - failure, initialize socket failure
				X> 0 - success, X is length of sending success
				X < 0 - failure, X is error msg's id  of calling sendto
				-20087 - psend isnull  [or] len_send is larger than len_buf_send_recv_10240 [or] strlen(psend) is larger than len_buf_send_recv_10240
		*/
		int send_(const char *psend, const unsigned int len_send);



		/* @ brief: close socket
		*  @ return - int
				0 - success 
				-20086 - failure, udp doesnt open
				X - failure, error msg's id
		*/
		int shutdown_() ;

		int uninit_();

		// -------------------------------------------------------------------------------
		// to service thread to define the following functions
		// -------------------------------------------------------------------------------
		udp_param_init& get_param_init_() { return udp_param_init_; }


		//  		 [ 23-08-2020 ] 
		// -------------------------------------------------------------------------------
		template<typename FormatString, typename... Args>
		void log_info_(const FormatString &fmt, const Args &... args)
		{
#ifdef _use_spdlog_
			if (udp_param_init_._param._is_log_debug)
				if (plog_file_)
					plog_file_->info(fmt, args...);
#endif // !_use_spdlog_

		}

		const int get_error_id_();

		bool get_thread_recv_is_running_() { return thread_recv_is_running_; }
		queue_udp_recv& get_queue_recv_() { return _queue_recv; }
		std::mutex& get_mutex_queue_recv_(){ return mutex_queue_recv_; }

		bool& get_thread_get_recv_is_running_() { return thread_get_recv_is_running_; }


	//----------------------------------------------------------------------------------------

		static void thread_recv_data_(void* param);
		static void thread_get_data_from_recv_queue_(void *param);

	protected:
		void init_params();



	private:
		udp_param_init			udp_param_init_;

#ifdef _use_spdlog_
		// logger 
		std::shared_ptr<spdlog::logger>	plog_file_ = nullptr;
#endif //!
		
		// -------------------------------------------------------------------------------
		// this thread is to recv data 
		std::thread				thread_recv_;
		bool					thread_recv_is_running_ = false;

		
		// -------------------------------------------------------------------------------
		//   23-08-2020   this thread is to pop data 
		std::thread				thread_get_recv_;
		bool					thread_get_recv_is_running_ = false;


		// use queue to recv data, his size is 2;
		queue_udp_recv	_queue_recv;
		// to lock _queue_recv
		std::mutex				mutex_queue_recv_;

	};
}


#endif // !_udp_socket_imp_h
