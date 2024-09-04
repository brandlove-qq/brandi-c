#ifndef QzskjDataStream_H
#define QzskjDataStream_H

#include <cstring>
#include <functional>
using namespace std;

#define MAX_STREAM_SIZE (1024*1024*2)   //最大内存2M
#define MAX_MEMORY_SIZE (1024*1024*4)   //设定最大内存4M

/*!
 * param1:位置
 * param2：源数据
 * param3:源数据长度
 * param4：输出数据
 * param5:输出数据长度
 */
typedef std::function<unsigned int(unsigned int,\
                                 const unsigned char *, unsigned int, \
                                 unsigned char **, unsigned int)> callback;

/*!
 * sunday算法简介：
 * 模式串与主串进行匹配时，当P[i] != T[j],我们关注的是T[lenP]是否出现在P中
 * 1、如果T[lenP]不在P中，则P整体往右移动lenP + 1长度
 * 2、如果T[lenP]在P中，则P整体往右移动lenP - max(T[lenP].index)
 *
 * \brief sunday sunday算法匹配字符串
 * \param T 主字符串
 * \param lenT 主字符串数据长度
 * \param P 模式字符串
 * \param lenP 模式字符串数据长度
 * \return 查找位置
 */
static unsigned int sunday(const unsigned char *T, unsigned int lenT, \
                    const unsigned char *P, unsigned int lenP)
{
    if(nullptr == T || nullptr == P || lenT < lenP)
    {
        return 0xffffffff;
    }

    //字符对应的移动长度
    unsigned int chOffset[256];
    for(auto &offset : chOffset)
    {
        //初始化为最大移动步数
        offset = lenP + 1;
    }

    //计算每个移动步数
    for(unsigned int i = 0; i < lenP; ++i)
    {
        unsigned int temp = static_cast<unsigned int>(P[i]);
        chOffset[temp] = lenP - i;
    }

    //匹配字符串
    unsigned int i = 0;
    while(i <= lenT - lenP)
    {
        unsigned int k = i; //T的索引，用于匹配
        unsigned int j = 0; //P的索引
        while(T[k] == P[j])
        {
            //继续比对
            ++k;
            ++j;
            //匹配完成
            if(j >= lenP)
            {
                return i;
            }
        }

        //T[i] != P[j],找到P[lenP-1]在T中对应位置的下一个字符
        auto nexti = i + lenP;
        auto ch = T[nexti];
        //移动
        i += chOffset[static_cast<unsigned int>(ch)];
    }

    return 0xffffffff;
}


class QzskjDataStream
{
public:
    QzskjDataStream() = default;

    QzskjDataStream(unsigned int len)
    {
        mallocStream(len);
    }

    QzskjDataStream(const unsigned char *pData, unsigned int len)
    {
        mallocStream(pData, len);
    }

    ~QzskjDataStream()
    {
        freeStream();
    }

  friend  class QCameraClient;

public:

    unsigned int mallocStream(unsigned int len)
    {
        //异常判断
        if(len > MAX_STREAM_SIZE || 0 == len) {
            return 0;
        }

        //删除现有数据
        if(m_pMemoryHead) {
            delete []m_pMemoryHead;
        }

        //
        m_dataLen           = 0;

        m_reservedSize      = len*2;

        m_pMemoryHead       = new unsigned char[m_reservedSize];
        //memset(m_pMemoryHead, 0, sizeof(unsigned char)*m_reservedSize);

        m_pDataHead         = nullptr;

        m_curDataHeadPos    = 0;

        return len;
    }

    /*!
     * \brief mallocStream 申请内存，删除已有数据
     * \param pData 数据
     * \param len 数据长度
     * \return 成功返回数据长度，失败返回0
     */
    unsigned int mallocStream(const unsigned char *pData, unsigned int len)
    {
        //异常判断
        if(nullptr == pData || len > MAX_STREAM_SIZE) {
            return 0;
        }

        //删除现有数据
        if(m_pMemoryHead) {
            delete []m_pMemoryHead;
        }

        //
        m_dataLen           = len;

        m_reservedSize      = len*2;

        m_pMemoryHead       = new unsigned char[m_reservedSize];
        //memset(m_pMemoryHead, 0, sizeof(unsigned char)*m_reservedSize);
        memcpy_s(m_pMemoryHead, len, pData, len);
        //memset(m_pMemoryHead + len, 0, sizeof (unsigned char)*len);

        m_pDataHead         = m_pMemoryHead;

        m_curDataHeadPos    = 0;

        return len;
    }

