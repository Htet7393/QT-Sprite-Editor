//CS3505 A7
//Alex Baburnic, Bradley Lund, Htet Naing

//This class handles all of the splash screen methods.
#include "welcometopixeditform.h"
#include "ui_welcometopixeditform.h"
#include "mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>

WelcomeToPixEditForm::WelcomeToPixEditForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WelcomeToPixEditForm),
    icon(":/images/PixEditLogoV2.PNG"),
    splashScreenImage(":/images/PixEditLogoV1.PNG")
{
    //Setup ui window
    ui->setupUi(this);
    this->setWindowTitle("Welcome to PixEdit!");
    this->setFixedSize(530,310);
    this->setWindowIcon(QIcon(QPixmap::fromImage(icon)));
    frameSize = 8;
    //Setup frame resolution drop down
    ui->createResolutionCBox->addItem("8x8");
    ui->createResolutionCBox->addItem("16x16");
    ui->createResolutionCBox->addItem("32x32");
    ui->createResolutionCBox->addItem("64x64");
    //Connect splash screen buttons
    connect(ui->createResolutionCBox, &QComboBox::currentIndexChanged, this, &WelcomeToPixEditForm::setResolution);
    connect(ui->loadProjectButton, &QPushButton::clicked, this, &WelcomeToPixEditForm::loadProject);
    connect(ui->newProjectButton, &QPushButton::clicked, this, &WelcomeToPixEditForm::newProject);
    connect(ui->aboutPixEditButton, &QPushButton::clicked, this, &WelcomeToPixEditForm::aboutPopUp);
    //Add splash screen image
    ui->pixEditSplashLogo->setPixmap(QPixmap::fromImage(splashScreenImage));
}

WelcomeToPixEditForm::~WelcomeToPixEditForm()
{
    delete ui;
}

/**
 * Sets the resolution of the sprite frame
 * @param option from drop down ui widget
 */
void WelcomeToPixEditForm::setResolution(int index){
    switch(index){
        case 0:
            frameSize = 8;
            break;
        case 1:
            frameSize = 16;
            break;
        case 2:
            frameSize = 32;
            break;
        case 3:
            frameSize = 64;
            break;
    }
}

/**
 * Opens a blank project
 */
void WelcomeToPixEditForm::newProject(){
    MainWindow *editorWindow = new MainWindow(this);
    editorWindow->setNewFrameCollection(frameSize);
    editorWindow->show();
    editorWindow->setWindowTitle(ui->projectNameLineEdit->text());
    editorWindow->title = editorWindow->windowTitle();
}

/**
 * Opens a save project
 */
void WelcomeToPixEditForm::loadProject(){
    MainWindow *editorWindow = new MainWindow(this);
    QString filePath = QFileDialog::getOpenFileName(nullptr, "Open Project", ".", "Sprite Sheet Project (*.ssp)");
    editorWindow->readFile(filePath.toStdString());
    editorWindow->show();
}

/**
 * Displays about pop up.
 */
void WelcomeToPixEditForm::aboutPopUp(){
    QString popUpText = "PixEdit is an open-source, sprite creation tool, created by students at the University of Utah."
                        " PixEdit features a easy-to use sprite editor and the ability to make animation frames. Frames "
                        "can be exported as GIFs to be used outside of PixEdit. Any content made with PixEdit can be used"
                        " for personal or commercial uses. To get started, click on LOAD PROJECT to open a pre-existing "
                        "PixEdit project, or create a new Project by entering the desired resolution and project name.";
    QMessageBox::information(this, "About PixEdit", popUpText, QMessageBox::Ok);
}


