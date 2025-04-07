
### 第一章内容概述
《ROS机器人开发实践》第一章"初识ROS"主要围绕ROS的定义、发展背景、核心特点展开，重点讲解了ROS在不同操作系统下的安装流程，并提供了源码获取方式和本章内容总结。核心内容包含：
1. ROS的起源与发展目标
2. ROS的模块化、分布式、跨平台等核心特性
3. 不同Ubuntu版本与ROS版本的对应关系
4. 完整的ROS安装流程（软件源配置、密钥添加、初始化等）
5. 环境变量设置与安装验证方法

### 重难点总结
#### 核心重点
1. **ROS设计目标**：
   - 模块化设计
   - 跨语言支持（C++/Python）
   - 代码复用机制
   - 社区共享生态

2. **安装流程关键步骤**：
   ```bash
   sudo sh -c 'echo "deb http://packages.ros.org/ros/ubuntu $(lsb_release -sc) main" > /etc/apt/sources.list.d/ros-latest.list'
   sudo apt-key adv --keyserver 'hkp://keyserver.ubuntu.com:80' --recv-key C1CF6E31E6BADE8868B172B4F42ED6FBAB17C654
   sudo apt update
   sudo apt install ros-<distro>-desktop-full
   sudo rosdep init
   rosdep update
   echo "source /opt/ros/<distro>/setup.bash" >> ~/.bashrc
   ```

3. **版本选择原则**：
   - Ubuntu 16.04对应Kinetic
   - Ubuntu 18.04对应Melodic
   - LTS版本长期支持特性

#### 核心难点
1. rosdep初始化失败的常见原因
2. 多版本ROS环境变量冲突问题
3. 非LTS系统版本兼容性处理
4. 代理服务器配置对安装的影响

### 多选题（每题至少2个正确答案）

1. **关于ROS设计目标，正确的描述是？**
   A. 实现硬件驱动程序标准化  
   B. 支持跨语言开发  
   C. 强制使用C++编程  
   D. 建立开源共享社区  
   E. 必须使用特定IDE开发

2. **正确安装ROS的步骤顺序应为？**
   A. 安装ROS软件包 → 配置软件源 → 初始化rosdep  
   B. 添加密钥 → 更新软件列表 → 安装核心包  
   C. 设置环境变量 → 安装依赖 → 编译工作空间  
   D. 初始化rosdep → 安装桌面完整版 → 更新系统补丁

3. **Ubuntu 16.04系统推荐安装的ROS版本是？**
   A. Kinetic Kame  
   B. Melodic Morenia  
   C. Noetic Ninjemys  
   D. Lunar Loggerhead  
   E. Indigo Igloo

4. **可能导致rosdep初始化失败的原因是？**
   A. 未配置HTTP代理  
   B. /etc/ros/rosdep/sources.list.d权限不足  
   C. Python2环境未安装  
   D. 系统时区设置错误  
   E. 未执行sudo apt update

5. **环境变量配置的正确方法是？**
   A. 在~/.zshrc中添加source命令  
   B. 使用export ROS_DISTRO=kinetic  
   C. 修改/etc/environment文件  
   D. 执行source /opt/ros/kinetic/setup.bash  
   E. 在~/.bashrc末尾追加source命令

6. **ROS的核心特点包括？**
   A. 仅支持单机运行  
   B. 分布式进程通信  
   C. 强制使用XML配置  
   D. 模块化软件包管理  
   E. 依赖特定传感器品牌

7. **安装完整桌面版ROS的正确命令是？**
   A. sudo apt install ros-kinetic-base  
   B. sudo apt install ros-melodic-desktop  
   C. sudo apt install ros-noetic-desktop-full  
   D. sudo apt install ros-kinetic-desktop-full  
   E. sudo apt install ros-core

8. **关于版本选择，正确的说法是？**
   A. Ubuntu 18.04必须安装Noetic  
   B. LTS版本提供5年支持周期  
   C. 非LTS系统无法安装ROS  
   D. Kinetic支持到2021年4月  
   E. 生产环境推荐使用最新版本

9. **初始化rosdep的正确操作包含？**
   A. rosdep update --include-eol-distros  
   B. sudo rosdep fix-permissions  
   C. rosdep install --from-paths src  
   D. sudo rosdep init  
   E. pip install rosdep

10. **验证安装成功的有效方法包括？**
    A. 执行roscore查看节点  
    B. 运行apt list --installed | grep ros  
    C. 检查/opt/ros目录结构  
    D. 查看系统日志文件  
    E. 编译官方tutorials功能包

---

### 答案与解析

1. **BD**  
   B正确：ROS支持C++/Python等多语言；D正确：开源社区是核心设计目标。A错误（硬件驱动非标准化目标），C/E错误（无语言/IDE限制）

2. **B**  
   正确流程：添加密钥→更新→安装。A顺序错误，D步骤不全

3. **AE**  
   Ubuntu 16.04对应Kinetic（A），E（Indigo）是旧版但也兼容

4. **AB**  
   A（网络问题常见），B（权限问题）。C错误（ROS Kinetic需要Python2），D/E无关

5. **DE**  
   D（临时生效），E（永久生效）。A可能有效但非标准方法，B语法错误

6. **BD**  
   B（分布式通信），D（模块化）。A/C/E与ROS特性相悖

7. **CD**  
   C（Noetic完整版），D（Kinetic完整版）。B缺少-full参数

8. **BD**  
   B（LTS支持周期正确），D（Kinetic支持周期）。A错误（18.04用Melodic），C错误（非LTS可安装）

9. **AD**  
   D（初始化命令），A（更新时包含过期版本）。C是依赖安装命令，E非官方方法

10. **ABC**  
   A（核心组件验证），B（包列表检查），C（目录验证）。D无直接关联，E非必要步骤