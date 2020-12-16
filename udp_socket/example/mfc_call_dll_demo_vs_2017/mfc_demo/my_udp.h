#pragma once
#include "include/udp_interface.h"
#include <memory>


using namespace lib_udp;

class my_udp : public irecv_data_interface
{
public:
	explicit my_udp();
	virtual ~my_udp();

	// -------------------------------------------------------------------------------
	int init_ip4(udp_param& param);
	int open(const unsigned int time_out_send);
	int send(const char *psend, const unsigned int len_send);
	int shutdown();
	void on_recv_data_(char *pdata_recv, unsigned int recv_data_len);

private:
	udp_socket_interface *	_pudp = nullptr;

	bool		_is_shutdown = false;
};



