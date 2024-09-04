#include "camera.h"
#include <QRegExpValidator>
#include <functional>


using namespace cv;

/*!
 * \brief QCameraWorker
 */
QCameraWorker::QCameraWorker()
{

}

U32 QCameraWorker::initial(ConfigData *pcfgData, void *pReserved)
{
    Q_UNUSED(pReserved)

    //保存配置信息
    m_config = *pcfgData;

    //检测usb摄像头
    if(checkUSBCamera()) {
        //有usb摄像头
        pcfgData->ConnectionType = CT_USB; //修改配置文件连接方式为USB连接
        m_config.ConnectionType = CT_USB;

        //打开摄像头
        bool b = m_pUsbCamera != nullptr ? m_pUsbCamera->isOpened() : false;
        if(b && m_openId != pcfgData->usbCameraNumber \
                && pcfgData->usbCameraNumber != -1) {
            //配置文件摄像头与打开的不一致，重新打开
            //m_pUsbCamera->close();
            if(!m_pUsbCamera->reopen(pcfgData->usbCameraNumber, m_openId)) {
                LOGE_(m_config.log, LOG_OUTPUT, "打开配置文件指定摄像头失败。")
                return EC_FAILURE;
            }

            createThread();
        }
        else if(b && pcfgData->usbCameraNumber == -1) {
            //摄像头已经开打，配置文件未指定，修改配置文件
            pcfgData->usbCameraNumber = m_openId;
            m_config.usbCameraNumber = m_openId;
        }
        else if(!b && pcfgData->usbCameraNumber != -1) {
            //摄像头未打开，打开指定摄像头
            if(nullptr == m_pUsbCamera) {
                m_pUsbCamera = new QCameraUsb;
            }

            if(!m_pUsbCamera->open(pcfgData->usbCameraNumber, m_openId)) {
                LOGE_(m_config.log, LOG_OUTPUT, "打开配置文件指定摄像头失败。");

                delete m_pUsbCamera;
                m_pUsbCamera = nullptr;

                return EC_FAILURE;
            }

            createThread();
        }
        else if(!b && pcfgData->usbCameraNumber == -1) {
            //摄像头未打开，配置文件未指定，则开打默认摄像头
            if(nullptr == m_pUsbCamera) {
                m_pUsbCamera = new QCameraUsb;
            }

            if(!m_pUsbCamera->open(-1, m_openId)) {
                LOGE_(m_config.log, LOG_OUTPUT, "打开默认摄像头失败。");

                delete m_pUsbCamera;
                m_pUsbCamera = nullptr;

                return EC_FAILURE;
            }

            pcfgData->usbCameraNumber = m_openId;
            m_config.usbCameraNumber = m_openId;

            createThread();
        }

    }
    else {
        if(CT_WIRELESS == pcfgData->ConnectionType) {
            createThread();
        }
#if 0
        else if(CT_WIRED == pcfgData->ConnectionType ) {

            QCameraInfo *pCameraInfo = reinterpret_cast<QCameraInfo *>(pcfgData->pCameraInfo);
            QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
            for(const QCameraInfo &cameraInfo : cameras) {
                if(*pCameraInfo == cameraInfo) {
                    m_pCamera = new QCamera(cameraInfo);
                    m_pImageCapture = new QCameraImageCapture(m_pCamera);
                    m_pCamera->setCaptureMode(QCamera::CaptureStillImage);
                }
            }

            connect(m_pImageCapture, &QCameraImageCapture::imageCaptured, \
                    this, &QCameraWorker::slotImageCaptured, Qt::DirectConnection);
            m_pCamera->start();
        }

#endif

    }

    LOGI_(m_config.log, LOG_OUTPUT, "Load Camera.dll succeed.")

    return EC_SUCCESS;
}

