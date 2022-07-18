#ifndef UV_UTILS_H
#define UV_UTILS_H
#include <string>
#include <assert.h>
#include <memory.h>
#include "uv.h"

//��Androidƽ̨��Ҳ��Ϊ��linux
#ifdef ANDROID
#define __linux__ ANDROID
#endif
#if defined (WIN32) || defined(_WIN32)
#include <windows.h>
#endif
#ifdef __linux__
#include <pthread.h>
#include <unistd.h>
#endif
//����
#if defined (WIN32) || defined(_WIN32)
#define uv_thread_close(t) (CloseHandle(t)!=FALSE)
#define uv_thread_sleep(ms) Sleep(ms);//˯��ms����
#define uv_thread_id GetCurrentThreadId//�õ���ǰ�߳̾��
#define ThreadSleep(ms) Sleep(ms);//˯��ms����
#elif defined(__linux__)
#define uv_thread_close(t) ()
#define uv_thread_sleep(ms) usleep((ms) * 1000)
#define uv_thread_id pthread_self//�õ���ǰ�߳̾��
#define ThreadSleep(ms) usleep((ms) * 1000)//˯��ms����
#else
#error "no supported os"
#endif



#define UV_LOG_LEVEL_ERROR	0
#define UV_LOG_LEVEL_INFO		1
void LogUv(int level, const char * file, int line, char *func, char* format, ...);
#define LOGI(format, ...)  do{  LogUv(UV_LOG_LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__);} while (0)
#define LOGE(format, ...)  do{  LogUv(UV_LOG_LEVEL_ERROR, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__);} while (0)

namespace uv
{
	inline std::string GetUVError(int errcode)
	{
		if (0 == errcode) {
			return "";
		}
		std::string err;
		auto tmpChar = uv_err_name(errcode);
		if (tmpChar) {
			err = tmpChar;
			err += ":";
		}else{
			err = "unknown system errcode "+std::to_string((long long)errcode);
			err += ":";
		}
		tmpChar = uv_strerror(errcode);
		if (tmpChar) {
			err += tmpChar;
		}
		return std::move(err);
	}

	class CMutex
	{
	public:
		explicit CMutex()
		{
			uv_mutex_init(&mut_);
		}
		~CMutex(void)
		{
			uv_mutex_destroy(&mut_);
		}
		void Lock()
		{
			uv_mutex_lock(&mut_);
		}
		void UnLock()
		{
			uv_mutex_unlock(&mut_);
		}
		bool TryLock()
		{
			return uv_mutex_trylock(&mut_) == 0;
		}
	private:
		uv_mutex_t mut_;
		friend class CUVCond;
		friend class CAutoLock;
	private:
		CMutex(const CMutex&);
		CMutex& operator =(const CMutex&);
	};

	class CAutoLock
	{
	public:
		explicit CAutoLock(uv_mutex_t* mut): mut_(mut)
		{
			uv_mutex_lock(mut_);
		}
		explicit CAutoLock(CMutex* mut): mut_(&mut->mut_)
		{
			uv_mutex_lock(mut_);
		}
		~CAutoLock(void)
		{
			uv_mutex_unlock(mut_);
		}
	private:
		uv_mutex_t* mut_;
	private://private�У���ֹ���ƺ͸�ֵ
		CAutoLock(const CAutoLock&);
		CAutoLock& operator =(const CAutoLock&);
	};

	template<typename T>
	class PodCircularBuffer
	{
	public:
		PodCircularBuffer(int capacity)
			:m_nBufSize(capacity),m_nReadPos(0),m_nWritePos(0)
			,m_bEmpty(true),m_bFull(false) {
			m_pBuf = new T[m_nBufSize];
		}
		virtual ~PodCircularBuffer() {
			delete[] m_pBuf;
		}

		bool full() const{
			return m_bFull;
		}
		bool empty() const{
			return m_bEmpty;
		}
		void clear() {
			m_nReadPos = 0;
			m_nWritePos = 0;
			m_bEmpty = true;
			m_bFull = false;
		}

		/************************************************************************/
		/* ��ȡ��������Ч���ݳ���                                               */
		/************************************************************************/
		size_t size() const {
			if(m_bEmpty) {
				return 0;
			} else if(m_bFull) {
				return m_nBufSize;
			} else if(m_nReadPos < m_nWritePos) {
				return m_nWritePos - m_nReadPos;
			} else {
				return m_nBufSize - m_nReadPos + m_nWritePos;
			}
		}

