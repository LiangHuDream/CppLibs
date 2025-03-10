/*
* windows系统下日志实现， 单个日志文件的最大阈值为200M， 超过200M会在末尾
* 添加_bak, CreoPlugin.log_bak作为短暂备份
*/
#ifndef LOGGER_H
#define LOGGER_H

#include<string>
#include<fstream>
#include <ctime>
#include <thread>
#include <sstream>

namespace LogConstants {
    const std::string LOG_THEME = "The Logger For CreoPlugin!"; // 日志的主标题
    const std::string LOG_PATH = ""; // 设置日志的路径
    const DWORD LOG_MAX_SIZE = 200 * (1 << 20); // 单个日志文件的最大阈值
    const std::string LOG_SUFFIX = "_bak"; // 日志文件的后缀
    const auto MAX_TIME_STR_LEN = 128U; // 最大时间字符串长度
    const auto MAX_LOG_STR_LEN =512U; // 最大日志字符串长度
}

class TextDecorator {
public:
    static std::string FileHeader(const std::string& logTitle) {
        return "=========\n" + logTitle + "\n" + "=======\n\n";
    }

    static std::string SessionOpen() {
        return "\n";
    }

    static std::string SessionClose() {
        return "\n";
    }

    static std::string Decorate(const std::string & mgsStr) {
        return mgsStr + "\n";
    }
};

template <class Decorator>
class Logger {
    public:
        static Logger & GetInstance(const std::string & fileName = LogConstants::LOG_PATH, const std::string & logTitle = LogConstants::LOG_THEME, bool timeStamp = true) {
            static Logger instance(fileName, logTitle, timeStamp);
            return instance;
        }

        explicit Logger(const std::string & fileName = LogConstants::LOG_PATH, const std::string & logTitle = LogConstants::LOG_THEME, bool timeStamp = true);

        void Log(const std::string &entryStr);
        bool RenameLogFile(const std::string &logFilePath, const std::string &suffix, DWORD maxSize);
        static std::string TimeStamp();

    protected:
        std::fstream m_logFile{};
        bool m_timeStamp {};
    private:
    ~Logger();    
};

template <class Decorator> Logger<Decorator>:: Logger(const std::string & fileName, const std::string & logTitle, bool timeStamp) {
    std::fstream filePath(fileName.c_str(), std::ios::in);
    if (filePath.is_open()) {
        filePath.close();
        m_logFile.open(fileName.c_str(), std::ios::out | std::ios::app);
    } else {
        m_logFile.open(fileName.c_str(), std::ios::out);
        m_logFile << Decorator::FileHeader(logTitle);
    }

    m_timeStamp = true;
    m_logFile << Decorator::SessionOpen();
    Log("Session opened.");
    m_timeStamp = timeStamp;
}

template <class Decorator> Logger<Decorator>::~Logger() {
    m_timeStamp = true;
    Log("Session closed.");
    m_logFile << Decorator::SessionClose();
    m_logFile.close();

    // 重命名失败不做处理
    (void)RenameLogFile(m_logFile.name(), LogConstants::LOG_SUFFIX, LogConstants::LOG_MAX_SIZE);
}

template <class Decorator> void Logger<Decorator>::Log(const std::string &entryStr) {
    std::string message{};
    if (m_timeStamp) {
        message += "[" + TimeStamp() + "] ";
    }

    message += entryStr;
    m_logFile << Decorator::Decorate(message);
    m_logFile.flush();
}

template <class Decorator> bool Logger<Decorator>::RenameLogFile(const std::string &logFilePath, const std::string &suffix, DWORD maxSize) {
    HANDLE hFile = CreateFile(logFilePath.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE) {
        return false;
    }

    DWORD logFileSize = GetFileSize(hFile, NULL);
    CloseHandle(hFile);

    if (logFileSize > maxSize) {
        std::string backupLogFilePath = logFilePath + suffix;
        if (FileExists(backupLogFilePath)) {
            (void)DeleteFile(StrToWstr(backupLogFilePath).data());
        }

        if (!MoveFile(StrToWstr(logFilePath).data(), StrToWstr(backupLogFilePath).data())) {
            return false;
        }

    }

    return true;
}

template<class Decorator> std::string Logger<Decorator>::TimeStamp() {
    char timeStr[MAX_TIME_STR_LEN]{};
    SYSTEMTIME st;
    GetLocalTime(&st);
    auto err = sprintf_s(timeStr, MAX_TIME_STR_LEN, "%04d-%02d-%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

    if (err < 0) {
        // 这个分支不会走， 用于屏蔽告警
        return "";
    }
    return timeStr;
}

using TextLog = Logger<TextDecorator>;

inline bool FileExists(const std::string & filePath) {
    std::fstream file(filePath.c_str(), std::ios::in);
    if (file.is_open()) {
        file.close();
        return true;
    }
    return false;
}

inline std::wstring StrToWstr(const std::string & str) {
    int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), (int)str.length(), NULL, 0);
    std::wstring wstrTo(len, 0);
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), (int)str.length(), &wstrTo[0], len);
    return wstrTo;
}

inline std::string WstrToStr(const std::wstring & wstr) {
    int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), (int)wstr.length(), NULL, 0, NULL, NULL);
    std::string strTo(len, 0);
    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), (int)wstr.length(), &strTo[0], len, NULL, NULL);
    return strTo;
}

// 获取当前线程的id
inline std::string GetThreadId() {
    std::thread::id threadId = std::this_thread::get_id();
    std::ostringstream oss;
    oss << threadId;
    return oss.str();
}

// 打印普通日志, logReturnErrCode < 0的分支不会走， 下面这么写用于屏蔽告警
#define LOG(fmt, ...)
do {
    char buf[MAX_LOG_STR_LEN]{};
    auto logReturnErrCode = sprintf_s(buf, sizeof(buf), "[%s][%s:%s:%ld]"fmt"\n",GetThreadId().data() __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__);
    if (logReturnErrCode >= 0) {     \
        TextLog::GetInstance().Log(std::string(buf));   \
    }   \
} while(false)  \

// 打印错误日志, logReturnErrCode < 0的分支不会走， 下面这么写用于屏蔽告警
#define ERR_LOG(fmt, ...)
do {
    char buf[MAX_LOG_STR_LEN]{};
    auto logReturnErrCode = sprintf_s(buf, sizeof(buf), "[ERROR][%s][%s:%s:%ld]"fmt"\n",GetThreadId().data() __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__);
    if (logReturnErrCode >= 0) {     \
        TextLog::GetInstance().Log(std::string(buf));   \
    }   \
} while(false)  \

#endif