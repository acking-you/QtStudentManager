#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QKeyEvent>
#include <QList>
#include <QStringList>
#include<QTreeWidgetItem>
#include <filesystem>
#include <QFile>
#include <QApplication>
#include <QThread>
#include<functional>
#include"mydialog.h"
#include"stusql.h"
#include <QAxObject>
#include<QFileDialog>
#include <QFileInfo>
#include<QStandardPaths>
//TODO 槽函数和槽连接函数的机制：singnal或者槽函数都是普通的成员函数，而槽连接函数则是将两个对象的地址关联起来。
// 达到你中有我，我中有你的情形(内部都存下了对方的地址)，一旦信号emit出去，
// 这个emit操作就会从已经记录的connect对象中寻找和该信号参数一致的函数然后再调用。
// 这整个过程不过就是指针来回的调用罢了。
// 所以connect函数只不过是一个用于连接的过程而不是一个被调用的工具，
// 故只要对象没被析构，connect后，都会根据你发送的信号来调用对应的函数。所以connect位置一般放在正好创建完对象的时候。

//TODO 简单封装一个用于多线程通信的类
class run_thread:public QThread{
public:
    run_thread() = delete;
    run_thread(const std::function<void()>&Runnable,QObject* parent = nullptr):m_task(nullptr){
        m_task = Runnable;
    }
    void run()override{
        if(m_task!=nullptr);
        m_task();
    }
private:
    std::function<void()>m_task;
};


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),cur_state(-1),max_buf_size(512)
{
    ui->setupUi(this);
    //隐藏顶部导航栏，用于自定义导航栏
    this->setWindowFlags ( Qt ::FramelessWindowHint);
    this->loginPage.setWindowFlags ( Qt ::FramelessWindowHint);
    loginPage.show();
    connect(&loginPage,&LoginPage::sendLoginSuccess,this,[&](){this->show();loginPage.setVisible(false);});
    ui->treeWidget->setColumnCount(1);//设置顶级节点只有一个
    QStringList list;//用于给每个节点项添加内容
    //QList<QTreeWidgetItem*> topItems;//可以直接添加里面一次性调用拿走

    list<<"学生信息管理系统";
    auto topLeaves =  new QTreeWidgetItem(ui->treeWidget,list);
    ui->treeWidget->addTopLevelItem(topLeaves);

    list.clear();list<<"学生管理";
    topLeaves->addChild(new QTreeWidgetItem(topLeaves,list));
    list.clear();list<<"管理员管理";
    topLeaves->addChild(new QTreeWidgetItem(topLeaves,list));
    ui->treeWidget->expandAll();

    //将qt的qss样式设置上去
    QFile f;
    auto path = QApplication::applicationDirPath()+R"(/stuqss.css)";
    f.setFileName(path);
    f.open(QIODevice::ReadOnly);
    auto qss = f.readAll();
    this->setStyleSheet(qss);
    this->loginPage.setStyleSheet(qss);
    f.close();

    //TODO 我被这个地方坑到了，原来Qt里面写了Q_Objiect宏后，你即便是指针也会被内存管理！！！之前在这定义的指针局部变量然后被释放了。。导致内存问题！！！
    qry = stuSql::getInstance();
    //TODO 开一个线程用于将数据库的所有数据先载入内存，接下来的各种数据查询啥的就都是和内存打交道
    QThread* sub_thread1 = new run_thread([&](){
        int cnt = qry->getStuCnt();
        m_stu_datas = std::move(qry->getPageStu(0,cnt));//一次性取完，因为目前学生不多
        m_users_datas = std::move(qry->getAllUser()); //管理用户肯定非常少，压根不需要留后路优化
    });
    sub_thread1->start();
}

//重写鼠标事件，以便重写顶部栏目
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_point = event->pos();
        //鼠标位置减去左上角的左边
        //可替换为m_point = event->pos();
    }
}
void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    move(event->pos() - m_point + pos());
    //鼠标位置减去按下点的坐标
    //可替换为event->pos() - m_point + pos()；
}
void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    //Q_UNUSED() 没有实质性的作用,用来避免编译器警告
}

