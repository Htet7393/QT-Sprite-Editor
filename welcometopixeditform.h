#ifndef WELCOMETOPIXEDITFORM_H
#define WELCOMETOPIXEDITFORM_H

//CS3505 A7
//Alex Baburnic, Bradley Lund, Htet Naing

//This class handles all of the splash screen methods.
#include <QWidget>
#include "mainwindow.h"

namespace Ui {
class WelcomeToPixEditForm;
}

class WelcomeToPixEditForm : public QWidget
{
    Q_OBJECT

public:
    explicit WelcomeToPixEditForm(QWidget *parent = nullptr);
    ~WelcomeToPixEditForm();

public slots:
    void setResolution(int);
    void newProject();
    void loadProject();
    void aboutPopUp();


private:
    Ui::WelcomeToPixEditForm *ui;
    int frameSize;
    QImage icon, splashScreenImage;
};

#endif // WELCOMETOPIXEDITFORM_H
