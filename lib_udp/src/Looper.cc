#include "Looper.h"
//#include "TcpClient.h"
#define MAXLISTSIZE 20
#include <thread>
#include <iostream>

namespace uv
{
	CLooper::CLooper():_is_closed(false),_inited(false)
	{
	}

	CLooper::~CLooper()
	{
		Close();
		uv_thread_join(&_thread_handle);
		uv_loop_close(&_loop);
	}

	bool CLooper::InitLooper()
	{
		_inited                     = false;
		int _ret                    = uv_loop_init(&_loop);
		if (_ret != 0) {
			_last_error             = GetUVError(_ret);
			return false;
		}

		_ret                        = uv_thread_create(&_thread_handle, WorkThread, this);
		if (_ret) {
			_last_error             = GetUVError(_ret);
			return false;
		}

		_ret                        = uv_async_init(&_loop, &_async_looper_close, AsyncCB);
		if (_ret != 0) {
			_last_error             = GetUVError(_ret);
			return false;
		}
		_async_looper_close.data    = this;

		_ret                        = uv_async_init(&_loop, &_async_event_handle, AsyncEvent);
		if (_ret != 0) {
			_last_error             = GetUVError(_ret);
			return false;
		}
		_async_event_handle.data    = this;

		do{
			if(_inited) break;
			Sleep(10);
		} while (true);

		return true;
	}

	void CLooper::Close()
	{
		if (_is_closed) {
			return;
		}
		_is_closed                  = true;
		uv_async_send(&_async_looper_close);
	}
	
	uv_loop_t* CLooper::GetLooper()
	{
		return &_loop;
	}

	void CLooper::WorkThread(void* arg)
	{
		CLooper* theClass           = (CLooper*)arg;
		theClass->_inited           = true;
		theClass->run();
	}

	bool CLooper::run(int status)
	{
		int _ret                    = uv_run(&_loop, (uv_run_mode)status);
		_is_closed                  = true;
		if (_ret) {
			_last_error             = GetUVError(_ret);
			return false;
		}
		return true;
	}

	void CLooper::CloseWalkCB(uv_handle_t* handle, void* arg)
	{
		CLooper* theclass           = static_cast<CLooper*>(handle->data);
		if(theclass                 == (CLooper* )arg)
		{
			if (!uv_is_closing(handle)){
				uv_close(handle, NULL);
			}
		}
		else
		{
			ILooperEvent* ptrClient = static_cast<ILooperEvent*>(handle->data);
			if(ptrClient){
				ptrClient->OnHandleClose(handle);
			}
		}
	}

	void CLooper::AsyncCB(uv_async_t* handle)
	{
		CLooper* theclass           = (CLooper*)handle->data;
		uv_walk(theclass->GetLooper(), CloseWalkCB, theclass);
	}

	void CLooper::AsyncEvent(uv_async_t* handle)
	{
		CLooper* theclass           = (CLooper*)handle->data;
		theclass->DoEvent();
	}
	
	void CLooper::PushEvent(UvEvent* _event)
	{
		//std::cout << "\n\n\n      =========PushEvent.id=" << std::this_thread::get_id() << "\n\n";

		CAutoLock locker(&_event_queue_mutex);
		_event_queue.push(_event);
		uv_async_send(&_async_event_handle);
	}

	void CLooper::DoEvent()
	{
		//std::cout << "\n\n\n      =========DoEvent.id=" << std::this_thread::get_id() << "\n\n";

		CAutoLock locker(&_event_queue_mutex);
		while(!_event_queue.empty())
		{
			UvEvent *pEvent         = _event_queue.front();
			ILooperEvent* ptrClient = (ILooperEvent*)pEvent->_data;
			ptrClient->DoEvent(pEvent);
			_event_queue.pop();
		}
	}
	
}