void QCameraWorker::createThread()
{
    if(CT_USB == m_config.ConnectionType) {
        //USB连接
        if(nullptr == m_pthUsbCamera) {
            m_pthUsbCamera = new QThread;
        }

        if(nullptr == m_pUsbCamera) {
            m_pUsbCamera = new QCameraUsb;
        }

        m_pthUsbCamera->start();

        m_pUsbCamera->moveToThread(m_pthUsbCamera);

        connect(m_pthUsbCamera, &QThread::started, \
                m_pUsbCamera, &QCameraUsb::slotReadMat, \
                Qt::AutoConnection);
        connect(m_pUsbCamera, &QCameraUsb::stop, \
                m_pthUsbCamera, &QThread::quit, \
                Qt::AutoConnection); //阻塞形式
        connect(m_pthUsbCamera, &QThread::finished, \
                m_pUsbCamera, &QObject::deleteLater, \
                Qt::AutoConnection);
        connect(m_pthUsbCamera, &QThread::finished, \
                m_pthUsbCamera, &QThread::deleteLater, \
                Qt::AutoConnection);

        qRegisterMetaType<cv::Mat>("cv::Mat&");
        connect(m_pUsbCamera, &QCameraUsb::sendMatFrame, \
                this, &QCameraWorker::slotRecvMatByUsb, \
                Qt::AutoConnection);
    }
    else if(CT_WIRELESS == m_config.ConnectionType) {
//        if(nullptr == m_pthCameraClient) {
//            m_pthCameraClient = new QThread;
//        }

        if(nullptr == m_pCameraClient) {
            m_pCameraClient = new QCameraClient(QString::fromStdString(m_config.ipStream), \
                                                m_config.portStream);
        }

//        m_pCameraClient->moveToThread(m_pthCameraClient);
//        connect(m_pthCameraClient, &QThread::started, m_pCameraClient, &QCameraClient::connectWork);
//        connect(m_pCameraClient, &QCameraClient::connectFinished, m_pthCameraClient, &QThread::quit);
//        connect(m_pthCameraClient, &QThread::finished, m_pthCameraClient, &QObject::deleteLater);
//        connect(m_pthCameraClient, &QThread::finished, m_pCameraClient, &QObject::deleteLater);
//        m_pthCameraClient->start();

        connect(m_pCameraClient, &QCameraClient::signalRecvOneFrame, \
                this, &QCameraWorker::slotRecvOneFrame);
    }
}

int QCameraWorker::getCameraCount()
{
    int cnt{0};
    for(;;) {
        if(m_openId == cnt && m_pUsbCamera && m_pUsbCamera->isOpened()) {
            ++cnt;
            continue;
        }

        cv::VideoCapture vc(cnt);
        bool b = vc.isOpened();
        vc.release();
        if(!b) {
            break;
        }

        ++cnt;
    }

    return cnt;
}

bool QCameraWorker::checkUSBCamera()
{
    if(m_pUsbCamera && m_openId != -1) {
        return true;
    }

    cv::VideoCapture vc;
    for(int i = 0; i < MAX_TRY_COUNT; ++i) {
        bool b = vc.open(i);
        vc.release();
        if(b)
        {
            return b;
        }

    }

    return false;
}

U32 QCameraWorker::networkModify(InfoType type, const string &ip, U32 port)
{
    U32 ret = EC_SUCCESS;

    switch (static_cast<int>(type))
    {
    case INFOCHANGE_IP_STREAM:
    {
        m_pCameraClient->disConnect();
        //QThread::sleep(1);
        m_config.ipStream = ip;
        m_pCameraClient->setIP(QString::fromStdString(m_config.ipStream));
        //start();
    }
        break;
    case INFOCHANGE_PORT_STREAM:
    {
        m_pCameraClient->disConnect();
        //QThread::sleep(1);
        m_config.portStream = port;
        m_pCameraClient->setPort(m_config.portStream);
        //start();
    }
        break;
    default:
        ret = EC_FAILURE;
        break;
    }

    return ret;
}

