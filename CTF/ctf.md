
    网页导航
    [navisec](https://navisec.io/)
    [渗透师](https://www.shentoushi.top/)

## 基本赛制与题型  
    题目 ---》 flag ---> 分数 ---》名次
    i春秋，e春秋
## 目标
    获取flag
![alt text](image-2.png)
## 比赛形式
![alt text](image-1.png)
![alt text](image.png)
## 题目类型
![alt text](image-3.png)
![alt text](image-4.png)
![alt text](image-5.png)
![alt text](image-6.png)

## 学习攻略

![alt text](image-7.png)
*  网络技术
*  http协议
*  数据库的基本操作，Sql语句（增删查改，ADFC），数据库优化

## 在线演练

![alt text](image-8.png)

    实验吧：http://shiyanbar.com/ctf/practice
    BugkuCTF: http://ctf.bugku.com/login
# 软件
    vm，bp
    kalilinux:已经安装了很多的软件和工具
## 已下载火狐延长版
```bash
    firefox-esr
```

# 杂项的解题思路

## 文件类型的识别

![alt text](image-10.png)
```bash

    file xxxx
```
    更改后缀名，可以将文件的原貌得出
![alt text](image-9.png)

```bash
    winhex //通过识别文件头，用于windows下的情况
```

```bash
    010editor filename
```
    通过010编辑器打开文件，通过查看头文件的第一位16位数值来判断是什么文件

![alt text](image-11.png)

## 文件分离

![alt text](image-12.png)

![alt text](image-13.png)

![alt text](image-14.png)

    EX1  
    1.txt:1234567890abcdefghjk  
    dd if=1.txt of=2.txt bs=5 count=1  
    2.txt:12345  
    dd if=1.txt of=3.txt bs=5 count=2  
    3.txt:1234567890
    dd if=1.txt of=4.txt bs=5 count=1 skip=1  
    4.txt:67890    

![alt text](image-15.png)

![alt text](image-16.png)

## 文件合并

![alt text](image-17.png)
    合并后还要根据md5来判断是否合并成功  

## 图片隐写

### 细微的颜色差异

### GIF多帧隐藏  
- 颜色通道隐藏
- 不同帧图信息隐藏
- 不同帧对比隐写

### Exif信息隐藏

``` bash
    exiftool filename
```
### Steglove 
! [alt text](image-18.png)
![alt text](image-19.png)
### 图片修复
- 图片头修复
- 尾修复
- crc循环效验修复
- 长宽高修复
### 最低位有效位LSB隐写
    Stegslove可以用来处理LSB隐写，在DATA EXact中
    对于图像的RGB有8位，把信息隐藏在最后一位
![alt text](image-20.png)
    对于pdf，bmp的
    ![alt text](image-21.png)
    图片残缺修复
![alt text](image-22.png)
![alt text](image-23.png)
#### PNG图像源文件含义
![alt text](image-24.png)
### 图片加密
![alt text](image-25.png)
![alt text](image-26.png)
![alt text](image-27.png)
![alt text](image-29.png)
![alt text](image-28.png)
![alt text](image-30.png)
- Stegdetect
- outguess
- Jphide
- F5
### 遇到打开是乱码的用16进制查看器

