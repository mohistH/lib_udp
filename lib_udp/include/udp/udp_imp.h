
#ifndef _udp_socket_imp_h
#define _udp_socket_imp_h

#include "udp_def.h"
#include <thread>
#include <mutex>
#include "iudp.h"

namespace lib_udp
{

	/// ---------------------------------------------------------------------------
	/// @brief: UDP派生实现
	/// ---------------------------------------------------------------------------
	class udp_imp : public iudp
	{
	public:
		explicit udp_imp();
		virtual ~udp_imp();


		///  --------------------------------------------------------------------------------
		///  @brief: 	初始化
		///  @param:	const st_udp_init & udp_init - 初始化参数 
		///  @param:	irecv_data * precv_data - 接收数据对象
		///  @return:	int	
		/// 			 0 - 成功
		///				
		///  --------------------------------------------------------------------------------
		virtual int init_(const st_udp_init& udp_init, irecv_data* precv_data);


		///  --------------------------------------------------------------------------------
		///  @brief: 	发送数据
		///  @param:	const unsigned char * pdata - 待发送数据
		///  @param:	const unsigned int pdata_len - 待发送数据长度
		///  @return:	int	
		/// 			 0 - 成功
		///				1 - 失败
		///  --------------------------------------------------------------------------------
		virtual int send_(const unsigned char *pdata, const unsigned int pdata_len);



		///  --------------------------------------------------------------------------------
		///  @brief: 	请显示调用该函数完成UDP的释放
		///  @return:	int	
		/// 			0 - 成功
		///  --------------------------------------------------------------------------------
		virtual int shutdown_();


		/// -------------------------------------------------------------------------------
		/// @brief: to service thread to define the following functions
		/// -------------------------------------------------------------------------------
		udp_param_config&		get_udp_config_();


		template<typename FormatString, typename... Args>
		void log_info_(const FormatString &fmt, const Args &... args)
		{
#ifdef _use_spdlog_
			if (udp_config_._param._is_log_debug)
				if (plog_file_)
					plog_file_->info(fmt, args...);
#endif // !_use_spdlog_

		}


	
		///  --------------------------------------------------------------------------------
		///  @brief: 	返回错误ID
		///  @return:	const int	
		/// 			错误ID
		///  --------------------------------------------------------------------------------
		const int get_error_id_();

		bool get_thread_recv_is_running_() { return thread_recv_is_running_; }
		//queue_udp_recv& get_queue_recv_() { return _queue_recv; }
		//std::mutex& get_mutex_queue_recv_(){ return mutex_queue_recv_; }

		//bool& get_thread_get_recv_is_running_() { return thread_get_recv_is_running_; }


	////----------------------------------------------------------------------------------------

		static void thread_recv_data_(void* param);
	//	static void thread_get_data_from_recv_queue_(void *param);

	protected:
		bool init_socket_();



	private:
		/// UDP配置初始化
		udp_param_config				udp_config_;
		
		/// 接收数据的线程
		std::thread						thread_recv_;
		bool							thread_recv_is_running_ = false;
	};
}


#endif // !_udp_socket_imp_h
