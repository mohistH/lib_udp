
## 更新日志
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


## 简介
 - 1.这是一个使用C++11语法封装的UDP，包括： ipv4 和 ipv6
 - 2.ipv4支持： 单播、组播、广播; linux + windows10测试通过，接收和发送均成功
 - 3.ipv6通过windows10初步测试。收发均成功； 组播待测
 - 5.支持接收和发送
 - 6.使用cmake管理的项目
 - 7.提供了调用范例
 - 8.Linux支持仔细测试，最近比较忙。 后期更新
 - 9.欢迎留言指导

## 接口分类
 针对使用场景： 初始化， 发送， 接收， 关闭， 如果不接收， 则只有3个接口。 接口函数返回值后面再更新

## 使用
### 接收
  接收需要重载 **iudp.h**中的类**irecv_data**的on_recv_data_函数。 **on_recv_data_**原型如下：
```
		///  --------------------------------------------------------------------------------
		///  @brief: 	接收函数，
		///  @param:	const unsigned char * pdata_recv - 接收的数据
		///  @param:	const unsigned int recv_data_len - 接收数据长度
		///  @return:	void	
		/// 			
		///  --------------------------------------------------------------------------------
		virtual void on_recv_data_(const unsigned char *pdata_recv, const unsigned int recv_data_len) = 0;
```

#### 接收继承范例
 - 你可以在 **example/main.cc** 中找到下面的代码
 - 类**my_udp**继承了类**irecv_data**类， 并实现函数**on_recv_data_**
```
class my_udp : public irecv_data
{
public:
	/// ...... 
	/// ...... 

	/// you must override this function to recv data
	void on_recv_data_(const unsigned char *pdata_recv, const unsigned int recv_data_len)
	{
		/// -------------------------------------------------------------------------------
		/// 1. recv data
		std::cout << "\n --------------AAAAAA data length = " << recv_data_len << "\n";
		for (unsigned int i = 0; i < recv_data_len; i++)
		{
			if (i == 10)
				std:: cout << "\n";

			std::cout << pdata_recv[i] << ", ";
		}
		
		std::cout << "\n";
	}

	/// .... 
```
  当接收到数据，底层会调用该函数

#### 接收要注意
 - 如果需要接收数据，接口类 **iudp** 的接口 **init_** 的第二个参数需要传递继承自 接口类**irecv_data**的 **on_recv_data_** 接口
 - 如果不接收数据，接口类 **iudp** 的接口 **init_** 的第二个参数传递 NULL（或nullptr)即可， 也不需要继承  接口类**irecv_data**
 

### 接口类iudp
 - iudp提供了udp的初始化、发送数据和关闭，提供了接口 **error_id_()** 获取错误代码
 - 接口返回值详见代码（代码写的比较详细了)
 - 接口尽量设计的简洁 

#### 代码
```
/// ----------------------------------------------------------------------------
	/// @brief: UDP接口类
	/// ----------------------------------------------------------------------------
	class iudp
	{
	public:
		virtual ~iudp(){}

		/// ------------------------------------------------------------
		/// @brief：初始化
		/// @param: const udp_param & param - 初始化参数
		/// @param: irecv_data * pfunc_recv - 接收对象
		/// @return: int
		/// 			0 - 成功
		///				-1 - 失败，param.socket_version_ 传递错误
		/// --------------------------------------------------------------
		///			ipv4和ipv6错误， 请调用error_id_()获取错误代码
		///				1 - 失败， 端口为0
		///				2 - 失败,  套接字创建失败
		///				3 - 失败，设置发送超时失败
		///				5 - 失败, 设置接收超时失败
		///				6 - 失败, 设置发送缓冲失败
		///				7 - 失败，设置接收缓冲失败
		///				8 - 失败，设置地址宠用失败
		///				9 - 失败， 绑定套接字失败
		///				10 、11、12 - 失败， 设置套接字 组播属性失败
		///				13 - 失败，设置广播失败
		///				15 - 失败，param._cast_type参数值传递错误
		/// ------------------------------------------------------------
		virtual int init_(const udp_param& param, irecv_data* pfunc_recv /* = nullptr */) = 0;
	

		/// ------------------------------------------------------------
		/// @brief：发送数据
		/// @param: const unsigned char * psend - 待发送数据
		/// @param: const unsigned int len_send - 待发送数据长度
		/// @return: int
		/// 		-1 - 失败。初始化socket版本错误
		///			0 - 成功
		///			1 - 失败, 参数【psend】为空或 【len_max_send】等于0 或则len_max_send大于发送缓冲区长度(10k)
		///			2 - 失败，套接字创建失败
		///			3 - 失败， 发送数据失败, 请调用error_id_()
		/// ------------------------------------------------------------
		virtual int send_(const unsigned char* psend, const unsigned int send_len) = 0;
		
		///  --------------------------------------------------------------------------------
		///  @brief: 	关闭
		///  @return:	int	
		/// 			0 - 成功
		///				其他， 失败
		///  --------------------------------------------------------------------------------
		virtual int shutdown_() = 0;

		/// ------------------------------------------------------------
		/// @brief：返回错误ID
		/// @return: int
		/// 			
		/// ------------------------------------------------------------
		virtual int error_id_() = 0;

	};
```

### 创建和销毁
#### 创建  
 - 调用 iudp.h 中的 **udp_create_()** 可创建iudp。 
 - 函数说明
```
	///  --------------------------------------------------------------------------------
	///  @brief: 	创建 udp对象
	///  @return:	lib_udp		*	
	/// 			NULL- 创建失败
	///				!= null -  成功
	///  --------------------------------------------------------------------------------
	lib_udp_api iudp		* udp_create_();
``` 

