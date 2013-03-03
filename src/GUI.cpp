#include "../include/GUI.h"

GUI::GUI():
    width(0),height(0)
{
    //ctor
}

GUI::GUI(float w, float h)
{
    width = w;
    height = h;
}

GUI::~GUI()
{
    //dtor
}

void GUI::Update()
{

}

void GUI::Display()
{
    guiWin.Create(sf::VideoMode(width,height,32), "GUI", sf::Style::None);
    sf::Event event;

    bool running = true;

    while(running)
    {
        while(guiWin.PollEvent(event))
        {
            if(event.Type == sf::Event::KeyPressed)
            {
                switch(event.Key.Code)
                {
                    case sf::Keyboard::Escape:
                    {
                        guiWin.Close();
                        return;
                    }
                }
            }
        }
        guiWin.Clear(sf::Color(0,0,0,255));

        guiWin.Display();
    }
    return;
}
