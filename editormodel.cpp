//CS3505 A7
//Alex Baburnic, Bradley Lund, Htet Naing

//This class handles all of the attributes of an editor model class.

#include "editormodel.h"
#include <unordered_set>

EditorModel::EditorModel()
{
    currentEditingMode = 0;
    currentEditingColor = Qt::black;
    currentFrame = 0;
}

//Frame management

/**
 * Creates a new collection of frames
 * @param sets dimension of frames
 */
void EditorModel::createFrameCollection(int frameDim){
    frames = new FrameCollection(frameDim, frameDim);
    pixelOffset = 512/frameDim;
}

/**
 * Gets the current frame
 * @return frame reference
 */
Frame& EditorModel::getCurrentFrame(){
    return frames->getFrame(currentFrame);
}

/**
 * Gets specified frame at index
 * @param frame index
 * @return frame reference
 */
Frame& EditorModel::getFrame(int index){
    return frames->getFrame(index);
}

/**
 * Sets the current frame index
 * @param frame index
 */
void EditorModel::setCurrentFrame(int frameIndex){
    if(frameIndex > -1 && frameIndex < frames->length()){
        currentFrame = frameIndex;
    }
}

/**
 * Gets the current frame index
 * @return frame index
 */
int EditorModel::getCurrentFrameIndex(){
    return currentFrame;
}

/**
 * Adds a frame to the frame collection at an index
 * @param index to add the frame at
 */
void EditorModel::addFrameToCollection(int frameIndex){
    frames->addFrame(frameIndex+1);
}

/**
 * Removes a frame at an index
 * @param index to remove frame at
 */
void EditorModel::removeFrameFromCollection(int frameIndex){
    if(frames->length() > 1){
        frames->removeFrame(frameIndex);
    }
}

/**
 * Gets the total quantity of frames in a collection
 * @return frame collection length
 */
int EditorModel::getFrameCollectionLength(){
    return frames->length();
}

//Editing mode management

/**
 * Gets the current editing mode
 * @return current editing mode (0-3)
 */
int EditorModel::getCurrentEditorMode(){
    return currentEditingMode;
}

/**
 * Sets the current editing mode
 * @param current editing mode (0-3)
 */
void EditorModel::setCurrentEditorMode(int modeIndex){
    currentEditingMode = modeIndex;
}

/**
 * Sets the size of the frames
 * @param width
 * @param height
 */
void EditorModel::setSize(int width, int height){
    frames->setSize(width,height);
}

/**
 * Gets the offset at which pixels are drawn in the editor
 * @return pixel offset
 */
int EditorModel::getPixelOffset(){
    return pixelOffset;
}

//Color management

/**
 * Gets current editing color
 * @return QColor
 */
QColor EditorModel::getCurrentEditingColor(){
    return currentEditingColor;
}

/**
 * Sets current editing color
 * @param QColor
 */
void EditorModel::setCurrentEditingColor(QColor selectedColor){
    currentEditingColor = selectedColor;
}

/**
 * Handles the pain bucket tool
 * @param posX - X position on the editor
 * @param posY - Y position on the editor
 * @param pen
 * @param brush
 * @param editorScene
 */
void EditorModel::paintBucket(int posX, int posY, QPen& pen, QBrush& brush, QGraphicsScene* editorScene){

    int adjustedPosX = posX/getPixelOffset();
    int adjustedPosY = posY/getPixelOffset();
    QColor color = getCurrentFrame().getPixelColor(adjustedPosX,adjustedPosY);

    std::vector<std::pair<int,int>> points;
    std::vector<std::pair<int,int>>& p = points;
    bucketRecursion(adjustedPosX,adjustedPosY,color,p);

    for(auto a : points){
        int x = a.first;
        int y = a.second;
        editorScene->addRect(x*getPixelOffset(), y*getPixelOffset(), getPixelOffset(), getPixelOffset(), pen, brush);
        getCurrentFrame().setPixelColor(x,y,getCurrentEditingColor());
    }
}

/**
 * @brief Recusively paint at locations
 * @param posX - X position on the editor
 * @param posY - Y position on the editor
 * @param colors - chosen color
 * @param points - points to be painted
 */
void EditorModel::bucketRecursion(int posX,int posY,QColor color,std::vector<std::pair<int,int>>& points){
    if(posX >= getCurrentFrame().getWidth() || posX < 0)
        return;
    if(posY >= getCurrentFrame().getHeight() || posY < 0)
        return;
    if(getCurrentFrame().getPixelColor(posX,posY) == color){
        if(std::find(points.begin(),points.end(),std::pair<int,int>(posX,posY)) != points.end())
            return;
        points.push_back(std::pair<int,int>(posX,posY));

        bucketRecursion(posX+1,posY,color,points);
        bucketRecursion(posX-1,posY,color,points);
        bucketRecursion(posX,posY+1,color,points);
        bucketRecursion(posX,posY-1,color,points);
    }
}