void MainWindow::update_state()
{
    auto top = ui->treeWidget->topLevelItem(0);
    if(top->child(0)->isSelected()){
        cur_state = 1; //选中学生
    }
    else if(top->child(1)->isSelected()){
        cur_state = 2;//选中老师
    }else{
        cur_state = -1;
    }
}

void MainWindow::clearToStu()
{
    QList<QString>list;
    ui->tableWidget->setColumnCount(8);
    ui->tableWidget->setRowCount(m_stu_datas.size());
    //    ui->tableWidget->setSelectionMode(QAbstractItemView::NoSelection);//把table默认为不可选
    //    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows); //整行选中的方式
    list<<"id"<<"名字"<<"年龄"<<"年级"<<"班级"<<"学号"<<"手机"<<"微信";
    ui->tableWidget->setHorizontalHeaderLabels(list);
    ui->tableWidget->clearContents();
}

void MainWindow::clearToUser()
{
    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->setRowCount(m_users_datas.size());
    //    ui->tableWidget->setSelectionMode(QAbstractItemView::NoSelection);//把table默认为不可选
    //    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows); //整行选中的方式
    QList<QString>list;
    list<<"用户名"<<"密码"<<"权限等级";
    ui->tableWidget->setHorizontalHeaderLabels(list);
    ui->tableWidget->clearContents();
}

void MainWindow::flush_addBuf()
{
    qry->AddAllStu(stu_add);
    qry->AddAllUser(user_add);
    stu_add.clear();
    user_add.clear();
}

void MainWindow::flush_removeBuf()
{
    qry->RemoveAllStu(stu_remove);
    qry->RemoveAllUser(user_remove);
    stu_remove.clear();
    user_remove.clear();
}

//测试了下，析构函数nmd竟然在程序退出后不执行？？？所以我还得自己写个缓冲区进行add...
//好家伙原因是直接调用的原始操作系统的exit退出程序，导致只会调用静态区的对象的析构函数！！！
MainWindow::~MainWindow()
{
    flush_addBuf();
    flush_removeBuf();
    delete ui;
}


//重写keyPressEvent便于调试事件
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_F6){
        QFile f;
        auto path = QApplication::applicationDirPath()+R"(/stuqss.css)";
        f.setFileName(path);
        f.open(QIODevice::ReadOnly);
        auto qss = f.readAll();
        this->setStyleSheet(qss);
        this->loginPage.setStyleSheet(qss);
        f.close();
    }
}


void MainWindow::on_pushButton_clicked()
{
    myDialog* tip = myDialog::getInstance();
    tip->setTip("确定退出账号？");
    tip->exec();
    if(tip->getCurState()){
        this->setVisible(false);
        flush_addBuf();
        flush_removeBuf();
        loginPage.setVisible(true);
    }
}


void MainWindow::on_pushButton_6_clicked()
{
    myDialog* tip = myDialog::getInstance();
    tip->setTip("您确定要退出整个学生管理系统吗？");
    tip->exec();
    if(tip->getCurState())
        QApplication::exit(0);
}


void MainWindow::on_pushButton_9_clicked()
{
    this->showMinimized();
}

