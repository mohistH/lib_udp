#ifndef lib_commu_def_h
#define lib_commu_def_h
#include "uv.h"


namespace lib_commu
{	
	enum
	{
		/// UDP发送缓冲区长度
		/// 建议在进行Internet(非局域网)的UDP编程时. 最好将UDP的数据长度控件在548字节(576-8-20)以内.
		UDP_PACKAGE_MAX_LEN_INTERNET = 576 - 8 - 20 , 
		/// 局域网， UDP，用sendto函数最大能发送数据的长度为：65535- IP头(20) - UDP头(8)＝65507字节。用sendto函数发送数据时，如果发送数据长度大于该值，则函数会返回错误。  
		UDP_PACJAGE_MAX_LEN_LAN		= 65535 - 20 - 8,
	};
}

#endif /// lib_commu_def_h