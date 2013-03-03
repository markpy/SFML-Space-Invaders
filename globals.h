#ifndef GLOBALS_H
#define GLOBALS_H

#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <math.h>
#include <time.h>
#include <algorithm>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <THOR/Animation.hpp>
#include <THOR/Geometry.hpp>
#include <THOR/Multimedia.hpp>
#include <THOR/Events.hpp>
#include <THOR/Time.hpp>
#include <Box2D\Box2D.h>

const int WIDTH = 800;
const int HEIGHT = 600;

/////////////////////////////
//some colors
/////////////////////////////

const sf::Color grey = sf::Color(200,200,200,200);
const sf::Color black = sf::Color(0,0,0,255);

const float PI = 3.14;

const float BLOCKWIDTH = 32.0f;
const float BLOCKHEIGHT = 32.0f;

/* We need this to easily convert between pixel and real-world coordinates*/
static const float SCALE = 30.f;

//////////////////////////////////////
//Box2d constants
//////////////////////////////////////

//definitions for physics world <-> window projection coordinate transformation
#define PIXELS_PER_METER	30.0f
#define METERS_PER_PIXEL	(1.0f/PIXELS_PER_METER)
//shorthand version
#define PPM PIXELS_PER_METER
#define MPP METERS_PER_PIXEL

#endif //GLOBALS_H

