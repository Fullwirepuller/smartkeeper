#include "myhome.h"
#include "ui_myhome.h"

Myhome::Myhome(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Myhome)
{
    fan_turn = 0;
    led_turn = 0;
    video_turn = 0;
    record_turn = 0;
    getdata_turn = 0;
    ui->setupUi(this);
    tcpsocket1 = new QTcpSocket;
    tcpsocket2 = new QTcpSocket;
    isgetsize=0;
    flag_audio = 0;
    mykey = "Q6loqQvQkCbE7lMHm4UxpkwL";
    mysecret = "YsEXZmKHaGT5RxrvFzqpfKBFYdkXbClx";

    //点击获取天气的信息-------------------
    netmanager = new QNetworkAccessManager;
    connect(netmanager,&QNetworkAccessManager::finished,this,&Myhome::slot_recvjson_weather);
    //-------------------------------

    //语音识别
    netmanager_speak = new QNetworkAccessManager;
    connect(netmanager_speak,&QNetworkAccessManager::finished,this,&Myhome::slot_recvjson_speak);

    //判断连接是否成功
    connect(tcpsocket1,&QTcpSocket::connected,this,[=](){ui->label->setText("Connect1 ok");});
    //判断网络中是否有数据可读
    connect(tcpsocket1,&QTcpSocket::readyRead,this,&Myhome::recvdata);


    connect(tcpsocket2,&QTcpSocket::connected,this,[=](){
            ui->label_connect->setText("Connected2 ok!");
        });
    //connect(tcpsocket2,&QTcpSocket::readyRead,this,&Myhome::recvdata2);

//    connect(this,&Myhome::signal_order,this,&Myhome::slot_handler_order);
}

Myhome::~Myhome()
{
    delete ui;
}

void Myhome::slot_myfun1(QString ipd,int p)
{
    QString ip = ipd;
    int port = p;
    ui->lineEdit_ip->setText(ip);
    if(p!=0)
        ui->lineEdit_port->setText(QString::number(p));
    else
         ui->lineEdit_port->setText("");
    tcpsocket1->connectToHost(ip,port);
    int i = 10000000000;
    while(i--);
    tcpsocket2->connectToHost(ip,port);

//    //链接服务器
//    tcpsocket1->connectToHost(ip,port);
    //ui->lineEdit->setText(QString::number(a));
}

void Myhome::on_pushButton_clicked()
{
    emit signal_swap();
}

void Myhome::on_pushButton_connect_clicked()
{
/*
    QString ip = ui->lineEdit_ip->text();
    int port = ui->lineEdit_port->text().toInt();
    //链接服务器
    tcpsocket1->connectToHost(ip,port);
    int i = 10000000000;
    while(i--);
    tcpsocket2->connectToHost(ip,port);
*/
}

void Myhome::on_pushButton_getdata_clicked()
{
    if(getdata_turn == 0){
        tcpsocket1->write("takepic");
        ui->pushButton_getdata->setText("stopdata");
        getdata_turn = 1;

    }
    else if(getdata_turn == 1){
        tcpsocket1->write("stoppic");
        ui->pushButton_getdata->setText("getdata");
        getdata_turn = 0;
    }

}

void Myhome::recvdata()
{
        if(isgetsize == 0)
        {
            if(tcpsocket1->bytesAvailable()>=16)
            {
                char chbuf[64] = {0};
                tcpsocket1->read(chbuf,16);
                sscanf(chbuf,"%d",&size);
                qDebug()<<size;
                isgetsize=1;
            }
        }
        else{
            if(tcpsocket1->bytesAvailable()>=size)
            {
                QByteArray buf;
                buf = tcpsocket1->read(size);
                QImage i;
                i.loadFromData(buf);
                QImage image = i.scaled(ui->label->size(),Qt::KeepAspectRatio);
                ui->label->setPixmap(QPixmap::fromImage(image));
                isgetsize=0;
            }
        }
}

void Myhome::recvdata2()
{
    ui->textEdit->setText(tcpsocket2->readAll());
}