//点击显示框，将显示你所处状态的所有信息（有学生和用户）
void MainWindow::on_pushButton_7_clicked()
{
    update_state();
    switch (cur_state) {
    case 1://todo 当前状态展示学生
        clearToStu();
        for(int i=0;i<m_stu_datas.size();i++){
            ui->tableWidget->setItem(i,0,new QTableWidgetItem(QString::number(m_stu_datas[i].id)));
            ui->tableWidget->setItem(i,1,new QTableWidgetItem(m_stu_datas[i].name));
            ui->tableWidget->setItem(i,2,new QTableWidgetItem(QString::number(m_stu_datas[i].age)));
            ui->tableWidget->setItem(i,3,new QTableWidgetItem(QString::number(m_stu_datas[i].grade)));
            ui->tableWidget->setItem(i,4,new QTableWidgetItem(QString::number(m_stu_datas[i].uiclass)));
            ui->tableWidget->setItem(i,5,new QTableWidgetItem(QString::number(m_stu_datas[i].stuid)));
            ui->tableWidget->setItem(i,6,new QTableWidgetItem(m_stu_datas[i].phone));
            ui->tableWidget->setItem(i,7,new QTableWidgetItem(m_stu_datas[i].wechat));
        }
        break;
    case 2://todo 当前状态展示用户
        clearToUser();
        for(int i=0;i<m_users_datas.size();i++){
            ui->tableWidget->setItem(i,0,new QTableWidgetItem(m_users_datas[i].username));
            ui->tableWidget->setItem(i,1,new QTableWidgetItem(m_users_datas[i].password));
            ui->tableWidget->setItem(i,2,new QTableWidgetItem(QString::number(int(m_users_datas[i].aut))));
        }
        break;
    default://否则弹出对话框提示未选中任何东西
        myDialog* dialog = myDialog::getInstance();
        dialog->setTip("请选中类型后再进行查看！");
        dialog->setNoVisibal(false);
        dialog->exec();
        dialog->setNoVisibal(true);
        break;
    }
}

//todo 增加操作，给他提示在第一行进行输入，我们已经为你清空到只剩下一行了，然后把按钮的文字换为“完成”，填完后再点击完成进行对应的增加操作
void MainWindow::on_pushButton_2_clicked()
{
    bool f = ui->pushButton_2->text()=="完成";//进行选择框状态的记录
    //接下来的操作和显示有些类似
    myDialog show;
    update_state();
    switch (cur_state) {
    case 1://todo 当前状态添加学生
        if(f){ //完成状态的处理，为了防止空指针！！！请对item是否存在进行判断
            StuInfo stu;
            if(ui->tableWidget->item(0,0))
                stu.id = ui->tableWidget->item(0,0)->text().toUInt();
            else   return;
            if(ui->tableWidget->item(0,1))
                stu.name = ui->tableWidget->item(0,1)->text();
            else return;
            if(ui->tableWidget->item(0,2))
                stu.age = ui->tableWidget->item(0,2)->text().toInt();
            else return;
            if(ui->tableWidget->item(0,3))
                stu.grade = ui->tableWidget->item(0,3)->text().toUInt();
            else return;
            if(ui->tableWidget->item(0,4))
                stu.uiclass =  ui->tableWidget->item(0,4)->text().toUInt();
            else return;
            if(ui->tableWidget->item(0,5))
                stu.stuid = ui->tableWidget->item(0,5)->text().toUInt();
            else return;
            if(ui->tableWidget->item(0,6))
                stu.phone = ui->tableWidget->item(0,6)->text();
            else return;
            if(ui->tableWidget->item(0,7))
                stu.wechat = ui->tableWidget->item(0,7)->text();
            else return;
            stu_add.append(stu);
            m_stu_datas.append(stu);
            ui->pushButton_2->setText("增加");
            show.setTip("增加成功！");
            show.setNoVisibal(false);
            show.exec();
            show.setNoVisibal(true);
        }
        else{//非完成状态的处理
            clearToStu();  //千万别放上面，我因为这个出了bug找半天都没找到！！！
            ui->pushButton_2->setText("完成");
            ui->tableWidget->setRowCount(1);
            show.setTip("请在下面的第一行中输入您要添加的信息！输入完后再点击完成即可");
            show.setNoVisibal(false);
            show.exec();
            show.setNoVisibal(true);
        }
        break;
    case 2://todo 当前状态添加用户
        if(f){ //完成状态的处理
            UserInfo usr;
            if(ui->tableWidget->item(0,0))
                usr.username = ui->tableWidget->item(0,0)->text();
            else return;
            if(ui->tableWidget->item(0,1))
                usr.password = ui->tableWidget->item(0,1)->text();
            else return;
            if(ui->tableWidget->item(0,2))
                usr.aut = UserAuth(ui->tableWidget->item(0,2)->text().toUInt());
            else return;
            user_add.append(usr);
            m_users_datas.append(usr);
            ui->pushButton_2->setText("增加");
            show.setTip("增加成功！");
            show.setNoVisibal(false);
            show.exec();
            show.setNoVisibal(true);
        }
        else{//非完成状态的处理
            clearToUser();
            ui->pushButton_2->setText("完成");
            ui->tableWidget->setRowCount(1);
            show.setTip("请在下面的第一行中输入您要添加的信息！输入完后再点击完成即可");
            show.setNoVisibal(false);
            show.exec();
            show.setNoVisibal(true);
        }
        break;
    default://否则弹出对话框提示未选中任何东西
        myDialog* dialog = myDialog::getInstance();
        dialog->setTip("请选中类型后再进行查看！");
        dialog->setNoVisibal(false);
        dialog->exec();
        dialog->setNoVisibal(true);
        break;
    }
    //缓冲区满了，直接刷新
    if(stu_add.size()>max_buf_size||user_add.size()>max_buf_size){
        flush_addBuf();
    }
}


