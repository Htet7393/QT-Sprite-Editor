//CS3505 A7
//Alex Baburnic, Bradley Lund, Htet Naing

//This class handles all of the main window operations and connections regarding the editor, animation preview, and frame managment.
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QColorDialog>
#include <iostream>
#include <fstream>
#include <string>
#include <QFile>
#include <QUrl>
#include <QFileDialog>
#include <QMouseEvent>
#include <QGraphicsRectItem>
#include <QTimer>
#include <QString>
#include <QMessageBox>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , iconTitle(":/images/PixEditLogoV2.PNG")
    , iconFirst(":/images/goToFirst.png")
    , iconLast(":/images/goToLast.png")
    , iconLeft(":/images/goPrevious.png")
    , iconRight(":/images/goNext.png")
    , iconAdd(":/images/add.png")
    , iconRemove(":/images/remove.png")
    , iconPen(":/images/pen.png")
    , iconEraser(":/images/eraser.png")
    , iconFill(":/images/paintBucket.png")
    , iconRectangle(":/images/rectangle.png")
{
    ui->setupUi(this);
    this->setFixedSize(908,632); //600,350
    editorScene = new QGraphicsScene(this);
    previewScene = new QGraphicsScene(this);
    ui->spriteGraphicsEditor->setScene(editorScene);
    ui->animationPreview->setScene(previewScene);
    this->setWindowIcon(QIcon(QPixmap::fromImage(iconTitle)));
    //Create default border
    QBrush emptyBrush(Qt::transparent);
    QPen blackpen(Qt::black);
    blackpen.setWidth(1);
    editorScene->addRect(0,0,512,512,blackpen,emptyBrush);
    previewScene->addRect(0,0,125,125,blackpen,emptyBrush);
    //Default editor settings
    ui->penToolButton->setEnabled(false);
    ui->currentColorLabel->setStyleSheet("QLabel { background-color : black}");
    animationSpeed = 0;
    previewFrameIndex = 0;
    //Menu bar connections
    connect(ui->actionSaveAs, &QAction::triggered, this, &MainWindow::openSaveDialog);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::openLoadDialog);
    connect(ui->actionHelp, &QAction::triggered, this, &MainWindow::helpPopUp);
    //Editor toolbox connections
    connect(ui->penToolButton, &QPushButton::clicked, this, &MainWindow::setPenEditorToolMode);
    connect(ui->eraseToolButton, &QPushButton::clicked, this, &MainWindow::setEraseEditorToolMode);
    connect(ui->fillToolButton, &QPushButton::clicked, this, &MainWindow::setFillEditorToolMode);
    connect(ui->squareToolButton, &QPushButton::clicked, this, &MainWindow::setRectEditorToolMode);
    connect(ui->openColorDialog, &QPushButton::clicked, this, &MainWindow::openColorDialog);
    connect(ui->enableOnionCheckBox, &QCheckBox::released, this, &MainWindow::onionSkinToggle);
    connect(ui->enableGridCheckBox, &QCheckBox::released, this, &MainWindow::gridToggle);
    //Frame panel connections
    connect(ui->goToFirstFrameButton, &QPushButton::clicked, this, &MainWindow::firstFrameInCollection);
    connect(ui->goToLastFrameButton, &QPushButton::clicked, this, &MainWindow::lastFrameInCollection);
    connect(ui->nextFrameButton, &QPushButton::clicked, this, &MainWindow::nextFrameInCollection);
    connect(ui->previousFrameButton, &QPushButton::clicked, this, &MainWindow::previousFrameInCollection);
    connect(ui->addFrameButton, &QPushButton::clicked, this, &MainWindow::addFrameToCollection);
    connect(ui->removeFrameButton, &QPushButton::clicked, this, &MainWindow::removeFrameFromCollection);
    connect(ui->playPauseButton, &QPushButton::clicked, this, &MainWindow::playPauseAnimation);
    connect(ui->animationSlider, &QSlider::valueChanged, this, &MainWindow::animationSliderValChanged);
    //Set editor icons
    ui->goToFirstFrameButton->setIcon(QIcon(QPixmap::fromImage(iconFirst)));
    ui->goToLastFrameButton->setIcon(QIcon(QPixmap::fromImage(iconLast)));
    ui->nextFrameButton->setIcon(QIcon(QPixmap::fromImage(iconRight)));
    ui->previousFrameButton->setIcon(QIcon(QPixmap::fromImage(iconLeft)));
    ui->addFrameButton->setIcon(QIcon(QPixmap::fromImage(iconAdd)));
    ui->removeFrameButton->setIcon(QIcon(QPixmap::fromImage(iconRemove)));
    ui->penToolButton->setIcon(QIcon(QPixmap::fromImage(iconPen)));
    ui->eraseToolButton->setIcon(QIcon(QPixmap::fromImage(iconEraser)));
    ui->fillToolButton->setIcon(QIcon(QPixmap::fromImage(iconFill)));
    ui->squareToolButton->setIcon(QIcon(QPixmap::fromImage(iconRectangle)));
}

