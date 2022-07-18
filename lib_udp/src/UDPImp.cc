#include <iostream>
#include <thread>

#include "UDPImp.h"
#include "uv_utils.h"
#include "Packet.h"
#include "lib_commu_def.h"

namespace lib_commu 
{

	/// --------------------------------------------------------------------------------
	/// @brief: NewUDPCTX
	/// --------------------------------------------------------------------------------
	lib_commu::UDPCTX* NewUDPCTX(void* parentServer)
	{
		UDPCTX* ctx			                     = (UDPCTX*)malloc(sizeof(*ctx));
		if (ctx)
		{
			ctx->read_buf_.base		             = (char*)malloc(lib_commu::UDP_PACJAGE_MAX_LEN_LAN);
			ctx->read_buf_.len		             = lib_commu::UDP_PACJAGE_MAX_LEN_LAN;
		
			ctx->send_req_.data		             = parentServer;
			ctx->send_buf_.base		             = (char*)malloc(lib_commu::UDP_PACJAGE_MAX_LEN_LAN);
			ctx->send_buf_.len		             = lib_commu::UDP_PACJAGE_MAX_LEN_LAN;
		}



		return ctx;
	}

	/// --------------------------------------------------------------------------------
	/// @brief: DelUDPCTX
	/// --------------------------------------------------------------------------------
	lib_commu::UDPCTX* DelUDPCTX(UDPCTX* ctx)
	{
		if (nullptr                              == ctx)
		{
			return ctx;
		}

		if (ctx->read_buf_.base)
		{
			free(ctx->read_buf_.base);
			ctx->read_buf_.base                  = nullptr;
		}

		if (ctx->send_buf_.base)
		{
			free(ctx->send_buf_.base);
			ctx->send_buf_.base                  = nullptr;
		}

		free(ctx);
		ctx                                      = nullptr;


		return ctx;
	}





	UDPImp::UDPImp()
		: IUDP()
		, uv::ILooperEvent()
	{
		/// 默认值 nullptr
		data_dispatch_		                     = nullptr;
		ptrLooper_			                     = nullptr;
		udp_ctx_			                     = nullptr;
		send_notify_ = nullptr;

	}

	UDPImp::~UDPImp()
	{
		UnInit();
	}

	/// ----------------------------------------------------------------
	/// @brief: 
	/// ----------------------------------------------------------------
	int UDPImp::Init(const UDPInit &param)
	{
		UnInit();

		InitCTX();

		int ret                                  = 0;

		/// ---------------------------------------------------------------------------------------
		/// 1. 初始化looper
		ret		                                 = InitLooper();

		/// 初始化失败， 请调用获取错误信息接口
		if (0	                                != ret)
		{
			return 1;
		}

		/// ---------------------------------------------------------------------------------------
		/// 2. 初始化UDPInit参数
		ret		                                 = BindSock(param);
		if (ret)
		{
			return 2;
		}

		/// ---------------------------------------------------------------------------------------
		/// 3. 初始化 UDP 类型
		ret		                                 = InitCastType(param);
		if (ret)
		{
			return 3;
		}

		/// ---------------------------------------------------------------------------------------
		/// 5. 如果不需接收数据， 则不用设置接收
		if (nullptr                              == param.recv_data_)
		{
			;
		}
		/// 需要接收数据
		else
		{
			ret                                  = uv_udp_recv_start(&udp_ctx_->udp_handle_, AllocBufferForRecv, UDPImp::AfterRecv);
			if (ret)
			{
				errmsg_                          = uv::GetUVError(ret);
				//LOGE("%s", errmsg_.c_str());
				return 5;
			}

			data_dispatch_	                     = param.recv_data_;
		}

		socket_ver_                              = param.socket_ver_;
		send_notify_								= param.send_notify_;

		return 0;
	}