    /*
     * @brief 插入数据
     * @param pData 插入的数据
     * @param len 插入数据的长度
     * return 插入数据的长度
     */
    /************************************************************************************
     *插入数据
     *1.第一次插入：直接根据数据的长度开辟内存，内存大小是目前的给的len大小的两倍
     *2.第二次插入：
     * (1)第二次插入的数据的长度，目前内存的剩余长度可以放的下就直接插入
     * (2)如果第二次或者第n次插入的数据的长度，目前内存剩余长度放不下：判断剩下的没有处理的数据加上新来的数据长度
     * 目前开辟的内存是否足够放下,可以先把旧的有效数据迁移到最前面位置，清理有效数据后的数据(防止查找时冲突),新来的
     * 数据放到有效旧数据后。如果放不下，就考虑重新开辟内存，先把有效数据拿出来,根据新来的数据长度加上旧的有效数据开辟
     *新的内存，然后把旧有效数据放前边，新的放有效数据后。
     * 注意：如果开辟的内存大于最大的设定内存的情况
     * **********************************************************************************/

    unsigned int streamIn(const unsigned char* pData, unsigned int len)
    {
        if(nullptr == pData || 0 == len)
        {
            return 0;
        }

        if(nullptr == m_pMemoryHead)
        {//第一次进入数据
            return mallocStream(pData, len);
        }
       //第二次插入数据
       //计算没有被使用的长度
       unsigned int remainsize=m_reservedSize - m_dataLen;
       if(remainsize<len)
       {//预留中没有使用的放不下

           //判断newMemoryDataLen与m_reservedSize（内存空间大小）的关系
           //如果新的数据长度超出现有的内存大小，则判断有效的旧数据长度+插入新数据长度与内存大小的关系
           //计算有效数据长度(旧的有效数据)
           auto validDataLen = m_dataLen - m_curDataHeadPos;
           //计算新的有效数据长度     //旧的有效数据 + 新数据长度
           auto newValidDataLen = validDataLen + len;

           //判断newValidDataLen与m_reservedSize（内存空间大小）的关系
           if(newValidDataLen > m_reservedSize)
           {
               //新的有效数据长度大于当前内存大小，则需要重新申请内存空间

               //计算新的内存大小
               auto newMemorySize = newValidDataLen * 2;
               //范围判断
               if(newMemorySize > MAX_MEMORY_SIZE)
               {
                   //新内存大小超出最大内存限制，判断newValidDataLen与MAX_MEMORY_SIZE
                   if(newValidDataLen > MAX_MEMORY_SIZE)
                   {
                       //超出限制，插入失败
                       return 0;
                   }

                   //重新分配内存大小
                   newMemorySize = newValidDataLen;
               }

               //数据进行迁移              //旧的没处理的有效数据(先放到临时内存)
               unsigned char *mallocData = new unsigned char[validDataLen];
               memcpy_s(mallocData, validDataLen, m_pDataHead, validDataLen);

               delete []m_pMemoryHead;  //删除原来的内存
               m_pMemoryHead = new unsigned char[newMemorySize]; //开辟新的内存(大小是新数据加旧的有效数据的长度)
               //memset(m_pMemoryHead, 0, sizeof(unsigned char)*newMemorySize);
               memcpy_s(m_pMemoryHead, validDataLen, mallocData, validDataLen);//旧内存放好
               m_pDataHead         = m_pMemoryHead;
               m_reservedSize      = newMemorySize;
               m_dataLen           = validDataLen;
               //新数据
               memcpy_s(m_pMemoryHead + m_dataLen, len, pData, len);
               m_dataLen = len + m_dataLen;
               m_curDataHeadPos    = 0;
               delete []mallocData;
               mallocData = nullptr;
           }
           else
           {  //新的数据大小加上就的有效数据的大小的大小小于预留的数据大小（就是能够放下新来的数据和旧的有效数据）
               //内存迁移
               //旧的有效数据
               memcpy_s(m_pMemoryHead, validDataLen, m_pDataHead, validDataLen);
               //迁移有效数据，对剩下的清理（防止在查找时找到错误的位置）
               memset(m_pMemoryHead + validDataLen, 0,sizeof (unsigned char)*(m_reservedSize - validDataLen));
               m_pDataHead         = m_pMemoryHead;
               m_dataLen           = validDataLen;
               //新数据
               memcpy_s(m_pMemoryHead + m_dataLen, len, pData, len);
               m_dataLen = len + m_dataLen;
               m_curDataHeadPos    = 0;
           }
       }
       else
       { //剩下的位置足够放下
           //插入数据(小于:新数据在预留的内存放的下)
           memcpy_s(m_pMemoryHead + m_dataLen, len, pData, len);
           m_dataLen = len + m_dataLen;
       }
        return len;
}
#if 0
    unsigned int streamIn(const unsigned char* pData, unsigned int len)
    {
        if(nullptr == pData || 0 == len)
        {
            return 0;
        }
        if(nullptr == m_pMemoryHead)
        {
            //当前数据为空
            return mallocStream(pData, len);
        }
        //计算新的数据长度
        auto newMemoryDataLen = len + m_dataLen;
        //判断newMemoryDataLen与m_reservedSize（内存空间大小）的关系
        if(newMemoryDataLen > m_reservedSize)
        {
            //如果新的数据长度超出现有的内存大小，则判断有效的数据长度+插入数据长度与内存大小的关系

            //计算有效数据长度
            auto validDataLen = m_dataLen - m_curDataHeadPos;

            //计算新的有效数据长度     //旧的有效数据   //新数据长度
            auto newValidDataLen = validDataLen + len;

            //判断newValidDataLen与m_reservedSize（内存空间大小）的关系
            if(newValidDataLen > m_reservedSize)
            {
                //新的有效数据长度大于当前内存大小，则需要重新申请内存空间

                //计算新的内存大小
                auto newMemorySize = newValidDataLen * 2;
                //范围判断
                if(newMemorySize > MAX_MEMORY_SIZE)
                {
                    //新内存大小超出最大内存限制，判断newValidDataLen与MAX_MEMORY_SIZE
                    if(newValidDataLen > MAX_MEMORY_SIZE)
                    {
                        //超出限制，插入失败
                        return 0;
                    }

                    //重新分配内存大小
                    newMemorySize = newValidDataLen;
                }

                //有效数据进行迁移
                unsigned char *mallocData = new unsigned char[validDataLen];
                memcpy_s(mallocData, validDataLen, m_pDataHead, validDataLen);

                delete []m_pMemoryHead;  //删除原来的内存
                m_pMemoryHead = new unsigned char[newMemorySize]; //开辟新的内存(大小是新数据加旧的有效数据的长度)
                //memset(m_pMemoryHead, 0, sizeof(unsigned char)*newMemorySize);
                memcpy_s(m_pMemoryHead, validDataLen, mallocData, validDataLen);
                m_pDataHead         = m_pMemoryHead;
                m_reservedSize      = newMemorySize;
                m_dataLen           = validDataLen;
                m_curDataHeadPos    = 0;

                delete []mallocData;
                mallocData = nullptr;
            }
            else
            {
                //内存迁移
                memcpy_s(m_pMemoryHead, validDataLen, m_pDataHead, validDataLen);
//              memset(m_pMemoryHead + validDataLen, 0,sizeof (unsigned char)*(m_reservedSize - validDataLen));
                m_pDataHead         = m_pMemoryHead;
                m_dataLen           = validDataLen;
                m_curDataHeadPos    = 0;
            }
        }
      memcpy_s(m_pMemoryHead + m_dataLen, len, pData, len);
      m_dataLen = len + m_dataLen;
       return len;
    }