		size_t capacity() const {
			return m_nBufSize;
		}
	
		T* data() {
			return m_pBuf;
		}

		const T* data() const{
			return m_pBuf;
		}
		/************************************************************************/
		/* �򻺳���д�����ݣ�����ʵ��д��Ķ�����                               */
		/************************************************************************/
		int write(const T* buf, int count) {
			if(count <= 0) {
				return 0;
			}
			if(m_bFull) {// ���������������ܼ���д��
				return 0;
			}
			m_bEmpty = false;
			if(m_nReadPos == m_nWritePos) { // ������Ϊ��ʱ
				/*                          == �ڴ�ģ�� ==
				   (empty)             m_nReadPos                (empty)
				|----------------------------------|-----------------------------------------|
					   m_nWritePos        m_nBufSize
				*/
				int leftcount = m_nBufSize - m_nWritePos;
				if(leftcount > count) {
					memcpy(&m_pBuf[m_nWritePos], buf, count * sizeof(T));
					m_nWritePos += count;
					return count;
				} else {
					memcpy(&m_pBuf[m_nWritePos], buf, leftcount * sizeof(T));
					m_nWritePos = (m_nReadPos > count - leftcount) ? count - leftcount : m_nWritePos;
					memcpy(m_pBuf, &buf[leftcount], m_nWritePos * sizeof(T));
					m_bFull = (m_nWritePos == m_nReadPos);
					return leftcount + m_nWritePos;
				}
			} else if(m_nReadPos < m_nWritePos) { // ��ʣ��ռ��д��
				/*                           == �ڴ�ģ�� ==
				 (empty)                 (data)                     (empty)
				|-------------------|----------------------------|---------------------------|
				   m_nReadPos                m_nWritePos       (leftcount)
				*/
				// ʣ�໺������С(��д��λ�õ�������β)

				int leftcount = m_nBufSize - m_nWritePos;
				if(leftcount > count) { // ���㹻��ʣ��ռ���
					memcpy(&m_pBuf[m_nWritePos], buf, count * sizeof(T));
					m_nWritePos += count;
					m_bFull = (m_nReadPos == m_nWritePos);
					assert(m_nReadPos <= m_nBufSize);
					assert(m_nWritePos <= m_nBufSize);
					return count;
				} else {   // ʣ��ռ䲻��
					// �������ʣ��ռ䣬�ٻ�ͷ�ҿռ���
					memcpy(&m_pBuf[m_nWritePos], buf, leftcount * sizeof(T));
					m_nWritePos = (m_nReadPos >= count - leftcount) ? count - leftcount : m_nReadPos;
					memcpy(m_pBuf, &buf[leftcount], m_nWritePos * sizeof(T));
					m_bFull = (m_nReadPos == m_nWritePos);
					assert(m_nReadPos <= m_nBufSize);
					assert(m_nWritePos <= m_nBufSize);
					return leftcount + m_nWritePos;
				}
			} else {
				/*                          == �ڴ�ģ�� ==
				 (unread)                 (read)                     (unread)
				|-------------------|----------------------------|---------------------------|
					m_nWritePos    (leftcount)    m_nReadPos
				*/
				int leftcount = m_nReadPos - m_nWritePos;
				if(leftcount > count) {
					// ���㹻��ʣ��ռ���
					memcpy(&m_pBuf[m_nWritePos], buf, count * sizeof(T));
					m_nWritePos += count;
					m_bFull = (m_nReadPos == m_nWritePos);
					assert(m_nReadPos <= m_nBufSize);
					assert(m_nWritePos <= m_nBufSize);
					return count;
				} else {
					// ʣ��ռ䲻��ʱҪ�������������
					memcpy(&m_pBuf[m_nWritePos], buf, leftcount * sizeof(T));
					m_nWritePos += leftcount;
					m_bFull = (m_nReadPos == m_nWritePos);
					assert(m_bFull);
					assert(m_nReadPos <= m_nBufSize);
					assert(m_nWritePos <= m_nBufSize);
					return leftcount;
				}
			}
		}

