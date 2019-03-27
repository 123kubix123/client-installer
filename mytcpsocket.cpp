#include "mytcpsocket.h"
#include <QTimer>
#include <QFile>
#include <QDebug>

MyTcpSocket::MyTcpSocket(QObject *parent) : QObject(parent)
{

}



void MyTcpSocket::doConnect()
{
    socket = new QTcpSocket(this);
    transmition_end = false;
    connect(socket, SIGNAL(connected()),this, SLOT(connected()));
    connect(socket, SIGNAL(disconnected()),this, SLOT(disconnected()));
    connect(socket, SIGNAL(bytesWritten(qint64)),this, SLOT(bytesWritten(qint64)));
    //connect(socket, SIGNAL(readChannelFinished()), this, SLOT(after_read()));
    connect(socket, SIGNAL(readyRead()),this, SLOT(readyRead()));

    qDebug() << "Connecting...";

    // this is not blocking call
    socket->connectToHost(serw_ip, 9999);

    // we need to wait...
    if(!socket->waitForConnected(5000))
    {
        qDebug() << "Error: " << socket->errorString();
        emit rozlonczdo();
    }
}

void MyTcpSocket::connected()
{
    qDebug() << "Connected...";
   // QTimer::singleShot(1000, this, SLOT(authorize()));
    authorize();
}
void MyTcpSocket::authorize()
{
    socket->write(pass.toUtf8());
    socket->flush();
}

void MyTcpSocket::disconnected()
{
    qDebug() << "Disconnected...";
    after_read();
    emit rozlonczdo();
}

void MyTcpSocket::bytesWritten(qint64 bytes)
{
    //qDebug() << bytes << " Bytes written...";
}

void MyTcpSocket::readyRead()
{
    // read the data from the socket
        //qDebug() << "Czytam wszystko.. ";
        QString zwrot = socket->readAll();
        //qDebug() << zwrot;
        dane_assemble += zwrot;

        //after_read();
}

void MyTcpSocket::after_read()
{
    QString zwrot = dane_assemble;
    //qDebug() << "after";
    if(zwrot == "Unauthorized connection")
    {
        socket->disconnect();
        qDebug()<<"Błąd autoryzacji";
    }
    else if(zwrot == "")
    {
        socket->disconnect();
        qDebug()<<"Błąd komunikacji z serwerem";
    }
    else if(zwrot != "")
    {
        dane = zwrot;
        transmition_end = true;
        //qDebug()<<dane;
        emit dostepne_dane();
    }
}
