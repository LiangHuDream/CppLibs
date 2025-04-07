### 第十四章核心内容总结

#### 一、重点内容
1. **ROS 2设计目标**
   - 解决ROS 1的实时性差、单点故障、依赖Linux等问题
   - 支持跨平台（Windows/RTOS/微控制器）
   - 支持生产环境部署
   - 改进通信机制（QoS配置）

2. **DDS中间件**
   - 核心通信层采用DDS标准
   - 支持实时通信和安全认证
   - 可配置QoS策略（可靠性/持久性/生命周期）

3. **通信模型**
   - 完全去中心化架构
   - 支持四种通信模式：
     * 基于话题的发布-订阅
     * 服务调用
     * 参数服务器
     * Actionlib动作库

4. **安装部署**
   - Ubuntu安装步骤：添加APT源->二进制安装->环境配置
   - Windows安装复杂度：需要VS2015/OpenSSL/Chocolatey
   - 支持多版本共存（ROS 1与ROS 2）

#### 二、难点解析
1. **DDS实现机制**
   - RTPS协议实现节点发现
   - 数据分片传输策略
   - 安全策略（DDS-Security）

2. **通信质量配置**
   - 可靠性策略（BEST_EFFORT vs RELIABLE）
   - 持久性策略（VOLATILE vs TRANSIENT_LOCAL）
   - 生命周期策略（自动清理机制）

3. **与ROS 1集成**
   - ros1_bridge工作原理
   - 消息类型映射规则
   - 双向通信性能损耗

4. **编译系统改进**
   - ament替代catkin
   - colcon统一编译工具
   - 包管理机制变化

---

### 10道进阶多选题

1. **ROS 2的设计目标包含哪些？**
   A. 增强实时性能  
   B. 支持Windows操作系统  
   C. 改进rviz图形界面  
   D. 适应生产环境需求  
   E. 完全兼容ROS 1所有功能

2. **关于DDS中间件的正确描述是：**
   A. 强制使用TCP协议  
   B. 支持安全认证机制  
   C. 提供QoS配置接口  
   D. 仅适用于Linux平台  
   E. 替代了ROS 1的XML-RPC

3. **ROS 2通信模型包含哪些模式？**
   A. 话题发布订阅  
   B. 参数服务器  
   C. 节点管理器中心化发现  
   D. 动作库  
   E. 分布式服务调用

4. **Windows安装ROS 2需要：**
   A. Visual Studio 2017  
   B. Python 3.5+  
   C. Chocolatey包管理器  
   D. Ubuntu子系统  
   E. OpenSSL库

5. **ros1_bridge功能包支持：**
   A. 自动转换所有消息类型  
   B. 双向话题通信  
   C. 服务调用转换  
   D. 实时数据同步  
   E. 零配置自动发现

6. **关于ROS 2 QoS策略的正确选项：**
   A. RELIABLE保证数据必达  
   B. VOLATILE不保留历史数据  
   C. TRANSIENT_LOCAL保存离线数据  
   D. BEST_EFFORT可能丢数据  
   E. LIFESPAN自动清理过期数据

7. **ament编译系统的特点包括：**
   A. 兼容catkin_make  
   B. 支持Python包管理  
   C. 使用colcon构建工具  
   D. 依赖CMake配置  
   E. 需要rosdep初始化

8. **ROS 2节点发现机制：**
   A. 依赖roscore  
   B. 基于DDS发现协议  
   C. 支持局域网自动发现  
   D. 需要手动配置IP  
   E. 使用UDP多播

9. **与ROS 1相比，ROS 2的重大改进：**
   A. 去除master节点  
   B. 支持多机器人系统  
   C. 增加节点生命周期管理  
   D. 改进调试工具  
   E. 增强Python支持

10. **创建ROS 2服务的正确步骤：**
    A. 定义.srv文件  
    B. 配置CMakeLists.txt  
    C. 注册消息依赖  
    D. 使用rosmsg生成代码  
    E. 修改package.xml

---

### 答案与解析

1. **ABD**  
   C错误-界面改进非核心目标；E错误-不完全兼容

2. **BCE**  
   A错误-支持多种协议；D错误-跨平台

3. **ABDE**  
   C错误-发现机制去中心化

4. **BCE**  
   A错误-需VS2015；D非必需

5. **BC**  
   A错误-需手动定义；D错误-有延迟；E错误-需配置

6. **ABCDE**  
   全符合QoS规范

7. **BCD**  
   A错误-不兼容；E非编译系统功能

8. **BCE**  
   A错误-无roscore；D可选配置

9. **ABC**  
   D改进有限；E已有支持

10. **ABCE**  
    D错误-rosmsg用于ROS1