    /*
     * @brief 输出数据
     * @param pData 输出数据
     * @param len 输出数据长度
     * @return 输出数据实际长度
     */
    unsigned int streamOut(unsigned char *pData, unsigned int len)
    {
        unsigned int outLen = 0;
        //memset(pData, 0, len);
        if(m_dataLen > m_curDataHeadPos) {
            //还有数据
            outLen = (m_dataLen - m_curDataHeadPos) > len ? len : (m_dataLen - m_curDataHeadPos);
            memcpy_s(pData, outLen, m_pDataHead, outLen);
            m_curDataHeadPos += outLen;
            m_pDataHead = m_pMemoryHead + m_curDataHeadPos;
        }

        //没有可用数据。清空内存
        if(m_curDataHeadPos >= m_dataLen) {
            clearStream();
        }

        return outLen;
    }
#endif

    /*!
     * \brief streamRead 从缓存中读取数据，不删除
     * \param pos   读取位置
     * \param data 输出数据
     * \param readLen 输出数据长度（等于0的时候表示由streamRead确定）
     * \param func 回调函数
     * \return  输出数据长度
     */
    unsigned int streamRead(unsigned int pos, \
                            unsigned char **data, unsigned int readLen = 0, \
                            callback func = nullptr)
    {
        if(nullptr == func)
        {         //??????
            if(0 == readLen || pos < m_curDataHeadPos)
            {
                return 0;
            }

            if(m_dataLen > pos)
            {
                //有数据
                readLen = (m_dataLen - pos) > readLen ? readLen : (m_dataLen - pos);
                *data = new unsigned char[readLen];
                memcpy_s(*data, readLen, m_pMemoryHead + pos, readLen);

            }
            else
            {
                readLen = 0;
            }

            return readLen;

        }
        else
        {
            return func(pos, m_pMemoryHead, m_dataLen, data, readLen);
        }
    }

