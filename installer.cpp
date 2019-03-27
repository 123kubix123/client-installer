#include "installer.h"
#include "ui_installer.h"
#include <QProcess>
#include <QFile>
#include <QTextStream>
#include <QApplication>
#include <QFileDialog>
#include <QDebug>
#include <QPixmap>
#include <QMessageBox>

installer::installer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::installer)
{
    socket = new MyTcpSocket(this);
    ui->setupUi(this);
    ui->strony->setCurrentIndex(0);
    qDebug()<<"client-installer - development release";
    qDebug()<<"Autorzy Jakub Wolski i Jakub Poplawski";
    qDebug()<<"https://github.com/123kubix123/client-installer kod źródłowy na licencji GPLv3";
}

installer::~installer()
{
    delete ui;
}

void installer::on_client_clicked()
{
    ui->strony->setCurrentIndex(3);
    ui->ip_serw->setInputMask("000.000.000.000;_");
   // ui->port->setInputMask("00000;_");
}


void installer::on_wruc_menu_clicked()
{
    socket->dane_assemble = "";
    socket->dane = "";
    ui->strony->setCurrentIndex(0);
}

void installer::on_wruc_menu_2_clicked()
{
    on_wruc_menu_clicked();
}

void installer::on_zainstaluj_client_clicked()
{
    QString ktory_komputer = ui->wybierz_komputer->currentText();
    QString name;
    QString ip_serwera;
    QString token;
    QString klucz_roota;
    QString klucz_distri;
    for(int i =0; i < dane_splitted.size(); i++)
    {
        QString current = dane_splitted.at(i);
        QStringList current_splited = current.split("^");
        if(current_splited.first() == ktory_komputer)
        {
            if(current_splited.size() == 5)
            {
                name = current_splited.at(0);
                //qDebug()<<ktory_komputer<<" = "<<name;
                ip_serwera = current_splited.at(1);
                token = current_splited.at(2);
                klucz_roota = current_splited.at(3);
                //qDebug()<<klucz_roota;
                klucz_distri = current_splited.at(4);
                //qDebug()<<klucz_distri;
            }
        }
    }
   //odczytywanie programu
    /*
    QString program;
    QFile file(":/client/res/client/pracownia-client");
          if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
              return;
          QTextStream in(&file);
          while (!in.atEnd()) {
              QString line = in.readLine();
              program += line+"\n";
          }
          */
    //odczytywanie uługi/serwisu
                      QString serwis;
                      QFile files(":/client/res/client/client.service");
                            if (!files.open(QIODevice::ReadOnly | QIODevice::Text))
                                return;
                            QTextStream ins(&files);
                            while (!ins.atEnd()) {
                                QString line = ins.readLine();
                                serwis += line+"\n";
                            }

                           // qDebug()<<"przed pentla";
        if(name !="" && ip_serwera != "" && token != "" && klucz_roota != "" && klucz_distri != "" && serwis != "")
        {
           // qDebug()<<"w pentla";
            QProcess dir_creator;
            dir_creator.start("/bin/bash");
            dir_creator.waitForStarted();
            std::string dir_creator_command = "mkdir -p /opt/pracownia/bin; \n mkdir -p /root/.ssh && exit || exit\n";
            dir_creator.write(dir_creator_command.c_str());
            dir_creator.waitForBytesWritten();
            dir_creator.waitForFinished();
            dir_creator.close();
        /*
        QFile file1("/opt/pracownia/bin/pracownia-client");
           if (!file1.open(QIODevice::WriteOnly | QIODevice::Text))
               return;

           QTextStream out(&file1);
           out << program;
           file1.close();
        */
        QFile file1k("/opt/pracownia/bin/config.conf");
           if (!file1k.open(QIODevice::WriteOnly | QIODevice::Text))
               return;

           QTextStream outk(&file1k);
           outk << token+"="+name+"="+ip_serwera;
           file1k.close();

        QFile file1s("/etc/systemd/system/client.service");
           if (!file1s.open(QIODevice::WriteOnly | QIODevice::Text))
               return;

           QTextStream outs(&file1s);
           outs << serwis;
           file1s.close();

        QProcess *instaluj_klienta = new QProcess(this);
        instaluj_klienta->start("/bin/bash");
        instaluj_klienta->waitForStarted();
        std::string do_basza = "echo '"+klucz_roota.toStdString()+"' >> /root/.ssh/authorized_keys \n";
        do_basza += "echo '"+klucz_distri.toStdString()+"' > /root/id_rsa \n";
        do_basza += "cd /opt/pracownia/bin \n";
        do_basza += "wget download.oedu.pl/pracownia-client -O pracownia-client \n";
        do_basza += "chmod +x /opt/pracownia/bin/pracownia-client \n";
        do_basza += "chmod 644 /etc/systemd/system/client.service \n";
        do_basza += "systemctl enable client.service \n";
        do_basza += "chmod 400 /root/id_rsa \n";
        do_basza += "systemctl start sshd \n";
        do_basza += "systemctl start client.service && exit || exit\n";
        //qDebug()<<QString::fromStdString(do_basza);
        instaluj_klienta->write(do_basza.c_str());
        connect(instaluj_klienta, SIGNAL(finished(int)), this, SLOT(finish()));
        ui->zainstaluj_client->setEnabled(false);
        ui->wruc_menu->setEnabled(false);
        }
}


