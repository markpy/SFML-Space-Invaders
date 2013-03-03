#include "globals.h"

#include "include/Bullet.h"
#include "include/Enemy.h"

#include "include/MsgBox.h"

enum MyAction
{
    MoveLeft,
    MoveRight,
    MoveUp,
    MoveDown,
    Shoot,
    AutoWin,
    AutoLose,
    Quit
};

//Converts float to a string with no decimals
std::string ToString(float value)
{
    std::stringstream stream;
    stream.setf(std::ios_base::fixed);
    stream.precision(0);
    stream << value;
    return stream.str();
}

int main(void)
{

    sf::Clock msgClock;
    if(msgClock.GetElapsedTime().AsSeconds() < 10.f)
    {
        //display the instructions
        std::auto_ptr<MsgBox> msgBox(new MsgBox(500,400,"Defeat the aliens \n\nbefore they reach you.\n\nPress Space to fire lasers...\n\nLeft and right arrows to move...\n\nEscape to quit.", 5.f));
        msgBox->Display();
    }
    sf::RenderWindow window(sf::VideoMode(800,600,32), "Shooter", sf::Style::Default);

    //window.SetVerticalSyncEnabled(true);
    window.SetFramerateLimit(60);
    window.SetKeyRepeatEnabled(false);

    sf::Event event;

    bool running = true;

    //seed the random number generator
    srand(time(NULL));

    ///////////////////////
    //set up messages
    ///////////////////////

    int WinLose = 0;//1=win 2=lose

    float numLives = 3.f;//set lives to 3

    sf::VertexArray boundary(sf::LinesStrip, 2);
    boundary[0].Position = sf::Vector2f(0, 560);
    boundary[1].Position = sf::Vector2f(800, 560);
    boundary[0].Color = sf::Color(0,255,0,255);
    boundary[1].Color = sf::Color(0,255,0,255);

    sf::Text livesText("");
    livesText.SetCharacterSize(24.f);
    livesText.SetPosition(0, 570);

    sf::Text playerOne("SCORE<1>");
    playerOne.SetCharacterSize(12.f);
    playerOne.SetPosition(window.GetSize().x/2.75f, 10.f);

    int playerOneScore = 0;
    sf::Text playerOneScoreText("");
    playerOneScoreText.SetPosition(playerOne.GetPosition().x, playerOne.GetPosition().y * 2);
    playerOneScoreText.SetCharacterSize(12.f);

    sf::Text highScore("HI-SCORE");
    highScore.SetPosition(window.GetSize().x/2.15f, 10.f);
    highScore.SetCharacterSize(12.f);

    int high = 0;
    sf::Text highScoreText("");
    highScoreText.SetPosition(highScore.GetPosition().x, highScore.GetPosition().y * 2);
    highScoreText.SetCharacterSize(12.f);

    sf::Text playerTwo("SCORE<2>");
    playerTwo.SetCharacterSize(12.f);
    playerTwo.SetPosition(window.GetSize().x/1.75f, 10.f);

    int playerTwoScore = 0;
    sf::Text playerTwoScoreText("");
    playerTwoScoreText.SetCharacterSize(12.f);
    playerTwoScoreText.SetPosition(playerTwo.GetPosition().x, playerTwo.GetPosition().y * 2);

    sf::Text GameOver;
    GameOver.SetOrigin(GameOver.GetGlobalBounds().Width/2, GameOver.GetGlobalBounds().Height/2);
    GameOver.SetCharacterSize(40.f);
    GameOver.SetColor(sf::Color(255,255,255,255));

    ///////////////////////
    //set up gfx
    ///////////////////////

    sf::Texture bg;
    if(!bg.LoadFromFile("images/bg.png"))
    {
        std::cerr<<"Could not load image"<<std::endl;
    }

    sf::Sprite bgSpr;
    bgSpr.SetTexture(bg);
    bgSpr.SetColor(sf::Color(255,255,255,125));

    std::vector<sf::Sprite> parallax;
    std::vector<sf::Sprite>::iterator paraIt;

    //load up the parallax vector with parallax sprites
    for(int i = 0; i<2; ++i)
    {
        bgSpr.SetPosition(0, i*600);
        parallax.push_back(bgSpr);
    }

    sf::Texture ship;
    if(!ship.LoadFromFile("images/invadersShip.png"))
    {
        std::cerr<<"I am error..."<<std::endl;
    }

    sf::Texture livesTexture;
    if(!livesTexture.LoadFromFile("images/lives.png"))
    {
        std::cerr<<"I am error..."<<std::endl;
    }

    sf::Sprite shipSpr;
    shipSpr.SetTexture(ship);
    shipSpr.SetOrigin(shipSpr.GetGlobalBounds().Width/2, shipSpr.GetGlobalBounds().Height/4);
    shipSpr.SetPosition(400.0f, 546.0f);
    shipSpr.SetScale(1.0f, 1.0f);

    thor::Animator shipAnimator;

    //create default animation
    thor::FrameAnimation::Ptr shipDefaultAnim = thor::FrameAnimation::Create();
    for(unsigned int i = 0; i < 1; ++i)
        shipDefaultAnim->AddFrame(1.f, sf::IntRect(16*i, 0, 24, 16));

    //create animation "boogerMove"
    thor::FrameAnimation::Ptr shipDestroy = thor::FrameAnimation::Create();
    for (unsigned int i = 1; i < 3; ++i)
        shipDestroy->AddFrame(1.f, sf::IntRect(0, 16*i, 24, 16));

    shipAnimator.SetDefaultAnimation(shipDefaultAnim, sf::Seconds(1.0f));
    shipAnimator.AddAnimation("shipDestroy", shipDestroy, sf::Seconds(0.3f));

    //push ship sprites to vector
    //as a representation of lives

    sf::Sprite ships;
    ships.SetTexture(livesTexture);
    ships.SetOrigin(ships.GetGlobalBounds().Width/2, ships.GetGlobalBounds().Height/4);
    ships.SetScale(1.0f, 1.0f);

    std::vector<sf::Sprite> lives;
    std::vector<sf::Sprite>::iterator livesIt;

    for(int i = 0; i<2; ++i)
    {
        ships.SetPosition((1.5f*i*ships.GetGlobalBounds().Width) + ships.GetGlobalBounds().Width*1.5f,600 - ships.GetGlobalBounds().Height/2);
        lives.push_back(ships);
    }

    sf::Texture blooper;
    if(!blooper.LoadFromFile("images/blooper.png"))
    {
        std::cerr<<"Could not load image"<<std::endl;
    }

    sf::Texture goblin;
    if(!goblin.LoadFromFile("images/goblin.png"))
    {
        std::cerr<<"Could not load image"<<std::endl;
    }

    sf::Texture booger;
    if(!booger.LoadFromFile("images/booger.png"))
    {
        std::cerr<<"Could not load image"<<std::endl;
    }

    Enemy enemySpr;
    enemySpr.SetScale(1.0f,1.0f);
    enemySpr.SetOrigin(enemySpr.GetGlobalBounds().Width/2, enemySpr.GetGlobalBounds().Height/2);
    enemySpr.SetPosition(400,200);

    std::list<Enemy> enemies;
    std::list<Enemy>::iterator enemyIt;
    bool enemiesLeft = false;//use later to detect which way enemies are facing

    //load up our list with a grid of enemy ships
    for(int i = 0; i<5; ++i)
    {
        for(int j = 0; j<11; ++j)
        {
            switch(i)
            {
            case 0:
                enemySpr.SetPosition(j*50 + 100,i*50 + 100);
                enemySpr.m_type = Enemy::blooper;
                enemySpr.SetTexture(blooper);
                enemies.push_back(enemySpr);
                break;

            case 1:
                enemySpr.SetPosition(j*50 + 100,i*50 + 100);
                enemySpr.m_type = Enemy::goblin;
                enemySpr.SetTexture(goblin);
                enemies.push_back(enemySpr);
                break;

            case 2:
                enemySpr.SetPosition(j*50 + 100,i*50 + 100);
                enemySpr.m_type = Enemy::goblin;
                enemySpr.SetTexture(goblin);
                enemies.push_back(enemySpr);
                break;

            case 3:
                enemySpr.SetPosition(j*50 + 100,i*50 + 100);
                enemySpr.m_type = Enemy::booger;
                enemySpr.SetTexture(booger);
                enemies.push_back(enemySpr);
                break;

            case 4:
                enemySpr.SetPosition(j*50 + 100,i*50 + 100);
                enemySpr.m_type = Enemy::booger;
                enemySpr.SetTexture(booger);
                enemies.push_back(enemySpr);
                break;
            }
        }
    }

    //set up the ufo ship
    sf::Texture ufo;
    if(!ufo.LoadFromFile("images/ufo.png"))
    {
        std::cerr<<"Could not load image"<<std::endl;
    }

    sf::Sprite ufoSpr;
    ufoSpr.SetTexture(ufo);
    ufoSpr.SetOrigin(ufoSpr.GetGlobalBounds().Width/2, ufoSpr.GetGlobalBounds().Height/2);
    ufoSpr.SetPosition(-50,75);

    //random number for controlling
    //the timing of ufo appearances
    int randUfo = 15 + (rand() % (int)(25 - 15 + 1));

    //bullets
    sf::Texture shot;
    if(!shot.LoadFromFile("images/shot.png"))
    {
        std::cerr<<"Could not load image"<<std::endl;
    }

    sf::Texture blooperShot;
    if(!blooperShot.LoadFromFile("images/blooperShot.png"))
    {
        std::cerr<<"Could not load image"<<std::endl;
    }

    sf::Texture goblinShot;
    if(!goblinShot.LoadFromFile("images/goblinShot.png"))
    {
        std::cerr<<"Could not load image"<<std::endl;
    }

    sf::Texture boogerShot;
    if(!boogerShot.LoadFromFile("images/boogerShot.png"))
    {
        std::cerr<<"Could not load image"<<std::endl;
    }

    sf::Texture ufoShot;
    if(!ufoShot.LoadFromFile("images/ufoShot.png"))
    {
        std::cerr<<"Could not load image"<<std::endl;
    }

    Bullet bullet(800.f);//wonder why it has to be that high????
    bullet.SetTexture(shot);
    bullet.SetScale(2.f,2.f);
    bullet.SetOrigin(bullet.GetGlobalBounds().Width/2, bullet.GetGlobalBounds().Height/2);

    std::vector<Bullet> bullets;
    std::vector<Bullet>::iterator bullIt;

    Bullet enemyBullet(500.f);//wonder why it has to be that high????
    enemyBullet.SetScale(1.f,1.f);
    enemyBullet.SetOrigin(bullet.GetGlobalBounds().Width/2, bullet.GetGlobalBounds().Height/2);

    std::list<Bullet> enemyBullets;
    std::list<Bullet>::iterator enemyBullIt;

    //explosions
    sf::Texture explode;
    if(!explode.LoadFromFile("images/invadersExplosion.png"))
    {
        std::cerr<<"Could not load image"<<std::endl;
    }

    sf::Sprite explodeSpr;
    explodeSpr.SetTexture(explode);
    explodeSpr.SetScale(2.f,2.f);
    explodeSpr.SetOrigin(explodeSpr.GetGlobalBounds().Width/4, explodeSpr.GetGlobalBounds().Height/2);

    sf::Texture explodeShip;
    if(!explodeShip.LoadFromFile("images/shipExplosion.png"))
    {
        std::cerr<<"Could not load image"<<std::endl;
    }

    sf::Sprite explodeShipSpr;
    explodeShipSpr.SetTexture(explodeShip);
    explodeShipSpr.SetScale(2.f,2.f);
    explodeShipSpr.SetOrigin(explodeSpr.GetGlobalBounds().Width/4, explodeSpr.GetGlobalBounds().Height/2);

    //set up barricades
    sf::RectangleShape pix;
    pix.SetFillColor(sf::Color::Green);
    pix.SetSize(sf::Vector2f(16.f,16.f));
    pix.SetOrigin(pix.GetGlobalBounds().Width/2, pix.GetGlobalBounds().Height/2);

    std::list<sf::RectangleShape> barricade;
    std::list<sf::RectangleShape>::iterator barricadeIt;

    //push back a chunk of barricade
    for(int i=0; i<8; ++i)
    {
        for(int j=0; j<3; ++j)
        {
            pix.SetPosition(i * 16 + 100, j * 16 + 440);
            barricade.push_back(pix);
        }
    }

    //push back another chunk of barricade
    for(int i=0; i<8; ++i)
    {
        for(int j=0; j<3; ++j)
        {
            pix.SetPosition(i * 16 + 350, j * 16 + 440);
            barricade.push_back(pix);
        }
    }

    //push back a final chunk of barricade
    for(int i=0; i<8; ++i)
    {
        for(int j=0; j<3; ++j)
        {
            pix.SetPosition(i * 16 + 600, j * 16 + 440);
            barricade.push_back(pix);
        }
    }

    //////////////////////////////
    //set up animations        ///
    //////////////////////////////

    //animator for enemy ships
    thor::Animator enemyAnimator;

    //create default animation
    thor::FrameAnimation::Ptr enemyDefaultAnim = thor::FrameAnimation::Create();
    for(unsigned int i = 0; i < 1; ++i)
        enemyDefaultAnim->AddFrame(1.f, sf::IntRect(16*i, 0, 24, 16));

    //create animation "boogerMove"
    thor::FrameAnimation::Ptr enemyMove = thor::FrameAnimation::Create();
    for (unsigned int i = 0; i < 2; ++i)
        enemyMove->AddFrame(1.f, sf::IntRect(0, 16*i, 24, 16));

    enemyAnimator.SetDefaultAnimation(enemyDefaultAnim, sf::Seconds(1.0f));
    enemyAnimator.AddAnimation("enemyMove", enemyMove, sf::Seconds(1.0f));

    enemyAnimator.PlayAnimation("enemyMove",true);

    //animator for explosion
    thor::Animator ShipExplodeAnimator;

    //default animation is an empty frame
    //outside the bounds of the sprite sheet
    thor::FrameAnimation::Ptr ShipExplodeDefaultAnim = thor::FrameAnimation::Create();
    for(unsigned int i = 0; i < 1; ++i)
        ShipExplodeDefaultAnim->AddFrame(1.f, sf::IntRect(16*i, 0, 0, 0));

    //set up "Explode" animation frames
    thor::FrameAnimation::Ptr ShipExplode = thor::FrameAnimation::Create();
    for (unsigned int i = 0; i < 2; ++i)
        ShipExplode->AddFrame(1.f, sf::IntRect(0, 16*i, 24, 16));

    ShipExplodeAnimator.SetDefaultAnimation(ShipExplodeDefaultAnim, sf::Seconds(1.f));
    ShipExplodeAnimator.AddAnimation("ShipExplode", ShipExplode, sf::Seconds(0.25f));

    thor::Animator enemyExplodeAnimator;

    //default animation is an empty frame
    //outside the bounds of the sprite sheet
    thor::FrameAnimation::Ptr enemyExplodeDefaultAnim = thor::FrameAnimation::Create();
    for(unsigned int i = 0; i < 1; ++i)
        enemyExplodeDefaultAnim->AddFrame(1.f, sf::IntRect(16*i, 0, 0, 0));

    //set up "Explode" animation frames
    thor::FrameAnimation::Ptr Explode = thor::FrameAnimation::Create();
    for (unsigned int i = 0; i < 1; ++i)
        Explode->AddFrame(1.f, sf::IntRect(0, 16*i, 24, 16));

    enemyExplodeAnimator.SetDefaultAnimation(enemyExplodeDefaultAnim, sf::Seconds(1.f));
    enemyExplodeAnimator.AddAnimation("Explode", Explode, sf::Seconds(0.25f));

    //clock to measure frame time
    sf::Clock frameClock;

    //clocks for animations
    sf::Clock deathClock;
    sf::Clock shipClock;
    sf::Clock ShipExplodeClock;
    sf::Clock enemyExplodeClock;
    sf::Clock enemyClock;
    sf::Clock enemyMotionClock;
    sf::Clock ufoClock;
    sf::Clock ufoTimer;
    sf::Clock bulletClock;
    sf::Clock enemyBulletClock;
    sf::Clock enemyBulletTimer;
    sf::Clock ufoBulletTimer;

    /////////////////////////
    //set up an action map //
    //for the ship         //
    /////////////////////////

    thor::ActionMap<MyAction> map(window);

    // Set up action map
    using thor::Action;
    map[MoveLeft] = Action(sf::Keyboard::Left, Action::Realtime);
    map[MoveRight] = Action(sf::Keyboard::Right, Action::Realtime);
    map[MoveUp] = Action(sf::Keyboard::Up, Action::Realtime);
    map[MoveDown] = Action(sf::Keyboard::Down, Action::Realtime);
    map[Shoot] = Action(sf::Keyboard::Space, Action::PressOnce);
    map[AutoWin] = Action(sf::Keyboard::W, Action::PressOnce);
    map[AutoLose] = Action(sf::Keyboard::L, Action::PressOnce);
    map[Quit] = Action(sf::Keyboard::Escape, Action::PressOnce);

    //////////////////
    //set up sounds
    //////////////////

    sf::SoundBuffer fireBuff;
    if(!fireBuff.LoadFromFile("sound/shoot.wav"))
    {
        std::cerr<<"Could not load sound"<<std::endl;
    }

    sf::Sound fire;
    fire.SetBuffer(fireBuff);
    fire.SetVolume(10.f);
    fire.SetLoop(false);

    sf::SoundBuffer enemyExplodeBuff;
    if(!enemyExplodeBuff.LoadFromFile("sound/explosion.wav"))
    {
        std::cerr<<"Could not load sound"<<std::endl;
    }

    sf::Sound enemyExplode;
    enemyExplode.SetBuffer(enemyExplodeBuff);
    enemyExplode.SetVolume(10.f);
    enemyExplode.SetLoop(false);

    sf::SoundBuffer musicBuff;
    if(!musicBuff.LoadFromFile("sound/drone.wav"))
    {
        std::cerr<<"Could not load sound"<<std::endl;
    }

    sf::Sound music;
    music.SetBuffer(musicBuff);
    music.SetVolume(20.f);
    music.SetLoop(true);
    music.Play();

    sf::SoundBuffer ufoBuff;
    if(!ufoBuff.LoadFromFile("sound/ufo_highpitch.wav"))
    {
        std::cerr<<"Could not load sound"<<std::endl;
    }

    sf::Sound ufoSnd;
    ufoSnd.SetBuffer(ufoBuff);
    ufoSnd.SetVolume(50.f);
    ufoSnd.SetLoop(true);

    //move green enemy ships
    sf::Time dt = enemyMotionClock.Restart();

    while(running)
    {
        map.Update();

        if(map.IsActive(MoveLeft))
        {
            //check that the ship bounds are within the window
            if(shipSpr.GetPosition().x <= 0 + shipSpr.GetGlobalBounds().Width/2)
                //set the ship back in bounds if it is not in bounds
                shipSpr.SetPosition(0  + shipSpr.GetGlobalBounds().Width/2,shipSpr.GetPosition().y);

            else
                //move the ship accordingly if it is in bounds
                shipSpr.Move(-10.0f, 0.0f);
        }

        if(map.IsActive(MoveRight))
        {
            //check that the ship bounds are within the window
            if(shipSpr.GetPosition().x >= 800 - shipSpr.GetGlobalBounds().Width/2)
                //set the ship back in bounds if it is not in bounds
                shipSpr.SetPosition(800 - shipSpr.GetGlobalBounds().Width/2,shipSpr.GetPosition().y);

            else
                //move the ship accordingly if it is in bounds
                shipSpr.Move(10.0f, 0.0f);
        }

        if(map.IsActive(MoveUp))
        {

        }

        if(map.IsActive(MoveDown))
        {

        }

        if(map.IsActive(Shoot))
        {
            //intitialize bullet position to ship coords
            bullet.SetPosition(shipSpr.GetPosition().x, shipSpr.GetPosition().y);
            bullet.setDirectionY(-1);
            //push the bullets to the bullet vector
            if(bullets.size() < 1)
            {
                bullets.push_back(bullet);
                //play the "fire" sound
                fire.Play();
            }
        }


        else if(!map.IsActive(Shoot))
        {

        }

        if(map.IsActive(AutoWin))
        {
            WinLose = 1;
        }

        if(map.IsActive(AutoLose))
        {
            WinLose = 2;
        }

        if(map.IsActive(Quit))
        {
            window.Close();
            return 0;
        }

        const float dt = enemyMotionClock.Restart().AsSeconds();

        switch(WinLose)
        {
        case 0:
        {
            //iterate through bullet vector
            //and fire them bullets!!
            sf::Time elapsed = bulletClock.Restart();
            for(bullIt = bullets.begin(); bullIt != bullets.end(); ++bullIt)
            {
                (*bullIt).Fire(elapsed);
            }

            //fire bullets from ufo
            //if ufo is on the screen...
            if(ufoBulletTimer.GetElapsedTime().AsSeconds() > 1.5f)
            {
                if(0 < ufoSpr.GetPosition().x &&
                        ufoSpr.GetPosition().x < 800)
                {
                    enemyBullet.SetTexture(ufoShot);
                    enemyBullet.SetPosition(ufoSpr.GetPosition().x, ufoSpr.GetPosition().y);
                    enemyBullet.setDirectionY(1);//fire downward
                    enemyBullets.push_back(enemyBullet);
                }

                //restart so that they fire every second
                ufoBulletTimer.Restart();
            }

            //fire random bullets from enemies
            int pickRandEnemy = rand()% enemies.size();
            std::list<Enemy>::iterator randEnemyIt = enemies.begin();
            if(enemyBulletTimer.GetElapsedTime().AsSeconds() > 0.5f)
            {
                //advance the list iterator to a random enemy
                //in the list every second
                std::advance(randEnemyIt, pickRandEnemy);

                //set enemy bullet defaults according to
                //random selection of enemies from list
                enemyBullet.SetPosition((*randEnemyIt).GetPosition().x, (*randEnemyIt).GetPosition().y);
                enemyBullet.setDirectionY(1);//fire downward

                //set enemy bullet texture
                //according to the enemy type
                if((*randEnemyIt).m_type == Enemy::blooper)
                {
                    enemyBullet.SetTexture(blooperShot);
                }

                if((*randEnemyIt).m_type == Enemy::goblin)
                {
                    enemyBullet.SetTexture(goblinShot);
                }

                if((*randEnemyIt).m_type == Enemy::booger)
                {
                    enemyBullet.SetTexture(boogerShot);
                }

                //push back one bullet at a time
                enemyBullets.push_back(enemyBullet);

                //reset the bullet timer
                //so a bullet fires every .5 seconds
                enemyBulletTimer.Restart();
            }

            //iterate through enemy bullet vector
            //and fire them bullets!!
            sf::Time enemyElapsed = enemyBulletClock.Restart();
            for(enemyBullIt = enemyBullets.begin(); enemyBullIt != enemyBullets.end(); ++enemyBullIt)
            {
                (*enemyBullIt).Fire(enemyElapsed);

                if((*enemyBullIt).GetPosition().y > 552)
                {
                    //explode bullet
                    explodeSpr.SetPosition((*enemyBullIt).GetPosition().x, 546);
                    explodeSpr.SetColor(sf::Color(0,255,0,255));
                    enemyExplodeAnimator.PlayAnimation("Explode", false);

                    //erase the enemy bullet
                    enemyBullIt = enemyBullets.erase(enemyBullIt);
                }
            }

            //coordinate ufo movement
            if(ufoTimer.GetElapsedTime().AsSeconds() > randUfo)
            {
                ufoSpr.Move(dt * 200.f,0.f);
                if(ufoSnd.GetStatus() == sf::Music::Stopped)
                {
                    ufoSnd.Play();
                }
            }

            if(ufoSpr.GetPosition().x > 850)
            {
                ufoTimer.Restart();
                ufoSnd.Stop();
                ufoSpr.SetPosition(-50.f, 75.f);

            }

            for(bullIt = bullets.begin(); bullIt != bullets.end();)
            {
                //if bullet(s) collide with enemy ships...
                if((*bullIt).GetPosition().y + (*bullIt).GetGlobalBounds().Height/2 > ufoSpr.GetPosition().y - ufoSpr.GetGlobalBounds().Height/2
                        && (*bullIt).GetPosition().y - (*bullIt).GetGlobalBounds().Height/2 < ufoSpr.GetPosition().y + ufoSpr.GetGlobalBounds().Height/2
                        && (*bullIt).GetPosition().x + (*bullIt).GetGlobalBounds().Width/2 > ufoSpr.GetPosition().x - ufoSpr.GetGlobalBounds().Width/2
                        && (*bullIt).GetPosition().x - (*bullIt).GetGlobalBounds().Width/2 < ufoSpr.GetPosition().x + ufoSpr.GetGlobalBounds().Width/2)
                {
                    //play the explosion sound
                    enemyExplode.Play();

                    //play the explosion animation
                    explodeSpr.SetPosition(ufoSpr.GetPosition().x + ufoSpr.GetGlobalBounds().Width/2, ufoSpr.GetPosition().y + ufoSpr.GetGlobalBounds().Height/2);
                    explodeSpr.SetColor(sf::Color(255,0,0,255));
                    enemyExplodeAnimator.PlayAnimation("Explode", false);

                    //reset the ufo offscreen
                    ufoSpr.SetPosition(-50.f, 75.f);

                    //erase the bullet from the vector
                    bullIt = bullets.erase(bullIt);

                    playerOneScore += 200;

                    ufoTimer.Restart();
                    ufoSnd.Stop();
                }

                //if the bullet misses its target....
                else if((*bullIt).GetPosition().y < 0)
                {
                    //erase the bullet if it is out of bounds
                    bullIt = bullets.erase(bullIt);
                }

                //continue iterating through the bullet vector
                else
                {
                    ++bullIt;
                }
            }

            //coordinate and execute
            //blooper ship movement
            float enemyVel = 1.f;
            sf::Time enemyDT = enemyMotionClock.Restart();
            std::list<Enemy>::iterator moveIt;
            for(enemyIt = enemies.begin(); enemyIt != enemies.end(); ++enemyIt)
            {
                enemyVel = (dt * 1000/enemies.size()) + 1.f;

                if((*enemyIt).GetPosition().x >= 800 - (*enemyIt).GetGlobalBounds().Width)
                {
                    for(moveIt = enemies.begin(); moveIt != enemies.end(); ++moveIt)
                    {
                        (*moveIt).Move(0,5);
                    }
                    enemiesLeft = true;
                }

                if((*enemyIt).GetPosition().x <= (*enemyIt).GetGlobalBounds().Width)
                {
                    for(moveIt = enemies.begin(); moveIt != enemies.end(); ++moveIt)
                    {
                        //set enemy color to green
                        //if they reach the barricades
                        if((*moveIt).GetPosition().y > 440.f)
                        {
                            (*moveIt).SetColor(sf::Color(0,255,0,255));
                        }

                        if((*moveIt).GetPosition().y > 508.f)
                        {
                            WinLose = 2;//lose
                        }
                        else
                            (*moveIt).Move(0,5);
                    }
                    enemiesLeft = false;
                }

                switch(enemiesLeft)
                {
                case 0:
                    (*enemyIt).Move(enemyVel,0);
                    break;

                case 1:
                    (*enemyIt).Move(-enemyVel,0);
                    break;
                }
            }

            //check for collisions and handle bullet
            //vector and enemy list
            for(enemyIt = enemies.begin(); enemyIt != enemies.end(); ++enemyIt)
            {
                //iterate through bullet list
                for(bullIt = bullets.begin(); bullIt != bullets.end();)
                {
                    //if bullet(s) collide with enemy ships...
                    if((*bullIt).GetPosition().y + (*bullIt).GetGlobalBounds().Height/2 > (*enemyIt).GetPosition().y - (*enemyIt).GetGlobalBounds().Height
                            && (*bullIt).GetPosition().y - (*bullIt).GetGlobalBounds().Height/2 < (*enemyIt).GetPosition().y + (*enemyIt).GetGlobalBounds().Height
                            && (*bullIt).GetPosition().x + (*bullIt).GetGlobalBounds().Width/2 > (*enemyIt).GetPosition().x - (*enemyIt).GetGlobalBounds().Width/2
                            && (*bullIt).GetPosition().x - (*bullIt).GetGlobalBounds().Width/2 < (*enemyIt).GetPosition().x + (*enemyIt).GetGlobalBounds().Width)
                    {
                        //Execute the following...

                        //update the score according
                        //to enemy ship type
                        if((*enemyIt).m_type == Enemy::blooper)
                        {
                            playerOneScore += 30;
                        }

                        if((*enemyIt).m_type == Enemy::goblin)
                        {
                            playerOneScore += 20;
                        }

                        if((*enemyIt).m_type == Enemy::booger)
                        {
                            playerOneScore += 10;
                        }

                        //play the explosion sound
                        enemyExplode.Play();

                        //play the explosion animation
                        explodeSpr.SetPosition((*enemyIt).GetPosition().x + (*enemyIt).GetGlobalBounds().Width/16, (*enemyIt).GetPosition().y + (*enemyIt).GetGlobalBounds().Width/2);
                        explodeSpr.SetColor(sf::Color(255,255,255,255));
                        enemyExplodeAnimator.PlayAnimation("Explode", false);

                        //erase the enemy from the list
                        std::cout<<"hit"<<std::endl;
                        enemyIt = enemies.erase(enemyIt);

                        //erase the bullet from the vector
                        bullIt = bullets.erase(bullIt);

                    }

                    //if the bullet misses its target....
                    else if((*bullIt).GetPosition().y <= 10)
                    {
                        explodeSpr.SetPosition((*bullIt).GetPosition().x, 10);
                        explodeSpr.SetColor(sf::Color(255,0,0,255));
                        enemyExplodeAnimator.PlayAnimation("Explode", false);

                        //erase the bullet if it is out of bounds
                        bullIt = bullets.erase(bullIt);
                    }

                    //continue iterating through the bullet vector
                    else
                    {
                        ++bullIt;
                    }
                }
            }

            //check enemy list to see if it's empty...
            if(enemies.size() < 1)
                WinLose = 1;//...if so, player is full of win

            for(enemyBullIt = enemyBullets.begin(); enemyBullIt != enemyBullets.end();)
            {
                if((*enemyBullIt).GetPosition().y + (*enemyBullIt).GetGlobalBounds().Height/2 > shipSpr.GetPosition().y - shipSpr.GetGlobalBounds().Height/2
                        && (*enemyBullIt).GetPosition().y - (*enemyBullIt).GetGlobalBounds().Height/2 < shipSpr.GetPosition().y + shipSpr.GetGlobalBounds().Height/2
                        && (*enemyBullIt).GetPosition().x + (*enemyBullIt).GetGlobalBounds().Width/2 > shipSpr.GetPosition().x - shipSpr.GetGlobalBounds().Width/2
                        && (*enemyBullIt).GetPosition().x - (*enemyBullIt).GetGlobalBounds().Width/2 < shipSpr.GetPosition().x + shipSpr.GetGlobalBounds().Width/2)
                {
                    //decrement number of lives
                    numLives--;

                    //push one ship out of lives vector
                    if(lives.size() > 0)
                        lives.pop_back();

                    else
                        WinLose = 2;//player loses

                    //erase the bullet from the vector
                    enemyBullIt = enemyBullets.erase(enemyBullIt);

                    shipAnimator.PlayAnimation("shipDestroy",false);
                    enemyExplode.Play();
                }

                else
                {
                    ++enemyBullIt;
                }
            }

            //check for collisions between ship bullets and barricades
            for(barricadeIt = barricade.begin(); barricadeIt != barricade.end(); ++barricadeIt)
            {
                for(bullIt = bullets.begin(); bullIt != bullets.end();)
                {
                    //if bullet(s) collide with enemy ships...
                    if((*bullIt).GetPosition().y + (*bullIt).GetGlobalBounds().Height/2 > (*barricadeIt).GetPosition().y - (*barricadeIt).GetGlobalBounds().Height/2
                            && (*bullIt).GetPosition().y - (*bullIt).GetGlobalBounds().Height/2 < (*barricadeIt).GetPosition().y + (*barricadeIt).GetGlobalBounds().Height/2
                            && (*bullIt).GetPosition().x + (*bullIt).GetGlobalBounds().Width/2 > (*barricadeIt).GetPosition().x - (*barricadeIt).GetGlobalBounds().Width/2
                            && (*bullIt).GetPosition().x - (*bullIt).GetGlobalBounds().Width/2 < (*barricadeIt).GetPosition().x + (*barricadeIt).GetGlobalBounds().Width/2)
                    {

                        //erase barricade part from the list
                        barricadeIt = barricade.erase(barricadeIt);

                        //erase the bullet from the vector
                        bullIt = bullets.erase(bullIt);
                    }

                    //if the bullet misses its target....
                    else if((*bullIt).GetPosition().y > 600 || (*bullIt).GetPosition().y < 0)
                    {
                        //erase the bullet if it is out of bounds
                        bullIt = bullets.erase(bullIt);
                    }

                    //continue iterating through the bullet vector
                    else
                    {
                        ++bullIt;
                    }
                }
            }

            //check for collisions between enemy bullets and barricades
            for(barricadeIt = barricade.begin(); barricadeIt != barricade.end(); ++barricadeIt)
            {
                for(enemyBullIt = enemyBullets.begin(); enemyBullIt != enemyBullets.end();)
                {
                    //change bullet color to green
                    //if they reach the barricades
                    if((*enemyBullIt).GetPosition().y > 440)
                    {
                        (*enemyBullIt).SetColor(sf::Color(0,255,0,255));
                    }

                    //if bullet(s) collide with enemy ships...
                    if((*enemyBullIt).GetPosition().y + (*enemyBullIt).GetGlobalBounds().Height/2 > (*barricadeIt).GetPosition().y - (*barricadeIt).GetGlobalBounds().Height/2
                            && (*enemyBullIt).GetPosition().y - (*enemyBullIt).GetGlobalBounds().Height/2 < (*barricadeIt).GetPosition().y + (*barricadeIt).GetGlobalBounds().Height/2
                            && (*enemyBullIt).GetPosition().x + (*enemyBullIt).GetGlobalBounds().Width/2 > (*barricadeIt).GetPosition().x - (*barricadeIt).GetGlobalBounds().Width/2
                            && (*enemyBullIt).GetPosition().x - (*enemyBullIt).GetGlobalBounds().Width/2 < (*barricadeIt).GetPosition().x + (*barricadeIt).GetGlobalBounds().Width/2)
                    {

                        //erase barricade part from the list
                        barricadeIt = barricade.erase(barricadeIt);

                        //erase the bullet from the vector
                        enemyBullIt = enemyBullets.erase(enemyBullIt);
                    }

                    //if the bullet misses its target....
                    else if((*enemyBullIt).GetPosition().y > 600)
                    {
                        //erase the bullet if it is out of bounds
                        enemyBullIt = enemyBullets.erase(enemyBullIt);
                    }

                    //continue iterating through the bullet vector
                    else
                    {
                        ++enemyBullIt;
                    }
                }
            }

            //update and move the parallax background
            for(paraIt = parallax.begin(); paraIt != parallax.end(); ++paraIt)
            {
                if((*paraIt).GetPosition().y >= 600)
                    (*paraIt).SetPosition(0,-600);

                else
                    (*paraIt).Move(0,1);
            }

            window.Clear();

            //draw parallax bg
            for(paraIt = parallax.begin(); paraIt != parallax.end(); ++paraIt)
            {
                window.Draw((*paraIt));
            }

            //draw the enemy ships
            for(enemyIt = enemies.begin(); enemyIt != enemies.end(); ++enemyIt)
            {
                window.Draw(*enemyIt);
                enemyAnimator.Update(enemyClock.Restart());
                enemyAnimator.Animate(*enemyIt);
            }

            //draw the bullets
            for(bullIt = bullets.begin(); bullIt != bullets.end(); ++bullIt)
            {
                window.Draw(*bullIt);
            }

            //draw the bullets
            for(enemyBullIt = enemyBullets.begin(); enemyBullIt != enemyBullets.end(); ++enemyBullIt)
            {
                window.Draw(*enemyBullIt);
            }

            //draw the barricades
            for(barricadeIt = barricade.begin(); barricadeIt != barricade.end(); ++barricadeIt)
            {
                window.Draw(*barricadeIt);
            }

            //update and draw explosions
            ShipExplodeAnimator.Update(ShipExplodeClock.Restart());
            ShipExplodeAnimator.Animate(explodeShipSpr);
            window.Draw(explodeShipSpr);

            //enemy explosion
            enemyExplodeAnimator.Update(enemyExplodeClock.Restart());
            enemyExplodeAnimator.Animate(explodeSpr);
            window.Draw(explodeSpr);

            //draw the ufo
            window.Draw(ufoSpr);

            //draw the player ship
            shipAnimator.Update(shipClock.Restart());
            window.Draw(shipSpr);
            shipAnimator.Animate(shipSpr);

            //draw the score, lives, etc

            window.Draw(boundary);

            for(livesIt = lives.begin(); livesIt != lives.end(); ++livesIt)
            {
                window.Draw(*livesIt);
            }

            livesText.SetString(ToString(numLives));
            window.Draw(livesText);

            window.Draw(playerOne);

            playerOneScoreText.SetString(ToString(playerOneScore));
            window.Draw(playerOneScoreText);

            window.Draw(highScore);
            highScoreText.SetString(ToString(high));
            window.Draw(highScoreText);

            window.Draw(playerTwo);

            playerTwoScoreText.SetString(ToString(playerTwoScore));
            window.Draw(playerTwoScoreText);
        }
        break;

        case 1:
        {
            window.Clear();

            //stop any sounds that might be playing
            music.Stop();
            ufoSnd.Stop();

            //draw the score, lives, etc

            window.Draw(boundary);

            for(livesIt = lives.begin(); livesIt != lives.end(); ++livesIt)
            {
                window.Draw(*livesIt);
            }

            livesText.SetString(ToString(numLives));
            window.Draw(livesText);

            window.Draw(playerOne);

            playerOneScoreText.SetString(ToString(playerOneScore));
            window.Draw(playerOneScoreText);

            window.Draw(highScore);
            highScoreText.SetString(ToString(high));
            window.Draw(highScoreText);

            window.Draw(playerTwo);

            playerTwoScoreText.SetString(ToString(playerTwoScore));
            window.Draw(playerTwoScoreText);

            GameOver.SetString("WELL DONE EARTHLING\n"
                               "\tYOU WIN THIS TIME");
            GameOver.SetPosition(window.GetSize().x/5, window.GetSize().y/4);
            window.Draw(GameOver);
        }
        break;

        case 2:
        {
            window.Clear();

            //stop any sounds that might be playing
            music.Stop();
            ufoSnd.Stop();

            //draw the score, lives, etc

            window.Draw(boundary);

            for(livesIt = lives.begin(); livesIt != lives.end(); ++livesIt)
            {
                window.Draw(*livesIt);
            }

            livesText.SetString(ToString(numLives));
            window.Draw(livesText);

            window.Draw(playerOne);

            playerOneScoreText.SetString(ToString(playerOneScore));
            window.Draw(playerOneScoreText);

            window.Draw(highScore);
            highScoreText.SetString(ToString(high));
            window.Draw(highScoreText);

            window.Draw(playerTwo);

            playerTwoScoreText.SetString(ToString(playerTwoScore));
            window.Draw(playerTwoScoreText);

            GameOver.SetString("YOU LOSE");
            GameOver.SetPosition(window.GetSize().x/2.75f, window.GetSize().y/4);
            window.Draw(GameOver);
        }
        break;
        }

        window.Display();
    }
    return 0;
}
