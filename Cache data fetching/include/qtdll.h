#ifndef QTDLL_H
#define QTDLL_H

//#include "qtdll_global.h"
#include "IEngine.h"
#include "dlldefine.h"

//using namespace std;
#define DLL_API __declspec(dllexport)
//class QTDLLSHARED_EXPORT Qtdll

class DLL_API Qtdll
{

public:


    Qtdll();

   int add(int a,int b);
   /*!
   * @brief 用户登录
   * @param username， 用户名
   * @param username， 用户密码
   * @return 成功返回true，失败返回false
   */

   bool login(std::string username, std::string password);
   /*!
   * @brief 初始化算法引擎
   * @param pReserve 预留参数，可置为nullptr
   * @return 成功返回0，失败返回错误码
   */
   unsigned int init(void *pReserve);


   /*!
   * @brief 设置处理的图像
   * @param buf 图像数据缓存地址
   * @param len 图像数据长度
   * @param width 图像宽度
   * @param height 图像高
   * @param imageType 图像格式
   * @return 成功返回0，失败返回错误码
   */
   unsigned int setImageData(const unsigned char* buf, unsigned int len, unsigned int width, unsigned int height, ImageType imageType) ;

   /*!
   * @brief 设置指定编号的物体名称
   * @param number 指定物体编号
   * @param name 指定物体名称，字符编码格式为utf-8
   * @return 成功返回0，失败返回错误码
   */
   unsigned int setObjectName(const unsigned int number, const std::string name) ;


   /*!
   * @brief 设置指定区域所属块的物体名称
   * @param rect 指定物体所在区域
   * @param name 指定物体名称，字符编码格式为utf-8
   * @return 成功返回0，失败返回错误码
   */
   unsigned int setObjectName1(const c_Rect rect, const std::string name) ;

   /*!
   * @brief 设置指定区域所属块的物体名称
   * @param rect 指定物体所在区域
   * @param name 指定物体名称，字符编码格式为utf-8
   * @return 成功返回0，失败返回错误码
   */
   unsigned int setObjectName2(const c_Point position, const std::string name) ;

   /*!
   * @brief 获取指定编号的物体名称
   * @param [IN OUT]name 物体名称，字符编码格式为utf-8
   * @return 成功返回0，失败返回错误码
   */
   unsigned int getObjectName(const unsigned int number, std::string &name) ;

   /*!
   * @brief 获取本桢的物体个数
   * @param cnt 物体对象个数
   * @return 成功返回0，失败返回错误码
   */
   unsigned int getObjectCount(unsigned int &cnt) ;

   /*!
   * @brief 获取本桢处理后的物体列表
   * @param [IN OUT]data 图像识别结果列表
   * @return 成功返回0，失败返回错误码
   */
   unsigned int getRecognizeObjects(std::vector<c_ObjectData> &data) ;

   /*!
   * @brief 获取列表中指定索引的物体对象
   * @param index 索引号
   * @param [IN OUT]data 得到的识别对象
   * @return 成功返回0，失败返回错误码
   */
   unsigned int getRecognizeObject(const unsigned char index, c_ObjectData &data) ;
   c_ObjectData getRecognizeObject1(unsigned int index) ;
   /*!
   * @brief 获取指定编号的物体对象
   * @param number 物体编号
   * @param [IN OUT]data 得到的识别对象
   * @return 成功返回0，失败返回错误码
   */
  unsigned int getRecognizeObjectFromNumber(const unsigned int number, c_ObjectData &data) ;

   /*!
   * @brief 获取指定指定索引的物体中心点坐标
   * @param index 索引
   * @return 坐标位置
   */
   c_Point getCenterPoint(const unsigned int index) ;

   /*!
   * @brief 获取指定指定索引的物体矩形区域
   * @param index 索引
   * @return 矩形位置
   */
  c_Rect getObjectRect(const unsigned char index) ;

   /*!
   * @brief 查询数据中是否存在指定字符串名称
   * @param utf-8编码的字符串
   * @return 成功为true，不存在为false
   */
   bool isKnowObject(const std::string name);

   /*!
   * @brief 查询图片中是否存在名为name的物体
   * @param name utf-8编码的字符串
   * @return 成功为true，不存在为false
   */
   bool objectExist(const std::string name);

   /*!
   * @brief 获取指定名称的物体数据
   * @param name utf-8编码的字符串
   * @param [IN OUT]data 得到的对象列表
   * @return 成功返回0，失败返回错误码
   */
   //extern "C" DLL_API
   unsigned int getObjectFromName(const std::string name, std::vector<c_ObjectData> &data) ;
};


extern "C" DLL_API Qtdll* getQtll(); //获取类QtDll的对象



#endif // QTDLL_H
