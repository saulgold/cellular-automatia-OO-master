#include "block.h"

block::block()
{
   setDead();
}

bool block::isFishOfBreedingAge(void){
    if(isFish ==true){
        if(blockAge>=2){
            return true;
        }
        else{
            return false;
        }
    }

    else{
        return false;
    }
}
bool block::isSharkOfBreedingAge(void){
  if(isShark == true){
        if(blockAge>=3){
            return true;
        }
        else{
            return false;
        }
    }
    else {
        return false;
    }
}

void block::setFish(void){
    blockType = 1;
    blockAge = 1;
    isFish = 1;
    isShark = 0;
    isDead = 0;
    breedingAge = 2;
    deathAge = 10;
    randomDeathProbability = 1/32;
    blockColour = cv::Vec3b(200,124,130);

}

void block::setShark(void){
    blockType = 2;
    blockAge = 1;
    isFish = 0;
    isShark = 1;
    isDead = 0;
    breedingAge = 2;
    deathAge = 10;
    randomDeathProbability = 0;
    blockColour = cv::Vec3b(100,0,100);

}

void block::setDead(void){
    blockType = 0;
    blockAge = 1;
    isFish = 0;
    isShark = 0;
    isDead = 1;
    breedingAge = 0;
    deathAge = 0;
    randomDeathProbability = 0;
    blockColour = cv::Vec3b(98,190,200);

}

block::~block(){
    delete this;
}
