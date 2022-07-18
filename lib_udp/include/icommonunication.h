#ifndef icommunication_h
#define icommunication_h

#include <string.h>
#include <string>
#include <list>


namespace uv
{
	class CLooper;
};


namespace lib_commu
{

#if defined(_WIN32) || defined(_WIN64)

#ifndef lib_export
#define lib_export __declspec(dllexport)
#else
#define lib_export __declspec(dllimport)

#endif ///

#ifndef os_is_win
#define os_is_win
#endif ///

/// #elif defined(__linux__) || defined(__unix__)
#else

#ifndef lib_export
#define lib_export __attribute__((visibility("default")))
#endif ///

#ifndef os_is_linux
#define os_is_linux
#endif ///

#endif ///

#ifdef __cplusplus
	extern "C"
	{
#endif ///

		/// ----------------------------------------------------------------------------------------
		/// @brief: 保证对象只生成在堆上
		/// ----------------------------------------------------------------------------------------
		class UnCopyable
		{
		protected:
			UnCopyable() {}
			virtual ~UnCopyable() {}

		private:
			UnCopyable(const UnCopyable &);
			UnCopyable &operator=(const UnCopyable &);
		};

		/// ----------------------------------------------------------------------------
		/// @brief: 数据处理接口类
		/// ----------------------------------------------------------------------------
		class IDataDispatch : public UnCopyable
		{
		public:
			virtual ~IDataDispatch() {}

		public:
			/// ----------------------------------------------------------------------------
			/// @brief：		接收数据处理， 拒绝执行复杂的操作
			/// @param: 	const unsigned char * pdata - 收到数据
			/// @param: 	const unsigned int len - 收到数据长度
			/// @return: 	int 
			///					返回 0 即可
			/// ----------------------------------------------------------------------------
			virtual int RecvData(const char *pdata, const size_t len) = 0;
		};

		/// ----------------------------------------------------------------------------------------
		/// @brief: 发送结束后, 拒绝执行复杂的操作
		/// ----------------------------------------------------------------------------------------
		class SendResultNotify :public UnCopyable
		{
		public:
			virtual ~SendResultNotify() {}

		public:
			/// -------------------------------------------------------------------------------
			/// @brief:		发送结果通知函数
			/// @param: 	const bool send_ret - true=成功, false=失败
			///  @ret:		void
			///				
			/// -------------------------------------------------------------------------------
			virtual void AfterSend(const bool send_ret, const std::string& str_ret) = 0;
		};


		/// ----------------------------------------------------------------------------
		/// @brief: UDP类型
		/// ----------------------------------------------------------------------------
		enum CAST_TYPE
		{
			CT_NONE = 0,
			/// 单播
			CT_UNI = 1,
			/// 组播
			CT_MULTI = 2,
			/// 广播
			CT_BROAD = 3,
		};

		/// ----------------------------------------------------------------------------
		/// @brief: socket版本
		/// ----------------------------------------------------------------------------
		enum SOCKET_VER
		{
			/// ipv6
			SOCKET_VERSION_6 = 1,
			/// ipv4
			SOCKET_VERSION_4 = 2,
		};


		/// ----------------------------------------------------------------------------
		/// @brief: UDP初始化参数
		/// ----------------------------------------------------------------------------
		struct UDPInit_
		{
		public:
			/// 组播类型
			int cast_type_;
			/// 本机IP
			// ip_config	local_ip_;
			std::string		local_ip_;
			/// 目标IP
			// ip_config	target_ip_;
			std::string		dest_ip_;
			/// 目标端口
			size_t			dest_port_;

			/// socket版本
			int socket_ver_;

			/// 接收数据对象
			IDataDispatch*	recv_data_;
			/// 数据发送通知函数
			SendResultNotify* send_notify_;

			void Zero()
			{
				/// 默认单播
				cast_type_	= CT_UNI;
				local_ip_	= std::string("127.0.0.1");
				dest_ip_	= std::string("127.0.0.1");
				dest_port_	= 27500;
				socket_ver_ = SOCKET_VERSION_4;
				recv_data_	= nullptr;
				send_notify_ = nullptr;
			}

			UDPInit_()
			{
				Zero();
			}
		};

		typedef UDPInit_ UDPInit;



		/// ----------------------------------------------------------------------------
		/// @brief: udp通信接口类
		/// ----------------------------------------------------------------------------
		class IUDP : public UnCopyable
		{
		public:
			virtual ~IUDP() {}

			/// ----------------------------------------------------------------------------
			/// @brief：		初始化
			/// @param: 	const udp_init_params & param - param
			/// @param: 	idata_handle * pdata_handle - 接收数据， 如果不需要，则传 nullptr
			/// @return: 	int 
			///					0 - 成功
			///					1 - 失败， 内部错误，请调用 [GetLastErrorMsg] 获取错误信息
			///					2 - 失败， 内部错误，请调用 [GetLastErrorMsg] 获取错误信息
			///					3 - 失败， 内部错误，请调用 [GetLastErrorMsg] 获取错误信息
			///					5 - 失败， 内部错误，请调用 [GetLastErrorMsg] 获取错误信息
			/// ----------------------------------------------------------------------------
			virtual int Init(const UDPInit &param) = 0;

			/// ----------------------------------------------------------------------------
			/// @brief：		发送数据 如果发送数据长度超过65507， 则返回错误
			///				注意：
			///				1. 局域网-	UDP，用sendto函数最大能发送数据的长度为：65535- IP头(20) - UDP头(8)＝65507字节。用sendto函数发送数据时，
			///							如果发送数据长度大于该值，则函数会返回错误。
			///				2. 互联网-	建议在进行Internet(非局域网)的UDP编程时. 最好将UDP的数据长度控件在548字节(576-8-20)以内.
			///							
			///				避免数据分包， 建议不要超过（MTU-20）字节 
			/// 
			/// @param: 	const char * pdata - 待发送数据
			/// @param: 	const size_t len - 发送数据长度
			/// @return: 	int 
			///					0 - 已经将结果投递到发送缓冲区
			///					1 - 失败，参数 [len] 大于 65507
			///					3 - 失败， [Init] 接口中的第一个参数.socket_ver参数传递错误
			///					2 - 失败， 内部错误，无法发送数据
			///					5 - 失败， 参数 [pdata] 为nullptr
			/// ----------------------------------------------------------------------------
			virtual int Send(const char* pdata, const size_t len) = 0;

			/// ----------------------------------------------------------------------------
			/// @brief：		退出前调用
			/// @return: 	int - 无
			///				0 - 成功
			/// ----------------------------------------------------------------------------
			virtual int UnInit() = 0;

			/// -------------------------------------------------------------------------------
			/// @brief:		获取errormsg
			///  @ret:		std::string
			///				错误内容
			/// -------------------------------------------------------------------------------
			virtual std::string GetLastErrorMsg() = 0;
		};

		/// ----------------------------------------------------------------------------
		/// @brief：		创建，失败，则返回nullptr
		/// @return: 	lib_export iudp* - 无
		///
		/// ----------------------------------------------------------------------------
		lib_export IUDP *NewUDP();

		/// ----------------------------------------------------------------------------
		/// @brief：		释放， 内部设置为nullptr再返回
		/// @param: 	iudp * pobj - pobj
		/// @return: 	lib_export iudp* - 无
		///
		/// ----------------------------------------------------------------------------
		lib_export IUDP *DeleteUDP(IUDP *pobj);






		/// ----------------------------------------------------------------------------------------
		/// @brief: 网卡信息
		/// ----------------------------------------------------------------------------------------
		struct InterfaceAddresses_
		{
		public:
			InterfaceAddresses_()
			{
				Zero();
			}

			void Zero()
			{
				m_name.clear();
				m_value.clear();
			}

			/// 网卡名
			std::string m_name;

			/// 网卡对应的IP
			std::string m_value;
		};

		typedef InterfaceAddresses_ InterfaceAddresses;

		typedef std::list<InterfaceAddresses> ListInterFaceAddresses;

		/// ----------------------------------------------------------------------------
		/// @brief: 网络适配器
		/// ----------------------------------------------------------------------------
		class lib_export BaseKits 
		{
		public:
			BaseKits();
			virtual ~BaseKits();

			/// ----------------------------------------------------------------------------
			/// @brief：		格式化字符串
			/// @param: 	const char * pformat - pformat
			/// @param: 	... -
			/// @return: 	std::string - 无
			///
			/// ----------------------------------------------------------------------------
			std::string Format(const char *pformat, ...);

			/// ----------------------------------------------------------------------------
			/// @brief：		读取本机所有IPv4
			/// @param: 	ListInterFaceAddresses & out_list - out_list
			/// @return: 	int - 无
			/// 			0 - 读取成功
			///
			/// ----------------------------------------------------------------------------
			void IPV4(ListInterFaceAddresses &out_list);

			/// ----------------------------------------------------------------------------
			/// @brief：		读取本机IPv6
			/// @param: 	ListInterFaceAddresses & out_list - out_list
			/// @return: 	int - 无
			/// 			0 - 读取成功
			/// ----------------------------------------------------------------------------
			void IPV6(ListInterFaceAddresses &out_list);

			/// -------------------------------------------------------------------------------
			/// @brief:		读取所有IP
			/// @param: 	ListInterFaceAddresses & out_list - 获取结果
			///  @ret:		int
			///				0 - 读取成功
			/// -------------------------------------------------------------------------------
			void IPAll(ListInterFaceAddresses &out_list);
		};

#ifdef __cplusplus
	}
#endif ///
}

#endif /// icommunication_h