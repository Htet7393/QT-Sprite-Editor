//CS3505 A7
//Alex Baburnic, Bradley Lund, Htet Naing

//This class handles all of the attributes of a frame object.

#include "frame.h"

using namespace std;
Frame::Frame()
{

}

Frame::Frame(int width, int height){
    framePixels = vector<vector<QColor>>(height);
    for(int i = 0; i < width; i++){
        framePixels[i] = vector<QColor>(width);
        for(int j = 0; j < height; j++){
            framePixels[i][j] = QColor(0,0,0,0);
        }
    }
    frameWidth = width;
    frameHeight = height;
}

/**
 * Set frame dimension
 * @param width
 * @param height
 */
void Frame::setFrameDim(int w, int h){
    frameHeight = h;
    frameWidth = w;
    vector<vector<QColor>> newFrame(h);
    //make a new empty canvas
    for(auto v : newFrame){
        vector<QColor> emptyvec(w);
        v = emptyvec;
    }
    //copy over pixels using std::min to prevent an out of bounds
    for(int i = 0; i < std::min(h, (int)framePixels.capacity()); i++){
        for(int j = 0; j < std::min(w, (int)framePixels[0].capacity()); j++){
            newFrame[i][j] = framePixels[i][j];
        }
    }
    framePixels = newFrame;
}

/**
 * Set the frame's color at a specific pixel
 * @param x coordinate of pixel
 * @param y coordinate of pixel
 * @param QColor
 */
void Frame::setPixelColor(int x, int y, QColor c){
    framePixels[x][y] = c;
}

/**
 * Get width of frame
 * @return width of frame
 */
int Frame::getWidth(){
    return frameWidth;
}

/**
 * Get height of frame
 * @return height of frame
 */
int Frame::getHeight(){
    return frameHeight;
}

/**
 * Number of rows of the frame
 * @return number of rows
 */
int Frame::numRows(){
    return framePixels.size();
}

/**
 * Number of columns of the frame
 * @return number of columns
 */
int Frame::numColumns(){
    return framePixels[0].size();
}

/**
 * Get color of a pixel at a specific coordinate
 * @param x coordinate of pixel
 * @param y coordinate of pixel
 * @return QColor
 */
QColor Frame::getPixelColor(int x, int y){
    return framePixels[x][y];
}

/**
 * Add stored graphic scene items to this frame
 * @param index at which graphic scene items are stored
 * @return a vector
 */
vector<QColor>& Frame::operator[](int index){
    return framePixels[index];
}
