//CS3505 A7
//Alex Baburnic, Bradley Lund, Htet Naing

//This class handles all of the attributes of a frame object.

#ifndef FRAME_H
#define FRAME_H

#include <QColor>

class Frame
{
public:
    //Constructors
    Frame();
    Frame(int,int);
    //Frame attributes & accessors
    void setFrameDim(int,int);
    void setPixelColor(int,int,QColor);
    int getWidth();
    int getHeight();
    int numRows();
    int numColumns();
    QColor getPixelColor(int,int);
    std::vector<QColor>& operator[](int);

private:
    int frameHeight;
    int frameWidth;
    std::vector<std::vector<QColor>> framePixels;
};

#endif // FRAME_H