MainWindow::~MainWindow()
{
    delete ui;
    for(auto p : pixels){ //FIX WARNING HERE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        for(auto _p : p){
            delete _p;
        }
    }
    for(auto o : onionSkin){ //FIX WARNING HERE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        for(auto _o : o){
            delete _o;
        }
    }
}

/**
 * Creates a new collection of frames and sets the view to the first frame.
 *
 * @param frameDim - the dimension of the the collection's frames, in pixels
 */
void MainWindow::setNewFrameCollection(int frameDim){
    editModel.createFrameCollection(frameDim);
    for(int i = 0; i < frameDim; i++){
        pixels.push_back(vector<QGraphicsItem*>());
        onionSkin.push_back(vector<QGraphicsItem*>());
        //create a vertical and horizontal gridline
        gridLines.push_back(new QGraphicsRectItem(i*editModel.getPixelOffset(),
                                                  0,
                                                  0,
                                                  editModel.getPixelOffset()*frameDim));
        gridLines.push_back(new QGraphicsRectItem(0,
                                                  i*editModel.getPixelOffset(),
                                                  editModel.getPixelOffset()*frameDim,
                                                  0));
        //add "dummy" values to the pixel and onion skin arrays
        for(int j = 0; j < frameDim; j++){
            pixels[i].push_back(new QGraphicsRectItem());
            onionSkin[i].push_back(new QGraphicsRectItem());
        }
    }
    editModel.addFrameToCollection(-1);
    ui->currentFrameLabel->setText(QString::number(editModel.getCurrentFrameIndex()+1) + "/" + QString::number(editModel.getFrameCollectionLength()));
}

/**
 * Loads the frame at the given index into the editor.
 *
 * @param frameIndex - the index of the frame to load
 */
void MainWindow::loadFrameInEditor(int frameIndex){
    Frame thisFrame = editModel.getFrame(frameIndex);
    for(int i = 0; i < thisFrame.getHeight(); i++){
        for(int j = 0; j < thisFrame.getWidth(); j++){
            QPen pixelPen(thisFrame.getPixelColor(i,j));
            QBrush pixelBrush(thisFrame.getPixelColor(i,j));
            //Create an rectangle visual item
            QGraphicsRectItem* item = new QGraphicsRectItem(i*editModel.getPixelOffset(),
                                                            j*editModel.getPixelOffset(),
                                                            editModel.getPixelOffset(),
                                                            editModel.getPixelOffset());
            item->setBrush(pixelBrush);
            item->setPen(pixelPen);
            //add it to the scene and give direct access
            editorScene->addItem(item);
            pixels[i][j] = item;
        }
    }
    if(ui->enableOnionCheckBox->isChecked() == true){
        ui->enableOnionCheckBox->setCheckState(Qt::CheckState::Unchecked);
    }
    ui->enableGridCheckBox->setCheckState(Qt::CheckState::Unchecked);
}

