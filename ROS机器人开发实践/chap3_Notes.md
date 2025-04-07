### 第三章《ROS基础》内容总结

#### 核心内容：
1. **第一个ROS例程**：通过turtlesim功能包实现小乌龟仿真控制
2. **工作空间与功能包**：
   - 工作空间结构（src/build/devel）
   - `catkin_init_workspace`和`catkin_make`命令
3. **开发环境搭建**：
   - Eclipse工程配置（头文件路径/编译设置）
   - RoboWare Studio的安装与可视化开发
4. **通信机制实践**：
   - 话题通信（Publisher/Subscriber）
   - 服务通信（Server/Client）
   - 自定义消息（msg）和服务（srv）
5. **高级特性**：
   - 命名空间与重映射
   - 分布式多机通信
   - TF工具基础使用

#### 重难点分析：
1. **工作空间覆盖机制**（环境变量优先级）
2. **自定义消息/服务的实现流程**（msg/srv文件定义、依赖包配置、编译生成）
3. **分布式通信环境变量设置**（ROS_MASTER_URI/ROS_HOSTNAME）
4. **launch文件参数配置**（<param>/<arg>区别）
5. **TF坐标变换基础**（广播器/监听器实现）

---

### 10道进阶多选题

1. **关于工作空间覆盖机制，以下描述正确的有：**
   A. 后source的工作空间优先级更高  
   B. `echo $ROS_PACKAGE_PATH`显示路径顺序决定覆盖优先级  
   C. 系统级功能包优先级总是高于用户空间  
   D. 覆盖机制仅影响功能包搜索不影响编译结果

2. **实现自定义话题消息时需要进行的操作包括：**
   A. 在msg目录下创建.msg文件  
   B. 修改CMakeLists.txt添加message_generation依赖  
   C. 必须在package.xml中添加std_msgs依赖  
   D. 需要执行`catkin_make install`生成头文件

3. **分布式多机通信的正确配置方法：**
   A. 所有机器必须设置相同的ROS_MASTER_URI  
   B. 从机需要设置ROS_IP环境变量  
   C. 主机需要开放11311端口  
   D. 必须修改/etc/hosts文件配置域名解析

4. **关于launch文件的参数设置，错误的有：**
   A. `<param name="rate" value="10"/>`设置全局参数  
   B. `<arg name="model" default="robot"/>`定义启动参数  
   C. `<param name="frame_id" command="$(find pkg)/script.py"/>`执行命令获取参数  
   D. `<rosparam command="load" file="$(find pkg)/config.yaml"/>`加载YAML参数

5. **TF坐标变换实现的关键步骤包括：**
   A. 使用tf::TransformBroadcaster发布变换  
   B. 必须采用静态坐标变换（static_transform_publisher）  
   C. 监听器需要继承tf::TransformListener类  
   D. 坐标变换必须通过服务调用实现

6. **关于命名空间重映射，正确的说法是：**
   A. `rosrun package node __ns:=new_namespace`修改节点命名空间  
   B. `<node name="talker" remap="chatter:=new_topic"/>`重映射话题  
   C. 私有命名空间参数访问格式为`~/private_param`  
   D. 全局命名空间参数优先级高于私有命名空间

7. **服务通信与服务话题通信的主要区别体现在：**
   A. 服务采用请求-响应模式  
   B. 服务消息必须包含response字段  
   C. 服务通过XML-RPC协议通信  
   D. 服务适用于低频的RPC式调用

8. **正确编译包含自定义消息的功能包的步骤：**
   A. 先编译依赖包再编译当前包  
   B. 需要先执行`catkin_make --only-pkg-with-deps`  
   C. 必须修改package.xml添加message_runtime依赖  
   D. 每次修改msg文件后必须clean重新编译

9. **RoboWare Studio的主要优势包括：**
   A. 提供可视化launch文件配置界面  
   B. 内置ROS节点依赖关系分析工具  
   C. 支持Python代码自动补全  
   D. 集成rqt图形化调试工具链

10. **在多机通信测试中出现`[ERROR] Unable to communicate with master!`的可能原因：**
    A. 主机未启动roscore  
    B. 防火墙阻塞11311端口  
    C. ROS_MASTER_URI使用了localhost  
    D. 未设置ROS_HOSTNAME环境变量

---

### 答案与解析

1. **AB**  
   - C错误：用户空间优先级高于系统包  
   - D错误：覆盖机制影响功能包搜索路径，直接影响编译依赖

2. **AB**  
   - C错误：std_msgs是默认依赖无需添加  
   - D错误：生成头文件通过`catkin_make`而非install

3. **AC**  
   - B错误：ROS_IP已弃用，应设置ROS_HOSTNAME  
   - D错误：可通过IP直连无需域名解析

4. **CD**  
   - C错误：command属性只能用于节点参数  
   - D正确：rosparam可加载YAML文件

5. **AC**  
   - B错误：动态变换更常见  
   - D错误：TF通过话题通信

6. **ABC**  
   - D错误：私有命名空间优先级最高

7. **AD**  
   - B错误：服务定义包含request和response  
   - C错误：服务使用TCP协议

8. **AC**  
   - B错误：--only-pkg-with-deps非必须  
   - D错误：增量编译即可

9. **ABC**  
   - D错误：rqt是独立工具

10. **ABD**  
    - C正确：localhost无法被其他机器访问  
    - D正确：需设置可被解析的主机名