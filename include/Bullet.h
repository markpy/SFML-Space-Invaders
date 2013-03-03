#ifndef BULLET_H
#define BULLET_H

#include "../globals.h"

class Bullet : public sf::Sprite{
public:
	Bullet();
	Bullet(float vY);
	~Bullet();

	void setVelocity(float vY){m_velY = vY;}
    float getVelocity(){return m_velY;}

    void setDirectionY(int d){
        if(d == 1 || d == -1)
        m_directionY = d;

        else
        std::cout<<"Must be either 1 or -1"<<std::endl;
    };

	void Fire(sf::Time dt);

public:

	float m_velY;
	int m_directionY;
};

#endif // BULLET_H
