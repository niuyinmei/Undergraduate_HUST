//#include "mainwindow.h"
#include "login.h"
#include "logindialog.h"
#include <QApplication>

QString realuserName;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //MainWindow w;
    login dlg;
    /*if (dlg.exec() == QDialog::Accepted)
    {
        w.show();
        return a.exec();
    }
    else return 0;*/
    dlg.show();
    return a.exec();
}
