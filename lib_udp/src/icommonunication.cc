#include <stdarg.h>

#include "icommonunication.h"
#include "uv.h"
#include "UDPImp.h"





namespace lib_commu
{	

	/// ----------------------------------------------------------------
	/// @brief: 
	/// ----------------------------------------------------------------
	IUDP* NewUDP()
	{
		IUDP* pret = new(std::nothrow) UDPImp;

		return pret;
	}

	/// ----------------------------------------------------------------
	/// @brief: 
	/// ----------------------------------------------------------------
	IUDP* DeleteUDP(IUDP* pobj)
	{
		if (nullptr != pobj)
		{
			//pobj->UnInit();
			delete pobj;
			pobj	= nullptr;
		}

		return pobj;
	}

	/// ----------------------------------------------------------------
	/// @brief: 
	/// ----------------------------------------------------------------
	BaseKits::BaseKits()
	{
		;
	}

	/// ----------------------------------------------------------------
	/// @brief: 
	/// ----------------------------------------------------------------
	BaseKits::~BaseKits()
	{
		;
	}

	/// ----------------------------------------------------------------
	/// @brief: 格式化字符串
	/// ----------------------------------------------------------------
	std::string BaseKits::Format(const char*pformat , ...)
	{
		va_list argptr;
		va_start(argptr , pformat);

		/// 计算格式化字符串的长度
		int size			= vsnprintf(NULL , NULL , pformat , argptr);
		if (0				== size)
		{
			va_end(argptr);
			return std::string("");
		}

		/// 申请缓冲区 ， +1是为了存放结束符
		char* pbuf			= (char*)malloc(size + 1);
		/// 申请失败，记得释放VAlist
		if (NULL			== pbuf)
		{
			va_end(argptr);
			return std::string("");
		}

		memset(pbuf , 0 , size + 1);

		/// 将数据写入申请的缓冲区
		int write_len		= vsnprintf(pbuf , size + 1 , pformat , argptr);
		/// 释放VA
		va_end(argptr);
		pbuf[size]			= '\0';

		/// 构造函数返回值
		std::string ret(pbuf);

		/// 释放申请的缓冲区
		free(pbuf);
		pbuf				= NULL;

		return ret;
	}


	/// --------------------------------------------------------------------------------
	/// @brief: BaseKits::IPV4
	/// --------------------------------------------------------------------------------
	void BaseKits::IPV4(ListInterFaceAddresses& out_list)
	{
		char buf[512] = {0};
		uv_interface_address_t* info = nullptr;
		int count, i;
		uv_interface_addresses(&info, &count);
		i = count;
		//printf("Number of interfaces: %d\n", count);

		lib_commu::InterfaceAddresses listItem;

		while (i--)
		{
			uv_interface_address_t interface = info[i];
			//printf("Name: %s\n", interface.name);
			//printf("Internal? %s\n", interface.is_internal ? "Yes" : "No");
			listItem.m_name = std::string(interface.name);

			if (interface.address.address4.sin_family == AF_INET)
			{
				uv_ip4_name(&interface.address.address4, buf, sizeof(buf));
			}

			listItem.m_value = std::string(buf);

			if (0 != listItem.m_value.length())
			{
				out_list.push_back(listItem);

				listItem.Zero();
				buf[0] = '\0';
			}
		}
		uv_free_interface_addresses(info, count);
	}

	/// --------------------------------------------------------------------------------
	/// @brief: BaseKits::IPV6
	/// --------------------------------------------------------------------------------
	void BaseKits::IPV6(ListInterFaceAddresses& out_list)
	{
		char buf[512] = { 0 };
		uv_interface_address_t* info = nullptr;
		int count, i;
		uv_interface_addresses(&info, &count);
		i = count;
		//printf("Number of interfaces: %d\n", count);

		lib_commu::InterfaceAddresses listItem;

		while (i--)
		{
			uv_interface_address_t interface = info[i];
			//printf("Name: %s\n", interface.name);
			//printf("Internal? %s\n", interface.is_internal ? "Yes" : "No");
			listItem.m_name = std::string(interface.name);

			if (interface.address.address4.sin_family == AF_INET6)
			{
				uv_ip6_name(&interface.address.address6, buf, sizeof(buf));
				//printf("IPv6 address: %s\n", buf);
			}
			//printf("\n");

			listItem.m_value = std::string(buf);

			if (0 != listItem.m_value.length())
			{
				out_list.push_back(listItem);

				listItem.Zero();
				buf[0] = '\0';
			}
		}
		uv_free_interface_addresses(info, count);
	}

	/// --------------------------------------------------------------------------------
	/// @brief: BaseKits::IPAll
	/// --------------------------------------------------------------------------------
	void BaseKits::IPAll(ListInterFaceAddresses& out_list)
	{
		this->IPV4(out_list);
		
		this->IPV6(out_list);
	}


}