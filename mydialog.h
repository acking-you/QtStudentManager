#ifndef MYDIALOG_H
#define MYDIALOG_H

#include <QDialog>
//TODO 状态0代表选中取消按钮，状态1代表选中确认按钮
namespace Ui {
class myDialog;
}

class myDialog : public QDialog
{
    Q_OBJECT

public:
    //使用简单的单例，如果不用单例，那么每次使用这个对话框都得静态申请内存又释放，比较耗资源
    static myDialog *getInstance();
    explicit myDialog(QWidget *parent = nullptr);
    ~myDialog();
    void setTitleTip(const QString& str);
    void setTip(const QString& str);
    int getCurState();
    QString getText();
    myDialog *dialog() const;
    void setDialog(myDialog *newDialog);
    void setOkVisibal(bool state);
    void setNoVisibal(bool state);
    void setEditVisibal(bool f);
signals:
    void dialogChanged();

private slots:
    void on_bt_ok_clicked();

    void on_bt_no_clicked();

private:
    int state;
    static myDialog *m_dialog;
    Ui::myDialog *ui;
    Q_PROPERTY(myDialog *dialog READ dialog WRITE setDialog NOTIFY dialogChanged)
};



#endif // MYDIALOG_H
