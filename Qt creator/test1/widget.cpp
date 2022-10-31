#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    myhomep = new Myhome;
    /*
    * 手动链接信号和槽函数
    * 信号和槽是qt中特有对象与对象之间的通信方式
    */
    // QT4风格
   connect(this,SIGNAL(signal_mysignal(QString,int)),myhomep,SLOT(slot_myfun1(QString,int)));

   connect(myhomep,SIGNAL(signal_swap()),this,SLOT(swap()));



    //QT5风格
    //connect(this,&Widget::signal_mysignal,this,&Widget::slot_myfun);

    //lambda表达式
#if 0
    connect(this,&Widget::signal_mysignal,this,[=](int a)
    {
       ui->lineEdit_3->setText(QString::number(a));
    });
#endif

}

Widget::~Widget()
{
    delete ui;
}

//void Widget::slot_myfun(int a)
//{
//    //number将整形转为Qstring
//    ui->lineEdit_3->setText(QString::number(a));
//}

void Widget::swap()
{
    myhomep->close();
    this->show();
}


void Widget::on_btn1_clicked()
{
    QString ip,port;
    //获取控件上的文本信息
    ip = ui->lineEdit->text();
    port = ui->lineEdit_2->text();
    //设置控件文本信息
//    ui->lineEdit_3->setText(ip+":"+port);
    //发送信号

    myhomep->show();
    emit signal_mysignal(ip,port.toInt());

    this->close();
}
