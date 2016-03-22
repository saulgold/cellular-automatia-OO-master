#include "world.h"

world::world(int worldWidth, int worldHeight,int sharkPercent, int fishPercent)
{
    ghoastWorld[worldWidth+2][worldHeight+2];

    int totalBlocks = worldWidth*worldHeight;
    int totalShark = (sharkPercent*totalBlocks)/100;
    int totalFish = (fishPercent*totalBlocks)/100;
    int sharkCount = totalShark;
    int fishCount = totalFish;

    for(int i =0; i<worldWidth +2 ;i++){
        for(int j=0; j<worldHeight+2; j++){

            if(sharkCount!=0){

            ghoastWorld[i][j].setShark();
            sharkCount--;
            }
            else if(fishCount!=0){
            ghoastWorld[i][j].setFish();
            fishCount--;
            }
            else{
                ghoastWorld[i][i].setDead();
            }
        }
    }
}
//initialises the world with fish, shark and empty blocks. where first set of bloks is shark, then fish then empty
//can be modified to be random or user selected
//void world::worldInit(int worldWidth, int worldHeight,int sharkPercent, int fishPercent){


//}

world::~world(){

}