/**
 * Handler for pressing the mouse button.
 * Checks if the user clicked inside the sprite editor window, then applies an action based on which painting mode they picked.
 *
 */
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        // Detect if the click is in the view and adjust mouse coordinates
        QPoint remapped = ui->spriteGraphicsEditor->mapFromParent( event->pos() );
        QRect rect = ui->spriteGraphicsEditor->rect();
        rect.adjust(1,22,1,20);
        // If graphic view containes mouse coordinates
        if (rect.contains( remapped ) ){
            //Offset mouse coordinates to fit view
            QPointF mousePoint = ui->spriteGraphicsEditor->mapToScene( remapped );
            QPointF offset(0,22);
            mousePoint -= offset;
            if (mousePoint.x() > 0 && mousePoint.x() < (511) && mousePoint.y() > 0 && mousePoint.y() < (511)){
                saved = false;
                setWindowTitle(title+"*");
                // 0 = pen
                // 1 = eraser
                // 2 = fill
                // 3 = rectangle
                switch(editModel.getCurrentEditorMode()){
                    case 0:
                        drawRect = false;
                        MainWindow::penEditingMode(mousePoint.x(), mousePoint.y(), ui->penSizeSpinBox->value());
                        break;
                    case 1:
                        drawRect = false;
                        MainWindow::eraseEditingMode(mousePoint.x(), mousePoint.y(), ui->penSizeSpinBox->value());
                        break;
                    case 2:
                        drawRect = false;
                        MainWindow::fillEditingMode(mousePoint.x(), mousePoint.y());
                        break;
                    case 3:
                        MainWindow::rectangleEditingMode(mousePoint);
                        break;
                }
            }
        }
    }
}

/**
 * Draws a rectangle by first listening for a second point, then filling in all pixels in between those two points.
 *
 * @param mousePoint - the starting position of the rectangle
 */
void MainWindow::rectangleEditingMode(QPointF mousePoint){
    if(drawRect)
    {
        drawRect = false;
        qreal startX = initialMousePosition.x();
        qreal startY = initialMousePosition.y();
        qreal length = abs(initialMousePosition.x() - mousePoint.x());
        qreal height = abs(initialMousePosition.y() - mousePoint.y());
        int startPosX = floor(startX/editModel.getPixelOffset())*editModel.getPixelOffset();
        int startPosY = floor(startY/editModel.getPixelOffset())*editModel.getPixelOffset();
        int lengthOffset = floor(length/editModel.getPixelOffset())*editModel.getPixelOffset();
        int heightOffset = floor(height/editModel.getPixelOffset())*editModel.getPixelOffset();
        int lengthCount = lengthOffset/editModel.getPixelOffset();
        int heightCount = heightOffset/editModel.getPixelOffset();
        QPen pixelPen(editModel.getCurrentEditingColor());
        QBrush pixelBrush(editModel.getCurrentEditingColor());
        pixelPen.setWidth(1);
        for(int i = 0; i <= lengthCount; i++)
        {
            int axisDirX = i;
            if(initialMousePosition.x() > mousePoint.x())
            {
                axisDirX = axisDirX*(-1);
            }
            for(int j = 0; j <= heightCount; j++)
            {
                int axisDirY = j;
                if(initialMousePosition.y() > mousePoint.y())
                {
                    axisDirY = axisDirY*(-1);
                }
                int offsetX = startPosX/editModel.getPixelOffset();
                int offsetY = startPosY/editModel.getPixelOffset();
                //Create an rectangle visual item
                QGraphicsRectItem* item = new QGraphicsRectItem(startPosX + (editModel.getPixelOffset() * axisDirX),
                                                                startPosY + (editModel.getPixelOffset() * axisDirY),
                                                                editModel.getPixelOffset(),
                                                                editModel.getPixelOffset());
                item->setBrush(pixelBrush);
                item->setPen(pixelPen);
                //add it to the scene and give direct access
                editorScene->addItem(item);
                pixels[offsetX + axisDirX][offsetY + axisDirY] = item;
                editModel.getCurrentFrame().setPixelColor(offsetX + axisDirX,offsetY + axisDirY,editModel.getCurrentEditingColor());
            }
        }
    }
    else
    {
        initialMousePosition = mousePoint;
        drawRect = true;
    }
}

//Editing Modes

/**
 * Draws a pixel(s) on the screen at the given coordinate.
 * When using a pen size greater than 1, the click location serves at the top left corner for drawing.
 *
 * @param mouseX - the x-position of the mouse
 * @param mouseY - the y-position of the mouse
 * @param penSize - the pen size being used
 */
