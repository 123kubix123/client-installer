#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include <QObject>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QDebug>

class MyTcpSocket : public QObject
{
    Q_OBJECT
public:
    explicit MyTcpSocket(QObject *parent = 0);

signals:
    void blond();
    void rozlonczdo();
    void dostepne_dane();

public slots:
    void doConnect();
    void connected();
    void disconnected();
    void bytesWritten(qint64 bytes);
    void readyRead();
    void authorize();
    void after_read();

public:
    QTcpSocket *socket;
    QString pass;
    QString dane;
    QString serw_ip;
    bool transmition_end;
    QString dane_assemble;

};

#endif // MYTCPSOCKET_H
