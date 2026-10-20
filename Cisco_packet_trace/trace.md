


## 网络设备  
 ![alt text](image.png)  

## 终端设备  
 ![alt text](image-1.png)  

## 接口  
 ![alt text](image-2.png)  

## 物理界面
 ![alt text](image-3.png)  
 可以右键来管理线缆
    Shift + P 切换物理模式
    Shift + L 切换逻辑模式
![alt text](image-4.png)
![alt text](image-5.png)
## 点对点
    客户端和服务器软件通常部署在一个独立的服务器上  
    当一个计算机也可以担任两个角色  
    在小型网络中，许多计算机在网络中即是，又是服务器又是客户段，这被称为对等网络  
![alt text](image-6.png)
## 终端设备
    为了区分不同的终端设备，每一台终端  
    都有一个地址，当一台终端设备发起通信的时候，会使用目的终端设备的地址来指定消息发到哪里去
![alt text](image-7.png)
## 中间设备
![alt text](image-8.png)
## 网络介质
![alt text](image-9.png)
## 网络表示
    方便理解
![alt text](image-10.png)
    网络接口（网卡INC）：将设备物理连接到网络。
    物理端口：物理设备上的接口或插口
    接口：网络设备上连接到独立网络的专用端口，在路由器上也成为网络接口
## 拓扑
    物理拓扑
![alt text](image-11.png)
    罗技拓扑
![alt text](image-12.png)
  
LAN局域网：覆盖范围较小
WAN广域网：由多lan组成

## terminal

    PC的terminal界面需要console线连接switch来控制  
    先输入enable进入Switch的管理界面#  

```bash
    configure terminal //进入全局配置界面
    eixt //返回上一级
    line console 0 //进入子路配置模式
    end //返回到普通模式
    ctrl + Z
```
![alt text](image-13.png)

## 基本的IOS命令结构

![alt text](image-14.png)

## Cli的指令热键

![alt text](image-15.png)

## 设备名称

- 以数字开头
- 不包含空格
- 以字母或数字结尾
- 仅使用字母，数字和波折号
- 小于64字符
## 配置密码

    保护用户EXEC模式安全
![alt text](image-16.png)
    保护特权EXEC模式安全  
![alt text](image-17.png)
    保护VTY线路的示例  
![alt text](image-18.png)
```bash
    configure terminal
    service password-encryption //加密密码
```
    查看加密
![alt text](image-19.png)

## 横幅消息
    banner motd #xxx内容#


## 配置文件


### startup-config(启动配置文件)
    启动或重启后不消失，存储再NVRAM中

```bash

    show startup-config
```

### running-config(运行配置文件)

    存储再ram中，重启后或启动消失

```bash
    show running-config
```

## 修改配置

![alt text](image-20.png)

```bash
    copy running-config startup-config  //将目前的配置保存到启动的配置  还可以用来返回上一次配置  
    dir flash //闪存  or nvram //内存  
    reload //重新加载  
    erase startup  //删除启动配置  
    reload //这次启动将恢复默认
```

## 2.6端口和地址  

### .1IP地址  

    IPV4地址，32位，要设置子网掩码，便于区分子网  
    IPV6地址，128位，不区分大小写 

### .2接口和端口 

![alt text](image-21.png)

## 2.7配置IP地址  

    DHCP自动配置  


### .4交换机虚拟接口配置  

![alt text](image-22.png)



## 2.8.1测试配置接口  

```bash
    show ip interface brief  //查看接口ip

```

![alt text](image-23.png)
    想要配置交换机的Vlan IP  

```bash
    enable 
    configure terminal  
    interface vlan 1
    ip address xxx.xxx.xxx.xxx xxx.xxx.xxx.xxx  
    no shutdown
```

```bash pC
    ping xxx.xxx.xxx.xxx
```

## 2.9总结  

![alt text](image-24.png)



##  3.1.2通信基础知识  
    三要素：  
    发送方  
    接收方  
    信道：消息从源到目的地提供路径的介质组成  

## 3.1.5网络协议要求  
    消息编码：采用适合介质的格式，先将信息转化位然后在介质上编码位电压模式，光纤中的红外光，目的主机接受并解码  
    消息的格式和封装：将消息封装在数据包中传输，数据包的格式  
![alt text](image-25.png)
    消息大小：要进行分组传输  
    消息的时序：
    ![alt text](image-26.png)
    消息传输选项  
    ![alt text](image-27.png)

### 网络协议

![alt text](image-28.png)

### tcp/ip协议组
![alt text](image-29.png)
![alt text](image-30.png)
![alt text](image-31.png)
![alt text](image-32.png)
![alt text](image-33.png)

## 3.6数据分装  

    分段  
    排序：标号多个片段以便定向和重组，标识用于片段在到达是对其排序和组装  
    PDU  
![alt text](image-34.png)

## 地址  
    网络层地址：负责将IP数据包从源设备传输到同一段网络或者其他网络中的目的主机  
    物理层地址：将数据链路帧从一个网卡传输到相同网络的另一个网络上  
![alt text](image-35.png)

### 网络层  
![alt text](image-36.png)


### 数据链路层：不同IP网络  
![alt text](image-37.png)

    必须配置默认网关IP，所有向远程网络中的数据包都会走默认网关  
![alt text](image-38.png)

    帧的MAC地址在每段链路上传输的会更改

![alt text](image-39.png)


# 物理层

![alt text](image-40.png)

## 编码  

![alt text](image-41.png)

## 信号  
![alt text](image-42.png)

![alt text](image-43.png)
![alt text](image-44.png)

## 带宽  

![alt text](image-45.png)