#ifndef FUNCTIONALMUTUALEXCLUSIONHANDLE_H
#define FUNCTIONALMUTUALEXCLUSIONHANDLE_H
#include <QObject>
#include <mutex>

#include "Singleton.h"

struct FunctionInfo
{
    QObject *receiver;                        //接收者
    int functionID;                           //功能ID
    std::function<bool(void)> checkFunction;  //检查功能是否开启
    std::function<bool(void)> closeFunction;  //关闭该功能
    std::function<bool(void)> openFunction;   //开启该功能
};
#define FunctionalMutualExclusionHandleIns() Singleton<FunctionalMutualExclusionHandle>::Instance()

class FunctionalMutualExclusionHandle : public QObject
{
    Q_OBJECT
public:
    explicit FunctionalMutualExclusionHandle(QObject *parent = nullptr) noexcept;
    virtual ~FunctionalMutualExclusionHandle();

    void FunctionOpen(int functionId, bool open = true);

    void RegisterFunction(FunctionInfo function);
    void UnRegisterFunction(FunctionInfo function);

    void RegisterMutuallyExclusiveRelationship(int functionID, const std::list<int> &mutualExclusiveFunction);
    void UnregisterMutuallyExclusiveRelationship(int functionID);

private:
    std::mutex m_handleMutex;
    std::multimap<int, FunctionInfo> m_mutualExclusionMap;  //功能互斥容器
    std::map<int, FunctionInfo> m_registerFunctionMap;      //注册的功能
};

#endif // FUNCTIONALMUTUALEXCLUSIONHANDLE_H