void MainWindow::on_pushButton_3_clicked()
{
    myDialog* dialog = myDialog::getInstance();
    QString line ;
    update_state();
    switch (cur_state) {
    case 1://todo 当前状态处理学生
        dialog->setTip("请输入你要删除的行(中间空格可以输入第二个数代表删除的范围)");
        dialog->setEditVisibal(true);
        dialog->exec();
        dialog->setEditVisibal(false);
        if(!dialog->getCurState())return;
        line =  dialog->getText();
        if(!line.isEmpty()){
            std::stringstream ss;
            int l=0,ll=0;
            ss<<line.toStdString();
            ss>>l>>ll;
            if(ll==0&&l-1<m_stu_datas.size()){//只删除一行
                stu_remove.append(m_stu_datas[l-1]);//加入待到数据库删除的列表
                m_stu_datas.remove(l-1);

            }else if(ll!=0&&l!=0&&l-1<m_stu_datas.size()&&ll-1<m_stu_datas.size()){
                for(int i=l-1;i<=ll-1;i++){
                    stu_remove.append(m_stu_datas[i]);
                }
                m_stu_datas.erase(m_stu_datas.begin()+l-1,m_stu_datas.begin()+ll);
            }
            on_pushButton_7_clicked();//删除后重新显示
        }
        break;
    case 2://todo 当前状态处理用户
        dialog->setTip("请输入你要删除的行(中间空格可以输入第二个数代表删除的范围)");
        dialog->setEditVisibal(true);
        dialog->exec();
        dialog->setEditVisibal(false);
        if(!dialog->getCurState())return;
        line =  dialog->getText();
        if(!line.isEmpty()){
            std::stringstream ss;
            int l=0,ll=0;
            ss<<line.toStdString();
            ss>>l>>ll;
            if(ll==0&&l-1<m_users_datas.size()){
                user_remove.append(m_users_datas[l-1]);//加入待到数据库删除的列表
                m_users_datas.remove(l-1);
            }else if(ll!=0&&l!=0&&l-1<m_users_datas.size()&&ll-1<m_users_datas.size()){
                for(int i=l-1;i<=ll-1;i++){
                    user_remove.append(m_users_datas[i]);
                }
                m_users_datas.erase(m_users_datas.begin()+l-1,m_users_datas.begin()+ll);
            }
            on_pushButton_7_clicked();
        }
        break;
    default://否则弹出对话框提示未选中任何东西
        dialog->setTip("请选中类型后再进行查看！");
        dialog->setNoVisibal(false);
        dialog->exec();
        dialog->setNoVisibal(true);
        break;
    }

}


