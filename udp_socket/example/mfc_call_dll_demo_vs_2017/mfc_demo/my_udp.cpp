#include "stdafx.h"
#include "my_udp.h"

#pragma comment(lib, "lib/mfc_lib_udp_shared.lib")


my_udp::my_udp()
{
	_pudp = udp_create();
}


my_udp::~my_udp()
{
	// do not call shutdown t
	if (!_is_shutdown)
		shutdown();
}

/*
*	@brief:
*/
int my_udp::init_ip4(udp_param& param)
{
	if (!_pudp)
		return -20000;

	return _pudp->init_ip4(param);
}

/*
*	@brief:
*/
int my_udp::open(const unsigned int time_out_send)
{
	if (!_pudp)
		return -20000;

	return _pudp->open(time_out_send, this);
}

/*
*	@brief:
*/
int my_udp::send(const char *psend, const unsigned int len_send)
{
	if (!_pudp)
		return -20000;

	return _pudp->send(psend, len_send);
}

/*
*	@brief:
*/
int my_udp::shutdown()
{
	if (!_pudp)
		return -20000;

	int ret_val	= _pudp->shutdown();
	udp_release(_pudp);

	_is_shutdown	= true;

	return ret_val;
}

/*
*	@brief:
*/
void my_udp::recv_data(char *pdata_recv, unsigned int recv_data_len)
{
#ifdef _DEBUG
	TRACE("\n------------- recv data: %d ---------------------------\n", recv_data_len);
	for (unsigned int i = 0; i < recv_data_len; i++)
	{
		TRACE("\ni = %d, 0x%.2X \n", i, pdata_recv[i]);
	}
#endif // 
}
