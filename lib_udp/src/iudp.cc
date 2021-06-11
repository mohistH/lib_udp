
#include "universe_def.h"
#include <new>
#include "iudp.h"
#include "udp_imp.h"



namespace lib_udp
{

	/*
	*	@brief:
	*/
	iudp* udp_create_()
	{
		return  new(std::nothrow) udp_socket_imp;

	}


	/*
	*	@brief: to release the object, whose type is udpsocket*
	*/
	iudp* udp_release_(iudp* pudp)
	{
		if ((NULL == pudp) || (nullptr == pudp))
			return pudp;

		delete pudp;
		pudp = NULL;

		return pudp;
	}

}



