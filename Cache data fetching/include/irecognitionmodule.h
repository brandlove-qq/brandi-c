#ifndef IRECOGNITIONMODULE_H
#define IRECOGNITIONMODULE_H
#include <QObject>
#include <QImage>
#include "er_types.h"
#include <vector>
#include <tuple>
#include <string>
#include "../../../../3rdLibrary/opencv-4.1.0/vc14-x64/include/opencv2/core.hpp"
#include "IEngine.h"

class IRecognitionModule :public QObject
{
    Q_OBJECT

public:
    IRecognitionModule() = default;
    virtual ~IRecognitionModule() {}

public:
    /*!
     * @brief:    初始化函数
     * @date:     2018/08/28
     * @param:    ConfigData *cfgData 配置数据项
     * @param:    void *pReserved 预留
     * @return:   U32
    */
    virtual U32 initial(ConfigData *cfgData, void *pReserved = nullptr) = 0;

    /*!
     * @brief:    模块退出函数，用于释放模块资源
     * @date:     2018/08/28
     * @param:    void
     * @return:   U32
    */
    virtual U32 exit(void) = 0;

    /*!
     * @brief:    图像数据更新函数
     * @date:     2018/08/28
     * @param:    const DataFramebuff 数据帧缓存地址
     * @return:   U32 成功返回0，失败返回错误码
    */
    virtual U32 updataImage(const DataFrame &buff) = 0;

    /*!
     * @brief:    图像数据更新函数
     * @date:     2018/08/28
     * @param:    const Mat &img 图像信息
     * @return:   U32 成功返回0，失败返回错误码
    */
    virtual U32 updataImage(cv::Mat &img) = 0;

    /*!
     * \brief command 操作指令函数，具体的控制命令通过该接口分发
     * \param cmd 命令类型
     * \param wParam 命令第一个参数
     * \param lParam 命令第二个参数
     * \return  U32 成功返回0，失败返回错误码
     */
    virtual U32 command(CommandType cmd, ZS_PTR wParam, ZS_PTR lParam) = 0;

signals:
    void signalDrawImage(const RecognitionEngineOutput&);
};

/*< 子模块DLL导出函数形式*/
typedef IRecognitionModule *(*getRecognitionModuleInstance)(void *pReserve);

#endif // IRECOGNITIONMODULE_H
