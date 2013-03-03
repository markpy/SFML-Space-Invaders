#include "../include/MsgBox.h"

#include "../include/RandomString.h"

MsgBox::MsgBox() :
    msg("Default"), alive(1.0f)
{
    //msg.SetCharacterSize(width/10);
    msg.SetPosition(width/8, height/6);
    msg.SetColor(sf::Color(50,200,50,200));
}

MsgBox::MsgBox(float w, float h, std::string m, float a)
{
    //set some defaults
    width = w;
    height = h;
    msg.SetString(m);
    //msg.SetCharacterSize(width/10);
    msg.SetPosition(width/8, height/6);
    msg.SetColor(sf::Color(50,200,50,200));
    alive = a;
}

MsgBox::~MsgBox()
{
    //dtor
}

void MsgBox::setMsg(std::string s)
{
    msg.SetString(s);
    //msg.SetCharacterSize(width/10);
    msg.SetPosition(width/8, height/6);
    msg.SetColor(sf::Color(50,200,50,200));
}

void MsgBox::Display()
{
    guiWin.Create(sf::VideoMode(width,height,32), "HUD", sf::Style::None);
    sf::Event event;

    bool running = true;

    //seed random number generator
    srand(time(NULL));

    //create random string object
    RandomString randStr;

    int dr = 0;
    int dg = 0;
    int db = 0;

    sf::Uint8 r = 255, g = 0,  b = 0;

    sf::Texture texture1;
    if(!texture1.LoadFromFile("images/techcircle.png"))
    {
        std::cerr<<"Error loading image"<<std::endl;
    }

    sf::Texture texture2;
    if(!texture2.LoadFromFile("images/techcircle2.png"))
    {
        std::cerr<<"Error loading image"<<std::endl;
    }

    sf::Sprite sprite1;
    sprite1.SetOrigin(50,50);
    sprite1.SetTexture(texture1);
    sprite1.SetPosition(0,0);
    sprite1.SetColor(sf::Color(255,255,255,155));

    sf::Sprite sprite2;
    sprite2.SetOrigin(50,50);
    sprite2.SetTexture(texture2);
    sprite2.SetPosition(guiWin.GetSize().x,0);
    sprite2.SetColor(sf::Color(255,255,255,155));

    sf::Sprite sprite3;
    sprite3.SetOrigin(50,50);
    sprite3.SetTexture(texture1);
    sprite3.SetPosition(guiWin.GetSize().x,guiWin.GetSize().y);
    sprite3.SetColor(sf::Color(255,255,255,155));

    sf::Font myFont;
    if(!myFont.LoadFromFile("fonts/contb.ttf"))
    {
        std::cerr<<"Could not load font"<<std::endl;
    }

    sf::Font myFont2;
    if(!myFont2.LoadFromFile("fonts/Font Alien.ttf"))
    {
        std::cerr<<"Could not load font"<<std::endl;
    }

    msg.SetFont(myFont);

    std::vector<sf::Text> strings;
    std::vector<sf::Text>::iterator strIt;

    for(int i=0; i<100; i++)
    {
        strings.push_back(bgText);
    }

    //initialize aspects of moving bg "matrix" style effect
    for(strIt = strings.begin(); strIt < strings.end(); strIt++)
    {
        int fontInt = rand() % 2;
        switch(fontInt)
        {
        case 0:
            (*strIt).SetFont(myFont);
            break;

        case 1:
            (*strIt).SetFont(myFont2);
            break;

        }

        (*strIt).SetCharacterSize(12);
        (*strIt).SetString(randStr.genRandom());
        (*strIt).SetColor(sf::Color(0,rand() % 155,0,150));
        (*strIt).SetPosition(rand() % (unsigned)width, rand() % (unsigned)height);
    }

    sf::SoundBuffer buffer1;
    //http://www.freesound.org/people/Oddworld/sounds/125099/
    if(!buffer1.LoadFromFile("sound/open.wav"))
    {
        std::cerr<<"Error loading sound file"<<std::endl;
    }

    sf::Sound openSnd;
    openSnd.SetBuffer(buffer1);
    openSnd.SetVolume(50.f);
    openSnd.SetLoop(false);
    openSnd.Play();

    sf::Clock clock;
    sf::Clock closeClock;
    sf::Clock resize;

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

        r += dr;
        g += dg;
        b += db;

        if(r == 255 && g == 0 && b == 0)
        {
            dr = 0;
            dg = 1;
            db = 0;
        }

        if(r == 255 && g == 255 && b == 0)
        {
            dr = -1;
            dg = 0;
            db = 0;
        }

        if(r == 0 && g == 255 && b == 0)
        {
            dr = 0;
            dg = 0;
            db = 1;
        }

        if(r == 0 && g == 255 && b == 255)
        {
            dr = 0;
            dg = -1;
            db = 0;
        }

        if(r == 0 && g == 0 && b == 255)
        {
            dr = 1;
            dg = 0;
            db = 0;
        }

        if(r == 255 && g == 0 && b == 255)
        {
            dr = 0;
            dg = 0;
            db = -1;
        }

        //color change the message
        msg.SetColor(sf::Color(r,200,50,200));

        //close after time alive expires
        if(closeClock.GetElapsedTime().AsSeconds() > alive)
        {
            guiWin.Close();
            return;
        }

        const sf::Time elapsed = clock.Restart();

        sprite1.Rotate(10.f * elapsed.AsSeconds());
        sprite2.Rotate(20.f * elapsed.AsSeconds());
        sprite3.Rotate(15.f * elapsed.AsSeconds());

        //move the strings down screen and reinitialize the position
        //at the top
        for(strIt = strings.begin(); strIt < strings.end(); strIt++)
        {
            float randVel = 500.f + ((float)rand()/(float)RAND_MAX/(1500.f-500.f));
            float randY = -10.f + ((float)rand()/(float)RAND_MAX/(-40.f -10.f));
            if((*strIt).GetPosition().y > height)
                ((*strIt).SetPosition(rand() % (unsigned)width, randY));
            else
                (*strIt).Move(0,randVel * elapsed.AsSeconds());
        }

        guiWin.Clear(sf::Color(0,0,0,255));

        for(strIt = strings.begin(); strIt < strings.end(); strIt++)
        {
            guiWin.Draw(*strIt);

        }

        guiWin.Draw(sprite1);
        guiWin.Draw(sprite2);
        guiWin.Draw(sprite3);

        guiWin.Draw(msg);

        guiWin.Display();
    }
    return;
}