#### 销毁
 - 调用 iudp.h 中的**udp_release_(iudp* pudp)** 可销毁**udp_create_**创建的对象
 - 函数说明
```
	///  --------------------------------------------------------------------------------
	///  @brief: 	释放申请的资源， 内部释放后，见其设置为NULL
	///  @param:	iudp * pudp - 来自【udp_create_】的创建结果
	///  @return:	lib_udp_api lib_udp *	
	/// 			pudp = NULL   
	///  --------------------------------------------------------------------------------
	lib_udp_api iudp * udp_release_(iudp* pudp);
```
- 调用范例
```
pudp_ = udp_release_(pudp_);
```

## 项目地址
  [gitee](https://gitee.com/mohistH/lib_udp/tree/master/lib_udp) 和 [github](https://github.com/mohistH/lib_udp)


## 接口使用范例
 - 你可以在 example/main.cc中找到下面的代码
### 代码
#### 接收
```
/// to recv data, you must inherit udpsocket_recv class
class my_udp : public irecv_data
{
public:

	/// constructor
	my_udp()
	{
		if (NULL == pudp_)
			pudp_ = udp_create_();//// std::unique_ptr<iudp>(lib_udp::udp_create_()).get();
	}


	/// deconstructor
	virtual ~my_udp()
	{
		pudp_ = udp_release_(pudp_);
	}


	/// you must override this function to recv data
	void on_recv_data_(const unsigned char *pdata_recv, const unsigned int recv_data_len)
	{
		/// -------------------------------------------------------------------------------
		/// 1. recv data
		std::cout << "\n --------------AAAAAA data length = " << recv_data_len << "\n";
		for (unsigned int i = 0; i < recv_data_len; i++)
		{
			if (i == 10)
				std:: cout << "\n";

			std::cout << pdata_recv[i] << ", ";
		}
		
		std::cout << "\n";
	}

	/// -------------------------------------------------------------------------------
	int init_(udp_param& param)
	{
		if (pudp_)
			return pudp_->init_(param, this);

		return -20000;
	}

	/// 
	int send_(const char *psend, const int len_send)
	{
		if (pudp_)
			return pudp_->send_((const unsigned char*)psend, len_send);

		return -20000;
	}

	/// 
	int shutdown_()
	{
		if (pudp_)
			return pudp_->shutdown_();

		return -20000;
	}

	int error_id_()
	{
		if (pudp_)
			return pudp_->error_id_();

		return -20000;
	}

private:
	iudp* pudp_ = NULL;
};
```

#### 初始化、发送、关闭
```
	/// -------------------------------------------------------------------------------
	/// 1. to prepare params to initialize
	udp_param param;

	param._is_log_debug		= false;
	param._cast_type 		= lib_udp::udp_multi_cast;
	param._port_dst 		= 10086;
	param._recv_loop 		= true;
	param.socket_version_	= kipv6;

#ifdef _WIN32
	char arr_ipv4[] = "10.0.0.5";
#elif __linux__
	char arr_ipv4[] = "192.168.15.129";
#else 
	char arr_ipv4[] = "10.1.1.3";
#endif///

	std::cout << "local IP = " << arr_ipv4 << std::endl;
	char arr_dst[] = "233.0.0.11";

	//param.dest_ip_.value_ = std::string(arr_dst);
	//param.local_ip_.value_ = std::string(arr_ipv4);

	std::string str_ipv6;
	int indexxxx = 0;
	for (auto list_item : ip6_list)
	{
		str_ipv6 = list_item;
		if (3 == indexxxx)
			break;
		++indexxxx;
	}

	param.dest_ip_.value_	= std::string("FF02::1");
	param.local_ip_.value_ = str_ipv6;/// ip6_list.front();


	
	
	/// -------------------------------------------------------------------------------
	/// 1. to create 
	std::unique_ptr<my_udp> pmy_udp(new(std::nothrow) my_udp);

	//my_udp* pmy_udp = new(std::nothrow) my_udp;

	/// failure
	if (!pmy_udp)
	{
		std::cout << "\n my_udp crated failure\n";

#ifdef _WIN32
		system("pause");
#endif ///_WIN32
		return 0;
	}
	

	/// -------------------------------------------------------------------------------
	/// 2. to initialize udp
	int ret = 0;
	ret = pmy_udp->init_(param);
	if (0 != ret)
	{
		std::cout << "\ninit error , id = " << ret << ", error id=" << pmy_udp->error_id_() << "\n\n";
		ret = pmy_udp->shutdown_();
		if (0 != ret)
				std::cout << "\nshutdown error , id = " << ret << "\n";
#ifdef _WIN32
		system("pause");
#endif ///_WIN32

		return 0;
	}

	//pmy_udp->shutdown_();

	////delete pmy_udp;
	////pmy_udp = NULL;

	//return 0;


	std::cout << "\n init_ip4 success\n";

	/// -------------------------------------------------------------------------------
	/// 4. to send data
	char arr[] = "1234567890";
	for (int i = 0; i < 1; i++)
	{
		int send_len = pmy_udp->send_(arr, strlen(arr));
		/// to output the result 
		std::cout << "udp send, send length = " << send_len << "\n";
		std::this_thread::sleep_for(std::chrono::milliseconds(1000 * 1));
	}



	/// to recv data, it needs to rest
	std::cout << "\n---------------------------main 1111----------------------------\n";
	std::this_thread::sleep_for(std::chrono::milliseconds(1000 * 1));
	std::cout << "\n---------------------------main 2222----------------------------\n";
	pmy_udp->shutdown_();
	std::cout << "\n---------------------------main 3333----------------------------\n";
	std::cout << "\nudp has closed\n";
```