void MainWindow::on_pushButton_4_clicked()
{
    myDialog* dialog = myDialog::getInstance();
    QString line ;
    update_state();
    switch (cur_state) {
    case 1://todo 当前状态处理学生
        dialog->setTip("请先完成修改，再在下面填入已经修改的行");
        dialog->setEditVisibal(true);
        dialog->exec();
        dialog->setEditVisibal(false);
        if(!dialog->getCurState())return;
        line =  dialog->getText();
        if(!line.isEmpty()){
            int idx = line.toUInt();
            if(idx-1<0||idx-1>=m_stu_datas.size())return;
            StuInfo& stu = m_stu_datas[idx-1];
            if(ui->tableWidget->item(idx-1,0))
                stu.id = ui->tableWidget->item(idx-1,0)->text().toUInt();
            else   return;
            if(ui->tableWidget->item(idx-1,1))
                stu.name = ui->tableWidget->item(idx-1,1)->text();
            else return;
            if(ui->tableWidget->item(idx-1,2))
                stu.age = ui->tableWidget->item(idx-1,2)->text().toInt();
            else return;
            if(ui->tableWidget->item(idx-1,3))
                stu.grade = ui->tableWidget->item(idx-1,3)->text().toUInt();
            else return;
            if(ui->tableWidget->item(idx-1,4))
                stu.uiclass =  ui->tableWidget->item(idx-1,4)->text().toUInt();
            else return;
            if(ui->tableWidget->item(idx-1,5))
                stu.stuid = ui->tableWidget->item(idx-1,5)->text().toUInt();
            else return;
            if(ui->tableWidget->item(idx-1,6))
                stu.phone = ui->tableWidget->item(idx-1,6)->text();
            else return;
            if(ui->tableWidget->item(idx-1,7))
                stu.wechat = ui->tableWidget->item(idx-1,7)->text();
            else return;
            qry->updateStu(stu);    //更新数据库
            dialog->setTip("修改成功!");
            dialog->setNoVisibal(false);
            dialog->exec();
            dialog->setNoVisibal(true);
            on_pushButton_7_clicked();//删除后重新显示
        }
        break;
    case 2://todo 当前状态处理用户
        dialog->setTip("请先完成修改，再在下面填入已经修改的行");
        dialog->setEditVisibal(true);
        dialog->exec();
        dialog->setEditVisibal(false);
        if(!dialog->getCurState())return;
        line =  dialog->getText();
        if(!line.isEmpty()){
            int idx = line.toUInt();
            if(idx-1<0||idx-1>=m_users_datas.size())return;
            UserInfo& usr = m_users_datas[idx-1];
            if(ui->tableWidget->item(idx-1,0)){
                usr.username = ui->tableWidget->item(idx-1,0)->text();
            }else return;
            if(ui->tableWidget->item(idx-1,1)){
                usr.password = ui->tableWidget->item(idx-1,1)->text();
            }else return;
            if(ui->tableWidget->item(idx-1,2)){
                usr.aut = decltype (usr.aut)(ui->tableWidget->item(idx-1,2)->text().toUInt());
            }else return;
            qry->updateUsr(usr);    //更新数据库
            dialog->setTip("修改成功！");
            dialog->setNoVisibal(false);
            dialog->exec();
            dialog->setNoVisibal(true);
            on_pushButton_7_clicked();
        }
        break;
    default://否则弹出对话框提示未选中任何东西
        dialog->setTip("请选中类型后再进行查看！");
        dialog->setNoVisibal(false);
        dialog->exec();
        dialog->setNoVisibal(true);
        break;
    }
}


