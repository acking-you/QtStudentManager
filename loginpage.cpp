#include "loginpage.h"
#include "ui_loginpage.h"
#include"stusql.h"
#include <QMouseEvent>

LoginPage::LoginPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginPage)
{
    ui->setupUi(this);
    ui->pass_error->hide();
}


LoginPage::~LoginPage()
{
    delete ui;
}

//登录的时候可以直接按下enter键代替登录按钮
void LoginPage::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_Return){
        on_loadBtn_clicked();
    }
}

void LoginPage::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_point = event->pos();
        //鼠标位置减去左上角的左边
        //可替换为m_point = event->pos();
    }
}
void LoginPage::mouseMoveEvent(QMouseEvent *event)
{
    move(event->pos() - m_point + pos());
    //鼠标位置减去按下点的坐标
    //可替换为event->pos() - m_point + pos()；
}
void LoginPage::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    //Q_UNUSED() 没有实质性的作用,用来避免编译器警告
}

void LoginPage::showError()
{
    ui->pass_error->setVisible(true);
}

void LoginPage::on_loadBtn_clicked()
{
    //失败就给提示
    if(ui->loginUsername->text().isEmpty()||ui->loginPassword->text().isEmpty()){
        showError();
        return;
    }
    //数据库查询用户名和密码
    stuSql* qry = stuSql::getInstance();
    if(!qry->isExist(ui->loginUsername->text(),ui->loginPassword->text())){
        showError();
        return;
    }
    //成功后发送信号进入主界面
    else emit sendLoginSuccess();
    //把密码账号框的数据重新清除
    ui->loginUsername->clear();
    ui->loginPassword->clear();
    ui->pass_error->setVisible(false);
}


void LoginPage::on_exitBtn_clicked()
{
    QApplication::exit(0);
}

