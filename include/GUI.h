#ifndef GUI_H
#define GUI_H

#include "../globals.h"

///////////////////////
//Base class for all
//widgets and gui
//objects for use in
//Brain Man or similar
//projects
///////////////////////

class GUI
{
public:

    //////////////////////
    //public methods
    //////////////////////

    GUI();
    GUI(float w, float h);
    virtual ~GUI();

    //public setters/getters
    void setWidth(float w){width = w;};
    float getWidth(){return width;};
    void setHeight(float h){height = h;};
    float getHeight(){return height;};

    virtual void Update();

    virtual void Display();

    ////////////////////////
    //public data
    ////////////////////////

    sf::RenderWindow guiWin;
    float width;
    float height;

protected:

private:

};

#endif // GUI_H
