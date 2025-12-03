#include <memory>
#include <string>

class Context {
public:
    std::string name;
    int day;
};

// 稳定点 抽象  变化点 扩展 （多态）
//  从单个处理节点出发，我能处理，我处理，我不能处理交给下一个人处理
//  链表关系如何抽象

class HandleByMainProgram;
class HandleByProjMgr;
class HandleByBoss;
class HandleByBeauty;

class IHandler {
public:
    IHandler() : next(nullptr) {}
    virtual ~IHandler();
    void SetNextHandler(IHandler *next) { // 链表关系
        next = next;
    }
    bool Handle(const Context &ctx) {
        if (CanHandle(ctx)) {
            return HandleRequest(ctx);
        } else if (GetNextHandler()) {
            return GetNextHandler()->Handle(ctx);
        } else {
            // err
        }
        return false;
    }
protected:
    virtual bool HandleRequest(const Context &ctx) = 0;
    virtual bool CanHandle(const Context &ctx) = 0;
    IHandler * GetNextHandler() {
        return next;
    }
private:
    IHandler *next; // 组合基类指针
};


class Request {
public:
    Request() {
        IHandler * h0 = new HandleByBeauty();
        IHandler * h1 = new HandleByMainProgram();
        IHandler * h2 = new HandleByProjMgr();
        IHandler * h3 = new HandleByBoss();
        h0->SetNextHandler(h1);
        h1->SetNextHandler(h2);
        h2->SetNextHandler(h3);
        handler = h0;
    }
    bool Handle(const Context &ctx) {
        return handler->Handle(ctx);
    }
private:
    IHandler *handler;
};

class HandleByMainProgram : public IHandler {
protected:
    virtual bool HandleRequest(const Context &ctx){
        //
        return true;
    }
    virtual bool CanHandle(const Context &ctx) {
        //
        if (ctx.day <= 1)
            return true;
        return false;
    }
};

class HandleByProjMgr : public IHandler {
protected:
    virtual bool HandleRequest(const Context &ctx){
        //
        return true;
    }
    virtual bool CanHandle(const Context &ctx) {
        //
        if (ctx.day <= 20)
            return true;
        return false;
    }
};
class HandleByBoss : public IHandler {
protected:
    virtual bool HandleRequest(const Context &ctx){
        //
        return true;
    }
    virtual bool CanHandle(const Context &ctx) {
        //
        if (ctx.day < 30)
            return true;
        return false;
    }
};

class HandleByBeauty : public IHandler {
protected:
    virtual bool HandleRequest(const Context &ctx){
        //
        return true;
    }
    virtual bool CanHandle(const Context &ctx) {
        //
        if (ctx.day <= 3)
            return true;
        return false;
    }
};

int main() {
// 抽象工厂
// nginx http 处理 
    // 设置下一指针 
    Context ctx;
    auto req = std::make_unique<Request>();
    req->Handle(ctx);
    
    return 0;
}