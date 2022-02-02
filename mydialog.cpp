#include "mydialog.h"
#include "./ui_mydialog.h"

myDialog* myDialog::m_dialog = nullptr;

myDialog *myDialog::getInstance()
{
    if(nullptr==m_dialog)
        m_dialog = new myDialog();
    return m_dialog;
}

myDialog::myDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::myDialog)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/提示说明.png"));
    this->setWindowTitle("请三思");
    state = 0;//默认为取消的状态
    ui->input_edit->hide();//默认输入框不显示，输入框仅用于增删改查按钮记录需要改的行
}

myDialog::~myDialog()
{
    delete ui;
}

void myDialog::setTitleTip(const QString &str)
{
    this->ui->label->setText(str);
}

void myDialog::setTip(const QString &str)
{
    this->ui->lineEdit->setText(str);
}

int myDialog::getCurState()
{
    return state;
}

QString myDialog::getText()
{
    return ui->input_edit->text();
}

void myDialog::on_bt_ok_clicked()
{
    this->accept();
    state = 1;
}


void myDialog::on_bt_no_clicked()
{
    this->reject();
    state = 0;
}

myDialog *myDialog::dialog() const
{
    return m_dialog;
}

void myDialog::setDialog(myDialog *newDialog)
{
    if (m_dialog == newDialog)
        return;
    m_dialog = newDialog;
    emit dialogChanged();
}

void myDialog::setOkVisibal(bool state)
{
    if(!state){
        ui->bt_ok->hide();
    }else{
        ui->bt_ok->setVisible(true);
    }
}

void myDialog::setNoVisibal(bool state)
{
    if(!state){
        ui->bt_no->hide();
    }else{
        ui->bt_no->setVisible(true);
    }
}

void myDialog::setEditVisibal(bool f)
{
    if(!f){
        ui->input_edit->hide();
    }else{
        ui->input_edit->setVisible(true);
    }
}

