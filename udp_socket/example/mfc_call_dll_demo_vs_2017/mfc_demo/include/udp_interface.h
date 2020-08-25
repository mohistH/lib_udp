#ifndef _udp_socket_interface_h
#define _udp_socket_interface_h

#include <string.h>


namespace lib_udp
{




#ifdef _WIN32

	#ifndef _oct_udp_api_export_export_
		#define _oct_udp_api_export_		__declspec(dllexport)
	#else
		#define _oct_udp_api_export_		__declspec(dllimport)
	#endif //! _oct_udp_api_export_export_



#else 

	#ifndef _oct_udp_api_export_export_
		#define _oct_udp_api_export_		__attribute__((visibility("default")))
		//#define _oct_udp_api_export_		__attribute__ ((visibility("default")))
	#endif // !_oct_udp_api_export_export_



#endif // !_WIN32





//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	

	/*
	* how to use udp socket
	*/
	enum udp_cast_type
	{
		udp_multi_cast	= 1,	// multi cast
		udp_uni_cast	= 2,	// uni cast
		udp_broad_cast	= 3,	// broad cast
	};
	

	// -------------------------------------------------------------------------------
	// the parameter of udp
	struct udp_param_
	{
	private:
		enum
		{
			ip4_len_16 = 16,
		};

	public:
		// Whether receive message that you sent
		bool	_recv_loop = false;

		// log file of debugging
		bool	_is_log_debug = false;

		// which type of udp do you wanna use
		udp_cast_type _cast_type = udp_multi_cast;

		//  the port of destination
		unsigned short	_port_dst = 20086;

		// the ip of destination
		char _pip4_dst[ip4_len_16] = { "233.3.3.3" };

		// the ip of local
		char _pip4_local[ip4_len_16] = { "127.0.0.1" };


	public:
		void zero()
		{
			memset(this, 0, sizeof(udp_param_));
		}

		udp_param_()
		{
			zero();
		}
	};
	using udp_param = udp_param_;

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	


#ifdef __cplusplus
	extern "C" {
#endif	// !__cplusplus

///------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	
	/*
	*	@ brief: to recv data, you must inherit udpsocket_recv class
	*/
	class udpsocket_recv
	{
	public:
		virtual ~udpsocket_recv(){}


		/* @ brief: when socket has received data, it will call this function to pass data
		*  @ char * pdata_recv - data received
		*  @ unsigned int recv_data_len - the received length 
		*  @ return - void
				
		*/
		virtual void recv_data(char *pdata_recv, unsigned int recv_data_len) = 0;
	};


	/*
	*	@ brief: udp socket. it has interfaces of udp socket, including initialize, send and receive
	*/
	class udpsocket
	{
	public:
		virtual ~udpsocket(){}

		/* @ brief: to check parameters
		*  @ upd_param & param - parameters of udp
		*  @ return - int
				0 - success
				1 - faluere, param._port_dstis zero.
				2 - faluere, param._pip4_dst's length is less than 7
		*/
		virtual int init_ip4(udp_param& param) = 0;
		
		
		/* @ brief: initialize socket of udp
		*  @ const unsigned int time_out_send - sending time out.seconds
		*  @ return - int
				0 - success
				20086 - failure, the parameter's type of udp_cast_type gets wrong
				X - faluere, the error's id to call  setsockopt function
		*/
		virtual int open(const unsigned int time_out_send, udpsocket_recv* pfunc_recv = nullptr) = 0;



		/* @ brief: send data after initializing success
		*  @ const char * psend - the data to send
		*  @ const int len_send - the length of sending data
		*  @ return - int
				-20086 - failure, initialize socket failure
				X> 0 - success, X is length of sending success
				X < 0 - failure, X is error msg's id  of calling sendto
				-20087 - psend is null 
						[or] len_send is larger than 65535-20-8
						[or] strlen(psend) is larger than 65535-20-8
		*/
		virtual int send(const char *psend, const unsigned int len_send) = 0;


		/* @ brief: close socket
		*  @ return - int
				0 - success
				-20086 - failure, udp doesnt open
				X - failure, error msg's id
		*/
		virtual int shutdown() = 0;
	};

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


#ifdef __cplusplus
	}
#endif	// !__cplusplus

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	/* @ brief: to create udpsocket oject
	*  @ return - udpsocket*
			X = NULL, failure,  
 	*/
	extern "C" _oct_udp_api_export_  udpsocket* udp_create();


	/* @ brief: release udpsocket pointer , and set pudp's value is NULL
	*  @ const udpsocket * pudp - from udpsocket* udp_create()  
	*  @ return -  void
	*/
	extern "C" _oct_udp_api_export_ void udp_release(udpsocket* pudp);

	

	/* @ brief: to create udpsocket oject, only for windows , cannot support non-windows
				if you call it on non-windows platform, it will return NULL.
				why? it only supports on windows, because it bases on overlapped model on windows
	*  @ return -  udpsocket*
			X = NULL, failure,
	*/
	extern "C" _oct_udp_api_export_  udpsocket* udp_wsa_create();



//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
}

#endif // ! _udp_socket_interface_h

