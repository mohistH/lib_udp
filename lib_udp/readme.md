
## 仓库地址
	- [gitee](https://gitee.com/mohistH/lib_udp)
	- [github](https://github.com/mohistH/lib_udp)


## 特点
 1. 支持IPV4 和 ipv6 
 2. 支持UDP 组播、单播、 广播


## 使用流程
 1. 初始化 
 2. 发送 | 接收
 3. 退出

## 一个范例
> 例子可在example目录下main.cc中找到

### 1. 创建
```
pudp   = lib_commu::NewUDP();
```

#### 2.1 初始化
```
UDPInit param;

int uni_or_multi                                = 0;
std::cout << "1-multi, 0 -uni:";
std::cin >> uni_or_multi;

if (1                                           == uni_or_multi)
{
	/// 组播
	param.cast_type_                            = CT_MULTI;
	param.dest_ip_                              = std::string("233.0.0.212");
}
else
{
	/// 单播
	param.cast_type_                            = CT_UNI;
	param.dest_ip_                              = std::string("10.0.0.6");
}
param.local_ip_                                 = std::string("10.0.0.6");

param.dest_port_                                = 27500;
param.socket_ver_                               = SOCKET_VERSION_4;

/// 初始化
int ret                                         = pudp->Init(param);
if (0 != ret)
{
	///
	std::cout << "\n\n init_failed              =" << ret << "\n\n";
}
else
{
	/// 成功
}
``` 
#### 2.2 接收
 1. 如果需要接收数据， 将**2.1**中param的成员变量指向**icommunication.h**中的**IDataDispatch**类的对象
 2. 下面的演示代码中， NetUDP 继承了**IDataDispatch** 并实现了RecvData函数，所以，赋值使用如下：
```
param.recv_data_							= this;
```

### 3. 发送数据
```
/// 发送数据
char arr[]                              	= "1234567890";

/// 循环发送
for (int index                              = 0; index < 6; ++index)
{
	ret                                     = pudp->Send(arr, strlen(arr));
	if (0 != ret)
	{
		std::cout << "\n\n send_failed, ret =" << ret << "\n\n";
	}
	else
	{
		/// 发送成功
		std::cout << "\n\n send_success\n\n";
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(100));
}
```

### 4. 接收数据
> 应用层接收数据需要继承**icommunication.h**中的**IDataDispatch**类, 当底层收到数据后， 调用该函数
```
/// ----------------------------------------------------------------------------------------
/// @brief:  继承 IDataDispatch  实现数据接收与发送结合
/// ----------------------------------------------------------------------------------------
class NetUDP : public lib_commu::IDataDispatch, public lib_commu::SendResultNotify
{
public:
	/// --------------------------------------------------------------------------------
	/// @brief: RecvData
	/// --------------------------------------------------------------------------------
	virtual int RecvData(const char* pdata, const size_t len) override
	{
		//throw std::logic_error("The method or operation is not implemented.");
		std::cout << "\n rec vdata                      =";
		for (size_t index                               = 0; index < len; ++index)
		{
			std::cout << pdata[index] << " ";
		}

		return 0;
	}
}
```




## 更新日志

### 18/07/2022 
 1. 重构初始化接口你Init, 将接收数据对象放入结构体中
 2. 增加发送结果通知函数， 应用层可继承实现，
 3. 增加发送缓冲 send_event_buf_, 改为创建10个， 当发送数据时，从这里获取， 如果event不够，则返回6(先前是，当发送数据时，创建UvEVent对象， 发送结束，再释放，改为初始化申请一段内存，避免频繁申请内存空间和释放)
 4. BUG： send_event_buf_ 需要增加互斥， 待完成（因为发送线程和回调线程都会访问该缓冲区）



### 16/07/2022
 1. 重构底层发送和接收 改为使用 LIBUV
 2. libuv使用的时VS2015 编译的64位， 后期改为使用源码
 3. 目前仅能支持windows， 后期改为支持linux等多平台
 4. 参考[TestLibuv](https://github.com/zym479466697/TestLibuv) 做了封装


### 11/06/2021
 1. 增加IPV6
 2. ipv6通过windows10初步测试
 3.ipv6包括： 接收和发送
 5.增加错误代码接口


### 30/05/2021
  1.增加IPv6，待测
  2.全部改为Unicode编码， 无签名


### 23-05-2021
  1.模块内部增加优化

### 15/12/2020
  1. cmake重新配置，改为支持**modern cmake**
  2. 创建cmake文件夹，并创建**spdlog.cmake**文件
  3. 类的成员函数和成员变量重命名
  4. 

### 11/06/2021
 1. 增加IPV6
 2. ipv6通过windows10初步测试
 3.ipv6包括： 接收和发送
 5.增加错误代码接口

### 30/05/2021
  1.增加IPv6，待测
  2.全部改为Unicode编码， 无签名
