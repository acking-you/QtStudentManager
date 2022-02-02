#ifndef STUSQL_H
#define STUSQL_H

#include <QObject>
#include<QSqlDatabase>


enum struct UserAuth:uint{low,mid,high};//用户权限枚举

struct StuInfo{
    int id;     //方便根据id分页查找
    QString name;
    uint8_t age;
    uint16_t grade;
    uint32_t uiclass;
    uint32_t stuid;
    QString phone;
    QString wechat;
};

struct UserInfo{//由于数量不多，就不分页了
    QString username;
    QString password;
    UserAuth aut;
};


//同样sql查询语句基本就只需要一个对象实例所以使用单例更好，当外部调用查询时，均只需要这一个实例对象
class stuSql : public QObject
{
    Q_OBJECT

public:
    static stuSql* getInstance();
    explicit stuSql(QObject *parent = nullptr);
    //查询学生数量
    uint32_t getStuCnt();
    //查询第几页的学生数据（学生可能有很多，所以需要分页）
    QList<StuInfo> getPageStu(uint32_t page,uint32_t uiCnt);
    //增加学生
    bool addStudent(const StuInfo&);
    //删除学生
    bool delStu(int);
    //清空学生表
    bool clearStu();
    //修改学生信息
    bool updateStu(const StuInfo&);
    //查找所有用户
    QList<UserInfo> getAllUser();
    //查找用户名是否存在
    bool isExist(const QString& name,const QString& password);
    //更新用户信息
    bool updateUsr(const UserInfo& usr);
    //添加单个用户
    bool AddUser(const UserInfo& info);
    //删除单个用户
    bool delUser(const QString &UserName);

    //用一次性事务把大量的数据插入到数据库，防止单次命令每次都要执行事务过程
    bool AddAllUser(const QList<UserInfo>& users);
    bool AddAllStu(const QList<StuInfo>&stus);
    bool RemoveAllUser(const QList<UserInfo>&users);
    bool RemoveAllStu(const QList<StuInfo>&stus);
private:
    QSqlDatabase db;
    static stuSql* qurey;
};

#endif // STUSQL_H