void Myhome::on_pushButton_light_clicked()
{
    if(led_turn == 0)
    {
        ui->label_light->setStyleSheet("border-image: url(:/image/led_on.png)");
        ui->textEdit->insertPlainText("led_on!\n");
        ui->pushButton_light->setText("关闭灯泡");
        led_turn = 1;
        tcpsocket2->write("led_on");
    }
    else if(led_turn == 1)
    {
        ui->label_light->setStyleSheet("border-image: url(:/image/led_off.png)");
        ui->textEdit->insertPlainText("led_off!\n");
        ui->pushButton_light->setText("打开灯泡");
        led_turn = 0;
        tcpsocket2->write("led_off");
    }
}


void Myhome::on_pushButton_fan_clicked()
{
    if(fan_turn == 0)
    {
         ui->label_fan->setStyleSheet("border-image: url(:/image/fan_on.png)");
         ui->textEdit->insertPlainText("fan_on!\n");
         ui->pushButton_fan->setText("关闭风扇");
         fan_turn = 1;
         tcpsocket2->write("fan_on");
    }

    else if(fan_turn == 1)
    {
        ui->label_fan->setStyleSheet("border-image: url(:/image/fan_off.png)");
        ui->textEdit->insertPlainText("fan_off!\n");
        ui->pushButton_fan->setText("打开风扇");
        fan_turn = 0;
        tcpsocket2->write("fan_off");
    }

}


void Myhome::on_pushButton_video_clicked()
{
    if(video_turn == 0)
    {
        ui->label_video->setStyleSheet("border-image: url(:/image/video_on.png)");
        ui->textEdit->insertPlainText("video_on!\n");
        ui->pushButton_video->setText("关闭蜂鸣器");
        video_turn = 1;
        tcpsocket2->write("video_on");
    }

    else if(video_turn == 1)
    {
        ui->label_video->setStyleSheet("border-image: url(:/image/video_off.png)");
        ui->textEdit->insertPlainText("vidoe_off!\n");
        ui->pushButton_video->setText("打开蜂鸣器");
        video_turn = 0;
        tcpsocket2->write("video_off");
    }

}

void Myhome::on_pushButton_recording_clicked()
{
    if(record_turn == 0)
    {
        ui->label_recording->setStyleSheet("border-image: url(:/image/microphone_on.png)");
        ui->textEdit->insertPlainText("microphone_on!\n");
        ui->pushButton_recording->setText("点击停止");
        record_turn = 1;
        tcpsocket2->write("microphone_on");
    }

    else if(record_turn == 1)
    {
        ui->label_recording->setStyleSheet("border-image: url(:/image/microphone_off.png)");
        ui->textEdit->insertPlainText("microphone_off!\n");
        ui->pushButton_recording->setText("开始录音");
        record_turn = 0;
        tcpsocket2->write("microphone_off");
    }

//    QNetworkRequest netrequest;
//    QString url_init = "https://aip.baidubce.com/oauth/2.0/token?grant_type=client_credentials&client_id=%1&client_secret=%2&";
//    QString url = url_init.arg(mykey).arg(mysecret);
//    //如果有附属体
//    QByteArray buf;
//    netrequest.setUrl(QUrl(url));
//    netmanager_speak->post(netrequest,buf);
    if(flag_audio == 0)
    {
        QAudioDeviceInfo device = QAudioDeviceInfo::defaultInputDevice();
        if(device.isNull())
        {
            QMessageBox::warning(this,"audiodevice","missing");
            return;
        }
        QAudioFormat myformat;
        myformat.setSampleRate(16000);
        myformat.setChannelCount(1);
        myformat.setSampleSize(16);
        myformat.setCodec("audio/pcm");
        if(!device.isFormatSupported(myformat))
        {
            myformat = device.nearestFormat(myformat);
        }
        myfile = new QFile;
        myfile->setFileName("1.pcm");
        myfile->open(QIODevice::ReadWrite);
        myaudio = new QAudioInput(myformat,this);
        myaudio->start(myfile);

        flag_audio = 1;
    }

    else
    {
        myaudio->stop();
        myfile->close();
        delete myfile;
        myfile = NULL;
        //准备请求内容
        QNetworkRequest netrequest;
        QString url_init = "http://vop.baidu.com/server_api?dev_pid=1537&cuid=%1&token=%2";
        QString hostid = QHostInfo::localHostName();
        QString token = "24.5f07f3275b4141654ef5a84bfaba4114.2592000.1657090562.282335-26349033";
        QString url = url_init.arg(hostid).arg(token);
        netrequest.setUrl(QUrl(url));
        QString header = "Content-Type";
        QString content = "audio/pcm;rate=16000";
        netrequest.setRawHeader(header.toLatin1(),content.toLatin1());
        //准备好附属体的内容 这里是值准备好音频文件
        QByteArray buf;
        myfile = new QFile("1.pcm");
        myfile->open(QIODevice::ReadWrite);
        buf = myfile->readAll();

        netmanager_speak->post(netrequest,buf);
        flag_audio = 0;
    }
}

