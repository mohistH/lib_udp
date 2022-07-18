#ifndef udp_imp_h
#define udp_imp_h

#include <icommonunication.h>
#include <uv.h>
#include <lib_commu_def.h>
#include <looper.h>
//#include <memory>
#include <List>
#include <map>



namespace lib_commu
{

	/// IPVX地址
	template<typename TSockAddrType>
	struct TSockAddr
	{
	public:
		TSockAddr()
		{
			memset(this, 0, sizeof(TSockAddr));
		}

		/// 目标地址
		TSockAddrType	dest_adress_;
		/// 本机地址
		TSockAddrType	local_adress_;
	};

	/// ----------------------------------------------------------------------------------------
	/// @brief: 地址信息: 目标IP和本机IP
	/// ----------------------------------------------------------------------------------------
	struct SockAddress_
	{
	public:
		SockAddress_()
		{
			memset(this, 0, sizeof(SockAddress_));
		}

		TSockAddr<struct sockaddr_in>			ipv4_;
		TSockAddr<struct sockaddr_in6>			ipv6_;
	};

	typedef SockAddress_ SockAddress;




	struct UDPCTX_
	{
		/// store this on data
		uv_udp_t	udp_handle_;
		/// 读取buf 
		uv_buf_t	read_buf_;
		/// udp 发送时使用
		uv_udp_send_t send_req_;
		uv_buf_t	send_buf_;
	};

	typedef UDPCTX_ UDPCTX;

	UDPCTX* NewUDPCTX(void *parentServer);
	UDPCTX* DelUDPCTX(UDPCTX* ctx);


	/// ----------------------------------------------------------------------------------------
	/// @brief: 
	/// ----------------------------------------------------------------------------------------
	struct UvEventStatus_ ///: public UvEvent
	{
		UvEventStatus_()
		{
			is_available_	= true;
			buf_id_			= 0;
		}

		/// 当前缓冲是否可用< true-空闲， false-使用中
		bool		is_available_;
		/// buf id
		size_t			buf_id_;
	};


	typedef UvEventStatus_ UvEventStatus;

	typedef std::map<size_t, UvEvent>	MapIntUVEvent;

	typedef std::map<size_t, UvEventStatus>	MapIntEventStatus;


	/// ----------------------------------------------------------------------------------------
	/// @brief: 发送事件buf， 
	/// ----------------------------------------------------------------------------------------
	struct SendEventPlus_
	{

		SendEventPlus_()
		{
			SetCount(10);
		}

		~ SendEventPlus_()
		{
			UnInit();
		}

		void ResetBufStatus()
		{
			for (MapIntEventStatus::iterator findIt = map_space_id_.begin(); findIt != map_space_id_.end(); ++findIt)
			{
				findIt->second.is_available_ = true;
			}
		}

		/// ----------------------------------------------------------------------------------------
		/// @brief: 释放
		/// ----------------------------------------------------------------------------------------
		void UnInit()
		{
			try
			{
				/// 释放
				for (MapIntUVEvent::iterator findIt = map_event_plus_.begin(); findIt != map_event_plus_.end(); ++findIt)
				{
					if (findIt->second._buff)
					{
						free(findIt->second._buff);
						//free(findIt->second._data);

						findIt->second._buff = nullptr;
						findIt->second._size = 0;
						findIt->second._data = nullptr;

					}
					else
					{
						;
					}
				}
			}
			catch (...)
			{
				;
			}

			{
				MapIntUVEvent tmp;
				map_event_plus_.swap(tmp);
			}

		}


