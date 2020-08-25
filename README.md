### about  
  A. This is an library of udp, including multicast, unicast and broadcast,  written by c++11.
  B. It could be compiled successfuly on windows(win10), MacOS(mojave) and linux(Fedora)

### 1. usage  
#### 1.1 initialize  
> init_ip4 
> open

#### 1.2  send  
> send  

#### 1.3 recv
> recv_data

#### 1.4 close
> shutdown  

### 2. create and release
#### 2.1 create
  > upd_create
  > udp_wsa_create  

#### 2.2  release
  > udp_release

### 3. Note
* 3.1 recv
> If a udp frame length is largger than 10k,  it will be abandoned  

* 3.2.send
> If the buffer length is largger than (65536 - 8 - 20), it will not be send.  

* 3.3. init_ip4
> this function's first parameter is to set sccket timeout, whose unit is second

* 3.5 recv data
> To get received data, you must create a new class to inherit **udpsocket_recv** class in the **udp_intrface.h** , and implement **recv_data** function, 


### 4.License
[The 3-Clause BSD License](https://opensource.org/licenses/BSD-3-Clause)


