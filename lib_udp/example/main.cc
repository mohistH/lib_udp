#include <iostream>
//#include "uv.h"
#include "icommonunication.h"

#include <thread>
//#include "Looper.h"

/// ----------------------------------------------------------------------------


class my_udp : public lib_commu::IDataDispatch
{
public:
	/// ----------------------------------------------------------------------------
	/// @brief：		接收数据处理
	/// @param: 	const unsigned char * pdata - 收到数据
	/// @param: 	const unsigned int len - 收到数据长度
	/// @return: 	int - 
	/// 			
	/// ----------------------------------------------------------------------------
	virtual int RecvData(const char* pdata , const size_t len) 
	{
		std::cout << "\n\n\n data is coming, len        = " << len << "\n";
		std::cout << "\n                                =================== thread_id=" << std::this_thread::get_id() << "\n";
		return 0;
	}
};



/// ----------------------------------------------------------------------------------------
/// @brief:  继承 IDataDispatch  实现数据接收与发送结合
/// ----------------------------------------------------------------------------------------
class NetUDP : public lib_commu::IDataDispatch, public lib_commu::SendResultNotify
{
public:
	NetUDP()
	{
		pudp                                            = lib_commu::NewUDP();
	}

	virtual ~NetUDP()
	{
		UnInit();
	}


	/// --------------------------------------------------------------------------------
	/// @brief: RecvData
	/// --------------------------------------------------------------------------------
	virtual int RecvData(const char* pdata, const size_t len) override
	{
		//throw std::logic_error("The method or operation is not implemented.");
		std::cout << "\n rec vdata                      =";
		for (size_t index                               = 0; index < len; ++index)
		{
			std::cout << pdata[index] << " ";
		}

		return 0;
	}


	/// --------------------------------------------------------------------------------
	/// @brief: Init
	/// --------------------------------------------------------------------------------
	int Init(lib_commu::UDPInit& param) 
	{
		if (pudp)
		{
			param.send_notify_							= this;
			int ret                                     = pudp->Init(param);

			if (0 != ret)
			{
				std::cout << "\n init, errmsg           =" << pudp->GetLastErrorMsg().c_str() << "\n";
			}

			return ret;
		}

		return -1;
	}


	/// --------------------------------------------------------------------------------
	/// @brief: Send
	/// --------------------------------------------------------------------------------
	int Send(const char* pdata, const size_t len)
	{
		if (pudp)
		{
			int ret                                     = pudp->Send(pdata, len);

			if (0 != ret)
			{
				std::cout << "\n Send, errmsg           =" << pudp->GetLastErrorMsg().c_str() << "\n";
			}

			return ret;
		}

		return -1;
	}


	/// --------------------------------------------------------------------------------
	/// @brief: UnInit
	/// --------------------------------------------------------------------------------
	int UnInit() 
	{
		if (pudp)
		{
			try
			{
				pudp                                    = lib_commu::DeleteUDP(pudp);
			}
			catch (...)
			{
				;
			}
		}
		else
		{
			;
		}

		return 0;
	}


	/// --------------------------------------------------------------------------------
	/// @brief: AfterSend
	/// --------------------------------------------------------------------------------
	virtual void AfterSend(const bool send_ret, const std::string& str_ret) override
	{
		//throw std::logic_error("The method or operation is not implemented.");
		static int index = 0;
		std::cout << "\n========================index=" << ++index << ", ret=" << send_ret << ", str_ret=" << str_ret.c_str() << "\n";
	}

private:
	lib_commu::IUDP* pudp;

};



/// ----------------------------------------------------------------------------


void IPAll()
{
	using namespace lib_commu;

	ListInterFaceAddresses listIA;
	BaseKits bk;


	bk.IPAll(listIA);
	ListInterFaceAddresses::iterator IPIt = listIA.begin();

	for (; IPIt != listIA.end(); ++IPIt)
	{
		std::cout << "name                          =" << IPIt->m_name.c_str() << "-" << IPIt->m_value.c_str() << "\n";
	}
}


