#ifndef MSGBOX_H
#define MSGBOX_H

#include "GUI.h"

///////////////////////////////
//class for displaying a short
//message for the player's
//information
///////////////////////////////

class MsgBox : public GUI
{

public:

    //////////////////////
    //public methods
    //////////////////////

    MsgBox();
    MsgBox(float w, float h, std::string m, float a);
    virtual ~MsgBox();

    void setMsg(std::string s);
    sf::Text getMsg()
    {
        return msg;
    };

    void Display();

    ////////////////////////
    //public data
    ////////////////////////



protected:

private:

    ////////////////////////
    //private data
    ////////////////////////

    float alive;

    sf::Text msg;
    sf::Text bgText;

};

#endif // MSGBOX_H
