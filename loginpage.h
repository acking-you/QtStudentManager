#ifndef LOGINPAGE_H
#define LOGINPAGE_H

#include <QWidget>

namespace Ui {
class LoginPage;
}

class LoginPage : public QWidget
{
    Q_OBJECT

public:
    explicit LoginPage(QWidget *parent = nullptr);
    ~LoginPage();

    virtual void keyPressEvent(QKeyEvent *event)override;

    void mousePressEvent(QMouseEvent *event)override;//鼠标点击
    void mouseMoveEvent(QMouseEvent *event)override;//鼠标移动
    void mouseReleaseEvent(QMouseEvent *event)override;//鼠标释放

    void showError();
private slots:

    void on_loadBtn_clicked();

    void on_exitBtn_clicked();

signals:
    void sendLoginSuccess();
private:
    Ui::LoginPage *ui;
    QPoint m_point;
};

#endif // LOGINPAGE_H
