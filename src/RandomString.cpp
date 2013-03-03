#include "../include/RandomString.h"

RandomString::RandomString()
{
    //ctor
}

RandomString::~RandomString()
{
    //dtor
}

char RandomString::genRandom()
{
    char alphanum[] =
        "0123456789"
        "!@#$%^&*"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    int stringLength = sizeof(alphanum) - 1;
    return alphanum[rand() % stringLength];
}