void MainWindow::on_pushButton_5_clicked()
{
    myDialog* dialog = myDialog::getInstance();
    auto txt = ui->lineEdit->text();
    update_state();
    int idx = 0;
    if(!txt.isEmpty()){
        switch(cur_state){
        case 1:
            for(auto& t:m_stu_datas){
                if(t.name==txt){
                    ui->tableWidget->setItem(idx,0,new QTableWidgetItem(QString::number(t.id)));
                    ui->tableWidget->setItem(idx,1,new QTableWidgetItem(t.name));
                    ui->tableWidget->setItem(idx,2,new QTableWidgetItem(QString::number(t.age)));
                    ui->tableWidget->setItem(idx,3,new QTableWidgetItem(QString::number(t.grade)));
                    ui->tableWidget->setItem(idx,4,new QTableWidgetItem(QString::number(t.uiclass)));
                    ui->tableWidget->setItem(idx,5,new QTableWidgetItem(QString::number(t.stuid)));
                    ui->tableWidget->setItem(idx,6,new QTableWidgetItem(t.phone));
                    ui->tableWidget->setItem(idx,7,new QTableWidgetItem(t.wechat));
                    idx++;
                }

            }
            ui->tableWidget->setRowCount(idx);
            break;

        case 2:
            for(auto& t:m_users_datas){
                if(t.username == txt){
                    ui->tableWidget->setItem(idx,0,new QTableWidgetItem(t.username));
                    ui->tableWidget->setItem(idx,1,new QTableWidgetItem(t.password));
                    ui->tableWidget->setItem(idx,2,new QTableWidgetItem(QString::number(int(t.aut))));
                    idx++;
                }
            }
            ui->tableWidget->setRowCount(idx);
            break;
        default:
            dialog->setTip("请选中类型后再进行查看！");
            dialog->setNoVisibal(false);
            dialog->exec();
            dialog->setNoVisibal(true);
        }
    }

}

//TODO 这里只是调用了Qt提供的excle读取库，具体读取后的存储逻辑没有去实现
void MainWindow::on_imp_btn_clicked()
{
    QString str = QFileDialog::getOpenFileName(this,"打开",
                                               QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),"Excel 文件(*.xls *.xlsx)");
    if(!QFileInfo::exists(str)||str.endsWith(".xls")||str.endsWith(".xlsx"))
        return;
    QAxObject *excel = new QAxObject(this);//建立excel操作对象
    excel->setControl("Excel.Application");//连接Excel控件
    excel->setProperty("Visible", false);//显示窗体看效果,选择ture将会看到excel表格被打开
    excel->setProperty("DisplayAlerts", true);//显示警告看效果
    QAxObject *workbooks = excel->querySubObject("WorkBooks");//获取工作簿(excel文件)集合


    workbooks->dynamicCall("Open(const QString&)", str);//打开刚才选定的excel
    QAxObject *workbook = excel->querySubObject("ActiveWorkBook");
    QAxObject *worksheet = workbook->querySubObject("WorkSheets(int)",1);
    QAxObject *usedRange = worksheet->querySubObject("UsedRange");//获取表格中的数据范围
    QVariant var = usedRange->dynamicCall("Value");//将所有的数据读取刀QVariant容器中保存
    QList<QList<QVariant>> excel_list;//用于将QVariant转换为Qlist的二维数组
    QVariantList varRows=var.toList();
    if(varRows.isEmpty())
    {
        return;
    }
    const int row_count = varRows.size();
    QVariantList rowData;
    for(int i=0;i<row_count;++i)
    {
        rowData = varRows[i].toList();
        excel_list.push_back(rowData);
    }//转换完毕
    qDebug()<<excel_list.at(0).at(0).toString();

    //然后将二维数组Qlist<Qlist<QVariant>>中的数据读取出来，到表格或者数据模型中，具体在这里过程省略

    workbook->dynamicCall( "Close(Boolean)", false );
    excel->dynamicCall( "Quit(void)" );
    delete excel;//因为前面选择了不显示excel窗口，如果这里不删除excel的话，excel依然是打开的，只是我们看不见（因为被隐藏起来了）
}