void installer::finish()
{
    ui->strony->setCurrentIndex(1);
}

void installer::on_exit_clicked()
{
    QApplication::exit();
}

void installer::on_reset_clicked()
{
    QProcess process1;
    process1.startDetached("/bin/bash", QStringList()<< "-c" << "systemctl reboot");
    QApplication::exit();
}

void installer::powrot_po_blendzie()
{
    if(socket->transmition_end == false)
    {
    ui->wybierz_komputer->clear();
    socket->dane_assemble = "";
    socket->dane = "";
    ui->opis_4->setText("Błąd połączenia - sprawdź wpisane dane:");
    ui->strony->setCurrentIndex(3);
    }
}

void installer::data_feeder()
{
    ui->wybierz_komputer->clear();
    QString pobrane_z_socketu = socket->dane;
    pobrane_z_socketu.replace("[enter]","\n");
    //
    //qDebug()<< pobrane_z_socketu;
    dane_splitted = pobrane_z_socketu.split("`");
    int ile = dane_splitted.size();
    if(ile != 0)
    {
        for(int i = 0; i<ile; i++)
        {
            QString single_host = dane_splitted.at(i);
            //qDebug()<< single_host;

            QStringList single_host_splitted = single_host.split("^");
/*
            qDebug()<<"Ile= "<<ile;
            qDebug()<< "5="<< single_host_splitted.size();
            qDebug()<< single_host_splitted.at(0);
            qDebug()<< single_host_splitted.at(1);
            qDebug()<< single_host_splitted.at(2);
            qDebug()<< single_host_splitted.at(3);
            qDebug()<< single_host_splitted.at(4);
*/
            ui->wybierz_komputer->addItem(single_host_splitted.at(0));
        }
    ui->strony->setCurrentIndex(2);
    }
    else
    {
        socket->socket->disconnect();
    }
}

/*
void installer::on_zainstaluj_serwer_clicked()
{
    QString wzorzec = ui->scierzka->text();
    //program
    QString program;
    QFile file(":/serwer/res/serwer/pracownia-serwer");
          if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
              return;
          QTextStream in(&file);
          while (!in.atEnd()) {
              QString line = in.readLine();
              program += line+"\n";
          }

    QFile file1("/sbin/pracownia-serwer");
          if (!file1.open(QIODevice::WriteOnly | QIODevice::Text))
              return;

     QTextStream out(&file1);
     out << program;
     file1.close();

     QProcess instaluj_serwera;
     instaluj_serwera.start("/bin/bash");
     instaluj_serwera.waitForStarted();
     std::string do_basza = "chmod 755 /sbin/pracownia-serwer; mkdir "+wzorzec.toStdString()+"/distribution; /usr/sbin/useradd -d "+wzorzec.toStdString()+"/distribution distribution; mkdir "+wzorzec.toStdString()+"/distribution/.ssh; chown distribution:users "+wzorzec.toStdString()+"/distribution/.ssh; chown distribution:users "+wzorzec.toStdString()+"/distribution; chmod 700 "+wzorzec.toStdString()+"/distribution; echo zakonczono \n";
     instaluj_serwera.write(do_basza.c_str());
     instaluj_serwera.waitForBytesWritten();
     instaluj_serwera.waitForReadyRead();
     qDebug()<< "Zwrot: " << instaluj_serwera.readAllStandardOutput();
     qDebug()<< "Errory: " << instaluj_serwera.readAllStandardError();
     // wrzucanie klucza
     QString klucz;
     QFile filek(":/serwer/res/serwer/id_rsa.pub");
           if (!filek.open(QIODevice::ReadOnly | QIODevice::Text))
               return;
           QTextStream ink(&filek);
           while (!ink.atEnd()) {
               QString line = ink.readLine();
               klucz += line+"\n";
           }
     QString lokalizacja_klucza = wzorzec+"/distribution/.ssh/authorized_keys";
     QFile file1k(lokalizacja_klucza);
        if (!file1k.open(QIODevice::WriteOnly | QIODevice::Text))
            return;

        QTextStream outk(&file1k);
        outk << klucz;
        file1k.close();

     std::string do_basza2 = "chmod 600 "+wzorzec.toStdString()+"/distribution/.ssh/authorized_keys; chown distribution:users "+wzorzec.toStdString()+"/distribution/.ssh/authorized_keys; echo zakonczono && exit || exit\n";
     instaluj_serwera.write(do_basza2.c_str());
     instaluj_serwera.waitForBytesWritten();
     instaluj_serwera.waitForReadyRead();
     qDebug()<< "Zwrot2: " << instaluj_serwera.readAllStandardOutput();
     qDebug()<< "Errory2: " << instaluj_serwera.readAllStandardError();
     instaluj_serwera.waitForFinished();
     instaluj_serwera.close();
     ui->strony->setCurrentIndex(1);
}
*/


