//
// Created by ka on 2020/8/22.
//

#ifndef KAZLIBUTIL_KAZLIBUTIL_H
#define KAZLIBUTIL_KAZLIBUTIL_H

#include <string>

//压缩类，对zlib的c++封装，仅仅提供了简单的压缩和解压缩的借口，操作简单
//直接使用C++中的string容器进行转换，更加方便安全，但是效率可能有所降低


class KaZlibUtil {
public:
    KaZlibUtil() = delete;
    KaZlibUtil(const KaZlibUtil& obj) = delete;
    KaZlibUtil& operator=(const KaZlibUtil& obj) = delete;
    ~KaZlibUtil() = delete;
public:
    static bool compressBuf(const char* pSrcBuf,size_t nSrcBufLength,char* pDestBuf,size_t &nDestBufLength);
    static bool compressBuf(const std::string& strSrcBuf,std::string& strDestBuf);
    static bool uncompressBuf(const std::string& strSrcBuf,std::string& strDestBuf,size_t nDestBufLength);
    //for gzip
};


#endif //KAZLIBUTIL_KAZLIBUTIL_H