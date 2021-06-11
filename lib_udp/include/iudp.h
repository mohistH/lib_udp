
#ifndef  iudp_h
#define  iudp_h

#include < string.h>
#include <string>


#if defined (_WIN32) || defined(_WIN64)

#ifndef _oct_udp_api_export_
	#define lib_udp_api		__declspec(dllexport)
#else 
	#define lib_udp_api		__declspec(dllimport)

#endif /// 


/// #elif defined(__linux__) || defined(__unix__) 
#else 

#ifndef _oct_udp_api_export_
	#define lib_udp_api		__attribute__((visibility("default")))
#endif /// 

#endif /// 
/// -----------------------------------------------------------------


namespace lib_udp 
{

	/// -----------------------------------------------------------------
	/// IP版本
	/// -----------------------------------------------------------------
	enum en_ip_version
	{
		kipv4 = 1,
		kipv6 = 2,
	};

	/// ---------------------------------------------------------------------------
	/// @brief: udp 初始化类型
	/// ---------------------------------------------------------------------------
	enum cast_type
	{
		udp_multi_cast	= 1,
		udp_uni_cast	= 2,
		udp_broad_cast	= 3,
	};

# pragma pack(1)
	struct st_ip_config_
	{
		/// 版本
		unsigned int ver_;
		/// 值
		std::string value_;

		void zero_()
		{
			ver_	= kipv4;
			value_.clear();
		}
		st_ip_config_()
		{
			zero_();
		}
	};

	using ip_config = st_ip_config_;


	/// ---------------------------------------------------------------------------
	/// @brief: udp 模块初始化参数
	/// ---------------------------------------------------------------------------
	struct st_udp_param_
	{
		/// 接收自己发出的消息
		bool	_recv_loop;
		/// 调试使用, 输出到console
		bool	_is_log_debug ;
		/// 组播类型
		cast_type	_cast_type;
		/// 目标端口
		unsigned int _port_dst;
		/// 接收超时
		unsigned int recv_timeout_;
		/// 发送超时
		unsigned int send_timeout_;
		/// 目标IP
		ip_config	dest_ip_;
		/// 本机IP
		ip_config	local_ip_;

		/// IP4还是ipv6, 见 【en_ip_ver】定义
		int socket_version_;

		void zero()
		{
			_recv_loop		= true;
			_is_log_debug	= false;
			_cast_type		= udp_multi_cast;
			_port_dst		= 10086;
			dest_ip_.value_	= std::string("230.0.0.11");
			local_ip_.value_	= std::string("127.0.0.1");
			recv_timeout_		= 1000;
			send_timeout_		= 1000;
			socket_version_		= kipv4;
		}

		st_udp_param_()
		{
			zero();
		}
	};

	using udp_param = st_udp_param_;


#pragma  pack()



	/// ---------------------------------------------------------------------------
	/// @brief: 接收数据必须要实现该类的接收函数
	/// ---------------------------------------------------------------------------
	class irecv_data
	{
	public:
		virtual ~irecv_data(){}

		///  --------------------------------------------------------------------------------
		///  @brief: 	接收函数，
		///  @param:	const unsigned char * pdata_recv - 接收的数据
		///  @param:	const unsigned int recv_data_len - 接收数据长度
		///  @return:	void	
		/// 			
		///  --------------------------------------------------------------------------------
		virtual void on_recv_data_(const unsigned char *pdata_recv, const unsigned int recv_data_len) = 0;
	};



	/// ----------------------------------------------------------------------------
	/// @brief: UDP接口类
	/// ----------------------------------------------------------------------------
	class iudp
	{
	public:
		virtual ~iudp(){}

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
		virtual int init_(const udp_param& param, irecv_data* pfunc_recv /* = nullptr */) = 0;
	

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
		virtual int send_(const unsigned char* psend, const unsigned int send_len) = 0;
		
		///  --------------------------------------------------------------------------------
		///  @brief: 	关闭
		///  @return:	int	
		/// 			0 - 成功
		///				其他， 失败
		///  --------------------------------------------------------------------------------
		virtual int shutdown_() = 0;

		/// ------------------------------------------------------------
		/// @brief：返回错误ID
		/// @return: int
		/// 			
		/// ------------------------------------------------------------
		virtual int error_id_() = 0;

	};


#ifdef __cplusplus
extern "C" {
#endif ///

	///  --------------------------------------------------------------------------------
	///  @brief: 	创建 udp对象
	///  @return:	lib_udp		*	
	/// 			NULL- 创建失败
	///				!= null -  成功
	///  --------------------------------------------------------------------------------
	lib_udp_api iudp		* udp_create_();

	///  --------------------------------------------------------------------------------
	///  @brief: 	释放申请的资源， 内部释放后，见其设置为NULL
	///  @param:	iudp * pudp - 来自【udp_create_】的创建结果
	///  @return:	lib_udp_api lib_udp *	
	/// 			pudp = NULL   
	///  --------------------------------------------------------------------------------
	lib_udp_api iudp * udp_release_(iudp* pudp);



#ifdef __cplusplus
}
#endif ///

}





#endif /// 