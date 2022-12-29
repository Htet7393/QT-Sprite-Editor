//CS3505 A7
//Alex Baburnic, Bradley Lund, Htet Naing

//This class handles all of the main window operations and connections regarding the editor, animation preview, and frame managment.

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "framecollection.h"
#include <QGraphicsScene>
#include "editormodel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    //Default constructors
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    //Read and write file
    void writeFile(std::string);
    void readFile(std::string);
    //Mouse press event and editing modes
    void mousePressEvent(QMouseEvent *);
    void penEditingMode(qreal, qreal, int);
    void fillEditingMode(qreal, qreal);
    void eraseEditingMode(qreal,qreal,int);
    void eraseAllEditingMode();
    void rectangleEditingMode(QPointF);
    //Load frame in preview and window title
    void loadFrameInPreview();
    QString title;

public slots:
    //Save and load files
    void openSaveDialog();
    void openLoadDialog();
    //Clear editor scene
    void loadFrameInEditor(int index);
    void clearEditorScene();
    //Editor modes
    void setPenEditorToolMode();;
    void setEraseEditorToolMode();
    void setFillEditorToolMode();
    void setRectEditorToolMode();
    //Frame navigation
    void nextFrameInCollection();
    void previousFrameInCollection();
    void firstFrameInCollection();
    void lastFrameInCollection();
    //Add and remove frame
    void addFrameToCollection();
    void removeFrameFromCollection();
    //Grid and onion skin toggle
    void onionSkinToggle();
    void gridToggle();
    //Misc methods
    void setNewFrameCollection(int);
    void openColorDialog();
    void playPauseAnimation();
    void animationSliderValChanged();
    void helpPopUp();
private:
    //Ui and graphics scenes
    Ui::MainWindow *ui;
    QGraphicsScene *editorScene;
    QGraphicsScene *previewScene;
    //Editor model and animation variables
    EditorModel editModel;
    int animationSpeed;
    int previewFrameIndex;
    //Icons and images
    QImage iconTitle,iconFirst,iconLast,iconLeft,
    iconRight,iconAdd,iconRemove,iconPen,iconEraser,iconFill, iconRectangle;
    //Is file saved
    bool saved = true;
    //Saved items on the graphics scene
    std::vector<std::vector<QGraphicsItem*>> pixels, onionSkin;
    std::vector<QGraphicsItem*> gridLines;
    //Reset editor buttons and draw rectangles
    void resetEditorToolButtons();
    bool drawRect = false;
    QPointF initialMousePosition;
};
#endif // MAINWINDOW_H