    /*!
     * \brief streamOut 输出数据，删除输出的数据
     * \param pos   读取位置
     * \param data 输出数据
     * \param readLen 输出数据长度（等于0的时候表示由streamRead确定,不能与func同时为0）
     * \param func 回调函数
     * \return  输出数据长度
     */
//    unsigned int    streamOut(unsigned int pos, \
//                           unsigned char **data, unsigned int readLen = 0, \
//                           callback func = nullptr)
//    {
//        if(nullptr == func)
//        {
//            if(0 == readLen || pos < m_curDataHeadPos)
//            {
//                return 0;
//            }

//            if(m_dataLen > pos)
//            {
//                //有数据
//                readLen = (m_dataLen - pos) > readLen ? readLen : (m_dataLen - pos);
//                *data = new unsigned char[readLen];
//                memcpy_s(*data, readLen, m_pMemoryHead + pos, readLen);
//                m_curDataHeadPos = pos + readLen;
//                m_pDataHead = m_pMemoryHead + m_curDataHeadPos;
//            }
//            else
//            {
//                readLen = 0;
//            }

//            //没有可用数据。清空内存
//            if(m_curDataHeadPos >= m_dataLen)
//            {
//                clearStream();
//            }

//            return readLen;

//        }
//        else
//        {
//            auto outlen = func(pos, m_pMemoryHead, m_dataLen, data, readLen);
//            if(outlen > 0)
//            {
//                m_curDataHeadPos = pos + outlen;
//                m_pDataHead = m_pMemoryHead + m_curDataHeadPos;

//                //没有可用数据。清空内存
//                if(m_curDataHeadPos >= m_dataLen)
//                {
//                    clearStream();
//                }
//            }

//            if(nullptr == *data)
//            {
//                return 0;
//            }

//            return outlen;
//        }
//    }

    //pos (ff d8) pos1(ff d9)

