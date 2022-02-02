#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include "loginpage.h"
#include<unordered_set>
struct StuInfo;
struct UserInfo;
class stuSql;
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    virtual void keyPressEvent(QKeyEvent *event)override;

    void mousePressEvent(QMouseEvent *event)override;//鼠标点击
    void mouseMoveEvent(QMouseEvent *event)override;//鼠标移动
    void mouseReleaseEvent(QMouseEvent *event)override;//鼠标释放
    void update_state();
    void clearToStu();
    void clearToUser();
    void flush_addBuf();   //把所有加入的缓冲区队列刷新到数据库
    void flush_removeBuf(); //刷新所有删除操作的缓冲区队列  //分开的原因在于，执行删除操作时，应该先执行加入操作才算得上正常
private slots:
    void on_pushButton_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_9_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_imp_btn_clicked();

private:
    int cur_state;  //用于表示当前所选中的是学生还是用户栏目
    int max_buf_size;//缓冲区最大容量
    //    static solve
    Ui::MainWindow *ui;
    LoginPage loginPage;
    QPoint m_point;
    stuSql* qry;
    QList<StuInfo> stu_add;
    QList<StuInfo> stu_remove;
    QList<StuInfo> m_stu_datas;
    QList<UserInfo> user_add;
    QList<UserInfo> user_remove;
    QList<UserInfo> m_users_datas;
};
#endif // MAINWINDOW_H
