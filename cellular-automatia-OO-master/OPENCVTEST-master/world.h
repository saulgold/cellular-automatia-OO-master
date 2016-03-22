#ifndef WORLD_H
#define WORLD_H
#include <QVector>
#include "block.h"



class world
{
public:
    world(int worldWidth, int worldHeight,int sharkPercent, int fishPercent);
    int worldWidth;
    int worldHeight;
    int sharkPercent;
    int fishPercent;
   // void worldInit(int worldWidth, int worldHeight,int sharkPercent, int fishPercent);
    block **ghoastWorld;
    ~world();

};

#endif // WORLD_H