void MainWindow::penEditingMode(qreal mouseX, qreal mouseY, int penSize){
    QPen pixelPen(editModel.getCurrentEditingColor());
    QBrush pixelBrush(editModel.getCurrentEditingColor());
    pixelPen.setWidth(1);
    //rounds the coordinates down to the nearest pixel points
    int mousePosX = floor(mouseX/editModel.getPixelOffset())*editModel.getPixelOffset();
    int mousePosY = floor(mouseY/editModel.getPixelOffset())*editModel.getPixelOffset();
    for(int i = 0; i < penSize; i++){
        for(int j = 0; j < penSize; j++){
            int adjustedMousePosX = mousePosX/editModel.getPixelOffset();
            int adjustedMousePosY = mousePosY/editModel.getPixelOffset();
            if(i+adjustedMousePosX < editModel.getCurrentFrame().getWidth() && i+adjustedMousePosX >= 0){
                if(j+adjustedMousePosY < editModel.getCurrentFrame().getHeight() && j+adjustedMousePosX >= 0){
                    //Create an rectangle visual item
                    QGraphicsRectItem* item = new QGraphicsRectItem(mousePosX+(i*editModel.getPixelOffset()),
                                                                    mousePosY+(j*editModel.getPixelOffset()),
                                                                    editModel.getPixelOffset(),
                                                                    editModel.getPixelOffset());
                    item->setBrush(pixelBrush);
                    item->setPen(pixelPen);
                    //add it to the scene and give direct access
                    editorScene->addItem(item);
                    pixels[i+adjustedMousePosX][j+adjustedMousePosY] = item;
                    editModel.getCurrentFrame().setPixelColor(i+adjustedMousePosX,j+adjustedMousePosY,editModel.getCurrentEditingColor());
                }
            }
        }
    }
}

/**
 * Erases a pixel(s) on the screen at the given coordinate.
 * When using a pen size greater than 1, the click location serves at the top left corner for erasing.
 *
 * @param mouseX - the x-position of the mouse
 * @param mouseY - the y-position of the mouse
 * @param penSize - the pen size being used
 */
void MainWindow::eraseEditingMode(qreal mouseX, qreal mouseY, int penSize){
    //rounds the coordinates down to the nearest pixel points
    int mousePosX = floor(mouseX/editModel.getPixelOffset())*editModel.getPixelOffset();
    int mousePosY = floor(mouseY/editModel.getPixelOffset())*editModel.getPixelOffset();
    for(int i = 0; i < penSize; i++){
        for(int j = 0; j < penSize; j++){
            int adjustedMousePosX = mousePosX/editModel.getPixelOffset();
            int adjustedMousePosY = mousePosY/editModel.getPixelOffset();
            if(i+adjustedMousePosX < editModel.getCurrentFrame().getWidth() && i+adjustedMousePosX >= 0){
                if(j+adjustedMousePosY < editModel.getCurrentFrame().getHeight() && j+adjustedMousePosY >= 0){
                    editorScene->removeItem(pixels[i+adjustedMousePosX][j+adjustedMousePosY]);
                    pixels[i+adjustedMousePosX][j+adjustedMousePosY] = new QGraphicsRectItem();
                    editModel.getCurrentFrame().setPixelColor(i+adjustedMousePosX,j+adjustedMousePosY,Qt::transparent);
                }
            }
        }
    }
    MainWindow::clearEditorScene();
    MainWindow::loadFrameInEditor(editModel.getCurrentFrameIndex());
}

/**
 * Fills in the clicked area, much like a paint bucket tool.
 * Only fills in pixels that are the same color as the one you intially clicked on.
 *
 * @param mouseX - the x-position of the mouse
 * @param mouseY - the y-position of the mouse
 */
void MainWindow::fillEditingMode(qreal mouseX, qreal mouseY){
    QPen pixelPen(editModel.getCurrentEditingColor());
    QBrush pixelBrush(editModel.getCurrentEditingColor());
    QPen& pen = pixelPen;
    QBrush& brush = pixelBrush;
    pixelPen.setWidth(1);
    //rounds the coordinates down to the nearest pixel points
    int mousePosX = floor(mouseX/editModel.getPixelOffset())*editModel.getPixelOffset();
    int mousePosY = floor(mouseY/editModel.getPixelOffset())*editModel.getPixelOffset();
    editModel.paintBucket(mousePosX, mousePosY, pen, brush, editorScene);
}

/**
 * Completely erases the sprite panel.
 */
void MainWindow::eraseAllEditingMode(){
    MainWindow::clearEditorScene();
    for(int i = 0; i < editModel.getCurrentFrame().getWidth(); i++){
        for(int j = 0; j < editModel.getCurrentFrame().getHeight(); j++){
            editModel.getCurrentFrame().setPixelColor(i,j,QColor(0,0,0,0));
        }
    }
}

/**
 * Unchecks the onion skin and grid checkboxes.
 * Removes all visual elements from the scene.
 * Then redraws the canvas border.
 */
