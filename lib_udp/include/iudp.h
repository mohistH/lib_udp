#ifndef _udp_socket_interface_h
#define _udp_socket_interface_h

#include <string.h>
#include <string>



#if defined(_WIN32) || defined(_WIN64)

#ifndef lib_udp_api
#define lib_udp_api		__declspec(dllexport)
#else
#define lib_udp_api		__declspec(dllimport)
#endif //! lib_udp_api

#elif defined (__linux) || defined(_linux_) || defined(__unix) || defined(_unix_)

#ifndef lib_udp_api
#define lib_udp_api		__attribute__((visibility("default")))
//#define lib_udp_api		__attribute__ ((visibility("default")))
#endif // !lib_udp_api

#endif // !_WIN32



namespace lib_udp
{

	/// ---------------------------------------------------------------------------
	/// @brief: UDP类型
	/// ---------------------------------------------------------------------------
	enum cast_type
	{
		kmulti_cast	= 1,	/// multi cast
		kuni_cast	= 2,	/// uni cast
		kbroad_cast	= 3,	/// broad cast
	};

	/// ---------------------------------------------------------------------------
	/// @brief:IP版本
	/// ---------------------------------------------------------------------------
	enum en_ip_version
	{
		kipv4 = 1,
		kipv6 = 2,
	};

	/// ---------------------------------------------------------------------------
	/// @brief: IP
	/// ---------------------------------------------------------------------------
	struct st_ip_
	{
		/// IP
		std::string		str_ip_;
		/// 版本
		int				version_;

		void zero_()
		{
			str_ip_.clear();
			version_ = kipv4;
		}

		st_ip_()
		{
			zero_();
		}
	};

	/// IP 
	typedef st_ip_ st_ip;
	

	/// ---------------------------------------------------------------------------
	/// @brief: some parameters of initialization
	/// ---------------------------------------------------------------------------
	struct st_udp_init_
	{
	public:
		// Whether receive message that you sent
		bool	recv_loop_ = false;

		// log file of debugging
		bool	is_log_debug_ = false;

		// which type of udp do you wanna use
		cast_type cast_type_ = kmulti_cast;

		//  the port of destination
		unsigned short	dest_port_ = 20086;

		/// 目标Ip
		st_ip		dest_ip_;
	
		/// 本机IP
		st_ip		local_ip_;

		/// 超时设置
		unsigned int time_out = 1000;

	public:
		void zero()
		{
			recv_loop_ = true;
			is_log_debug_ = true;
			cast_type_ = kmulti_cast;
			dest_port_ = 20086;
			time_out = 1000;
			dest_ip_.zero_();
			local_ip_.zero_();

		}

		st_udp_init_()
		{
			zero();
		}
	};
	using st_udp_init = st_udp_init_;
	



	
	/// ---------------------------------------------------------------------------
	/// @brief: to recv data, you must inherit udpsocket_recv class
	/// ---------------------------------------------------------------------------
	class irecv_data
	{
	public:
		virtual ~irecv_data(){}

		/// @ brief: when socket has received data, it will call this function to pass data
		/// @ char * pdata_recv - data received
		/// @ unsigned int recv_data_len - the received length 
		/// @ return - void
		virtual void on_recv_data_(const unsigned char *pdata, const unsigned int pdata_len) = 0;
	};


	/// ---------------------------------------------------------------------------
	/// @brief: udp socket. it has interfaces of udp socket, including initialize, send and receive
	/// ---------------------------------------------------------------------------
	class iudp
	{
	public:
		virtual ~iudp(){}


		///  --------------------------------------------------------------------------------
		///  @brief: 	初始化
		///  @param:	const st_udp_init & udp_init - 初始化参数 
		///  @param:	irecv_data * precv_data - 接收数据对象
		///  @return:	int	
		/// 			 0 - 成功
		///				
		///  --------------------------------------------------------------------------------
		virtual int init_(const st_udp_init& udp_init, irecv_data* precv_data) = 0;


		///  --------------------------------------------------------------------------------
		///  @brief: 	发送数据
		///  @param:	const unsigned char * pdata - 待发送数据
		///  @param:	const unsigned int pdata_len - 待发送数据长度
		///  @return:	int	
		/// 			 0 - 成功
		///				1 - 失败
		///  --------------------------------------------------------------------------------
		virtual int send_(const unsigned char *pdata, const unsigned int pdata_len) = 0;


	
		///  --------------------------------------------------------------------------------
		///  @brief: 	请显示调用该函数完成UDP的释放
		///  @return:	int	
		/// 			0 - 成功
		///  --------------------------------------------------------------------------------
		virtual int shutdown_() = 0;
	};



	///  --------------------------------------------------------------------------------
	///  @brief: 	创建UDP对象
	///  @return:	extern " lib_udp_api iudp*	
	/// 			
	///  --------------------------------------------------------------------------------
	extern "C" lib_udp_api  iudp* create_();

	///  --------------------------------------------------------------------------------
	///  @brief: 	释放UDP
	///  @param:	iudp * pudp - 来自  create_ 创建结果
	///  @return:	extern " lib_udp_api iudp*	
	/// 			
	///  --------------------------------------------------------------------------------
	extern "C" lib_udp_api iudp* release_(iudp* pudp);

}

#endif /// 

