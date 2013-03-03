#ifndef ENEMY_H
#define ENEMY_H

#include "../globals.h"

class Enemy : public sf::Sprite
{
    public:
        Enemy();
        virtual ~Enemy();

    private:

    public:
        enum Type{
            blooper = 1,
            goblin = 2,
            booger = 3
        };

        Type m_type;
};

#endif // ENEMY_H
