#ifndef SINGLETON_H
#define SINGLETON_H
template <typename T>
class Singleton
{
public:
    static T &Instance()
    {
        if (nullptr == value_)
        {
            value_ = new T();
        }
        return *value_;
    }

private:
    Singleton();
    ~Singleton() = default;

    Singleton(const Singleton &) = delete;
    Singleton &operator=(const Singleton &) = delete;

    static void init()
    {
        value_ = new T();
    }

    static void destroy()
    {
        delete value_;
    }

private:
    static T *value_;
};

template <typename T>
T *Singleton<T>::value_ = nullptr;
#endif // SINGLETON_H
