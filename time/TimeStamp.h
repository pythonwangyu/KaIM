//
// Created by ka on 2021/1/13.
//

#ifndef KANET_TIMESTAMP_H
#define KANET_TIMESTAMP_H

#include <stdint.h>
#include <algorithm>
#include <string>

using namespace std;

//1秒=1000000微秒
#define MicroSecondsPerSecond 1000000


//这个时间戳其实就是一个整数序列的概念，是不

class Timestamp{
public:
    Timestamp()
            :m_microSecondsSinceEpoch(0)
    {}

    explicit Timestamp(int64_t microSecondsPerSecond)
            :m_microSecondsSinceEpoch(microSecondsPerSecond)
    {}

    Timestamp& operator+=(Timestamp lhs)
    {
        this->m_microSecondsSinceEpoch += lhs.m_microSecondsSinceEpoch;
        return *this;
    }

    Timestamp& operator+=(int64_t lhs)
    {
        this->m_microSecondsSinceEpoch += lhs;
        return *this;
    }
    Timestamp& operator-=(Timestamp lhs)
    {
        this->m_microSecondsSinceEpoch -= lhs.m_microSecondsSinceEpoch;
        return *this;
    }

    Timestamp& operator-=(int64_t lhs)
    {
        this->m_microSecondsSinceEpoch -= lhs;
        return *this;
    }

    void swap(Timestamp& that)
    {
        std::swap(m_microSecondsSinceEpoch, that.m_microSecondsSinceEpoch);
    }


    string ToString();

    bool vaild() const {return m_microSecondsSinceEpoch > 0;}

    int64_t GetMicroSecondValue()  {return m_microSecondsSinceEpoch;}

    time_t GetSecondValue(){
        return static_cast<time_t>(m_microSecondsSinceEpoch / MicroSecondsPerSecond);
    }


    //static function for get now time

    static Timestamp now();         //获取当前的时间戳
    static Timestamp invalid();


private:
    int64_t         m_microSecondsSinceEpoch;
};




// inline some operator

inline bool operator<(Timestamp lhs,Timestamp rhs){
    return lhs.GetMicroSecondValue() < rhs.GetMicroSecondValue();
}

inline bool operator>(Timestamp lhs,Timestamp rhs){
    return lhs.GetMicroSecondValue() > rhs.GetMicroSecondValue();
}

inline bool operator<=(Timestamp lhs,Timestamp rhs){
    return lhs.GetMicroSecondValue() <= rhs.GetMicroSecondValue();
}

inline bool operator>=(Timestamp lhs,Timestamp rhs){
    return lhs.GetMicroSecondValue() >= rhs.GetMicroSecondValue();
}

inline bool operator==(Timestamp lhs,Timestamp rhs){
    return lhs.GetMicroSecondValue() == rhs.GetMicroSecondValue();
}

inline bool operator!=(Timestamp lhs,Timestamp rhs){
    return lhs.GetMicroSecondValue() != rhs.GetMicroSecondValue();
}


inline double timeDiff(Timestamp big,Timestamp small)
{
    int64_t diff_num = big.GetMicroSecondValue() - small.GetMicroSecondValue();
    return static_cast<double>(diff_num) / MicroSecondsPerSecond;
}

inline Timestamp timeAdd(Timestamp timestamp,int64_t value)
{
    return Timestamp(timestamp.GetMicroSecondValue() + value);
}


inline Timestamp timeAdd(Timestamp timestamp1,Timestamp timestamp2)
{
    return Timestamp(timestamp1.GetMicroSecondValue() + timestamp2.GetMicroSecondValue());
}


#endif //KANET_TIMESTAMP_H
