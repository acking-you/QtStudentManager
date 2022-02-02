#include "stusql.h"
#include<QMessageBox>
#include <QSqlQuery>
#include<QCoreApplication>
#include<QDebug>

stuSql* stuSql::qurey = nullptr;
stuSql *stuSql::getInstance()
{
    if(qurey==nullptr){
        qurey = new stuSql(nullptr);
    }
    return qurey;
}

stuSql::stuSql(QObject *parent)
    : QObject{parent}
{   //初始化数据库连接
    if(QSqlDatabase::drivers().isEmpty()){
        QMessageBox::warning(nullptr,"没有发现数据库驱动","执行此程序的功能需要数据库");
        return;
    }
    db = QSqlDatabase::addDatabase("QSQLITE");//得到对应数据库类型的连接媒介
#if 0
    auto path = QCoreApplication::applicationDirPath()+R"(/data.db)";//打包发布程序的时候需要拼接绝对路径
#endif
    db.setDatabaseName(R"(C:\Users\Alone\Desktop\小demo的数据库\date.db)");//数据库的所在位置
    if(!db.open()){
        QMessageBox::warning(nullptr,"open error","数据库启动失败");
    }
}

uint32_t stuSql::getStuCnt()
{
    QSqlQuery qry(db);
    qry.exec("select count(id) from student;");
    int cnt = 0;
    while(qry.next()){
        cnt = qry.value(0).toInt();
    }
    return cnt;
}

QList<StuInfo> stuSql::getPageStu(uint32_t page, uint32_t uiCnt)
{
    QList<StuInfo>l;
    QSqlQuery qry(db);
    qry.exec(QString("select * from student order by id limit %1 offset %2")    //此处sql语句offset表示偏移量，也就是查询数据得到的起始位置，所以直接让它偏移uiCnt*page
             .arg(uiCnt).arg(uiCnt*page)
             );
    StuInfo info;
    while(qry.next()){
        info.id = qry.value(0).toUInt();
        info.name = qry.value(1).toString();
        info.age = qry.value(2).toUInt();
        info.grade = qry.value(3).toUInt();
        info.uiclass = qry.value(4).toUInt();
        info.stuid = qry.value(5).toUInt();
        info.phone = qry.value(6).toString();
        info.wechat = qry.value(7).toString();
        l.append(info);
    }

    return l;
}

bool stuSql::addStudent(const StuInfo &info)
{
    QSqlQuery qurey(db);
    QString strSql = QString("INSERT INTO student VALUES (NULL,'%1',%2,%3,%4,%5,'%6','%7')")
            .arg(info.name).arg(info.age).arg(info.grade).arg(info.uiclass)
            .arg(info.id).arg(info.phone).arg(info.wechat);
    return qurey.exec(strSql);
}

bool stuSql::delStu(int id)
{
    QSqlQuery qry(db);
    return qry.exec(QString("delete from student where id = %1").arg(id));
}

bool stuSql::clearStu()
{
    QSqlQuery qry(db);
    return qry.exec("delete from student");
}

bool stuSql::updateStu(const StuInfo &info)
{
    QSqlQuery qurey(db);
    QString strSql = QString("update student set name='%1',age=%2,grade = %3,class=%4,studentid=%5,phone='%6',wechat='%7' where id=%8")
            .arg(info.name).arg(info.age).arg(info.grade).arg(info.uiclass)
            .arg(info.id).arg(info.phone).arg(info.wechat).arg(info.id);
    return qurey.exec(strSql);
}

QList<UserInfo> stuSql::getAllUser()
{
    QSqlQuery qry(db);
    qry.exec("select * from username");
    QList<UserInfo> l;
    UserInfo usr;
    while(qry.next()){
        usr.username = qry.value(0).toString();
        usr.password = qry.value(1).toString();
        usr.aut = UserAuth(qry.value(2).toUInt());
        l.append(usr);
    }
    return l;
}

bool stuSql::isExist(const QString &name,const QString& password)
{
    QSqlQuery qry(db);
    qry.exec(QString("select * from username where username='%1' and password='%2'").arg(name).arg(password));
    return qry.next();
}

bool stuSql::updateUsr(const UserInfo& usr)
{
    QSqlQuery qry(db);
    return qry.exec(QString("update username set password='%1',auth=%2 where username='%3'")
                    .arg(usr.password).arg(uint(usr.aut)).arg(usr.username));
}

bool stuSql::AddUser(const UserInfo &info)
{
    if(isExist(info.username,info.password)) //如果已经存在，则不插入并提示信息
    {
        QMessageBox::warning(nullptr,"error:用户已经存在","用户已存在，请重新输入！");
        return false;
    }
    QSqlQuery qurey(db);
    //否则直接插入
    return qurey.exec(QString("INSERT INTO username(username,password,auth) VALUES ('%1','%2',%3)")
                      .arg(info.username).arg(info.password).arg(int(info.aut)));
}

bool stuSql::delUser(const QString &UserName)
{
    QSqlQuery qry(db);
    return qry.exec(QString("delete from username where username='%1'").arg(UserName));
}

bool stuSql::AddAllUser(const QList<UserInfo> &users)
{
    QSqlQuery qry(db);
    db.transaction();//执行事务
    for(auto& info:users){
        qry.exec(QString("INSERT INTO username(username,password,auth) VALUES ('%1','%2',%3)")
                 .arg(info.username).arg(info.password).arg(int(info.aut)));
    }
    db.commit();//提交事务
    return true;
}

bool stuSql::AddAllStu(const QList<StuInfo> &stus)
{
    QSqlQuery qry(db);
    db.transaction();//执行事务
    for(auto& info:stus){
        qry.exec(QString("INSERT INTO student VALUES (NULL,'%1',%2,%3,%4,%5,'%6','%7')")
                 .arg(info.name).arg(info.age).arg(info.grade).arg(info.uiclass)
                 .arg(info.id).arg(info.phone).arg(info.wechat));
    }
    db.commit();//提交事务
    return true;
}

bool stuSql::RemoveAllUser(const QList<UserInfo> &users)
{
    QSqlQuery qry(db);
    db.transaction();//执行事务
    for(auto& info:users){
        qry.exec(QString("delete from username where username='%1'")
                 .arg(info.username));
    }
    db.commit();//提交事务
    return true;
}

bool stuSql::RemoveAllStu(const QList<StuInfo> &stus)
{
    QSqlQuery qry(db);
    db.transaction();//执行事务
    for(auto& info:stus){
        qry.exec(QString("delete from student where id=%1")
                 .arg(info.id));
    }
    db.commit();//提交事务
    return true;
}


// q.exec("INSERT INTO student VALUES (NULL,'张三',12,3,2,1,2323233,'sfsddf')");
