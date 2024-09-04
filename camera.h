#ifndef CAMERA_H
#define CAMERA_H

#define RCV_MAXDATASIZE (65536*2)
#define RCV_LENGTHONCE 8192

#define MICRO_CLOCK std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds>
#define GET_NOW_MICROSECOND (std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now()))

#include "camera_global.h"
#include <QtNetwork>
#include <QThread>
#include "qzskjdatastream.hpp"
#include "icameramodule.h"
#include <QImage>
#include <cctype>
#include <cstdlib>

/*!
 * \brief 网络摄像头
 */
class QCameraClient : public QObject
{
    Q_OBJECT
public:
   explicit QCameraClient(const QString &ip, U32 port, QObject *parent = nullptr);
signals:
    //void connectFinished();
    void signalRecvOneFrame(DataFrame &);
    void signalProcessData(QByteArray&);
public slots:
    //void connectWork();
    void slotConnected();
    void slotDisconnected();
    void slotReadData();
    void slotDisplayError(QAbstractSocket::SocketError e);
    void slotProcessData(QByteArray&);
public:
    /*!
     * \brief isConnected connect status
     * \return
     */
    bool isConnected()
    {
        return (m_pTcpSocket && m_isConnected);
    }

    /*!
     * \brief disConnect (call by cameraWork class)disconnects
     *                   and does not reconnect untill
     *                   (cameraWork)call the function "autoConnect"
     */
    void disConnect()
    {
        m_autoConnect = false;
        if(isConnected())
        {
            m_pTcpSocket->close();
            delete m_pTcpSocket;
            m_pTcpSocket = nullptr;
         //   m_pTcpSocket->disconnectFromHost();
         //   m_pTcpSocket->waitForDisconnected(1000); //退出有问题
        }
    }

    /*!
     * \brief exitConnect disconnects and exit the thread. And we cannot reconnect the
     *                      camera again untill the camera module restart.
     */
    void exitConnect()
    {
        //m_connectThreadFlag = false;
        m_isRecvData = false;
        disConnect();
    }

    /*!
     * \brief set network
     * \param
     */
    void setIP(const QString &ip)
    {
        m_ip = ip;
    }

    void setPort(U32 port)
    {
        m_port = port;
    }

    /*!
     * \brief startNetWorkCamera 启动连接
     */
    void startNetWorkCamera();

    /*!
     * \brief startReceive connect and receive data
     */
    void startReceive()
    {
        if(!isConnected()) {
            startNetWorkCamera();
        }

        m_isRecvData = true;
    }

    void stopReceive()
    {
        m_isRecvData = false;
        m_data.clearStream();
    }

private:
    /*!
     * \brief sendData send data to server
     * \param data data
     */
    void sendData(const string &data)
    {
        qDebug()<<"cmd "<<data.c_str();
        if(!data.empty()) {
            m_pTcpSocket->write(data.c_str(), static_cast<I64>(data.length()));
            m_pTcpSocket->flush();
            //m_pTcpSocket->waitForBytesWritten(100);
            //m_pTcpSocket->waitForReadyRead(500);
        }
    }

    /*!
     * \brief getContentLength 读取图像长度数据
     * \param pos 读取位置
     * \param src 源数据
     * \param slen 源数据长度
     * \param out 输出数据
     * \param oulen 输出数据长度
     * \return
     */
    unsigned int getContentLength(unsigned int pos,\
                                  const unsigned char *src, unsigned int slen, \
                                  unsigned char **out, unsigned int oulen);

    /*!
     * \brief getJPEGData 读取图像数据
     * \param pos 读取位置,对应的0xff 0xd8
     * \param src 源数据
     * \param slen 源数据长度
     * \param out 输出数据
     * \param oulen 输出数据长度
     * \return
     */
    //    unsigned int getJPEGData(unsigned int pos,\
    //                             const unsigned char *src, unsigned int slen, \
    //                             unsigned char **out, unsigned int oulen);
    // pos1 对应0xff 0xd9 位置
    unsigned int getJPEGData(unsigned int pos,\
                             const unsigned char *src, unsigned int slen, \
                             unsigned char **out, unsigned int oulen);


private:
    //thread running flag.
    //true:running
    //false:stop
    //bool m_connectThreadFlag{true};

    //connect_status flag
    bool m_isConnected{false};

    //reconnect'flag
    //true:if the client is disconnected,it will reconnect
    bool m_autoConnect{false};

    //receive data'flag
    bool m_isRecvData{false};

    QString m_ip;   //ip
    U32     m_port{0}; //port

    QTcpSocket  *m_pTcpSocket{nullptr};    //socket

    QzskjDataStream m_data; //视频流数据
    bool  findagain{false}; //是否继续查找
};

//usb摄像头最大尝试数量
const int MAX_TRY_COUNT = 10;

/*!
 * \brief Mat格式转换成Image格式
 */
class Mat_to_QImage
{
public:
    static QImage start(const Mat &img, QImage::Format fm = QImage::Format_RGB888)
    {
        cvtColor(img, img, COLOR_BGR2RGB);
        return QImage(img.data, img.cols, img.rows, static_cast<int>(img.step), fm);
    }
};

/*!
 * \brief 通过usb摄像头获取图像信息
 */