void Myhome::on_pushButton_weather_clicked()
{
    QNetworkRequest netrequest;
    QString w_url = "http://wthrcdn.etouch.cn/weather_mini?city=%1";
    QString w_city = w_url.arg(ui->lineEdit_city->text());
    qDebug()<<w_city;

    netrequest.setUrl(QUrl(w_city));
    netmanager->get(netrequest);
    tcpsocket2->write("weatheron");
}
void Myhome::slot_recvjson_weather(QNetworkReply *reply)
{
    QByteArray buf;
    buf = reply->readAll();
    qDebug()<<buf;

    QJsonParseError e;
    //将网络中的数据转换为json文档
    QJsonDocument json_docu = QJsonDocument::fromJson(buf,&e);
    if(e.error!=QJsonParseError::NoError)
    {
        //QMessageBox message;
        //message.set
        qDebug()<<"fromJson error";
        QMessageBox message;
        message.warning(this,"警告","fromJson error",QMessageBox::Ok,
                        QMessageBox::NoButton);
        return;
    }
    QJsonObject obj1;
    //判断json文档中是否可以生成json对象
    if(json_docu.isObject())
    {
        //将json文档转换为json对象
        obj1 = json_docu.object();
    }
    else
    {
        //弹出警告框
        QMessageBox message;
        message.warning(this,"警告","object missing",QMessageBox::Ok,
                        QMessageBox::NoButton);
        return;
    }
    QJsonObject obj2;
    //json对象中，也就是大括号中是否包含，指定的键
    if(obj1.contains("data"))
    {
        //把指定键中的值转换为json对象
        obj2 = obj1.value("data").toObject();

        //把指定键中的值转换为字符串
        QString city = obj2.value("city").toString();
        qDebug()<<"city:"<<city;
        QString wendu = obj2.value("wendu").toString();
        qDebug()<<"wendu:"<<city;
        //指定键中的值转换为json数组
        QJsonArray json_array = obj2.value("forecast").toArray();
        //json数组引用元素还是普通数组一样
        //第一个元素转换为json对象
        QJsonObject obj3 = json_array[0].toObject();

        QString date = obj3.value("date").toString();
        qDebug()<<"date"<<date;

        QString high = obj3.value("high").toString();
        qDebug()<<"high"<<high;

        QString fengli = obj3.value("fengli").toString();
        qDebug()<<"fengli"<<fengli;

        QString low = obj3.value("low").toString();
        qDebug()<<"low"<<low;

        QString fengxiang = obj3.value("fengxiang").toString();
        qDebug()<<"fengxiang"<<fengxiang;

        QString type = obj3.value("type").toString();
        qDebug()<<"type"<<type;

        QString weather_image;
        if(type=="大雨")
           weather_image = "big_rain";
        else if(type=="晴")
           weather_image = "sunny";
        else if(type=="小雨")
           weather_image = "small_rain";
        else if(type=="多云")
           weather_image = "cloud";
        else if(type=="阴")
           weather_image = "no_sun";
        ui->label_weather->setStyleSheet("border-image: url(:/image/"+weather_image+".png)");
        ui->lineEdit_weather->setText(type+" "+wendu+"℃");
    }
    else
    {
        QMessageBox message;
        message.warning(this,"警告","请首先输入城市",QMessageBox::Ok,
                        QMessageBox::NoButton);
        return;
    }
}

