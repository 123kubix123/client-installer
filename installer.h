#ifndef INSTALLER_H
#define INSTALLER_H

#include <QWidget>
#include "mytcpsocket.h"
#include <QTcpSocket>

namespace Ui {
class installer;
}

class installer : public QWidget
{
    Q_OBJECT

public:
    explicit installer(QWidget *parent = 0);
    ~installer();

private slots:
    void on_client_clicked();

    void on_wruc_menu_clicked();

    void on_zainstaluj_client_clicked();

    void on_exit_clicked();

    void on_reset_clicked();

    void on_wruc_menu_2_clicked();

    void on_lonczenie_clicked();

    void powrot_po_blendzie();

    void data_feeder();

    void finish();

    void on_freeze_clicked();

    void on_wruc_menu_3_clicked();

    void on_accept_freeze_clicked();

private:
    Ui::installer *ui;
    MyTcpSocket *socket;
    QStringList dane_splitted;
    QString freeze_partition;
};

#endif // INSTALLER_H
