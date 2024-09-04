#ifndef ICAMERAMODULE_H
#define ICAMERAMODULE_H
#include <QObject>
#include <string>
#include <QList>
#include <QCameraInfo>
#include "er_types.h"
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

/*!
 * \brief 摄像头的接口函数
 */
class ICameraModule : public QObject
{
    Q_OBJECT

public:
    ICameraModule() = default;
    virtual ~ICameraModule(){}
public:
    /*!
     * @brief:    初始化函数
     * @date:     2018/08/28
     * @param:    ConfigData *pcfgData 配置数据项
     * @param:    void *pReserved 预留
     * @return:   U32
    */
    virtual U32 initial(ConfigData *pcfgData, void *pReserved = nullptr) = 0;

    /*!
     * @brief:    模块退出函数，用于释放模块资源
     * @date:     2018/08/28
     * @param:    void
     * @return:   U32
    */
    virtual U32 exit(void) = 0;

    /*!
     * @brief:    配置信息改变通知函数
     * @date:     2018/08/28
     * @param:    InfoType type 信息项类型
     * @param:    const string &ip 当type为修改ip信息的时候，该选项生效
     * @param:    U32 port 当type为修改port信息的时候，该选项生效
     * @return:   U32
    */
    virtual U32 networkModify(InfoType type, const string &ip, U32 port) = 0;

    /*!
     * \brief usbCameraChanged usb摄像头改变
     * \param id 摄像头编号
     * \return u32
     */
    virtual U32 usbCameraChanged(int id) = 0;

    /*!
     * @brief:    获取USB摄像头数量
     * @date:     2018/08/28
     * @return:   int 摄像头数量
    */
    virtual int getCameraCount() = 0;

    /*!
     * \brief startNetWorkCamera 网络摄像头。连接
     */
    virtual void startNetWorkCamera() = 0;

    /*!
     * \brief startWork 启动接收图像
     */
    virtual void startWork() = 0;

    /*!
     * \brief stopWork 停止接收图像（不中断连接）
     */
    virtual void stopWork() = 0;

//和mainframe的交互
signals:
    void signalCatchUsbImage(Mat&);
    void signalCatchNetWorkImage(DataFrame&);
};

/*< 子模块DLL导出函数形式*/
typedef ICameraModule *(*getCameraModuleInstance)(void *pReserve);

#endif // ICAMERAMODULE_H