U32 QCameraWorker::usbCameraChanged(int id)
{
    if(m_pUsbCamera && m_pUsbCamera->isOpened()) {
        m_pUsbCamera->reopen(id, m_openId);
    }
    else if(m_pUsbCamera && !m_pUsbCamera->isOpened()) {
        m_pUsbCamera->open(id, m_openId);
    }
    else {
        if(nullptr == m_pUsbCamera) {
            m_pUsbCamera = new QCameraUsb;
        }

        if(!m_pUsbCamera->open(-1, m_openId)) {
            LOGE_(m_config.log, LOG_OUTPUT, "打开默认摄像头失败。");

            delete m_pUsbCamera;
            m_pUsbCamera = nullptr;

            return EC_FAILURE;
        }

        m_config.usbCameraNumber = m_openId;

        createThread();
    }

    return EC_SUCCESS;
}

U32 QCameraWorker::exit()
{
    if(CT_USB == m_config.ConnectionType) {
        //关闭USB摄像头
        if(m_pUsbCamera) {
            m_pUsbCamera->close();
        }

    }
    else if(CT_WIRELESS == m_config.ConnectionType) {
        if(m_pCameraClient) {
            m_pCameraClient->exitConnect();
        }

    }
    else {
#if 0
        delete m_pImageCapture;
        m_pImageCapture = nullptr;
        delete  m_pCamera;
        m_pCamera = nullptr;
#endif
    }

    return EC_SUCCESS;
}

void QCameraWorker::startWork()
{
    if(CT_USB == m_config.ConnectionType) {
        //USB接收
        if(m_pUsbCamera && m_pUsbCamera->isOpened()) {
            m_pUsbCamera->startReceive();
        }

    }
    else {
        //网络接收
        if(nullptr != m_pCameraClient) {
            m_pCameraClient->startReceive();
        }

    }
}

void QCameraWorker::stopWork()
{
    if(CT_USB == m_config.ConnectionType) {
        //USB接收
        if(m_pUsbCamera && m_pUsbCamera->isOpened()) {
            m_pUsbCamera->stopReceive();
        }

    }
    else {
        //网络接收
        if(nullptr != m_pCameraClient) {
            m_pCameraClient->stopReceive();
        }

    }
}

void QCameraWorker::startNetWorkCamera()
{
    if(CT_WIRELESS == m_config.ConnectionType) {
        if(nullptr != m_pCameraClient) {
            m_pCameraClient->startNetWorkCamera();
        }
    }
}

#if 0
void QCameraWorker::slotImageCaptured(int id, const QImage &preview)
{
    Q_UNUSED(id);

    emit signalCatchImage(preview);
}
#endif

void QCameraWorker::slotRecvMatByUsb(Mat &img)
{
    //emit signalCatchImage(Mat_to_QImage::start(img));
    emit signalCatchUsbImage(img);
}

void QCameraWorker::slotRecvOneFrame(DataFrame &data)
{
//    QImage img;
//    img.loadFromData(data.buff, static_cast<int>(data.size));
    emit signalCatchNetWorkImage(data);
}

/*!
 * ******************QCaremaUsb**********************************
 */
void QCameraUsb::slotReadMat()
{
    while(m_readFlag) {
        if(m_isRecvData && isOpened()) {
            cv::Mat img;
            m_cvVideo.read(img);
            if(nullptr != img.data) {
                //读取到图像
                emit sendMatFrame(img);
            }
        }
        else
        {
            QThread::msleep(50);
        }

        //qDebug() << QThread::currentThreadId();
    }

    emit stop();
}

/*! ***********************QCameraClient****************************************/

QCameraClient::QCameraClient(const QString &ip, U32 port, QObject *parent)
    :QObject (parent)
    ,m_ip(ip)
    ,m_port(port)
{
    //m_data.mallocStream(RCV_MAXDATASIZE);
    qRegisterMetaType<QByteArray>("QByteArray&");
    connect(this, &QCameraClient::signalProcessData, this, &QCameraClient::slotProcessData, Qt::QueuedConnection);
}