/// ----------------------------------------------------------------------------------------
/// @brief: 将数据接收和数据发送分离
/// ----------------------------------------------------------------------------------------
void call_udp_recv_data_example_01()
{
	using namespace lib_commu;

	/// 数据接收对象 没有和 IDUP 绑定一起
	my_udp		udp_recv;

	/// udp
	IUDP* pudp                                          = NewUDP();

	if (nullptr                                         == pudp)
		;
	else
	{
		UDPInit param;

		int uni_or_multi                                = 0;
		std::cout << "1-multi, 0 -uni:";
		std::cin >> uni_or_multi;

		if (1                                           == uni_or_multi)
		{
			param.cast_type_                            = CT_MULTI;
			param.dest_ip_                              = std::string("233.0.0.212");
		}
		else
		{
			param.cast_type_                            = CT_UNI;
			param.dest_ip_                              = std::string("10.0.0.6");
		}
		param.local_ip_                                 = std::string("10.0.0.6");

		param.dest_port_                                = 27500;
		param.socket_ver_                               = SOCKET_VERSION_4;

		/// 初始化
		int ret                                         = pudp->Init(param);
		if (0 != ret)
		{
			///
			std::cout << "\n\n init_failed              =" << ret << "\n\n";
		}
		else
		{
			for (int index                              = 0; index < 6; ++index)
			{
				/// 发送数据
				char arr[]                              = "1234567890";
				ret                                     = pudp->Send(arr, strlen(arr));
				if (0 != ret)
				{
					std::cout << "\n\n send_failed, ret =" << ret << "\n\n";
				}
				else
				{
					/// 发送成功
					std::cout << "\n\n send_success\n\n";
				}

				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
		}

		std::cout << "\n\n";

		std::cout << "\n 33333333333333333 thread_id    =" << std::this_thread::get_id() << "\n";

		std::this_thread::sleep_for(std::chrono::milliseconds(1000 * 1));

		pudp                                            = DeleteUDP(pudp);
	}
}

/// ----------------------------------------------------------------------------------------
/// @brief: 将接收和发送聚合到一个类中
/// ----------------------------------------------------------------------------------------
void call_udp_recv_data_example_02()
{
	NetUDP net_udp;

	using namespace lib_commu;
	UDPInit param;

	int uni_or_multi                                    = 0;

	std::cout << "1-multi, 0 -uni:";
	std::cin >> uni_or_multi;

	if (1                                               == uni_or_multi)
	{
		param.cast_type_                                = CT_MULTI;
		param.dest_ip_                                  = std::string("233.0.0.212");
	}
	else
	{
		param.cast_type_                                = CT_UNI;
		param.dest_ip_                                  = std::string("10.0.0.6");
	}
	param.local_ip_                                     = std::string("10.0.0.6");

	param.dest_port_                                    = 27500;
	param.socket_ver_                                   = SOCKET_VERSION_4;

	/// 初始化
	int ret                                             = net_udp.Init(param);
	if (0 != ret)
	{
		///
		std::cout << "\n\n init_failed                  =" << ret << "\n\n";
	}
	else
	{
		for (int index                                  = 0; index < 100; ++index)
		{
			/// 发送数据
			char arr[]                                  = "1234567890";
			ret                                         = net_udp.Send(arr, strlen(arr));
			if (0 != ret)
			{
				std::cout << "\n\n send_failed, ret     =" << ret << "\n\n";
			}
			else
			{
				/// 发送成功
				//std::cout << "\n\n send_success\n\n";
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(3));
		}
	}

	std::cout << "\n 33333333333333333 thread_id        =" << std::this_thread::get_id() << "\n";

	std::this_thread::sleep_for(std::chrono::milliseconds(1000 * 1));
}


#include "uv.h"
namespace uv_timer
{
	void timer_callback(uv_timer_t* handle)
	{
		static int index =0 ;
		std::cout << "\n index=" << ++ index;
	}

	int call_uv_timer()
	{
		uv_loop_t* ploop = uv_default_loop();
		uv_timer_t timer_req;
		uv_timer_init(ploop, &timer_req);

		uv_timer_start(&timer_req, timer_callback, 1000, 1000);

		return uv_run(ploop, UV_RUN_DEFAULT);
	}
}


int main()
{
	 call_udp_recv_data_example_02();

	//return uv_timer::call_uv_timer();

	return 0;
}