class QCameraUsb : public QObject
{
    Q_OBJECT
public:
    //打开usb摄像头，成功true,失败 false
    bool open(int indx, int &id, int apiPreference = cv::CAP_ANY)
    {
        bool r{false};
        if(-1 == indx) {
            id = -1;
            for (int i = 0; i < MAX_TRY_COUNT; ++i) {
                if(m_cvVideo.open(i, apiPreference)) {
                    m_cvVideo.set(CAP_PROP_FPS, 30.0);
                    m_cvVideo.set(CAP_PROP_FRAME_HEIGHT, 480.0);
                    m_cvVideo.set(CAP_PROP_FRAME_WIDTH, 640.0);
                    r = true;
                    id = i;
                    break;
                }

                m_cvVideo.release();
            }
        }
        else {
            r = m_cvVideo.open(indx, apiPreference);
            id = r == true ? indx : -1;
        }

        if(r) {
            m_readFlag = true;
        }

        return r;
    }

    bool open(const string &filename, int apiPreference = cv::CAP_ANY)
    {
        return m_cvVideo.open(filename, apiPreference);
    }

    //重新打开摄像头
    bool reopen(int indx, int &id, int apiPreference = cv::CAP_ANY)
    {
        m_isRecvData = false;
        m_cvVideo.release();
        return open(indx, id, apiPreference);
    }

    //判断是否打开摄像头
    bool isOpened()
    {
        return m_cvVideo.isOpened();
    }

    void close()
    {     
        m_isRecvData = false;
        m_readFlag = false;
        m_cvVideo.release();
    }

    void startReceive()
    {
        m_isRecvData = true;
    }

    void stopReceive()
    {
        m_isRecvData = false;
    }

signals:
    void stop();
    void sendMatFrame(cv::Mat &);
public slots:
    void slotReadMat();
private:
    bool m_readFlag{true};    //读取标识，true：读取；false：退出线程
    //usb连接
    cv::VideoCapture    m_cvVideo;

    bool m_isRecvData{false};    //接收标识
};

/*!
 * \brief 摄像头总控制模块
 */
class QCameraWorker : public ICameraModule
{
    Q_OBJECT
public:
    static QCameraWorker *getInstance()
    {
        static QCameraWorker cameraWorker;
        return &cameraWorker;
    }

private:
    QCameraWorker();
    ~QCameraWorker() override
    {
        exit();
    }

public:
    /*!
     * @brief:    初始化函数
     * @date:     2018/08/28
     * @param:    ConfigData *pcfgData 配置数据项
     * @param:    void *pReserved 预留
     * @return:   U32
    */
    virtual U32 initial(ConfigData *pcfgData, void *pReserved = nullptr) override;

    /*!
     * @brief:    模块退出函数，用于释放模块资源
     * @date:     2018/08/28
     * @param:    void
     * @return:   U32
    */
    virtual U32 exit(void) override;

    /*!
     * @brief:    配置信息改变通知函数
     * @date:     2018/08/28
     * @param:    InfoType type 信息项类型
     * @param:    const string &ip 当type为修改ip信息的时候，该选项生效
     * @param:    U32 port 当type为修改port信息的时候，该选项生效
     * @return:   U32
    */
    virtual U32 networkModify(InfoType type, const string &ip, U32 port) override;

    /*!
     * \brief usbCameraChanged usb摄像头改变
     * \param id 摄像头编号
     * \return u32
     */
    virtual U32 usbCameraChanged(int id) override;

    /*!
     * @brief:    获取USB摄像头数量
     * @date:     2018/08/28
     * @return:   int 摄像头数量
    */
    virtual int getCameraCount() override;

    /*!
     * \brief start 启动接收图像
     */
    virtual void startWork() override;

    /*!
     * \brief startNetWorkCamera 网络摄像头。连接
     */
    virtual void startNetWorkCamera() override;

    /*!
     * \brief stopWork 停止接收图像（不中断连接）
     */
    virtual void stopWork() override;

//usb摄像头相关signals && slots
signals:
//    void signalReadMatByUsb();
private slots:
    void slotRecvMatByUsb(Mat &);

//无线连接相关signals && slots
private slots:
    void slotRecvOneFrame(DataFrame &);

//有线连接相关signals && slots
//signals:
//    void signalImageCaptured(int id, const QImage &preview);

//private slots:
//    void slotImageCaptured(int id, const QImage &preview);

private:
    //检测是否有usb摄像头
    bool checkUSBCamera();

    //创建线程，建立信号连接
    void createThread();

private:
    ConfigData  m_config;   //配置数据

    QCameraClient   *m_pCameraClient{nullptr};
 //   QThread         *m_pthCameraClient{nullptr};    //连接线程

//    QCameraClient    *m_pCathPictureWorker{nullptr};
//    QThread *m_pthCathPicture{nullptr};    //读取图像线程

    //有线
//    QCamera             *m_pCamera;
//    //QCameraViewfinder   *m_pViewfinder;
//    QCameraImageCapture *m_pImageCapture;
    //QImage              m_Image;

    //usb摄像头
    QThread             *m_pthUsbCamera{nullptr};
    QCameraUsb          *m_pUsbCamera{nullptr};

    int                 m_openId{-1};   //打开的摄像头

};

extern "C" Q_DECL_EXPORT ICameraModule *getInstance(void *pReserve);

#endif // CAMERA_H