	/// ----------------------------------------------------------------
	/// @brief: 
	/// ----------------------------------------------------------------
	int UDPImp::Send(const char* pdata , const size_t len)
	{
		/// UDP避免分包， 单次发送最长为 PACK_UDP_SIZE
		//if (PACK_UDP_SIZE <= len)
		if (lib_commu::UDP_PACJAGE_MAX_LEN_LAN < len)
		{
			return 1;
		}

		/// 检查数据
		if (nullptr == pdata)
		{
			return 5;
		}

		if (ptrLooper_)
		{
			UvEvent* pEvent						= send_event_buf_.EventBuf(pdata, len); ///CreateUvEvent(UV_EVENT_TYPE_WRITE, pdata, len);
			if (nullptr == pEvent)
			{
				return 6;
			}
			
			if (lib_commu::SOCKET_VERSION_4      == socket_ver_)
			{
				pEvent->_addr                    = *((const struct sockaddr*)(&sock_address_.ipv4_.dest_adress_));
			}
			else if (lib_commu::SOCKET_VERSION_6 == socket_ver_)
			{
				pEvent->_addr                    = *(const struct sockaddr*)(&sock_address_.ipv6_.dest_adress_);
			}
			/// socket version error 
			else
			{
				return 3;
			}

			pEvent->_data		                 = this;

			ptrLooper_->PushEvent(pEvent);
		}
		/// 无法发送数据
		else
		{
			return 2;
		}

		return 0;
	}

	/// ----------------------------------------------------------------
	/// @brief: 
	/// ----------------------------------------------------------------
	int UDPImp::UnInit()
	{
		try
		{
			if (ptrLooper_)
			{
				ptrLooper_->Close();
				delete ptrLooper_;
				ptrLooper_                       = nullptr;
			}

			udp_ctx_	                         = DelUDPCTX(udp_ctx_);
		}
		catch (...)
		{
			;
		}

		return 0;
	}

	/// --------------------------------------------------------------------------------
	/// @brief: UDPImp::GetLastErrorMsg
	/// --------------------------------------------------------------------------------
	std::string UDPImp::GetLastErrorMsg()
	{
		return errmsg_;
	}

	/// --------------------------------------------------------------------------------
	/// @brief: UDPImp::DoEvent
	/// --------------------------------------------------------------------------------
	void UDPImp::DoEvent(UvEvent* pEvent )
	{
		switch (pEvent->type)
		{
			case UV_EVENT_TYPE_WRITE:
			{
				SendInline(&pEvent->_addr, pEvent->_buff, pEvent->_size);
			}
			break;

			default:
				break;
		}

		/// 发送结束，标记当前buf为可用
		send_event_buf_.UpdateEventStatus(pEvent->_id, true);
		//FreeUvEvent(pEvent);
	}

	/// --------------------------------------------------------------------------------
	/// @brief: UDPImp::OnHandleClose
	/// --------------------------------------------------------------------------------
	void UDPImp::OnHandleClose(uv_handle_t* handle)
	{
		if (!uv_is_closing(handle))
		{
			uv_close(handle, nullptr);
		}
	}

	/// --------------------------------------------------------------------------------
	/// @brief: UDPImp::SendInline
	/// --------------------------------------------------------------------------------
	void UDPImp::SendInline(const struct sockaddr* addr, const char* pdata, const size_t len)
	{		
		/// ---------------------------------------------------------------------------------------
		if (udp_ctx_->send_buf_.base)
		{
			std::memcpy(udp_ctx_->send_buf_.base, pdata, len);
			udp_ctx_->send_buf_.len              = (ULONG)len;
		}
		

		int iret                                 = uv_udp_send(&udp_ctx_->send_req_, &udp_ctx_->udp_handle_, &udp_ctx_->send_buf_, 1, (struct sockaddr*)addr, UDPImp::AfterSend);
		if (iret								!= 0)
		{
			//FreeSendParam(sendp);
			std::string error_str                = uv::GetUVError(iret);
		}
	}

	///// --------------------------------------------------------------------------------
	///// @brief: UDPImp::onCloseCompleted
	///// --------------------------------------------------------------------------------
	//void UDPImp::onCloseCompleted()
	//{

	//}

	///// --------------------------------------------------------------------------------
	///// @brief: UDPImp::onMessage
	///// --------------------------------------------------------------------------------
	//void UDPImp::onMessage(const sockaddr* from, const char* data, unsigned size)
	//{

	//}

	///// --------------------------------------------------------------------------------
	///// @brief: UDPImp::onMesageReceive
	///// --------------------------------------------------------------------------------
	//void UDPImp::onMesageReceive(uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf, const struct sockaddr* addr, unsigned flags)
	//{

	//}