#if 0
void QCameraClient::connectWork()
{
    while(m_connectThreadFlag) {
        while(!m_isConnected && m_autoConnect) {
            //如果未建立连接，则尝试创建连接
            if(nullptr == m_pTcpSocket) {
                //socket == null,创建socket
                m_pTcpSocket = new QTcpSocket;

                //创建singal && slot
                connect(m_pTcpSocket, &QTcpSocket::connected, this, &QCameraClient::slotConnected);
                connect(m_pTcpSocket, &QTcpSocket::disconnected, this, &QCameraClient::slotDisconnected);
                connect(m_pTcpSocket, &QTcpSocket::readyRead, this, &QCameraClient::slotReadData);
                connect(m_pTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotDisplayError(QAbstractSocket::SocketError)));
            }

            m_pTcpSocket->connectToHost(m_ip, static_cast<unsigned short>(m_port));
            m_pTcpSocket->waitForConnected(1000);
            //QThread::msleep(1000);
        }

        QThread::msleep(500);
    }

    //emit connectFinished();
}
#endif

void QCameraClient::slotConnected()
{
    m_isConnected = true;
    //m_connectThreadFlag = false;
    //send command
    string strCommand("GET /?action=stream\n\n");
    sendData(strCommand);
    qDebug() << "connected!";
}

void QCameraClient::slotDisconnected()
{
    //m_connectThreadFlag = false;
    m_isConnected     = false;
}

void QCameraClient::slotReadData()
{
    //30fps的摄像头
    static bool recordFlag{true};
    static MICRO_CLOCK uc1{GET_NOW_MICROSECOND};
    MICRO_CLOCK uc2{GET_NOW_MICROSECOND};
    I64 periodTime{(uc2 - uc1).count()};
    if(recordFlag || periodTime >= 25000)
    {

        recordFlag = false;

        QByteArray qRead{m_pTcpSocket->readAll()};
        if(0 != qRead.size())
        {
           qDebug() << "readall = " << qRead.size();
            if(m_isRecvData)
            {
                emit signalProcessData(qRead);
            }
        }
        else
        {
            string strCommand("GET /?action=stream\n\n");
            sendData(strCommand);
        }

        uc1 = GET_NOW_MICROSECOND;
    }

}

void QCameraClient::slotDisplayError(QAbstractSocket::SocketError e)
{
    if(QAbstractSocket::SocketError::RemoteHostClosedError == e
            ||QAbstractSocket::SocketError::SocketTimeoutError == e)
    {
        if(m_autoConnect)
        {
            //连接断开
            if(nullptr == m_pTcpSocket) {
                //socket == null,创建socket
                m_pTcpSocket = new QTcpSocket;

                //创建singal && slot
                connect(m_pTcpSocket, &QTcpSocket::connected, this, &QCameraClient::slotConnected);
                connect(m_pTcpSocket, &QTcpSocket::disconnected, this, &QCameraClient::slotDisconnected);
                connect(m_pTcpSocket, &QTcpSocket::readyRead, this, &QCameraClient::slotReadData);
                connect(m_pTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotDisplayError(QAbstractSocket::SocketError)));
            }

            m_pTcpSocket->abort();
            m_pTcpSocket->connectToHost(m_ip, static_cast<unsigned short>(m_port));
            m_pTcpSocket->waitForConnected(2000);
        }
    }
}