void MainWindow::clearEditorScene(){
    if(ui->enableOnionCheckBox->isChecked() == true){
        ui->enableOnionCheckBox->setCheckState(Qt::CheckState::Unchecked);
        onionSkinToggle();
    }
    ui->enableGridCheckBox->setCheckState(Qt::CheckState::Unchecked);
    gridToggle();
    editorScene->clear();
    QBrush emptyBrush(Qt::transparent);
    QPen blackpen(Qt::black);
    blackpen.setWidth(1);
    //add the boundary back into the window
    editorScene->addRect(0,0,512,512,blackpen,emptyBrush);
}

//Frame collection buttons

/**
 * Adds a new frame to the editor, then displays it.
 */
void MainWindow::addFrameToCollection(){
    editModel.addFrameToCollection(editModel.getCurrentFrameIndex());
    editModel.setCurrentFrame(editModel.getCurrentFrameIndex()+1);
    ui->currentFrameLabel->setText(QString::number(editModel.getCurrentFrameIndex()+1) + "/" + QString::number(editModel.getFrameCollectionLength()));
    MainWindow::clearEditorScene();
}

/**
 * Deletes the current frame from the editor.
 */
void MainWindow::removeFrameFromCollection(){
    editModel.removeFrameFromCollection(editModel.getCurrentFrameIndex());
    editModel.setCurrentFrame(editModel.getCurrentFrameIndex()-1);
    ui->currentFrameLabel->setText(QString::number(editModel.getCurrentFrameIndex()+1) + "/" + QString::number(editModel.getFrameCollectionLength()));
    MainWindow::clearEditorScene();
    MainWindow::loadFrameInEditor(editModel.getCurrentFrameIndex());
}

/**
 * Loads in the next frame in the editor.
 */
void MainWindow::nextFrameInCollection(){
    editModel.setCurrentFrame(editModel.getCurrentFrameIndex()+1);
    ui->currentFrameLabel->setText(QString::number(editModel.getCurrentFrameIndex()+1) + "/" + QString::number(editModel.getFrameCollectionLength()));
    MainWindow::clearEditorScene();
    MainWindow::loadFrameInEditor(editModel.getCurrentFrameIndex());
}

/**
 * Loads in the previous frame in the editor.
 */
void MainWindow::previousFrameInCollection(){
    editModel.setCurrentFrame(editModel.getCurrentFrameIndex()-1);
    ui->currentFrameLabel->setText(QString::number(editModel.getCurrentFrameIndex()+1) + "/" + QString::number(editModel.getFrameCollectionLength()));
    MainWindow::clearEditorScene();
    MainWindow::loadFrameInEditor(editModel.getCurrentFrameIndex());
}

/**
 * Loads in the first frame in the editor.
 */
void MainWindow::firstFrameInCollection(){
    editModel.setCurrentFrame(0);
    ui->currentFrameLabel->setText(QString::number(editModel.getCurrentFrameIndex()+1) + "/" + QString::number(editModel.getFrameCollectionLength()));
    MainWindow::clearEditorScene();
    MainWindow::loadFrameInEditor(editModel.getCurrentFrameIndex());
}

/**
 * Loads in the last frame in the editor.
 */
void MainWindow::lastFrameInCollection(){
    editModel.setCurrentFrame(editModel.getFrameCollectionLength()-1);
    ui->currentFrameLabel->setText(QString::number(editModel.getCurrentFrameIndex()+1) + "/" + QString::number(editModel.getFrameCollectionLength()));
    MainWindow::clearEditorScene();
    MainWindow::loadFrameInEditor(editModel.getCurrentFrameIndex());
}

//Pen editor mode selection

/**
 * Re-enables the editor tool buttons.
 */
void MainWindow::resetEditorToolButtons(){
    ui->penToolButton->setEnabled(true);
    ui->eraseToolButton->setEnabled(true);
    ui->fillToolButton->setEnabled(true);
    ui->squareToolButton->setEnabled(true);
}

/**
 * Sets the current editing mode to the pen.
 */
void MainWindow::setPenEditorToolMode(){
    editModel.setCurrentEditorMode(0);
    MainWindow::resetEditorToolButtons();
    ui->penToolButton->setEnabled(false);
}

/**
 * Sets the current editing mode to the eraser.
 */
void MainWindow::setEraseEditorToolMode(){
    editModel.setCurrentEditorMode(1);
    MainWindow::resetEditorToolButtons();
    ui->eraseToolButton->setEnabled(false);
}

