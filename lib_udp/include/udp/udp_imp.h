
#ifndef _udp_socket_imp_h
#define _udp_socket_imp_h

#include <udp/udp_def.h>

#include <thread>
#include <mutex>



namespace lib_udp
{

	
	/*
	*	@brief:  
	*/
	class udp_socket_imp : public iudp
	{
	public:
		explicit udp_socket_imp();
		virtual ~udp_socket_imp();

		udp_socket_imp(const udp_socket_imp& instance) = delete;
		udp_socket_imp& operator = (const udp_socket_imp& instance) = delete;

		udp_socket_imp(udp_socket_imp&& instance) = delete;
		udp_socket_imp& operator=(udp_socket_imp&& instance) = delete;
		

		
		/// ------------------------------------------------------------
		/// @brief：初始化
		/// @param: const udp_param & param - 初始化参数
		/// @param: irecv_data * pfunc_recv - 接收对象
		/// @return: int
		/// 			0 - 成功
		///				-1 - 失败，param.socket_version_ 传递错误
		/// --------------------------------------------------------------
		///			ipv4和ipv6错误， 请调用error_id_()获取错误代码
		///				1 - 失败， 端口为0
		///				2 - 失败,  套接字创建失败
		///				3 - 失败，设置发送超时失败
		///				5 - 失败, 设置接收超时失败
		///				6 - 失败, 设置发送缓冲失败
		///				7 - 失败，设置接收缓冲失败
		///				8 - 失败，设置地址宠用失败
		///				9 - 失败， 绑定套接字失败
		///				10 、11、12 - 失败， 设置套接字 组播属性失败
		///				13 - 失败，设置广播失败
		///				15 - 失败，param._cast_type参数值传递错误
		/// ------------------------------------------------------------
		int init_(const udp_param& param, irecv_data* pfunc_recv = nullptr) override;



		/// ------------------------------------------------------------
		/// @brief：发送数据
		/// @param: const unsigned char * psend - 待发送数据
		/// @param: const unsigned int len_send - 待发送数据长度
		/// @return: int
		/// 		-1 - 失败。初始化socket版本错误
		///			0 - 成功
		///			1 - 失败, 参数【psend】为空或 【len_max_send】等于0 或则len_max_send大于发送缓冲区长度(10k)
		///			2 - 失败，套接字创建失败
		///			3 - 失败， 发送数据失败, 请调用error_id_()
		/// ------------------------------------------------------------
		int send_(const unsigned char *psend, const unsigned int len_send) override;



		/// ------------------------------------------------------------
		/// @brief：关闭套接字
		/// @return: int
		/// 			0 - 成功
		/// ------------------------------------------------------------
		int shutdown_() override;

		/// ------------------------------------------------------------
		/// @brief：返回错误ID
		/// @return: int
		/// 			错误ID
		/// ------------------------------------------------------------
		int error_id_() override;



		/// -------------------------------------------------------------------------------
		/// to service thread to define the following functions
		/// -------------------------------------------------------------------------------
		udp_param_init& get_param_init_();


		///  		 [ 23-08-2020 ] 
		/// -------------------------------------------------------------------------------
		template<typename FormatString, typename... Args>
		void log_info_(const FormatString &fmt, const Args &... args)
		{
#ifdef _use_spdlog_
			if (udp_param_init_.param_._is_log_debug)
				if (plog_file_)
					plog_file_->info(fmt, args...);
#endif /// !_use_spdlog_

		}

		/// ------------------------------------------------------------
		/// @brief：返回错误代码
		/// @return: const int
		/// 			错误代码
		/// ------------------------------------------------------------
		const int get_error_id_();


		/// ------------------------------------------------------------
		/// @brief：返回线程结束标识
		/// @return: bool
		/// 			true - 结束
		///				false - 继续执行
		/// ------------------------------------------------------------
		bool get_thread_recv_is_running_();


		/// ------------------------------------------------------------
		/// @brief：接收数据函数
		/// @param: void * param - 参数
		/// @return: void
		/// 			
		/// ------------------------------------------------------------
		static void ipv4_thread_recv_data_func_(void* param);