/**************************************************************************************************
 ******************************从缓存中提取图像数据****************************************************
 *图像的标志:返回的图像数据“boundarydonotcross\r\nContent-Type: image/jpeg\r\nContent-Length: 30696\r\
 * nX-Timestamp: 314.660934带着这样的字符串;图像数据开始标志：0XFF,0XD8,图像的结束标志：0XFF，0XD9 长度:30696
 *
 * 注意:返回的数据需要考虑tcp数据截断问题,且图像尾减去图像头并不一定刚好等于返回的长度。有时不够是在图像尾添00 00 00 00
 *     server每次返回图像数据都带有上述字符串
 *
 * 处理：
 * 原理：找Content-Length:,获取返回的长度，找图像数据头和图像数据尾,相减和返回的图像长度对比，发出。
 * 1.停止接收数据和tcp断开连接对缓存数据清理
 * 2.第一次处理数据,先找字符串Content-Length:,没有清空数据。有进入循环，如果第一次进入的数据找到字符串，获取了图像的长度
 * 但是找不到图像头，就根据当前找到的字符串位置再次去找新的字符串位置，如果没有说明数据截断，跳出等待新数据插入。有说明无效
 * 数据：迁移实际数据指针的位置和实际数据的长度。
 * 3.循环体：考虑目前tcp读取了好几帧的图像数据情况
 *   如果对第一次数据找到发出后，对剩余的数据进行再次的查找，如果数据没找完,继续找,直到最后。进一步判断数据的有效性，如果数据无效
 *   迁移指针，继续查找，如果数据截断跳出循环等待新数据插入。
 *  注意有效数据的指针移动位置和有效数据的长度，
 *****************************************************************************************************/