/**
 * Sets the current editing mode to the fill tool.
 */
void MainWindow::setFillEditorToolMode(){
    editModel.setCurrentEditorMode(2);
    MainWindow::resetEditorToolButtons();
    ui->fillToolButton->setEnabled(false);
}

/**
 * Sets the current editing mode to the rectangle tool.
 */
void MainWindow::setRectEditorToolMode(){
    editModel.setCurrentEditorMode(3);
    MainWindow::resetEditorToolButtons();
    ui->squareToolButton->setEnabled(false);
}

/**
 * Opens the color picker dialogue and saves the picked color to use later.
 */
void MainWindow::openColorDialog(){
    editModel.setCurrentEditingColor(QColorDialog::getColor());
    ui->currentColorLabel->setStyleSheet("background-color: #" + QString::number(editModel.getCurrentEditingColor().rgb(), 16));
}

/**
 * Toggles the "onion skin" effect.
 * This will display a semi-transparent view of the previous frame, if able.
 */
void MainWindow::onionSkinToggle(){
    //Turn off onion skin
    if(!ui->enableOnionCheckBox->isChecked()){
        MainWindow::clearEditorScene();
        MainWindow::loadFrameInEditor(editModel.getCurrentFrameIndex());
    }
    //Turn on onion skin
    else{
        //Has multiple frames
        if(editModel.getCurrentFrameIndex() > 0){
            QBrush emptyBrush(Qt::transparent);
            QPen borderPen(Qt::black);
            editorScene->clear();
            editorScene->addRect(0,0,512,512,borderPen,emptyBrush);
            Frame onionFrame = editModel.getFrame(editModel.getCurrentFrameIndex()-1);
            for(int i = 0; i < onionFrame.getHeight(); i++){
                for(int j = 0; j < onionFrame.getWidth(); j++){
                    QColor onionColor = onionFrame.getPixelColor(i,j);
                    if (onionColor.alpha() == 255){
                        onionColor.setAlpha(70);
                    }
                    QPen pixelPen(onionColor);
                    QBrush pixelBrush(onionColor);
                    editorScene->addRect(i*editModel.getPixelOffset(), j*editModel.getPixelOffset(), editModel.getPixelOffset(), editModel.getPixelOffset(), pixelPen, pixelBrush);
                }
            }
            Frame thisFrame = editModel.getCurrentFrame();
            for(int i = 0; i < thisFrame.getHeight(); i++){
                for(int j = 0; j < thisFrame.getWidth(); j++){
                    QPen pixelPen(thisFrame.getPixelColor(i,j));
                    QBrush pixelBrush(thisFrame.getPixelColor(i,j));
                    QGraphicsRectItem* item = new QGraphicsRectItem(i*editModel.getPixelOffset(),
                                                                    j*editModel.getPixelOffset(),
                                                                    editModel.getPixelOffset(),
                                                                    editModel.getPixelOffset());
                    item->setBrush(pixelBrush);
                    item->setPen(pixelPen);
                    editorScene->addItem(item);
                    pixels[i][j] = item;
                }
            }
        }
        //Only one frame
        else{
            ui->enableOnionCheckBox->setChecked(false);
        }
    }
}

/**
 * Toggles the visability for the editor grid view.
 * The grid view shows a grid to make editing easier.
 */
void MainWindow::gridToggle(){
    if(!ui->enableGridCheckBox->isChecked()){
        for(int i = 0; i < (int)gridLines.size(); i++)
            editorScene->removeItem(gridLines[i]);
        }
    else{
        for(int i = 0; i < (int)gridLines.size(); i++){
            editorScene->addItem(gridLines[i]);
        }
    }
}

/**
 * Plays/pauses the animation in the sprite editor window at the given FPS.
 */
void MainWindow::playPauseAnimation(){
    //Play animation
    if(animationSpeed == 0){
        animationSpeed = ui->animationSlider->value();
        QTimer::singleShot(1000/animationSpeed, this, &MainWindow::loadFrameInPreview);
        ui->animationSlider->setEnabled(false);
    }
    //Pause animation
    else{
        animationSpeed = 0;
        ui->animationSlider->setEnabled(true);
    }
}

/**
 * Updates the visual component of the FPS bar to match the FPS value.
 */
void MainWindow::animationSliderValChanged(){
    ui->fpsAnimLabel->setText(QString::number(ui->animationSlider->value())+" FPS");
}

