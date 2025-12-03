#include <string>

class Context {
public:
    std::string name;
    int day;
};

class LeaveRequest {
public:
    bool HandleRequest(const Context &ctx) {
        if (ctx.day <= 1)
            return HandleByBeaty(ctx);
        if (ctx.day <= 3)
            return HandleByMainProgram(ctx);
        else if (ctx.day <= 10)
            return HandleByProjMgr(ctx);
        else
            return HandleByBoss(ctx);
    }

private:
    bool HandleByBeaty(const Context &ctx) {
        return true;
    }
    bool HandleByMainProgram(const Context &ctx) {
        return true;
    }
    bool HandleByProjMgr(const Context &ctx) {
        return true;
    }
    bool HandleByBoss(const Context &ctx) {
        return true;
    }
};