		/************************************************************************/
		/* �ӻ����������ݣ�����ʵ�ʶ�ȡ���ֽ���                                 */
		/************************************************************************/
		int read(T* buf, int count) {
			if(count <= 0) {
				return 0;
			}
			if(m_bEmpty) {// �������գ����ܼ�����ȡ����
				return 0;
			}
			m_bFull = false;
			if(m_nReadPos == m_nWritePos) { // ��������ʱ
				/*                          == �ڴ�ģ�� ==
				 (data)          m_nReadPos                (data)
				|--------------------------------|--------------------------------------------|
				  m_nWritePos         m_nBufSize
				*/
				int leftcount = m_nBufSize - m_nReadPos;
				if(leftcount > count) {
					memcpy(buf, &m_pBuf[m_nReadPos], count * sizeof(T));
					m_nReadPos += count;
					m_bEmpty = (m_nReadPos == m_nWritePos);
					return count;
				} else {
					memcpy(buf, &m_pBuf[m_nReadPos], leftcount * sizeof(T));
					m_nReadPos = (m_nWritePos > count - leftcount) ? count - leftcount : m_nWritePos;
					memcpy(&buf[leftcount], m_pBuf, m_nReadPos * sizeof(T));
					m_bEmpty = (m_nReadPos == m_nWritePos);
					return leftcount + m_nReadPos;
				}
			} else if(m_nReadPos < m_nWritePos) { // дָ����ǰ(δ�����������ӵ�)
				/*                          == �ڴ�ģ�� ==
				 (read)                 (unread)                      (read)
				|-------------------|----------------------------|---------------------------|
				   m_nReadPos                m_nWritePos                     m_nBufSize
				*/
				int leftcount = m_nWritePos - m_nReadPos;
				int c = (leftcount > count) ? count : leftcount;
				memcpy(buf, &m_pBuf[m_nReadPos], c * sizeof(T));
				m_nReadPos += c;
				m_bEmpty = (m_nReadPos == m_nWritePos);
				assert(m_nReadPos <= m_nBufSize);
				assert(m_nWritePos <= m_nBufSize);
				return c;
			} else {      // ��ָ����ǰ(δ�����ݿ����ǲ����ӵ�)
				/*                          == �ڴ�ģ�� ==
				   (unread)                (read)                      (unread)
				|-------------------|----------------------------|---------------------------|
					m_nWritePos                  m_nReadPos                  m_nBufSize

				*/
				int leftcount = m_nBufSize - m_nReadPos;
				if(leftcount > count) { // δ������������ֱ�Ӷ�ȡ����
					memcpy(buf, &m_pBuf[m_nReadPos], count * sizeof(T));
					m_nReadPos += count;
					m_bEmpty = (m_nReadPos == m_nWritePos);
					assert(m_nReadPos <= m_nBufSize);
					assert(m_nWritePos <= m_nBufSize);
					return count;
				} else {   // δ�����������㣬��ص�������ͷ��ʼ��
					memcpy(buf, &m_pBuf[m_nReadPos], leftcount * sizeof(T));
					m_nReadPos = (m_nWritePos >= count - leftcount) ? count - leftcount : m_nWritePos;
					memcpy(&buf[leftcount], m_pBuf, m_nReadPos * sizeof(T));
					m_bEmpty = (m_nReadPos == m_nWritePos);
					assert(m_nReadPos <= m_nBufSize);
					assert(m_nWritePos <= m_nBufSize);
					return leftcount + m_nReadPos;
				}
			}
		}

	private:
		bool m_bEmpty, m_bFull;
		T * m_pBuf;
		int m_nBufSize;
		int m_nReadPos;
		int m_nWritePos;
	private://Noncopyable
		PodCircularBuffer(const PodCircularBuffer&);
		const PodCircularBuffer& operator=(const PodCircularBuffer&);
	};


	typedef void (*entry)(void* arg);
	class CUVThread
	{
	public:
		explicit CUVThread(entry fun, void* arg)
			: fun_(fun), arg_(arg), isrunning_(false)
		{

		}
		~CUVThread(void)
		{
			if (isrunning_) {
				uv_thread_join(&thread_);
			}
			isrunning_ = false;
		}
		void Start()
		{
			if (isrunning_) {
				return;
			}
			uv_thread_create(&thread_, fun_, arg_);
			isrunning_ = true;
		}
		void Stop()
		{
			if (!isrunning_) {
				return;
			}
			uv_thread_join(&thread_);
			isrunning_ = false;
		}
		void Sleep(int64_t millsec)
		{
			uv_thread_sleep(millsec);
		}
		int GetThreadID(void) const
		{
			return uv_thread_id();
		}
		bool IsRunning(void) const
		{
			return isrunning_;
		}
	private:
		uv_thread_t thread_;
		entry fun_;
		void* arg_;
		bool isrunning_;
	};
}

#endif