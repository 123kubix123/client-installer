#include "installer.h"
#include <QApplication>
#include <QProcess>
#include <QIcon>
#include <QPixmap>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    installer w;

    QProcess process;
        process.start("/bin/bash", QStringList()<< "-c" << "whoami && exit || exit\n");
        process.waitForFinished();
        process.waitForReadyRead();
        QString out = process.readAllStandardOutput();
        out.replace("\n","");
        process.close();
        if(out=="root")
        {
        w.show();
        }
        else if(out != "")
        {
            QProcess process1;
            process1.startDetached("/bin/bash", QStringList()<< "-c" << "pkexec env DISPLAY=$DISPLAY XAUTHORITY=$XAUTHORITY $PWD/client-installer || xdg-su ./client-installer || gksu ./client-installer || gksudo ./client-installer || gnomesu ./client-installer || kdesu ./client-installer || kdesudo ./client-installer && exit || exit\n");
            return 0;
        }
        else
        {
            w.show();
        }
    return a.exec();
}
