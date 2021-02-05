//
// Created by ka on 2021/1/22.
//

#include "KaZlibUtil.h"
#include <cstring>          //for strlen....function
#include <zlib.h>

#define MAX_COMPRESS_BUF_SIZE 10*1024*1024


bool KaZlibUtil::compressBuf(const char *pSrcBuf, size_t nSrcBufLength, char *pDestBuf, size_t &nDestBufLength) {
    if(pSrcBuf == NULL || nSrcBufLength > MAX_COMPRESS_BUF_SIZE || nSrcBufLength== 0 || pDestBuf == NULL)
    {
        return false;
    }
    nDestBufLength = compressBound(nSrcBufLength);
    int ret = compress((Bytef*)(pDestBuf),(uLongf*)&nDestBufLength,(const Bytef*)pSrcBuf,nSrcBufLength);
    if(ret != Z_OK)
    {
        return  false;
    }
    return true;
}


bool KaZlibUtil::compressBuf(const std::string &strSrcBuf, std::string &strDestBuf) {
    if(strSrcBuf.empty()){
        return false;
    }
    int nSrcLength = strSrcBuf.length();
    if(nSrcLength > MAX_COMPRESS_BUF_SIZE){
        return false;
    }
    int nDestBuflength = compressBound(nSrcLength);
    if(nDestBuflength <= 0)
    {
        return false;
    }
    char* pDestBuf = new char[nDestBuflength];          //需要创建临时的空间进行缓存
    memset(pDestBuf,0,nDestBuflength*sizeof (char));

    int ret = compress((Bytef*)pDestBuf,(uLongf*)(&nDestBuflength),(const Bytef*)strSrcBuf.c_str(),nSrcLength);
    if(ret != Z_OK){
        delete[] pDestBuf;
        return false;
    }
    strDestBuf.append(pDestBuf,nDestBuflength);
    delete[] pDestBuf;
    return true;
}


bool KaZlibUtil::uncompressBuf(const std::string &strSrcBuf, std::string &strDestBuf, size_t nDestBufLength) {
    char *pDestBuf = new char[nDestBufLength];
    memset(pDestBuf, 0, nDestBufLength * sizeof(char));
    int nPrevDestBufLength = nDestBufLength;
    int ret = uncompress((Bytef *) pDestBuf, (uLongf *) &nDestBufLength, (const Bytef *) strSrcBuf.c_str(),
                         strSrcBuf.length());
    if (ret != Z_OK) {
        delete[] pDestBuf;
        return false;
    }
    strDestBuf.append(pDestBuf, nDestBufLength);
    delete[] pDestBuf;
    return true;
}