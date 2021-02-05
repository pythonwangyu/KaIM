//
// Created by ka on 2021/1/13.
//

#include "KaChannel.h"
#include "KaPoller.h"
#include "EventLoop.h"
#include "../Util/Platfrom.h"

using namespace KaNet;

//读事件，写事件，非空事件。
const int KaChannel::NoneEvent = 0;
const int KaChannel::ReadEvent = EPOLLIN | EPOLLPRI;
const int KaChannel::WriteEvent = EPOLLOUT;

KaChannel::KaChannel(EventLoop *loop, int fd)
        : m_loop(loop),
          m_fd(fd),
          m_events(0),
          m_revents(0),
          m_index(KAdded),          //index初始化为KAdded
          m_logHup(true),
          m_tied(false) {}

KaChannel::~KaChannel() = default;


bool KaChannel::enableReading() {
    m_events |= ReadEvent;
    return update();
}

bool KaChannel::disableReading() {
    m_events &= ~ReadEvent;
    return update();
}

bool KaChannel::enableWriting() {
    m_events |= WriteEvent;
    return update();
}

bool KaChannel::disableWriting() {
    m_events &= ~WriteEvent;
    return update();
}

bool KaChannel::disableAll() {
    m_events = NoneEvent;
    return update();
}


//TO DO
bool KaChannel::update() {
    return m_loop->updateChannel(this);
}



//TO DO，需要完成loop后再完成这个
void KaChannel::remove() {
    if(!isNonEvent())
        return;

}

void KaChannel::handleEvent(Timestamp receiveTime) {
    std::shared_ptr<void> guard;
    if(m_tied)
    {
        guard = m_tie.lock();
        if(guard)
        {
            handleEventWithGuard(receiveTime);
        }
    }
    else
    {
        handleEventWithGuard(receiveTime);
    }
}

//这个关联的是什么函数？
void KaChannel::tie(const std::shared_ptr<void> &obj) {
    m_tie = obj;
    m_tied = true;
}

//Key function,根据不同的m_revent决定不同的处理时间并且执行

void KaChannel::handleEventWithGuard(Timestamp receiveTime) {

}

