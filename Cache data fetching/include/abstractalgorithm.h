#ifndef ABSTRACTALGORITHM_H
#define ABSTRACTALGORITHM_H


#define ABSTRACT_NULL "NULL"
struct uchar_size{
    char * buff;
    int size;
    int Height;
    int conZSJSQFZ;
};
class AbstractAlgorithm
{
public:
    virtual bool get_openDB() =0;
    virtual QImage * LearnAPicAuto(const U8 *buff, const U32 size,bool Cover = true, int ChangeNo = -1, unsigned long ChangeTextID = 0) =0;
    virtual unsigned int Get_AllZSMNGA_Count()=0;
    virtual unsigned long Fun_AllZSMNGA_AddAdata(int ZSSource,unsigned long SourceID,unsigned long SourceID2,unsigned long TextID)=0;
    virtual int Get_AreaResult_Count()=0;
    virtual int* Get_AreaResult_R(int i)=0;
    virtual int Get_AreaResult_XCenter(int i)=0;
    virtual int Get_AreaResult_YCenter(int i)=0;
    virtual int Get_AreaResult_MaxR(int i)=0;
    virtual int Get_AreaResult_MainAngle(int i)=0;
    virtual int Get_AreaResult_PointCount(int i)=0;
    virtual int Get_AreaResult_Color(int i)=0;
    virtual char Get_AreaResult_MoveDir(int i)=0;
    virtual char Get_AreaResult_ShakeCounter(int i)=0;
    virtual unsigned long Get_AreaResult_BXID(int i)=0;
    virtual unsigned long Get_AreaResult_ZSID(int i)=0;
    virtual unsigned long Get_AreaResult_ColorID(int i)=0;
    virtual unsigned long Get_AreaResult_ComPoundID(int i)=0;
    virtual int Get_AreaResult_ZSJSQ(int i)=0;
    virtual unsigned long Get_AreaResult_TextID(int i)=0;
    virtual unsigned long Get_AreaResult_OldTextID(int i)=0;
    virtual void Set_AreaResult_OldTextID(int i,unsigned long Value)=0;
    virtual int Get_AreaResult_No(int i)=0;
    virtual bool Get_AreaResult_bMoveing(int i)=0;
    virtual bool Get_AreaResult_bNewZS(int i)=0;
    virtual bool Get_AreaResult_OutSize(int i)=0;
    virtual QRect Get_AreaResult_Rect(int i)=0;
    virtual bool Get_AreaResult_bStable(int i)=0;
    virtual bool Get_AreaResult_bNeedShow(int i)=0;
    virtual int Get_AreaResult_HSV_H(int i)=0;
    virtual int Get_AreaResult_HSV_S(int i)=0;
    virtual int Get_AreaResult_HSV_V(int i)=0;
    virtual int Get_conZSJSQFZ()=0;
    virtual int Get_constMinLearnPoint()=0;


    virtual int query_struMyObjectInf(int ID,int No)=0;
    virtual unsigned long AllZSMNGA_AddAdata(int ZSSource,unsigned long SourceID,unsigned long SourceID2,unsigned long TextID)=0;
    virtual bool SetText(int ObjectNo,QString Name) =0;   //设置物品名称
    virtual QString GetText(int ObjectNo)=0;    //获取物品名称
    virtual int GetTextID(int ObjectNo)=0;      //获取物品名称ID
    virtual void SetPath(QString DBPathA)=0;    //设置二进制文件存放路径
    virtual void SaveDB()=0;    //保存二进制文件
    virtual bool SetServer(QString strConnect)=0;   //设置服务器数据库连接信息
    virtual void SetTopNotLearnAreaBL( float a )=0;
    virtual bool Set_bLearning(bool bLearning)=0;   //设置模块中bLearning的值
    virtual bool Get_bLearning()=0;    //获取模块中bLearning的值
    virtual int GetRecordInfi(QString bstrSQL,QString sFieldName)=0;
    virtual QString GetRecordInf(QString Sql,QString sFieldName)=0;
    virtual bool openDB(const QString strSvrName,
                           const QString strDBname,
                           const QString strUserID,
                           const QString strUserPwd)=0;    //连接数据库
    virtual bool ExecSQL(QString bstrSQL) =0;
//    virtual const CAreaAnlays GetAreaAnlaysA(QString)=0;    //获取块信息
//    virtual bool LearnAPicAuto(CMyImageMHInfo *SourcePic, bool Cover, int ChangeNo, unsigned long ChangeTextID)=0;   //处理帧
    virtual ~AbstractAlgorithm(){}
};

#endif // ABSTRACTALGORITHM_H
