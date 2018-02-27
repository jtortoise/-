# 无障碍物自主小车

![demo](_v_images/_demo_1519462518_21695.gif)
## 上位机控制程序环境
- 操作系统环境：基于VM14.0的Manjaro(xfce)操作系统(Arch系)
- 通信硬件：蓝牙4.0  
如果PC或笔记本已经有蓝牙4.0，VMware会自动加载蓝牙模块，不过需要取消与虚拟机共享蓝牙功能：  
**虚拟机-->设置-->USB控制器-->与虚拟机共享蓝牙设备(如果打勾则取消)**。
- Qt版本：Qt5.10+蓝牙操作模块
``` shell
yaourt -S qtcreator
yaourt -S qtconnectivity5
```
- OpenCV版本：OpenCV3.4.0+OpenCV_Contrib  
关于如何安装最新版本的OpenCV以及OpenCV_Contrib库，可以参考youtube上的[OpenCV Basics 11](https://www.youtube.com/watch?v=fIpTks0G2m0&list=PLAp0ZhYvW6XbEveYeefGSuLhaPlFML9gP&index=11)教程。  
## 下位机控制程序环境
小车模型是基于三轮全向轮模型构建的，主控制板是stm32f103c8，电机驱动为a4950驱动芯片，采用CC2541(低功耗蓝牙模块)与上位机进行通信，其负责的主要功能是成功从上位机接收设置电机PWM的命令。
- STM32开发环境：[MDK5](https://www.keil.com/demo/eval/arm.htm#/DOWNLOAD)+STM32CubeMx+STLINK/V2
- 三个电机与主控板端口连接  
motorA 正级 --> PA6(TIM3_CH1)   
motorA 负极 --> PA8(TIM1_CH1)  
motorB 正级 --> PA7(TIM3_CH2)  
motorB 负极 --> PA9(TIM1_CH2)  
motorC 正极 --> PB0(TIM3_CH3)  
motorC 负极 --> PA10(TIM1_CH3)  
- CC2541模块与主板端口连接  
5V    --> 5v  
GND --> GND   
RX    --> PB10(USART3_TX)  
TX    --> PB11(USART3_RX)  
## 简介
这是一个上位机基于Opencv3.x版本的Aruco Marker控制的自主小车(下位机STM32F103C8主控)项目。它是基于Opencv3.x的ArucoMarker算法检测出Marker的3D姿态以及从Marker坐标系转换到摄像头坐标系的旋转向量和目标向量。基于这两个向量，能够把图像中的坐标点(Target)从摄像头坐标系转换成Marker坐标系的坐标点，从而计算出Target跟Marker重心(Marker坐标系原点)的位移以及方向信息。基于以上的位移和方向信息的反馈，构建两个简单的PID控制器，从而能够控制小车自主调节方向以及位移调节。
### 上位机简介
上位机基于OpenCV的GUI窗口交互，设置鼠标事件的回调函数。使用鼠标左键确定目标点，再通过鼠标右键开始驱动小车自主运动到指定的目标点。当鼠标右键确定小车最终的目标点后，上位机利用Qt5.10的BLE(低功耗蓝牙)与下位机的CC2541(低功耗蓝牙模块)进行通信，不断的发送通过2个PID控制器(方向控制器以及位移控制器)计算出的电机PWM值，将PWM值按照给定的数据包格式通过与CC2541固件上已经定义好的Services以及Characteristics进行通信，其中PID控制器的反馈值是由ArucoMarker算法计算出来的方向信息以及位移信息提供的。
### 下位机简介
下位机的主控板是STM32F103C8，使用ST公司提供的最新STM32CubeMx软件以及STM32CubeF1库的HAL驱动，STM32CubeMx用来进行MCU的外设(GPIO、串口、定时器、PWM、中断等)的硬件初始化，利用其串口接收中断回调函数+有限状态机模型组成的数据包接收器，能够轻松地接收到一个个完整的数据包。这些数据包作为一个个上位机发送过来的命令，采用循环队列的数据结构将这些数据包缓存起来，最大程度避免丢包问题。最后关于接收到的命令执行方式采用散列映射的数据结构，将命令字节与相应的回调函数对应起来，能够快速的针对命令进行查找执行。
### 为什么做这东西
这个项目是受到github上[类似项目](https://github.com/JasonZhangHkust/MarkerBasedControl)吸引，看着有点酷就自己动手实现了下。
## 其它资料
### 自定义的数据包格式
| 起始标志 | 数据长度 | 命令 | 附加数据 | BCC校验 |
| --- | --- | --- | --- | --- |
| 1字节 | 1字节 | 1字节 | 不定 | 1字节 |
### 自定义数据包格式各字段说明
- 起始标志：0x55，表示开始接收一个数据包
- 数据长度：命令字段和附加数据字段所占的字节数
- 命      令：用来说明数据包的功能
- 附加数据：命令需要携带的数据，不需要则留空
- BCC校验：命令字段和附加数据字段的所有字节的异或校验
### 参考资料
1. 队列  
[Simple C implementation of queue](https://gist.github.com/ArnonEilat/4471278/20dc5fb9ebd7e17f40e129c7ee2c33833566bcdd)  
[单片机C编程 自定义通用队列接口](https://gist.github.com/jtortoise/86a3554dcbf7463fa22c6658d767b8cf)
2. 散列数据结构  
[Data Structure in C - HashMap](http://www.kaushikbaruah.com/posts/data-structure-in-c-hashmap/)
3. 有限状态机  
[FSM in C](https://gist.github.com/ankurs/355708)  
[Simple Finite State Machines in C](http://blog.ankurs.com/2010/04/simple-finite-state-machines-in-c/)  
[Fundamentals of Programming Finite State Machines](http://retis.sssup.it/~lipari/courses/infbase2012/09.state_machines.pdf)
4. STM32应用程序开发  
[牛人总结的单片机应用程序架构](http://www.ing10bbs.com/forum.php?mod=viewthread&tid=746)  
[Description of STM32F1 HAL and Low layer drivers](http://www.st.com/content/ccc/resource/technical/document/user_manual/72/52/cc/53/05/e3/4c/98/DM00154093.pdf/files/DM00154093.pdf/jcr:content/translations/en.DM00154093.pdf)  
[在具有RTOS的STM32Cube上开发应用](http://www.st.com/content/ccc/resource/technical/document/user_manual/2d/60/ff/15/8c/c9/43/77/DM00105262.pdf/files/DM00105262.pdf/jcr:content/translations/zh.DM00105262.pdf)  
[CMSIS-RTOS](http://www.keil.com/pack/doc/CMSIS/RTOS/html/group__CMSIS__RTOS.html)  
[STM32CubeMX Tutorial Series](https://www.waveshare.com/wiki/STM32CubeMX_Tutorial_Series)
5. 电机控制以及STM32CubeMx上PWM设置
- PWM频率与DC电机调速关系  
[Criteria behind selecting pwm frequency for speed control of a dc motor?](https://electronics.stackexchange.com/questions/67663/criteria-behind-selecting-pwm-frequency-for-speed-control-of-a-dc-motor)  
[AB-022 : PWM Frequency for Linear Motion Control](https://www.precisionmicrodrives.com/application-notes/ab-022-pwm-frequency-for-linear-motion-control)  
[用pwm控制直流电机速度，多大频率合适](https://www.amobbs.com/thread-884767-1-1.html)  
[如何确定直流电机驱动的PWM频率](http://d1.amobbs.com/bbs_upload782111/files_28/ourdev_542192.pdf)
- PWM占空比与DC电机调速关系  
[Pulse Width Modulation](https://www.electronics-tutorials.ws/blog/pulse-width-modulation.html)  
[基于单片机用软件实现直流电机 PWM 调速系统](http://www.fxdzw.com/dpj/MCU_PWM.pdf)
6. 程序堆内存调试工具  
[Using Valgrind to Find Memory Leaks and Invalid Memory Use](https://www.cprogramming.com/debugging/valgrind.html)  
[Valgrind](http://www.valgrind.org/downloads/current.html)
7. BLE协议  
[Bluetooth Low Energy (BLE)](http://www.cypress.com/file/220246/download)  
[introduction-to-bluetooth-low-energy](https://learn.adafruit.com/introduction-to-bluetooth-low-energy?view=all)  
[Bluetooth Low Energy - Part 1: Introduction To BLE](https://www.mikroe.com/blog/bluetooth-low-energy-part-1-introduction-ble)  
[GATT Profile 简介](https://www.race604.com/gatt-profile-intro/)  
[Bluetooth low energy Characteristics, a beginner's tutorial](https://devzone.nordicsemi.com/tutorials/b/bluetooth-low-energy/posts/ble-characteristics-a-beginners-tutorial)
8. OpenCV参考教程  
[OpenCV Basics](https://www.youtube.com/watch?v=l_4fNNyk1aw&list=PLAp0ZhYvW6XbEveYeefGSuLhaPlFML9gP)