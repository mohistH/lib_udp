



#include <new>
#ifdef _WIN32
	#include <udp/udp_interface.h>
	#include <udp/udp_socket_imp.h>
	#include <udp/udp_wsa.h>		

#else
	#include <udp/udp_interface.h>
	#include <udp/udp_socket_imp.h>


#endif // !_WIN32


/*
*	@brief:
*/
lib_udp::udpsocket* lib_udp::udp_create()
{
	return  new(std::nothrow) udp_socket_imp;

	// return NULL;
}



/*
*	@brief: to release the object, whose type is udpsocket*
*/
void lib_udp::udp_release(udpsocket* pudp)
{
	if (NULL == pudp || nullptr == pudp)
		return;

	delete pudp;
	pudp						= NULL;
}





/*
*	@brief:
*/
lib_udp::udpsocket* lib_udp::udp_wsa_create()
{
#ifdef _WIN32
	return  new(std::nothrow) udp_wsa;
#else
	return NULL;
#endif // !_WIN32
}