	/// --------------------------------------------------------------------------------
	/// @brief: UDPImp::InitLooper
	/// --------------------------------------------------------------------------------
	int UDPImp::InitLooper()
	{
		int ret				                     = 0;

		if (nullptr			                     == ptrLooper_)
		{
			ptrLooper_		                     = new(std::nothrow) uv::CLooper;
		}

		if (ptrLooper_)
		{
			/// 1. 初始化loop
			ptrLooper_->InitLooper();

			/// 2. 初始化UDP
			ret                                  = uv_udp_init(ptrLooper_->GetLooper(), &udp_ctx_->udp_handle_);

			/// 初始化失败
			if (ret)
			{
				errmsg_                          = uv::GetUVError(ret);
			}
		}
		else
		{
			/// 无法获取内存
			ret                                  = 1;
		}

		return 0;
	}

	/// --------------------------------------------------------------------------------
	/// @brief: UDPImp::InitCTX
	/// --------------------------------------------------------------------------------
	void UDPImp::InitCTX()
	{
		if (nullptr                              == udp_ctx_)
		{
			udp_ctx_                             = NewUDPCTX(this);
		}

		if (udp_ctx_)
		{
			udp_ctx_->udp_handle_.data           = this;
		}
	}

	/// --------------------------------------------------------------------------------
	/// @brief: UDPImp::Bind
	/// --------------------------------------------------------------------------------
	int UDPImp::BindSock(const UDPInit& param_init)
	{
		int ret			                         = 0;
		/// 检查端口是否正确
		if (0									>= param_init.dest_port_)
		{
			errmsg_		                         = std::string("param_init.dest_port is zero or negative, please check");
			ret			                         = 3;
			//LOGE("%s", errmsg_.c_str());
			return ret;
		}

		/// 如果是IPV4
		if (lib_commu::SOCKET_VERSION_4          == param_init.socket_ver_)
		{
			ret                                  = uv_ip4_addr(param_init.local_ip_.c_str(), param_init.dest_port_, &sock_address_.ipv4_.local_adress_);
			ret                                  = uv_ip4_addr(param_init.dest_ip_.c_str(), param_init.dest_port_, &sock_address_.ipv4_.dest_adress_);

			ret                                  = uv_udp_bind(&udp_ctx_->udp_handle_, (struct sockaddr*)&sock_address_.ipv4_.local_adress_, UV_UDP_REUSEADDR);
			if (0                               != ret)
			{
				errmsg_                          = uv::GetUVError(ret);
				//LOGE("%s", errmsg_.c_str());
				return ret;
			}
		}
		else if (lib_commu::SOCKET_VERSION_6     == param_init.socket_ver_)
		{
			ret                                  = uv_ip6_addr(param_init.local_ip_.c_str(), param_init.dest_port_, &sock_address_.ipv6_.local_adress_);
			ret                                  = uv_ip6_addr(param_init.dest_ip_.c_str(), param_init.dest_port_, &sock_address_.ipv6_.dest_adress_);
			ret                                  = uv_udp_bind(&udp_ctx_->udp_handle_, (struct sockaddr*)&sock_address_.ipv6_.local_adress_, UV_UDP_REUSEADDR);
			if (0                               != ret)
			{
				errmsg_                          = uv::GetUVError(ret);
				//LOGE("%s", errmsg_.c_str());
				return ret;
			}
		}
		/// socket version 错误，
		else
		{
			errmsg_                              = std::string("param_init.socket_ver is false, please check the value");
			ret                                  = 1;
			//LOGE("%s", errmsg_.c_str());
			return ret;
		}

		return ret;
	}

