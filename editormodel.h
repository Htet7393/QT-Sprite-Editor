#ifndef EDITORMODEL_H
#define EDITORMODEL_H

//CS3505 A7
//Alex Baburnic, Bradley Lund, Htet Naing

//This class handles all of the attributes of an editor model class.

#include "framecollection.h"
#include "frame.h"
#include <QPen>
#include <QBrush>
#include <QGraphicsScene>
#include <unordered_set>


class EditorModel
{
public:
    EditorModel();

public slots:
    //Editor Modes
    int getCurrentEditorMode();
    void setCurrentEditorMode(int);
    int getPixelOffset();
    void paintBucket(int,int,QPen&,QBrush&,QGraphicsScene* editorScene);
    //Editing Color
    QColor getCurrentEditingColor();
    void setCurrentEditingColor(QColor);
    //Frame collection
    Frame& getCurrentFrame();
    Frame& getFrame(int);
    int getCurrentFrameIndex();
    void setCurrentFrame(int);
    void setSize(int,int);
    void addFrameToCollection(int);
    void removeFrameFromCollection(int);
    void createFrameCollection(int);
    int getFrameCollectionLength();

private:
    FrameCollection *frames;
    int currentFrame;
    QColor currentEditingColor;
    int currentEditingMode;
    double pixelOffset;
    void bucketRecursion(int,int,QColor,std::vector<std::pair<int,int>>&);
};

#endif // EDITORMODEL_H
