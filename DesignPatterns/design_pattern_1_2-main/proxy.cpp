
class ISubject {
public:
    virtual void Handle() = 0;
    virtual ~ISubject() {}
};

class RealSubject : public ISubject {
public:
    virtual void Handle() {
    }
};

class Proxy1 : public ISubject {
public:
    Proxy1(ISubject *subject) : _subject(subject) {}
    virtual void Handle() {
        // 在访问 RealSubject 之前做一些处理
        //if (不满足条件)
        //    return;
        _subject->Handle();
        count++;
        // 在访问 RealSubject 之后做一些处理
    }
private:
    ISubject* _subject;
    static int count;
};
int Proxy1::count = 0;

class Proxy2 : public ISubject {
public:
    virtual void Handle() {
        // 在访问 RealSubject 之前做一些处理
        
        // 发送到数据到远端  网络处理  同步非阻塞 ntyco c协程
        //IResult * val = rpc->call("RealSubject", "Handle");

        // 在访问 RealSubject 之后做一些处理
    }
private:
    /*void callback(IResult * val) {
        // 在访问 RealSubject 之后做一些处理
    }*/
};

class Proxy3 : public ISubject {
public:
    Proxy1(ISubject *subject) : _subject(subject),_loaded(false) {}
    virtual void Handle() {
        if (! _loaded) {
            _loaded = true;
            /*
            加载资源
            */
        }
        _subject->Handle();
    }
private:
    ISubject* _subject;
    bool _loaded;
};