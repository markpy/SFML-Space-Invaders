#include "../include/Bullet.h"

Bullet::Bullet() :
m_velY(1)
{

}

Bullet::Bullet(float vY)
{
    m_velY = vY;
}

Bullet::~Bullet()
{

}

void Bullet::Fire(sf::Time dt)
{
    this->Move(0, m_directionY * dt.AsSeconds() * m_velY);
}
