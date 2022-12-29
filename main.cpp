#include "mainwindow.h"
#include "welcometopixeditform.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WelcomeToPixEditForm f;
    f.show();
    return a.exec();
}
