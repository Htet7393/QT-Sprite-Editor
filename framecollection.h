//CS3505 A7
//Alex Baburnic, Bradley Lund, Htet Naing

//This class handles all of the attributes of a frame collection class.

#ifndef FRAMECOLLECTION_H
#define FRAMECOLLECTION_H

#include "frame.h"

class FrameCollection
{
public:
    //Frame collection constructors
    FrameCollection();
    FrameCollection(int,int);
    //Frame managment
    void addFrame(int);
    void removeFrame(int);
    void setSize(int,int);
    //Accessors
    int length();
    Frame& getFrame(int);
    bool empty();
    int getCurrentSelected();
    void setCurrentSelected(int);

private:
    std::vector<Frame> frameArray;
    int currentSelectedFrame;
    int width;
    int height;
};

#endif // FRAMECOLLECTION_H