/**
 * Loads a scene within the preview frame than recurses itself to draw additional frames if needed.
 */
void MainWindow::loadFrameInPreview(){
    QBrush emptyBrush(Qt::transparent);
    QPen borderPen(Qt::black);
    previewScene->clear();
    previewScene->addRect(0,0,125,125,borderPen,emptyBrush);
    Frame thisFrame = editModel.getFrame(previewFrameIndex);
    for(int i = 0; i < thisFrame.getHeight(); i++){
        for(int j = 0; j < thisFrame.getWidth(); j++){
            QPen pixelPen(thisFrame.getPixelColor(i,j));
            QBrush pixelBrush(thisFrame.getPixelColor(i,j));
            previewScene->addRect((i*editModel.getPixelOffset())/4.096, (j*editModel.getPixelOffset())/4.096, editModel.getPixelOffset()/4.096, editModel.getPixelOffset()/4.096, pixelPen, pixelBrush);
        }
    }
    previewFrameIndex++;
    if(previewFrameIndex >= editModel.getFrameCollectionLength()){
        previewFrameIndex = 0;
    }
    if (animationSpeed != 0){
        QTimer::singleShot(1000/animationSpeed, this, &MainWindow::loadFrameInPreview);
    }
}

//File reading and writing methods

/**
 * Opens the file saving window and optionally saves the file using the given name.
 */
void MainWindow::openSaveDialog(){
    QString list = this->windowTitle();

    //deletes the asterick from the save
    if(list[list.size()-1]=='*'){
        list.chop(1);
    }
    QString filePath = QFileDialog::getSaveFileName(nullptr, "Project Save As", list, "Sprite Sheet Project (*.ssp)");
    MainWindow::writeFile(filePath.toStdString());
    title = QUrl::fromLocalFile(filePath).fileName();
    setWindowTitle(title);
}

/**
 * Opens the file saving window and optionally loads the selected file into the editor.
 */
void MainWindow::openLoadDialog(){
    QString filePath = QFileDialog::getOpenFileName(nullptr, "Open Project", ".", "Sprite Sheet Project (*.ssp)");
    MainWindow::readFile(filePath.toStdString());
    title = QUrl::fromLocalFile(filePath).fileName();
    setWindowTitle(title);
}

/**
 * Parses the given .ssp file into the editor view.
 * @param path - the path to an .ssp file
 */
void MainWindow::readFile(string path){
    //Get the file path and set default frame values
    QFile file(QString::fromStdString(path));
    std::cout << file.exists() << std::endl;
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)){
        int frameHeight = 0;
        int frameWidth = 0;
        //parse the height, width, number of frames, and finally frame data
        while (!file.atEnd()) {
            QString line = file.readLine();
            if(line.contains("height:")){
                QStringList heightVal = line.split(" ");
                frameHeight = heightVal.at(1).toInt();
            }
            if(line.contains("width:")){
                QStringList WidthVal = line.split(" ");
                frameWidth = WidthVal.at(1).toInt();
                editModel.createFrameCollection(frameHeight);
                editModel.setSize(frameWidth, frameHeight);

                for(int i = 0; i < frameWidth; i++){
                    pixels.push_back(vector<QGraphicsItem*>());
                    onionSkin.push_back(vector<QGraphicsItem*>());

                    for(int j = 0; j < frameHeight; j++){
                        pixels[i].push_back(new QGraphicsRectItem());
                        onionSkin[i].push_back(new QGraphicsRectItem());
                    }
                }
                editModel.addFrameToCollection(-1);
            }
            if(line.contains("numberOfFrames:")){
                QStringList frameCount = line.split(" ");
                int sizeOfCurrentFCollection = editModel.getFrameCollectionLength();
                for(int i = sizeOfCurrentFCollection; i < (frameCount.at(1).toInt()-sizeOfCurrentFCollection+1); i++){
                    editModel.addFrameToCollection(i-1);
                }
            }
            if(line.contains("frames:")){
                int frameIndex = -1;
                int frameX = 0;
                int frameY = 0;
                while(!file.atEnd()){
                    QString frameLine = file.readLine();
                    if (frameLine.contains("frame")){
                        frameIndex++;
                        frameX = 0;
                        frameY = 0;
                    }
                    if (frameLine.contains("[")){
                        QStringList pixelColVal = frameLine.split("[");
                        pixelColVal = pixelColVal.at(1).split("]");
                        pixelColVal = pixelColVal.at(0).split(",");
                        int r = pixelColVal.at(1).toInt();
                        int g = pixelColVal.at(2).toInt();
                        int b = pixelColVal.at(3).toInt();
                        int a = pixelColVal.at(0).toInt();
                        QColor thisPixelCol = QColor(r, g, b, a);
                        editModel.getFrame(frameIndex).setPixelColor(frameX, frameY, thisPixelCol);
                        frameX++;
                        if(!(frameX < editModel.getFrame(frameIndex).numRows())){
                            frameX = 0;
                            frameY++;
                        }
                    }
                }
            }
        }
        file.flush();
        file.close();
    }
    //Clear scene, set default frame, then load the scene in editor.
    MainWindow::clearEditorScene();
    MainWindow::loadFrameInEditor(0);
    ui->currentFrameLabel->setText(QString::number(editModel.getCurrentFrameIndex()+1) + "/" + QString::number(editModel.getFrameCollectionLength()));
}

