//
// Created by ka on 2021/1/15.
//

#ifndef KANET_KASINGLETON_H
#define KANET_KASINGLETON_H

/*@ Kanon
 * 简单的单例模式,使得创建的对象全局唯一。
 * */



template<class T>
class KaSingleton{
public:
    KaSingleton() = delete;
    ~KaSingleton() = delete;
    KaSingleton(const KaSingleton&) = delete;
    KaSingleton& operator=(const KaSingleton&) = delete;

    static T& Instance()         //简单的实例化的方式？
    {
        if(nullptr == m_value)
        {
            m_value = new T();
        }
        return *m_value;
    }

private:
    //私有函数

    static void init()
    {
        m_value = new T();
    }

    static void destory()
    {
        delete m_value;
    }


private:
    static T*           m_value;
};

template<typename T>
T* KaSingleton<T>::m_value = nullptr;

#endif //KANET_KASINGLETON_H