void Myhome::slot_recvjson_speak(QNetworkReply *reply)
{
    QByteArray buf;
    buf = reply->readAll();
    qDebug()<<buf;

    QJsonParseError e;
    //将网络中的数据转换为json文档
    QJsonDocument json_docu = QJsonDocument::fromJson(buf,&e);
    if(e.error!=QJsonParseError::NoError)
    {
        //QMessageBox message;
        //message.set
        qDebug()<<"fromJson error";
        QMessageBox message;
        message.warning(this,"警告","fromJson error",QMessageBox::Ok,
                        QMessageBox::NoButton);
        return;
    }
    QJsonObject obj1;
    //判断json文档中是否可以生成json对象
    if(json_docu.isObject())
    {
        //将json文档转换为json对象
        obj1 = json_docu.object();
    }
    else
    {
        //弹出警告框
        QMessageBox message;
        message.warning(this,"警告","object missing",QMessageBox::Ok,
                        QMessageBox::NoButton);
        return;
    }
    QJsonArray access_token;
    QString buf1;
    //json对象中，也就是大括号中是否包含，指定的键
    if(obj1.contains("result"))
    {
        //把指定键中的值转换为json对象
        access_token = obj1.value("result").toArray();
        buf1 = access_token[0].toString();
        qDebug()<<"result"<<access_token[0].toString();
        ui->textEdit_speak->clear();
        ui->textEdit_speak->setText(access_token[0].toString());
    }
    else
    {
        QMessageBox message;
        message.warning(this,"警告","object error",QMessageBox::Ok,
                        QMessageBox::NoButton);
        return;
    }

    voice_control(buf1);

}

void Myhome::on_pushButton_connect_2_clicked()
{
    ui->textEdit->clear();
}

void Myhome::voice_control(QString buf1){
    if(buf1 == "打开灯泡。"){
        ui->label_light->setStyleSheet("border-image: url(:/image/light_1.png)");
        ui->textEdit->insertPlainText("led_on!\n");
        ui->pushButton_light->setText("关闭灯泡");
        led_turn = 1;
        tcpsocket2->write("led_on");
    }
    if(buf1 == "关闭灯泡。"){
        ui->label_light->setStyleSheet("border-image: url(:/image/light_0.png)");
        ui->textEdit->insertPlainText("led_off!\n");
        ui->pushButton_light->setText("打开灯泡");
        led_turn = 0;
        tcpsocket2->write("led_off");
    }
    if(buf1 == "打开风扇。"){
        ui->label_fan->setStyleSheet("border-image: url(:/image/fan_1.png)");
        ui->textEdit->insertPlainText("fan_on!\n");
        ui->pushButton_fan->setText("关闭风扇");
        fan_turn = 1;
        tcpsocket2->write("fan_on");
    }
    if(buf1 == "关闭风扇。"){
        ui->label_fan->setStyleSheet("border-image: url(:/image/fan_0.png)");
        ui->textEdit->insertPlainText("fan_off!\n");
        ui->pushButton_fan->setText("打开风扇");
        fan_turn = 0;
        tcpsocket2->write("fan_off");
    }
    if(buf1 == "打开蜂鸣器。"){
        ui->label_video->setStyleSheet("border-image: url(:/image/video_1.png)");
        ui->textEdit->insertPlainText("video_on!\n");
        ui->pushButton_video->setText("关闭蜂鸣器");
        video_turn = 1;
        tcpsocket2->write("video_on");
    }
    if(buf1 == "关闭蜂鸣器。"){
        ui->label_video->setStyleSheet("border-image: url(:/image/video_0.png)");
        ui->textEdit->insertPlainText("vidoe_off!\n");
        ui->pushButton_video->setText("打开蜂鸣器");
        video_turn = 0;
        tcpsocket2->write("video_off");
    }
}