		/// ------------------------------------------------------------
		/// @brief：接收数据函数
		/// @param: void * param - 
		/// @return: void
		/// 			
		/// ------------------------------------------------------------
		static void ipv6_thread_recv_data_func_(void* param);
	protected:

		///  --------------------------------------------------------------------------------
		///  @brief: 	create the socket 
		///  @param:	const int & ip_ver -  the socket's version 
		///  @return:	void	
		/// 			
		///  --------------------------------------------------------------------------------
		int init_socket_(const int& ip_ver);

		///  --------------------------------------------------------------------------------
		///  @brief: 	检查是IPv6还是IPV4
		///  @param:	const int & socket_ver - socket_version_ 参数值
		///  @return:	int	
		/// 			X > 0 - 参数是Ipv6或者IPv4
		///				X < 0 - 参数错误
		///  --------------------------------------------------------------------------------
		int check_socket_version_(const int& socket_ver);


	private:
		/// ------------------------------------------------------------
		/// @brief：初始化IPV4
		/// @return: int
		/// 			0 - 成功
		///				1 - 失败， 端口为0
		///				2 - 失败,  套接字创建失败
		///				3 - 失败，设置发送超时失败
		///				5 - 失败, 设置接收超时失败
		///				6 - 失败, 设置发送缓冲失败
		///				7 - 失败，设置接收缓冲失败
		///				8 - 失败，设置地址宠用失败
		///				9 - 失败， 绑定套接字失败
		///				10 、11、12 - 失败， 设置套接字 组播属性失败
		///				13 - 失败，设置广播失败
		///				15 - 失败，param._cast_type参数值传递错误
		/// ------------------------------------------------------------
		int init_ipv4_(const udp_param& param, irecv_data* pfunc_recv);

		/// ------------------------------------------------------------
		/// @brief：初始化IPv6
		/// @param: const udp_param & param - 初始化参数 
		/// @param: irecv_data * pfunc_recv - 接收对象
		/// @return: int
		/// 			0  - 成功
		/// ------------------------------------------------------------
		int init_ipv6_(const udp_param& param, irecv_data* pfunc_recv);

		/// ------------------------------------------------------------
		/// @brief：发送数据
		/// @param: const unsigned char * psend - 待发送数据
		/// @param: const unsigned int len_send - 待发送数据长度
		/// @return: int
		///			0 - 成功
		///			1 - 失败，psend为空或则len_send大于发送缓冲区大小 或则 len_send = 0
		///			2 - 失败，套接字创建失败
		///			3 - 失败，发送数据失败，请调用 error_id_()获取错误代码
		/// ------------------------------------------------------------
		int send_ipv4_(const unsigned char *psend, const unsigned int len_send);

		/// ------------------------------------------------------------
		/// @brief：发送数据
		/// @param: const unsigned char * psend - 待发送数据
		/// @param: const unsigned int len_send - 待发送数据长度
		/// @return: int
		///			0 - 成功
		///			1 - 失败，psend为空或则len_send大于发送缓冲区大小 或则 len_send = 0
		///			2 - 失败，套接字创建失败
		///			3 - 失败，发送数据失败，请调用 error_id_()获取错误代码
		/// ------------------------------------------------------------
		int send_ipv6_(const unsigned char *psend, const unsigned int len_send);


		/// ------------------------------------------------------------
		/// @brief: 初始化共有属性
		/// @param: const int socket - socket
		/// @param: const udp_param & param - 初始化参数 
		/// @return: int
		/// 			0 - 成功
		///				X - 失败
		/// ------------------------------------------------------------
		int init_common_properties_(const int& socket, const udp_param& param);

	private:
		/// 初始化参数
		udp_param_init			udp_param_init_;

		/// this thread is to recv data 
		std::thread				thread_recv_;

		/// 线程运行标识
		bool					thread_recv_is_running_ = false;
	};
}


#endif /// !_udp_socket_imp_h