		/// ----------------------------------------------------------------------------------------
		/// @brief: 设置 event的数量
		/// ----------------------------------------------------------------------------------------
		void SetCount(const size_t& event_count)
		{
			UvEventStatus event_status;

			UvEvent _event;

			for (size_t index = 1; index <= event_count; ++index)
			{
				/// 保存空闲ID
				event_status.buf_id_ = index;
				map_space_id_.insert(std::make_pair(index, event_status));
				
				/// 创建buf
				//UvEvent* _event = new(std::nothrow)  UvEvent;
				_event.type = UV_EVENT_TYPE_WRITE;
				_event._buff = (char*)malloc(UDP_PACJAGE_MAX_LEN_LAN);
				memset(_event._buff, 0, UDP_PACJAGE_MAX_LEN_LAN);
				_event._size = UDP_PACJAGE_MAX_LEN_LAN;
				_event._data = (int*)malloc(1);
				_event._id = index;

				map_event_plus_.insert(std::make_pair(index, _event));
			}
		}

		/// ----------------------------------------------------------------------------------------
		/// @brief: 更新buf状态
		/// ----------------------------------------------------------------------------------------
		void UpdateEventStatus(const size_t index, const bool index_is_space)
		{
			MapIntEventStatus::iterator findIt = map_space_id_.find(index);
			if (findIt == map_space_id_.end())
			{
				return;
			}

			findIt->second.is_available_ = index_is_space;
		}


		/// ----------------------------------------------------------------------------------------
		/// @brief: 获取数据Event
		/// ----------------------------------------------------------------------------------------
		UvEvent* EventBuf(const char* pdata, const size_t len)
		{
			if ((nullptr == pdata) || (0 >= len))
			{
				return nullptr;
			}

			size_t space_id = 0;
			for (MapIntEventStatus::iterator findIt = map_space_id_.begin(); findIt != map_space_id_.end(); ++findIt)
			{
				/// 如果空闲，则跳出
				if (true == findIt->second.is_available_)
				{
					space_id = findIt->second.buf_id_;
					findIt->second.is_available_ = false;
					break;
				}
			}

			/// 获取index对应的buf
			MapIntUVEvent::iterator findBufIt = map_event_plus_.find(space_id);
			if (findBufIt == map_event_plus_.end())
			{
				return nullptr;
			}


			/// 如果长度过大， 则返回
			if (UDP_PACJAGE_MAX_LEN_LAN < len)
			{
				return nullptr;
			}


			UvEvent* pret = &findBufIt->second;
			memcpy(pret->_buff, pdata, len);
			pret->_size = len;
			pret->_id = space_id;

			return pret;
		}

		/// 空闲bufID
		MapIntEventStatus		map_space_id_;
		/// buf
		MapIntUVEvent				map_event_plus_;
	};


	typedef SendEventPlus_ SendEventPlus;






	/// ----------------------------------------------------------------------------
	/// @brief: udp实现类
	/// ----------------------------------------------------------------------------
	class UDPImp :public uv::ILooperEvent , public IUDP
	{
	public:
		UDPImp();
		virtual ~UDPImp();
		/// ----------------------------------------------------------------------------


		/// ----------------------------------------------------------------------------
		/// @brief：		初始化
		/// @param: 	const udp_init_params & param - param
		/// @param: 	idata_handle * pdata_handle - 接收数据， 如果不需要，则传 nullptr
		/// @return: 	int - 无
		/// 			
		/// ----------------------------------------------------------------------------
		virtual int Init(const UDPInit &param) override;

		/// ----------------------------------------------------------------------------
		/// @brief：		发送数据
		/// @param: 	const char * pdata - pdata
		/// @param: 	const size_t len - 
		/// @return: 	int - 无
		/// 			0 - 成功
		///				1 - 失败，udp没有创建成功
		///				2 - 失败，pdata为nullptr 
		/// ----------------------------------------------------------------------------
		virtual int Send(const char* pdata, const size_t len)	override;

		/// ----------------------------------------------------------------------------
		/// @brief：		退出前调用
		/// @return: 	int - 无
		/// 			
		/// ----------------------------------------------------------------------------
		virtual int UnInit() override;

		/// -------------------------------------------------------------------------------
		/// @brief:		返回错误代码
		///  @ret:		std::string
		///				
		/// -------------------------------------------------------------------------------
		virtual std::string GetLastErrorMsg() override;