void QCameraClient::slotProcessData(QByteArray& qRead)
{
    I64 readLen(qRead.size());
    unsigned int lenPos=0;
    m_data.streamIn(reinterpret_cast<unsigned char*>(qRead.data()), static_cast<U32>(readLen));
    if(!m_data.isEmpty()&&m_isRecvData)
    {
        qDebug()<< "readsize  " << qRead.size();
        U8 chLength[] = "Content-Length: ";
        lenPos = m_data.streamFind(chLength, 16, false);
        if(0xffffffff == lenPos)
        {
            m_data.clearStream(); //说明都是无效数据
            findagain=false;
            return;
        }
    }
   //处理数据
 do{
        if(!isConnected()||!m_isRecvData)
        {
            m_data.clearStream(); //清理
            findagain=false;
            break;
        }

        if(findagain)
        {
         U8 chLength[] = "Content-Length: "; //返回尾部位置
         lenPos= m_data.streamFind(chLength, 16, false,m_data.m_curDataHeadPos);
         if(0xffffffff ==lenPos )
          {//再次循环找不到
             qDebug()<<"findagain content";
             break;
          }
        }

        unsigned char *data = nullptr;  //图像数据大小存放 (获取图像的长度)
        auto func1 = std::bind(&QCameraClient::getContentLength, this, \
                              std::placeholders::_1,\
                              std::placeholders::_2,\
                              std::placeholders::_3,\
                              std::placeholders::_4,\
                              std::placeholders::_5);
        auto dataLen = m_data.streamRead(lenPos, &data, 0, func1);
        if(0 == dataLen)
        { //没有带图像大小尺寸
            //移动指针到conten--的位置处，重新进入循环
            m_data.m_curDataHeadPos=lenPos;
            m_data.m_pDataHead =m_data.m_pMemoryHead+lenPos;
            findagain=true;
            //判断是否数据尾
            qDebug()<<"dataLen have no ";
            continue;
        }

        U8 chBegin[2] = {0xFF, 0xD8}; //查找图像头0xff 0xd8
        auto beginPos = m_data.streamFind(chBegin, 2, true, lenPos);
        if(0xffffffff == beginPos)
        { //找不到ff d8
            //移动指针到conten--的位置处，重新进入循环
            qDebug()<<"ff d8 have no  ";
            //判断是否为数据尾
            if(findagain)
            {
                //循环处理数据后,再次进来（确认是无效数据还是tcp截断导致）
                U8 chLength[] = "Content-Length: "; //返回头部位置
                auto newlen1 = m_data.streamFind(chLength, 16, true,lenPos);//从循环后新的lenpos开始找
                if(0xffffffff ==newlen1)
                { //找不到说明是数据截断
                    qDebug()<<"ffd8 tcp cut again ";
                    break;
                }
                //数据无效，迁移位置,再次循环处理数据
                m_data.m_curDataHeadPos=newlen1;
                m_data.m_pDataHead =m_data.m_pMemoryHead+newlen1;
                findagain=true;
                qDebug()<<"ffd8 continu ";
                continue;
            }
            else
            {//数据是第一次处理
                U8 chLength[] = "Content-Length: "; //返回头部位置
                auto newlen2 = m_data.streamFind(chLength, 16, true,lenPos);//从最开始的lenpos找(第一次插入数据找的lenpos)
                if(0xffffffff ==newlen2)
                {//找不到，数据截断
                    qDebug()<<"one tcp cut off ";
                    break;
                }
                m_data.m_curDataHeadPos=newlen2;
                m_data.m_pDataHead =m_data.m_pMemoryHead+newlen2;
                findagain=true;
                qDebug()<<"one continue";
                continue;
            }
        }
        unsigned int jpegLen = strtoul(reinterpret_cast<char*>(data), nullptr, 10);//server返回图像实际尺寸大小
        if(data)
        {
            delete []data;
        }

        //查找图像数据尾0xff，0xd9
        U8 chEnd[2]={0xFF,0XD9};  //图像尾
        auto endpos=m_data.streamFind(chEnd,2,false,beginPos);//返回尾部,从0xff0xd8开始找
      if(0xffffffff == endpos)
        { //找不到0xff，0xd9
            qDebug()<<"can not find ";
            //找Content-Length
            U8 chLength[] = "Content-Length: "; //返回头部位置
            auto lengthPos = m_data.streamFind(chLength, 16,true,beginPos);//从0xff0xd8开始找
            if(0xffffffff == lengthPos)
            { //找不到数据，说明应该是数据截断
                qDebug()<<"tcp cut off";
                findagain=false;
                break; //直接跳出循环，等待数据插入
            }
            else
            {
                //找到说明数据无效(此时的图像应该就只有半帧),指针和标志移动到新找的conten--位置处
                m_data.m_curDataHeadPos=lengthPos;
                m_data.m_pDataHead=m_data.m_pMemoryHead+lengthPos;
                findagain=true;
                continue;
            }
        }
        else
        {
            //找到判断数据长度(返回的数据大小有时不会刚好等于相减的大小，在fffd添加000000)
            auto imagesize=endpos-beginPos;
            if(imagesize<=jpegLen)
            { //读取发出
                qDebug()<<"image size "<<jpegLen;
                auto func2 = std::bind(&QCameraClient::getJPEGData, \
                                       this,\
                                       std::placeholders::_1,\
                                       std::placeholders::_2,\
                                       std::placeholders::_3,\
                                       std::placeholders::_4,\
                                       std::placeholders::_5);
                DataFrame jpegData;
                jpegData.size=m_data.streamOut(beginPos,endpos,&jpegData.buff, jpegLen, func2);
                emit signalRecvOneFrame(jpegData);
                //判断是否处理完数据
                U8 chLength[] = "Content-Length: ";
                auto  newlen= m_data.streamFind(chLength, 16,true,endpos); //从刚发出的图像尾部找
                if(0xffffffff == newlen)
                {
                  qDebug()<<"deal no data  ";
                  m_data.clearStream(); //如果刚好在con就截断呢，清理还要考虑下
                  findagain=false;
                  break; //直接跳出循环
                }else
                    {
                    qDebug()<<"have data ";
                    findagain=true;
                    continue;
                    }
            }
            else
            {
                //有无效数据
                //找Content-Length：找到说明前边的无效(此时找到的是下一张图片的0xff0xd9)
                //找(从当前0xff和0xd8开始)conten——length位置和 0xff0xd9位置比较
                //d9的位置大于conten——lenth说明无效
                qDebug()<<"tuxiangdaxiao  "<<imagesize;
                U8 chLength[] = "Content-Length: ";
                auto lengthPos = m_data.streamFind(chLength, 16,true,beginPos);//从0xff0xd8开始找
                if(0xffffffff == lengthPos)
                {
                    qDebug()<<"mistake all";
                    m_data.clearStream();
                    findagain=false;
                    break;

                }

                if(endpos>lengthPos)
                {  //无效数据(找到的是下一张的ff d9)
                    qDebug()<<"invalild data ";
                    m_data.m_curDataHeadPos=lengthPos ;//移动(位置移动到新的conten)
                    m_data.m_pDataHead=m_data.m_pMemoryHead+lengthPos;
                    findagain=true;
                    continue;
                }else{//说明没计算对结果 //??
                     qDebug()<<"mistake all2";
                     findagain=false;
                     break;
                     }
              }
        }
    }while(1);
}