    unsigned int    streamOut(unsigned int pos, unsigned int pos1,\
                           unsigned char **data, unsigned int readLen = 0, \
                           callback func = nullptr)
    {
        if(nullptr == func)
        {
            //            if(0 == readLen || pos < m_curDataHeadPos)
            //            {
            //                return 0;
            //            }

            //            if(m_dataLen > pos)
            //            {
            //                //有数据
            //                readLen = (m_dataLen - pos) > readLen ? readLen : (m_dataLen - pos);
            //                *data = new unsigned char[readLen];
            //                memcpy_s(*data, readLen, m_pMemoryHead + pos, readLen);
            //                m_curDataHeadPos = pos + readLen;
            //                m_pDataHead = m_pMemoryHead + m_curDataHeadPos;
            //            }
            //            else
            //            {
            //                readLen = 0;
            //            }

            //            //没有可用数据。清空内存
            //            if(m_curDataHeadPos >= m_dataLen)
            //            {
            //                clearStream();
            //            }

                       return readLen;

        }
        else
        {
            auto outlen = func(pos, m_pMemoryHead, m_dataLen, data, readLen);
            if(outlen > 0)
            {
                m_curDataHeadPos = pos1;  //移到读取完的数据ffd9后
                m_pDataHead = m_pMemoryHead + pos1;
            }

            if(nullptr == *data)
            {
                return 0;
            }

            return outlen;
        }
    }


#if 0
    /*!
     * \brief streamOut 截取bg --> end字符串
     * \param bg    开始标识
     * \param end   结束标识
     * \param out   输出字符串
     * \param len   输出字符串长度
     * \return  输出字符串长度
     */
    unsigned int streamOut(const unsigned char *bg, int bgLen, \
                           const unsigned char *end, int endLen,\
                           unsigned char **out, unsigned int &len)
    {
//        //开始标识字符串长度
//        auto bgLen = strlen(reinterpret_cast<const char*>(bg));
//        //结束标识字符串长度
//        auto endLen = strlen(reinterpret_cast<const char*>(end));

//        unsigned char chBegin[2] = {0xFF, 0xD8};
//        unsigned char chEnd[2] = {0xFF, 0xD9};

        //查找开始标识
        unsigned int iBegin = 0; //chBegin的起始位置
        bool flag    = false; //查找标识
        auto bgFindLen = m_dataLen - bgLen + 1; //查找长度
        for(auto idx = m_curDataHeadPos; idx < bgFindLen; ++idx)
        {
            if(0 == memcmp(m_pMemoryHead + idx, bg, sizeof (unsigned char)*bgLen)) {
                //chBegin的起始位置
                iBegin = idx;
                flag = true;
                break;
            }
        }

        if(!flag) {
            //未找到
            len = 0;
            return len;
        }

        flag = false;

        //查找chEnd
        unsigned int iEnd = 0;
        auto endFindLen = m_dataLen - endLen + 1; //查找长度
        //从chBegin下一个字符开始查找
        for(auto idx = iBegin + bgLen; idx < endFindLen; ++idx)
        {
            if(0 == memcmp(m_pMemoryHead + idx, end, sizeof (unsigned char)*endLen)) {
                //chEnd的结束位置(最后一个字符的下一个位置,方便截取字符串)
                iEnd = static_cast<unsigned int>(idx + endLen);
                flag = true;
                break;
            }
        }

        if(!flag) {
            //未找到
            len = 0;
            return len;
        }

        len = iEnd - iBegin;

        *out = new unsigned char[len];

        //拷贝数据
        memcpy_s(*out, len, m_pDataHead + iBegin, len);

        //更新内存信息·
        m_curDataHeadPos = iEnd;
        m_pDataHead = m_pMemoryHead + m_curDataHeadPos;

        //没有可用数据。清空内存
        if(m_curDataHeadPos >= m_dataLen) {
            clearStream();
        }

        return len;
    }
#endif
    /*!
     * \brief streamFind 查找目标字符串在源字符串中的位置
     * \param chFind 目标字符串
     * \param chLen 目标字符串长度
     * \param isReturnHead true:返回头部位置，false：返回尾部位置
     * \param fromBegin 开始查询位置（源字符串）
     * \param toEnd 结束查询位置（源字符串）
     * \return 第一个目标字符串的位置，没找到返回0xffffffff(-1)
     */
    unsigned int streamFind(const unsigned char *chFind, unsigned int chLen, \
                      bool isReturnHead = true, \
                      unsigned int fromBegin = 0, unsigned int toEnd = 0xffffffff)
    {
        //查找到的位置
        unsigned int findPos{0xffffffff};

        //确定范围
        if(0 == fromBegin)
        {
            fromBegin = m_curDataHeadPos;
        }

        if(0xffffffff == toEnd)
        {
            toEnd = m_dataLen - 1;
        }

        //判断
        if(fromBegin < m_curDataHeadPos
                || toEnd > m_dataLen - 1
                || (toEnd - fromBegin + 1) < chLen)
        {
            return findPos;
        }

        //sunday算法查找
        unsigned char *T = m_pMemoryHead + fromBegin;
        unsigned int lenT = toEnd - fromBegin + 1;

        findPos = sunday(T, lenT, chFind, chLen);

//        //查找
//        for(auto idx = fromBegin; idx <= toEnd; ++idx)
//        {
//            if(0 == memcmp(m_pMemoryHead + idx, chFind, sizeof (unsigned char)*chLen))
//            {
//                findPos = idx;
//                break;
//            }
//        }

        if(0xffffffff != findPos)
        {
            return (isReturnHead == true ? findPos + fromBegin : (findPos + fromBegin + chLen - 1));
        }
        else
        {
            return findPos;
        }

    }

    //删除内存
    void freeStream()
    {
        if(nullptr != m_pMemoryHead) {
            delete []m_pMemoryHead;
        }

        m_pMemoryHead = nullptr;
        m_pDataHead = nullptr;
        m_curDataHeadPos = 0;
        m_dataLen = 0;
        m_reservedSize = 0;
    }

    //清空数据，不删除内存
    void clearStream()
    {
        if(nullptr != m_pMemoryHead) {
            //memset(m_pMemoryHead, 0, sizeof (unsigned char)*m_reservedSize);
        }

        m_pDataHead = nullptr;
        m_curDataHeadPos = 0;
        m_dataLen = 0;
    }

    bool isEmpty()
    {
        return m_dataLen == 0;
    }

private:
    QzskjDataStream(const QzskjDataStream&) = delete;
    QzskjDataStream& operator=(const QzskjDataStream&) = delete;
private:
    unsigned char   *m_pMemoryHead{nullptr};     //内存地址,申请内存的地址，一旦申请是不变的
    unsigned char   *m_pDataHead{nullptr};      //数据地址，指向实际的数据地址
    unsigned int    m_curDataHeadPos{0};        //m_pDataHead的位置m_pDataHead = m_pMemoryHead + m_curDataHeadPos
    unsigned int    m_dataLen{0};               //数据实际长度
    unsigned int    m_reservedSize{0};          //预留空间大小，一般为m_dataLen*2
};

#endif // QzskjDataStream_H
