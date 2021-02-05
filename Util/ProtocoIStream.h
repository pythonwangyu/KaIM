//
// Created by ka on 2021/1/19.
//

#ifndef KANET_PROTOCOISTREAM_H
#define KANET_PROTOCOISTREAM_H


#include <string>
#include <cstdint>


namespace KaNet{

    enum {
        TEXT_PACK_LEN = 4,
        TEXT_PACKAGE_MAXLEN = 0xffff,
        BINARY_PACKLEN_LEN = 2,
        BINARY_PACKAGE_MAXLEN = 0xffff,

        TEXT_PACKLEN_LEN_2 = 6,
        TEXT_PACKAGE_MAXLEN_2 = 0xffffff,

        BINARY_PACKLEN_LEN_2 = 4,
        BINARY_PACKAGE_MAXLEN_2 = 0x10000000,

        CHECKSUM_LEN = 2,

    };
    unsigned short checksum(const unsigned short* buffer,int size);

    void write7BitEncode(uint32_t value,std::string& buf);

    void write7BitEncode(uint64_t value,std::string& buf);

    void read7BitEncode(const char* buf,uint32_t len,uint32_t& value);

    void read7BitEncode(const char* buf,uint64_t len,uint64_t& value);

    class BinaryStreamReader final {
    public:
        BinaryStreamReader(const char* ptr,size_t len);
        ~BinaryStreamReader() = default;

        //删除拷贝构造函数和赋值构造函数
        BinaryStreamReader(const BinaryStreamReader&)  = delete;
        BinaryStreamReader& operator=(const BinaryStreamReader& ) = delete;

        const char* GetData() const;
        size_t GetSize() const;
        bool isEmpty() const;

        bool ReadString(std::string* str,size_t maxlen,size_t& outlen);
        bool ReadCString(char* str,size_t strlen,size_t& len);
        bool ReadCCString(char** str,size_t maxlen,size_t& len);

        bool ReadInt32(int32_t& i);
        bool ReadInt64(int64_t& i);
        bool ReadShort(short& i);
        bool ReadChar(char& c);
        size_t ReadAll(char* szBuffer,size_t len) const;
        bool isEnd() const;

        const char* GetCurrent() const {return m_cur;}

    public:
        bool ReadLength(size_t& len);
        bool ReadLengthWithOutOffSet(size_t &headlen,size_t& outlen);



    private:
        const char* const   m_ptr;
        const size_t        m_len;
        const char*         m_cur;
    };




    class BinaryStreamWriter final {
    public:
        BinaryStreamWriter(std::string* data);
        ~BinaryStreamWriter() = default;

        BinaryStreamWriter(const BinaryStreamWriter&) = delete;
        BinaryStreamWriter& operator=(const BinaryStreamWriter& rhs) = delete;

    public:

        const char* GetData() const;
        size_t GetSize() const;

        bool WriteCString(const char* str,size_t len);

        bool WriteString(const std::string& str);

        bool WriteDouble(double value,bool isNULL= false);
        bool WriteInt64(int64_t value,bool isNULL = false);
        bool WriteInt32(int32_t value,bool isNULL = false);
        bool WriteShort(short value,bool isNULL = false);
        bool WriteChar(char c,bool isNULL = false);
        size_t  GetCurrentPos() const {
            return m_data->length();
        }

        void Flush();
        void Clear();
        

    private:
        std::string* m_data;

    };
}


#endif //KANET_PROTOCOISTREAM_H