void QCameraClient::startNetWorkCamera()
{
    //连接网络摄像头
    if(nullptr == m_pTcpSocket) {
        //socket == null,创建socket
        m_pTcpSocket = new QTcpSocket;

        //创建singal && slot
        connect(m_pTcpSocket, &QTcpSocket::connected, this, &QCameraClient::slotConnected);
        connect(m_pTcpSocket, &QTcpSocket::disconnected, this, &QCameraClient::slotDisconnected);
        connect(m_pTcpSocket, &QTcpSocket::readyRead, this, &QCameraClient::slotReadData);
        connect(m_pTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotDisplayError(QAbstractSocket::SocketError)));
    }

    m_pTcpSocket->abort();
    m_pTcpSocket->connectToHost(m_ip, static_cast<unsigned short>(m_port));
    m_pTcpSocket->waitForConnected(1000);
    qDebug(" qnetwork State2:%d\n",m_pTcpSocket->state());//状态3对2不对

    //自动连接标识开启
    m_autoConnect = true;
}

unsigned int QCameraClient::getContentLength(unsigned int pos, \
                                             const unsigned char *src, unsigned int slen, \
                                             unsigned char **out, unsigned int outlen)
{
    if(nullptr == src)
    {
        return 0;
    }

    outlen = 0;
    //下一个字符读取
    auto sp = pos + 1;
    while(1)
    {
        if(sp + outlen >= slen)
        {
            //图像数据长度的信息后面肯定会有大量数据，出现这种情况说明数据不完整，放弃读取
            return 0;
        }

        if(isdigit(src[sp + outlen]))
        {
            outlen++;
        }
        else
        {
            break;
        }
    }

    if(outlen > 0)
    {
        *out = new unsigned char[outlen];
        memcpy_s(*out, outlen, src + sp, outlen);
    }

    return outlen;
}

unsigned int QCameraClient::getJPEGData(unsigned int pos,
                                        const unsigned char *src, unsigned int slen, \
                                        unsigned char **out, unsigned int outlen)
{     //0xff,0xd8位置
    if(pos >= slen || outlen <= 4 || nullptr == src)
    {  //数据出错
        return 0;
    }
    //拷贝数据
    *out = new unsigned char[outlen];
    memcpy_s(*out, outlen, src + pos, outlen);
    return outlen;
}
# if 0
//unsigned int QCameraClient::getJPEGData(unsigned int pos, \
//                                        const unsigned char *src, unsigned int slen, \
//                                        unsigned char **out, unsigned int outlen)
//{     //0xff,0xd8位置
//    if(pos >= slen || pos + outlen > slen || outlen <= 4 || nullptr == src) //？？？目前问题在这
//    {
//        //图像数据不完整
//        return 0;
//    }
//    //检测最后标记是否为0xff 0xd9
//    if(src[pos + outlen - 1] != 0xD9
//            || src[pos + outlen - 2] != 0xFF)
//    {
//        //图像数据错误，应该丢弃这块数据
//        return outlen;
//    }
//    //拷贝数据
//    *out = new unsigned char[outlen];
//    memcpy_s(*out, outlen, src + pos, outlen);
//    return outlen;
//}
#endif
extern "C" Q_DECL_EXPORT ICameraModule *getInstance(void *pReserve)
{
    Q_UNUSED(pReserve);

    static QCameraWorker* camera;
    camera = QCameraWorker::getInstance();
    return camera;
}