		/// --------------------------------------------------------------------------------
		/// @brief: DoEvent
		/// --------------------------------------------------------------------------------
		virtual void DoEvent(UvEvent* pevent) override;


		/// --------------------------------------------------------------------------------
		/// @brief: OnHandleClose
		/// --------------------------------------------------------------------------------
		virtual void OnHandleClose(uv_handle_t* phandle) override;


	//private:

	//	void onCloseCompleted();

	//	void onMessage(const sockaddr* from, const char* data, unsigned size);

	//	static void onMesageReceive(uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf, const struct sockaddr* addr, unsigned flags);

	private:
		/// -------------------------------------------------------------------------------
		/// @brief:		初始化looper
		///  @ret:		void
		///				
		/// -------------------------------------------------------------------------------
		int InitLooper();

		/// -------------------------------------------------------------------------------
		/// @brief:		初始化CTX
		///  @ret:		void
		///				
		/// -------------------------------------------------------------------------------
		void InitCTX();

		/// -------------------------------------------------------------------------------
		/// @brief:		初始化 UDPInit 参数
		/// @param: 	const UDPInit & param_init - 外部传递进来
		///  @ret:		int
		///				0 - 成功
		/// -------------------------------------------------------------------------------
		int BindSock(const UDPInit& param_init);

		/// -------------------------------------------------------------------------------
		/// @brief:		初始化udp类型
		/// @param: 	const UDPInit & param_init - 
		///  @ret:		int
		///				0 - 成功
		/// -------------------------------------------------------------------------------
		int InitCastType(const UDPInit& param_init);


	protected:
		/// -------------------------------------------------------------------------------
		/// @brief:		uv_start_recv中调用
		/// @param: 	uv_handle_t * handle - 
		/// @param: 	size_t suggested_size - 
		/// @param: 	uv_buf_t * buf - 
		///  @ret:		void
		///				
		/// -------------------------------------------------------------------------------
		static void AllocBufferForRecv(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);

		/// -------------------------------------------------------------------------------
		/// @brief:		接收数据回调函数
		/// @param: 	uv_udp_t * pHandle - 
		/// @param: 	ssize_t nRead - 
		/// @param: 	const uv_buf_t * pBuf - 
		/// @param: 	const struct sockaddr * pAddr - 
		/// @param: 	unsigned iFlag - 
		///  @ret:		void
		///				
		/// -------------------------------------------------------------------------------
		static void AfterRecv(uv_udp_t* pHandle, ssize_t nRead, const uv_buf_t* pBuf, const struct sockaddr* pAddr, unsigned iFlag);

		/// -------------------------------------------------------------------------------
		/// @brief:		发送回调
		/// @param: 	uv_udp_send_t * pReq - 
		/// @param: 	int iStatus - 
		///  @ret:		void
		///				
		/// -------------------------------------------------------------------------------
		static void AfterSend(uv_udp_send_t* pReq, int iStatus);

	private:
		/// -------------------------------------------------------------------------------
		/// @brief:		发送数据
		/// @param: 	const struct sockaddr * addr - 
		/// @param: 	const char * pdata - 
		/// @param: 	const size_t len - 
		///  @ret:		void
		///				
		/// -------------------------------------------------------------------------------
		void SendInline(const struct sockaddr* addr, const char* pdata, const size_t len);

	private:
		/// 错误消息
		std::string				errmsg_;

		/// 接收函数处理对象
		IDataDispatch*			data_dispatch_;

		uv::CLooper*			ptrLooper_;
		
		UDPCTX*					udp_ctx_;

		/// 保存地址信息, 用于发送数据时使用
		SockAddress				sock_address_;

		/// 保存socket版本
		int						socket_ver_;

		/// 多个 event buf
		SendEventPlus			send_event_buf_;
		
		/// 发送结果通知函数
		SendResultNotify*		send_notify_;

	};
}



#endif /// udp_imp_h