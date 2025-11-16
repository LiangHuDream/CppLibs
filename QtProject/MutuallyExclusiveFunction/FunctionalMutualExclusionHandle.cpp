#include <QTimer>
#include "FunctionalMutualExclusionHandle.h"

FunctionalMutualExclusionHandle::FunctionalMutualExclusionHandle(QObject *parent) noexcept
    : QObject(parent)
{
}

FunctionalMutualExclusionHandle::~FunctionalMutualExclusionHandle() {}

void FunctionalMutualExclusionHandle::FunctionOpen(int functionId, bool open)
{
    std::unique_lock<std::mutex> lock(m_handleMutex);
    auto supportFunction = m_registerFunctionMap.find(functionId);
    if (supportFunction != m_registerFunctionMap.end()) {
        if (open) {
            //先关闭互斥功能
            auto pair = m_mutualExclusionMap.equal_range(functionId);
            for (auto iter = pair.first; iter != pair.second; iter++) {
                FunctionInfo function = iter->second;
                if (!function.checkFunction || !function.closeFunction) {
                    //考虑先注册互斥关系，后注册功能的情况：互斥列表中没有对应的检查操作和关闭操作，去功能容器中查找
                    auto tmpFunction = m_registerFunctionMap.find(function.functionID);
                    if (tmpFunction != m_registerFunctionMap.end()) {
                        function.closeFunction = tmpFunction->second.closeFunction;
                        function.checkFunction = tmpFunction->second.checkFunction;
                    }
                }

                //最后再检查一次
                if (function.checkFunction && function.closeFunction) {
                    QTimer::singleShot(1,
                                       function.receiver,
                                       std::bind(
                                           [](FunctionInfo function) {
                                               if (function.checkFunction()) {
                                                   function.closeFunction();
                                               }
                                           },
                                           function));
                }
            }

            if (supportFunction->second.openFunction) {
                QTimer::singleShot(
                    1,
                    supportFunction->second.receiver,
                    std::bind([](FunctionInfo function) { function.openFunction(); }, supportFunction->second));
            }
        } else if (supportFunction->second.closeFunction) {
            QTimer::singleShot(
                1,
                supportFunction->second.receiver,
                std::bind([](FunctionInfo function) { function.closeFunction(); }, supportFunction->second));
        }
    }
}

void FunctionalMutualExclusionHandle::RegisterFunction(FunctionInfo function)
{
    std::unique_lock<std::mutex> lock(m_handleMutex);
    m_registerFunctionMap.insert({function.functionID, function});
}

void FunctionalMutualExclusionHandle::UnRegisterFunction(FunctionInfo function)
{
    std::unique_lock<std::mutex> lock(m_handleMutex);
    m_registerFunctionMap.erase(function.functionID);
}

void FunctionalMutualExclusionHandle::RegisterMutuallyExclusiveRelationship(
    int functionID,
    const std::list<int> &mutualExclusiveFunction)
{
    FunctionInfo functionInfo;
    std::unique_lock<std::mutex> lock(m_handleMutex);
    for (auto mutualFunctionId : mutualExclusiveFunction) {
        //查找对应的 注册功能容器
        auto tempInfo = m_registerFunctionMap.find(mutualFunctionId);
        if (tempInfo != m_registerFunctionMap.end()) {
            functionInfo = tempInfo->second;
        } else {
            //此时没有找到，先存放空功能项，最终触发互斥关系时再检查
            functionInfo.functionID = mutualFunctionId;
            functionInfo.checkFunction = nullptr;
            functionInfo.openFunction = nullptr;
            functionInfo.closeFunction = nullptr;
        }

        m_mutualExclusionMap.insert({functionID, functionInfo});
    }
}

void FunctionalMutualExclusionHandle::UnregisterMutuallyExclusiveRelationship(int functionID)
{
    std::unique_lock<std::mutex> lock(m_handleMutex);
    m_mutualExclusionMap.erase(functionID);
}
