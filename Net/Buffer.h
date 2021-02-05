//
// Created by ka on 2021/1/14.
//

#ifndef KANET_BUFFER_H
#define KANET_BUFFER_H

#include "../Util/Platfrom.h"
#include "EventLoop.h"

#include <cstring>
#include <string>

#include <vector>
#include <algorithm>
#include <memory>

namespace KaNet{


    class Buffer{
    public:
        static const size_t Prepend = 8;
        static const size_t InitSize = 1024;

        explicit Buffer(size_t initsize = InitSize)
        :m_buffer(Prepend + initsize),
        m_readIndex(Prepend),
        m_writeIndex(Prepend)
        {}

        void swap(Buffer& rhs)
        {
            m_buffer.swap(rhs.m_buffer);
            std::swap(m_readIndex,rhs.m_readIndex);
            std::swap(m_writeIndex,rhs.m_writeIndex);
        }

        //已经写入数据，并且可以读取的字节大小
        size_t readableBytes() const{
            return m_writeIndex - m_readIndex;
        }

        size_t writableBytes() const{
            return m_buffer.size() - m_writeIndex;
        }

        //预留头部字节的大小
        size_t prependableBytes() const{
            return m_readIndex;
        }

        const char* peek() const{
            return begin() + m_readIndex;
        }

        char* beginWrite(){
            return begin() + m_writeIndex;
        }

        const char* beginWrite() const{
            return begin() +m_writeIndex;
        }

        bool hasWritten(size_t len)
        {
            if(len < writableBytes())
                return false;
            m_writeIndex += len;
            return true;
        }



        //查找特定的字符串
        const char* findString(const char* targetStr)const{
            const char* found = std::search(peek(),begin(),
                                            targetStr,targetStr+strlen(targetStr));
            return found;
        }

        const char* findCRLF() const{
            const char* crlf = std::search(peek(),begin(),CRLF,CRLF+2);
            return crlf == beginWrite() ? nullptr :crlf;
        }

        const char* findCRLF(const char* start) const{
            if(peek() > start)
                return nullptr;
            if(start > beginWrite())
                return nullptr;
            const char* clf = std::search(start,beginWrite(),CRLF,CRLF+2);
            return clf == beginWrite()? nullptr:clf;
        }

        const char* findEOF() const{
            const void* eol = memchr(peek(),'\n',readableBytes());
            return static_cast<const char*>(eol);
        }

        const char* findEOF(const char* start) const{
            if(peek() > start)
            {
                return nullptr;
            }
            if(start > beginWrite())
            {
                return nullptr;
            }
            const void* eol = memchr(start,'\n',beginWrite()-start);
            return static_cast<const char*>(eol);
        }

        void retrieveAll(){
            m_readIndex = Prepend;
            m_writeIndex = Prepend;
        }

        bool retrieve(size_t len)
        {
            if(len < readableBytes())
                return false;
            if(len < readableBytes())
                m_readIndex += len;
            else
                retrieveAll();
            return true;
        }

        bool retrieveUntil(const char* end)
        {
            if(peek() < end)
                return false;
            if(end > beginWrite())
                return false;
            retrieve(end - peek());
            return true;
        }


        std::string retrieveAsString(size_t len)
        {
            if(len > readableBytes())
                return {};
            std::string result(peek(),len);
            retrieve(len);
            return result;
        }

        std::string retrieveAllAsString()
        {
            return retrieveAsString(readableBytes());
        }


        /*
         * std::copy(_First,_End,_Dest);
         * */
        void append(const std::string& str){
            append(str.c_str(),str.size());
        }

        void append(const char* data,size_t len)
        {
            ensureWritableBytes(len);
            std::copy(data,data+len,beginWrite());
            hasWritten(len);
        }

        void append(const void* data,size_t len)
        {
            append(static_cast<const char*>(data),len);
        }

        bool prepend(const void* data,size_t len)
        {
            if(len > prependableBytes())
            {
                return false;
            }
            m_readIndex -= len;
            const char* d= static_cast<const char*>(data);
            std::copy(d,d+len,begin() + m_readIndex);
            return true;
        }

        std::string toStringPiece() const{
            return std::string(peek(),static_cast<int>(readableBytes()));
        }

        void shrink(size_t reserve){
            Buffer other;
            other.ensureWritableBytes(readableBytes() + reserve);
            other.append(toStringPiece());
            swap(other);
        }



        /*确保可以写入len个字节的数据*/
        bool ensureWritableBytes(size_t len)
        {
            if(writableBytes() < len)
            {
                makeSpace(len);
            }
            return true;
        }

        size_t internalCapacity() const{
            return m_buffer.capacity();
        }

        int32_t readFd(int fd,int *Errno);

    private:
        char* begin(){
            return &*m_buffer.begin();
        }
        const char* begin()const{
            return &*m_buffer.begin();
        }

        //创建能够容纳len长度的空间，使用resize来扩容？
        void makeSpace(size_t len)
        {
            if(writableBytes() + prependableBytes() < len + Prepend)
            {
                m_buffer.resize(m_writeIndex + len);
            }
            else
            {
                if(Prepend >= m_readIndex)
                    return;
                size_t readble = readableBytes();
                std::copy(begin() + m_readIndex,
                          begin() + m_writeIndex,
                          begin()+Prepend);
                m_readIndex = Prepend;
                m_writeIndex = m_readIndex + readble;
            }
        }

    private:
        std::vector<char>  m_buffer;
        size_t             m_readIndex;
        size_t             m_writeIndex;
        static const char CRLF[];
    };

    void defaultMessageCallback(const std::shared_ptr<KaNet::TcpConnection> &conn, Buffer *buff, Timestamp receiveTime);
}// end of namespace KaNet;

#endif //KANET_BUFFER_H
