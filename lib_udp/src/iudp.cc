

#include "universe_def.h"

#include <new>
#if defined(os_is_win)
	#include "iudp.h"
	#include <udp_imp.h>	

#else
	#include <udp/udp_interface.h>
	#include <udp/udp_imp.h>


#endif // !_WIN32


namespace lib_udp
{
	/*
*	@brief:
*/
	lib_udp::iudp* lib_udp::create_()
	{
		return  new(std::nothrow) udp_imp;
	}

	/*
	*	@brief: to release the object, whose type is udpsocket*
	*/
	iudp* lib_udp::release_(iudp* pudp)
	{
		if (pudp)
		{
			pudp->shutdown_();
			delete pudp;
		}

		pudp = NULL;

		return pudp;
	}


}