	/// --------------------------------------------------------------------------------
	/// @brief: UDPImp::InitType
	/// --------------------------------------------------------------------------------
	int UDPImp::InitCastType(const UDPInit& param_init)
	{
		int ret                                  = 0;
		/// 1. 判断戳哪种SOCKET
		switch (param_init.cast_type_)
		{
			/// ---------------------------------------------------------------------------------------
			/// 广播
			/// ---------------------------------------------------------------------------------------
		case lib_commu::CT_BROAD:
		{
			/// ipv4 
			ret                                  = uv_udp_set_broadcast(&udp_ctx_->udp_handle_, 1);
			if (ret)
			{
				errmsg_                          = uv::GetUVError(ret);
				//LOGE("%s", errmsg_.c_str());
				return ret;
			}

			/// 设置ttl, 这里设置为128
			ret                                  = uv_udp_set_ttl(&udp_ctx_->udp_handle_, 128);
			if (ret)
			{
				errmsg_                          = uv::GetUVError(ret);
				//LOGE("%s", errmsg_.c_str());
				return ret;
			}
		}
			break;

			/// ---------------------------------------------------------------------------------------
			/// 组播
			/// ---------------------------------------------------------------------------------------
		case lib_commu::CT_MULTI:
		{
			/// 参数2： 1-开， 0-关
			ret                                  = uv_udp_set_multicast_loop(&udp_ctx_->udp_handle_, 1);
			if (ret)
			{
				errmsg_                          = uv::GetUVError(ret);
				//LOGE("%s", errmsg_.c_str());
				return ret;
			}

			/// 设置ttl
			ret                                  = uv_udp_set_multicast_ttl(&udp_ctx_->udp_handle_, 128);
			if (ret)
			{
				errmsg_                          = uv::GetUVError(ret);
				//LOGE("%s", errmsg_.c_str());
				return ret;
			}

			/// 设置组播接口
			ret                                  = uv_udp_set_multicast_interface(&udp_ctx_->udp_handle_, param_init.local_ip_.c_str());
			if (ret)
			{
				errmsg_                          = uv::GetUVError(ret);
				//LOGE("%s", errmsg_.c_str());
				return ret;
			}

			/// 加入组播
			ret                                  = uv_udp_set_membership(&udp_ctx_->udp_handle_, param_init.dest_ip_.c_str(), param_init.local_ip_.c_str(), UV_JOIN_GROUP);
			if (ret)
			{
				errmsg_                          = uv::GetUVError(ret);
				//LOGE("%s", errmsg_.c_str());
				return ret;
			}
		}
			break;

			/// ---------------------------------------------------------------------------------------
			/// 单播
			/// ---------------------------------------------------------------------------------------
		case lib_commu::CT_UNI:
		{
			/// 单播， do nothing
			;
		}
			break;

			/// ---------------------------------------------------------------------------------------
			/// qita
			/// ---------------------------------------------------------------------------------------
		case lib_commu::CT_NONE:
		default:
			ret                                  = 1;
			errmsg_			                     = std::string("param_init.cast_type_ is false or CT_NONE, please check");
			//LOGE("%s", errmsg_.c_str());
			break;
		}

		return ret;
	}

	/// --------------------------------------------------------------------------------
	/// @brief: UDPImp::AllocBufferForRecv
	/// --------------------------------------------------------------------------------
	void UDPImp::AllocBufferForRecv(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
	{
		UDPImp* the_class	                     = (UDPImp*)handle->data;
		UDPCTX* udp_ctx		                     = the_class->udp_ctx_;
	
		if (udp_ctx)
		{
			*buf			                     = udp_ctx->read_buf_;
		}
		/// 空指针
		else
		{
			;
		}
	}

	/// --------------------------------------------------------------------------------
	/// @brief: UDPImp::AfterRecv
	/// --------------------------------------------------------------------------------
	void UDPImp::AfterRecv(uv_udp_t* pHandle, ssize_t nRead, const uv_buf_t* pBuf, const struct sockaddr* pAddr, unsigned iFlag)
	{
		/// 读取字节数错误
		if (0                                   >= nRead)
		{
			return;
		}

		UDPImp* the_class                        = (UDPImp*)pHandle->data;

		/// 执行用户接收数据函数
		if (the_class->data_dispatch_)
		{
			the_class->data_dispatch_->RecvData(pBuf->base, nRead);
		}
		/// 回调函数为空
		else
		{
			;
		}
	}

	/// --------------------------------------------------------------------------------
	/// @brief: UDPImp::AfterSend
	/// --------------------------------------------------------------------------------
	void UDPImp::AfterSend(uv_udp_send_t* pReq, int iStatus)
	{

		UDPImp* the_class                      = (UDPImp*)pReq->data;
		if (0 > iStatus)
		{
			//LOGE("send error:%s", GetUVError(iStatus).c_str());
			
		}
		const std::string&&	error_ret			 = uv::GetUVError(iStatus);
		bool is_success = (0 > iStatus ? false : true);

		//FreeSendParam((send_param*)pReq);
		if (the_class)
		{
			if (the_class->send_notify_)
			{
				the_class->send_notify_->AfterSend(is_success, error_ret);
			}
			else
			{
				;
			}
		}
		else
		{
			;
		}
	}

}
