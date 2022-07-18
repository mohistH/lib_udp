#ifndef LOOPER_H_
#define LOOPER_H_
#include <string>
#include <list>
#include <queue>
#include "uv.h"
#include "Packet.h"

namespace uv
{
	class ILooperEvent
	{
	public:
		virtual void DoEvent(UvEvent* pevent) = 0;
		virtual void OnHandleClose(uv_handle_t* phandle) = 0;
	};

	class CLooper
	{
	public:
		CLooper();
		~CLooper();
		uv_loop_t* GetLooper();
		bool InitLooper();
		void Close();
		void PushEvent(UvEvent* _event);

		static void WorkThread(void* arg);//connect thread,run until use close the client
		static void AsyncCB(uv_async_t* handle);
		static void CloseWalkCB(uv_handle_t* handle, void* arg);//close all handle in loop
		static void AsyncEvent(uv_async_t* handle);
	protected:
		void DoEvent();
		bool run(int status = UV_RUN_DEFAULT);
	private:
		std::queue<UvEvent*> _event_queue;
		uv::CMutex _event_queue_mutex;
		uv_async_t _async_event_handle;
		uv_async_t _async_looper_close;
		uv_thread_t _thread_handle;
		uv_loop_t _loop;
		bool _is_closed;
		bool _inited;
		std::string _last_error;
	};

}

#endif // LOOPER_H_