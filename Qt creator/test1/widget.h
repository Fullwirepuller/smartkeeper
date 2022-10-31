#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "myhome.h"
QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    //按照声明函数的方式声明信号，信号只有声明，没有定义
signals:
    void signal_mysignal(QString ip_address1,int port1);
//声明一个槽函数 在.h文件中声明槽函数，.cpp文件中实现槽函数
public slots:
//    void slot_myfun(int a);
    void swap();

private slots:
    void on_btn1_clicked();

private:
    Ui::Widget *ui;
    Myhome *myhomep;
};
#endif // WIDGET_H
