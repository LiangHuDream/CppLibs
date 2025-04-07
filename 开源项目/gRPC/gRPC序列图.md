下面提供一个详细的 gRPC 序列图示例，描述了客户端调用远程服务的完整交互流程。该图展示了从客户端发起调用，到 gRPC 通道、负载均衡、传输层（包含 TLS 握手）以及服务端处理（包括拦截器和服务处理器）的完整过程。你可以将下面的代码粘贴到支持 Mermaid 的编辑器中进行预览：

```mermaid
sequenceDiagram
    participant Developer as 开发者
    participant ClientStub as 客户端存根
    participant Channel as gRPC 通道
    participant LB as 负载均衡器
    participant TLS as TLS 握手模块
    participant Transport as 传输层 (HTTP/2)
    participant ServerDispatcher as 服务端调度器
    participant Interceptor as 拦截器
    participant Service as 服务处理器

    Developer->>ClientStub: 调用远程方法
    ClientStub->>Channel: 创建调用请求
    Channel->>LB: 查询目标服务器
    LB-->>Channel: 返回服务器地址
    Channel->>TLS: 发起 TLS 握手
    TLS-->>Channel: 握手成功，建立安全通道
    Channel->>Transport: 封装请求消息
    Transport->>ServerDispatcher: 发送请求 (HTTP/2)
    ServerDispatcher->>Interceptor: 调用拦截器（如日志、监控）
    Interceptor-->>ServerDispatcher: 完成拦截处理
    ServerDispatcher->>Service: 调用服务处理器
    Service-->>ServerDispatcher: 返回处理结果
    ServerDispatcher->>Transport: 发送响应 (HTTP/2)
    Transport-->>Channel: 返回响应数据
    Channel-->>ClientStub: 解封装响应消息
    ClientStub-->>Developer: 返回远程调用结果
```

### 说明

- **开发者**：发起远程调用请求。
- **客户端存根 (ClientStub)**：客户端 API 封装，负责将调用请求传递给 gRPC 通道。
- **gRPC 通道 (Channel)**：管理客户端与服务端之间的连接，负责调用创建、连接管理以及消息的封装与解封装。
- **负载均衡器 (LB)**：在多实例部署环境下选择合适的服务器地址。
- **TLS 握手模块 (TLS)**：确保通信建立前完成 TLS 握手，保证数据传输的安全性。
- **传输层 (HTTP/2)**：负责底层消息传输，将请求和响应在客户端与服务端之间传递。
- **服务端调度器 (ServerDispatcher)**：接收请求，并调用后续处理逻辑。
- **拦截器 (Interceptor)**：可选模块，用于实现日志、监控、认证等中间件功能，对请求进行预处理或后处理。
- **服务处理器 (Service)**：实际执行业务逻辑，处理请求并生成响应。

这种序列图能够较详细地展示 gRPC 从客户端发起调用，到服务端处理返回结果的整个流程，便于理解内部工作机制。