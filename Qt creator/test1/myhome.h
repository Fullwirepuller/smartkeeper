#ifndef MYHOME_H
#define MYHOME_H

#include <QWidget>
#include <qtcpsocket.h>
#include <QImage>
#include <QNetworkRequest>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QMessageBox>
#include <QHostAddress>
#include <QHostInfo>
#include <QAudioDeviceInfo>
#include <QAudioInput>
#include <qaudio.h>
#include <QFile>
namespace Ui {
class Myhome;
}

class Myhome : public QWidget
{
    Q_OBJECT

public:
    explicit Myhome(QWidget *parent = nullptr);
    ~Myhome();

public slots:
    void slot_myfun1(QString,int);

signals:
    void signal_swap();
    void signal_order(QString);
private slots:

    void on_pushButton_clicked();
    void on_pushButton_connect_clicked();
    void on_pushButton_getdata_clicked();
    void recvdata();
    void recvdata2();
    void on_pushButton_fan_clicked();
    void on_pushButton_light_clicked();
    void on_pushButton_video_clicked();
    void on_pushButton_recording_clicked();
    void slot_recvjson_weather(QNetworkReply *reply);
    void slot_recvjson_speak(QNetworkReply *reply);
    void on_pushButton_weather_clicked();

    void on_pushButton_connect_2_clicked();

    void voice_control(QString buf);

private:
    Ui::Myhome *ui;
    QTcpSocket *tcpsocket1;
    QTcpSocket *tcpsocket2;
    int isgetsize;
    int size;
    int fan_turn,video_turn,record_turn,led_turn,getdata_turn;

    //发送请求 天气
    QNetworkAccessManager *netmanager;

    QNetworkAccessManager *netmanager_speak;
    //百度中的API和 secret
    QString mykey;
    QString mysecret;
    QAudioInput *myaudio;
    QFile *myfile;
    int flag_audio;
};

#endif // MYHOME_H