/**
 * Writes the current state of the editor into an .ssp file and saves it to the given location.
 * @param path - the location to save the file
 */
void MainWindow::writeFile(string path){
    QFile file(QString::fromStdString(path));
    if(file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)){
        QTextStream out(&file);
        out << "height: " << editModel.getFrame(0).getHeight() << "\n";
        out << "width: " << editModel.getFrame(0).getWidth() << "\n";
        out << "numberOfFrames: " << editModel.getFrameCollectionLength() << "\n";
        out << "frames: " << "\n" << "{" << "\n";
        for(int i = 0; i < editModel.getFrameCollectionLength(); i++){
            out << "frame" << i << ": \n" << "{" << "\n";
            for(int j = 0; j < editModel.getFrame(i).getHeight(); j++){
                for(int k = 0; k < editModel.getFrame(i).getWidth(); k++){
                    out << "[" <<
                           editModel.getFrame(i).getPixelColor(k,j).alpha() << "," <<
                           editModel.getFrame(i).getPixelColor(k,j).red() << "," <<
                           editModel.getFrame(i).getPixelColor(k,j).green() << "," <<
                           editModel.getFrame(i).getPixelColor(k,j).blue() <<
                           "]\n";
                }
            }
            out << "}" << "\n";
        }
    }
    file.flush();
    file.close();
    saved = true;
    setWindowTitle(title);
}

/**
 * Displays the help window.
 */
void MainWindow::helpPopUp(){
    QString popUpText = "Welcome to the PixEdit sprite editor! \n"
                        "\n* To begin drawing, select the Pen tool from the toolbar. Then click anywhere on the editor window to place a pixel. \n"
                        "* To erase, pick Erase tool from the toolbar and click on any pixel in the editor window. \n"
                        "* We're all lazy sometimes! You can fill an area by clicking fill and then selecting anywhere on the editor to fill an "
                        "area with a single color. \n"
                        "* For adding rectangles, click on Rectangle from the toolbar. Click anywhere on the editor window to start the first"
                        " corner of a rectangle, then click in another area of the editor to create the rectangle. Rectangles automatically "
                        "fill in. \n"
                        "* If you want to switch pen, fill, or rectangle fill colors, click on the color picker at the bottom of window to open "
                        "up the color picker. Then select a color and click OK. You can also use the PICK SCREEN COLOR as an eye dropper or to "
                        "get any color visible on your screen. \n"
                        "* You can also change the size of your pen between 1 to 4 pixels using the pen size spin box. \n"
                        "* Sprite frames is how you can turn a still image into an animation. At the bottom is the frame "
                        "panel. You can use this to add new frames, remove frames, or navigate between frames. \n"
                        "* You'll probably want to see the previous frame as a reference when working with multiple frames. Click Enable Onion Skin"
                        " to view the previous frame. In addition, you can click on Enable Grid to view the pixel grid. \n"
                        "* You're ready to view your masterpiece! In the upper right, is a preview of your sprite. Click Play/Pause to play or pause"
                        " the animation. To change the speed of the animation, pause the animation and then change the slider (between 1 fps and 60 fps. \n"
                        "\n You're ready to begin. Have fun! Remember to click on file to save and open projects as well as view this help again.";
    QMessageBox::information(this, "How To Use PixEdit", popUpText, QMessageBox::Ok);
}
