#ifndef BLOCK_H
#define BLOCK_H
#include <opencv2/core/core.hpp>

class block
{
public:
    block();
    int blockType;
    bool isShark;
    bool isFish;
    bool isDead;
    int blockAge;
    int deathAge;
    int randomDeathProbability;
    int breedingAge;
    cv::Vec3b blockColour;

    void setShark(void);
    void setFish(void);
    void setDead(void);
    cv::Vec3b getColour(){return blockColour;}

    ~block();
};

#endif // BLOCK_H
