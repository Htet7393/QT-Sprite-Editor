//CS3505 A7
//Alex Baburnic, Bradley Lund, Htet Naing

//This class handles all of the attributes of a frame collection class.
#include "framecollection.h"

FrameCollection::FrameCollection()
{
    currentSelectedFrame = 0;
}

FrameCollection::FrameCollection(int _width, int _height){
    width = _width;
    height = _height;
    currentSelectedFrame = 0;
}

/**
 * Add a frame to the collection
 * @param index to add frames at
 */
void FrameCollection::addFrame(int index){
    Frame f(width, height);
    std::vector<Frame>::iterator it = frameArray.begin();
    frameArray.insert(it+index, f);
}

/**
 * Remove a frame from the collection
 * @param index to remove frames at
 */
void FrameCollection::removeFrame(int index){
    std::vector<Frame>::iterator it = frameArray.begin();
    frameArray.erase(it+index);
}

/**
 * Set size of frames within the collection
 * @param width of frames
 * @param height of frames
 */
void FrameCollection::setSize(int _width, int _height){
    width = _width;
    height = _height;
}

/**
 * Total number of frames in collection
 * @return length of frame collection
 */
int FrameCollection::length(){
    return frameArray.size();
}

/**
 * Get frame at an index
 * @param index to get frame at
 * @return frame reference
 */
Frame& FrameCollection::getFrame(int index){
    currentSelectedFrame = index;
    return frameArray[index];
}

/**
 * Is the frame collection emtpy?
 * @return bool
 */
bool FrameCollection::empty(){
    return frameArray.empty();
}

/**
 * Get current selected frame index
 * @return frame index
 */
int FrameCollection::getCurrentSelected(){
    return currentSelectedFrame;
}

/**
 * Set current selected frame index
 * @param frame index
 */
void FrameCollection::setCurrentSelected(int i){
    currentSelectedFrame = i;
}