void installer::on_lonczenie_clicked()
{
    ui->wybierz_komputer->clear();
    if(ui->haslo_install->text() != "")
    {
    ui->opis_4->setText("Połączenie z serwerem:");
    socket->serw_ip = ui->ip_serw->text();
    socket->pass = ui->haslo_install->text();
    connect(socket, SIGNAL(rozlonczdo()),this , SLOT(powrot_po_blendzie()));
    connect(socket, SIGNAL(dostepne_dane()), this, SLOT(data_feeder()));
    socket->doConnect();
    }
}

void installer::on_freeze_clicked()
{
    QProcess partition_search;
    partition_search.start("/bin/bash");
    partition_search.waitForStarted();
    std::string partition_search_command = " lsblk -r -o NAME,SIZE,FSTYPE,MOUNTPOINT | grep btrfs | awk '{ print $1\":\"$2\":\"$4 }' | sed ':a;N;$!ba;s/\\n/-/g' && exit || exit \n";
    partition_search.write(partition_search_command.c_str());
    partition_search.waitForBytesWritten();
    partition_search.waitForFinished();
    QString partitions = partition_search.readAllStandardOutput();
    partition_search.close();
    partitions.replace("\n","");
    partitions.replace(" ","");
    if(partitions.size() > 0)
    {
        QStringList partitions_splitted = partitions.split("-");
        foreach(QString single_partition, partitions_splitted)
        {
            ui->partitions->addItem(single_partition);
            ui->strony->setCurrentIndex(4);
        }
    }
    else
    {
        QMessageBox::information(this,tr("Error"),tr("Nie znaleziono partycji btrfs!"));
        ui->strony->setCurrentIndex(0);
    }
}

void installer::on_wruc_menu_3_clicked()
{
    ui->strony->setCurrentIndex(0);
}

void installer::on_accept_freeze_clicked()
{
    QString from_combo_box = ui->partitions->currentText();
    QStringList from_combo_box_splitted = from_combo_box.split(":");
    freeze_partition = from_combo_box_splitted.value(0);
    QProcess freezer;
    freezer.start("/bin/bash");
    freezer.waitForStarted();
    QString freezer_command;
    if(ui->czy_kopiowac->isChecked() == true)
    {
        freezer_command = "mkdir /pracownia; mount /dev/"+freeze_partition+" /pracownia; btrfs sub create /pracownia/home; rsync -aogu /home/ /pracownia/home; echo -e '#!/bin/bash\\nmkdir /home;\\nmount /dev/"+freeze_partition+" /pracownia\\nbtrfs sub del /pracownia/home2\\nbtrfs sub snap /pracownia/home /pracownia/home2\\nsubvolid=`btrfs sub list /pracownia | grep home2 | awk \\x27{ print $2 }\\x27`\\numount /pracownia\\nmount -t btrfs -o subvolid=$subvolid /dev/"+freeze_partition+" /home\\nrestorecon -R /home\\n' > /sbin/freeze.sh;  chmod +x /sbin/freeze.sh; echo -e  '[Unit]\\nDescription=Mrozenie pracownia\\nAfter=local-fs.target\\nDefaultDependencies=no\\n\\n[Service]\\nType=oneshot\\nExecStart=/sbin/freeze.sh\\n\\n[Install]\\nWantedBy=basic.target\\nRequiredBy=multi-user.target\\n' > /etc/systemd/system/freeze.service; systemctl enable freeze.service && exit || exit\n";
    }
    else
    {
        freezer_command = "mkdir /pracownia; mount /dev/"+freeze_partition+" /pracownia; btrfs sub create /pracownia/home; echo -e '#!/bin/bash\\nmkdir /home;\\nmount /dev/"+freeze_partition+" /pracownia\\nbtrfs sub del /pracownia/home2\\nbtrfs sub snap /pracownia/home /pracownia/home2\\nsubvolid=`btrfs sub list /pracownia | grep home2 | awk \\x27{ print $2 }\\x27`\\numount /pracownia\\nmount -t btrfs -o subvolid=$subvolid /dev/"+freeze_partition+" /home\\nrestorecon -R /home\\n' > /sbin/freeze.sh;  chmod +x /sbin/freeze.sh; echo -e  '[Unit]\\nDescription=Mrozenie pracownia\\nAfter=local-fs.target\\nDefaultDependencies=no\\n\\n[Service]\\nType=oneshot\\nExecStart=/sbin/freeze.sh\\n\\n[Install]\\nWantedBy=basic.target\\nRequiredBy=multi-user.target\\n' > /etc/systemd/system/freeze.service; systemctl enable freeze.service && exit || exit\n";
    }
    freezer.write(freezer_command.toStdString().c_str());
    freezer.waitForBytesWritten();
    freezer.waitForFinished(-1);
    freezer.close();
    ui->strony->setCurrentIndex(1);
}
