#ifndef PACKET_SYNC_H_
#define PACKET_SYNC_H_

#include "uv_utils.h"
#include <functional>
//
//#define NET_PACKAGE_HEADLEN		3*sizeof(uint32_t)
//#define DEF_PACK_HEAD_FLAG			0xD532D532 //协议头标志
//#define PACK_BUFFER_SIZE					(1024*4)
//#define PACK_UDP_SIZE							1400


enum
{
	NET_PACKAGE_HEADLEN = 3 * sizeof(uint32_t),
	DEF_PACK_HEAD_FLAG	= 0xD532D532,
	PACK_BUFFER_SIZE	= 4 * 1024,
	PACK_UDP_SIZE		= 1400,

};



#pragma pack(push,1) 
typedef struct PackHeader
{
	uint32_t flag;
	uint32_t type;
	uint32_t dataSize;
	char data[1];
} NetPacket;
#pragma pack(pop)

enum UV_EVENT_TYPE
{
	UV_EVENT_TYPE_CONNECT,
	UV_EVENT_TYPE_WRITE,
	UV_EVENT_TYPE_CLOSE,
	UV_EVENT_TYPE_SHUTDOWN,
	UV_EVENT_TYPE_BROADCAST,
};

struct	UvEvent
{
	UvEvent()
	{
		_buff = nullptr;
		_size = 0;
		_data = nullptr;
		_id = 0;
	}
	UV_EVENT_TYPE type;
	char* _buff;
	size_t _size;
	std::vector<int> _list;
	struct sockaddr _addr;
	void* _data;

	size_t	 _id = 0;
};

inline UvEvent* CreateUvEvent(UV_EVENT_TYPE type, const char* pBuff = nullptr, int size = 0)
{
	UvEvent* _event = new UvEvent;
	_event->type = type;
	if(pBuff)
	{
		_event->_buff = (char*)malloc(size);
		memcpy(_event->_buff, pBuff, size);
		_event->_size = size;
	}
	return _event;
}

inline void FreeUvEvent(UvEvent* _event)
{
	if(_event->_buff)
	{
		free(_event->_buff);
		_event->_buff = nullptr;
		_event->_size = 0;
		_event->_data = nullptr;
	}
	delete _event;
	_event = nullptr;
}


class PacketSync
{
public:
    PacketSync(){
        read_data = uv_buf_init((char*)malloc(PACK_BUFFER_SIZE), PACK_BUFFER_SIZE); //负责从circulebuffer_读取数据
        real_packet_len = 0;//readdata有效数据长度
    }

    virtual ~PacketSync() {
        free(read_data.base);
    }

    bool Init(uint32_t packhead) {
        _head = packhead;
        return true;
    }

	//接收到数据，把数据保存在circulebuffer_
   void OnRecvData(const unsigned char* data, int len) { 
        int iret = 0;
		bool hasPack = true;
		if(real_packet_len + len > read_data.len) 
		{
			read_data.base = (char*)realloc(read_data.base, real_packet_len + len);
			read_data.len = real_packet_len + len;
		}
		std::memcpy(read_data.base + real_packet_len, data, len);
		real_packet_len += len;

        while (hasPack &&  real_packet_len > NET_PACKAGE_HEADLEN) 
		{
			uint32_t head_flag = *(uint32_t*)(read_data.base);
			if(head_flag == _head)
			{
				uint32_t real_data_len = real_packet_len;
				uint32_t pack_size = *(uint32_t*)(read_data.base + 2*sizeof(uint32_t)) + NET_PACKAGE_HEADLEN;

				if(pack_size <= real_data_len)
				{
					//回调包数据给用户
					if (_func_package_cb){
						_func_package_cb(read_data.base, pack_size);
					}
					std::memmove(read_data.base, read_data.base + pack_size, real_packet_len - pack_size);
					real_packet_len = real_packet_len - pack_size;//删除数据
				}
				else{
					hasPack = false;
				}
			}
			else{
				real_packet_len = 0;
			}
        }
    }

	void OnPackageCBEvent(std::function<void(const char*, int)> func){
		_func_package_cb = func;
	}
public:
	std::function<void(const char*, int)> _func_package_cb;
private:
    uv_buf_t  read_data;//负责从circulebuffer_读取packet 中data部分
    int real_packet_len;//readdata有效数据长度
	uint32_t _head;//包头
private:
    PacketSync(const PacketSync&);
    PacketSync& operator = (const PacketSync&);
};

typedef struct _write_param{//param of uv_write
	uv_write_t write_req_;//store TCPClient on data
	uv_buf_t buf_;
}write_param;

inline write_param* AllocWriteParam(int size)
{
	write_param* param = (write_param*)malloc(sizeof(*param));
	param->buf_.base = (char*)malloc(size);
	param->buf_.len = size;
	return param;
}

inline void FreeWriteParam(write_param* param)
{
	free(param->buf_.base);
	free(param);
}


typedef struct _send_param{//param of send_udp
	uv_udp_send_t send_req_;//store udp on data
	uv_buf_t buf_;
}send_param;

inline send_param* AllocSendParam(int size)
{
	send_param* param = (send_param*)malloc(sizeof(*param));
	param->buf_.base = (char*)malloc(size);
	param->buf_.len = size;
	return param;
}

inline void FreeSendParam(send_param* param)
{
	free(param->buf_.base);
	free(param);
}

#endif//PACKET_SYNC_H_