#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include "opencv2/imgproc/types_c.h"
#include <QPicture>
#include<iostream>
#include <QtCore>
#include <QDebug>
#include <vector>
#include<time.h>
#include<shark.h>

int count = 0;

cv::Mat  frame2;
int const mapSize = 100;

block blockArray[mapSize][mapSize];
block blockArrayNext[3*mapSize][3*mapSize];
block blockGhostArray[3*mapSize][3*mapSize];


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //initialise bolckArrayNExt as all dead
    for (int i = 0; i < 3*mapSize; ++i){
        for (int j = 0; j < 3*mapSize; ++j){

            blockArrayNext[i][j].setDead();
        }

    }



    ui->setupUi(this);
    timer = new QTimer(this);
   connect(timer,SIGNAL(timeout()),this,SLOT(updateGUI()));
     timer->start(20);
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::updateGUI(){
//display data
    ui->lcdNumber->setPalette(Qt::black);
    ui->lcdNumber_2->setPalette(Qt::black);

ui->lcdNumber->display(QString::number(numberOfFish));
ui->lcdNumber_2->display(QString::number(numberOfSharks));

    //set ghoast array to current array

    for (int i = 0; i < mapSize; ++i){
        for (int j = 0; j < mapSize; ++j){

        blockGhostArray[i][j]=blockArray[i][j];
        }
    }
    //10-2*mapSize, 0-10
    for (int i = mapSize; i < 2*mapSize; ++i){
        for (int j = 0; j < mapSize; ++j){

        blockGhostArray[i][j]=blockArray[i-mapSize][j];
        }
    }
    //0-3*mapSize, 0-10
    for (int i = mapSize; i < 2*mapSize; ++i){
        for (int j = 0; j < mapSize; ++j){

        blockGhostArray[i][j]=blockArray[i-mapSize][j];
        }
    }
    //2*mapSize-3*mapSize, 0-10
    for (int i = 2*mapSize; i < 3*mapSize; ++i){
        for (int j = 0; j < mapSize; ++j){

        blockGhostArray[i][j]=blockArray[i-2*mapSize][j];
        }
    }
    //0-10, 10-2*mapSize
    for (int i = 0; i < mapSize; ++i){
        for (int j = mapSize; j < 2*mapSize; ++j){

        blockGhostArray[i][j]=blockArray[i][j-mapSize];
        }
    }
    //0-10, 2*mapSize-3*mapSize
    for (int i = 0; i < mapSize; ++i){
        for (int j = 2*mapSize; j < 3*mapSize; ++j){

            blockGhostArray[i][j]=blockArray[i][j-2*mapSize];
        }
    }
    //2*mapSize-3*mapSize, 2*mapSize-3*mapSize
    for (int i = 2*mapSize; i < 3*mapSize; ++i){
        for (int j = 2*mapSize; j < 3*mapSize; ++j){

            blockGhostArray[i][j]=blockArray[i-2*mapSize][j-2*mapSize];
        }
    }
    //10-2*mapSize, 10-2*mapSize
    for (int i = mapSize; i < 2*mapSize; ++i){
        for (int j = mapSize; j < 2*mapSize; ++j){

        blockGhostArray[i][j]=blockArray[i-mapSize][j-mapSize];
        }

    }
    //10-2*mapSize, 2*mapSize-3*mapSize
    for (int i = mapSize; i < 2*mapSize; ++i){
        for (int j = 2*mapSize; j < 3*mapSize; ++j){

        blockGhostArray[i][j]=blockArray[i-mapSize][j-2*mapSize];
        }

    }
    //2*mapSize-3*mapSize, 2*mapSize-3*mapSize
    for (int i = 2*mapSize; i < 3*mapSize; ++i){
        for (int j = 2*mapSize; j < 3*mapSize; ++j){

        blockGhostArray[i][j]=blockArray[i-2*mapSize][j-2*mapSize];
        }

    }
    //2*mapSize-3*mapSize, mapSize-2*mapSize
    for (int i = 2*mapSize; i < 3*mapSize; ++i){
        for (int j = mapSize; j < 2*mapSize; ++j){

        blockGhostArray[i][j]=blockArray[i-2*mapSize][j-mapSize];
        }

    }





    //apply rules

    int sharkNeighbours, fishNeighbours,fishOfBreedingAge,sharksOfBreedingAge;

      for (int i = mapSize; i <2*mapSize; ++i){
        for (int j = mapSize; j <2*mapSize; ++j){
            sharkNeighbours =  blockGhostArray[i + 1][j].isShark + blockGhostArray[i][j + 1].isShark + blockGhostArray[i + 1][j + 1].isShark + blockGhostArray[i - 1][j - 1].isShark + blockGhostArray[i - 1][j].isShark + blockGhostArray[i][j - 1].isShark + blockGhostArray[i + 1][j - 1].isShark+blockGhostArray[i-1][j + 1].isShark;
            fishNeighbours =  blockGhostArray[i + 1][j].isFish + blockGhostArray[i][j + 1].isFish + blockGhostArray[i + 1][j + 1].isFish + blockGhostArray[i - 1][j - 1].isFish + blockGhostArray[i - 1][j].isFish + blockGhostArray[i][j - 1].isFish + blockGhostArray[i + 1][j - 1].isFish+blockGhostArray[i-1][j + 1].isFish;
            fishOfBreedingAge = blockGhostArray[i + 1][j].isFishOfBreedingAge() + blockGhostArray[i][j + 1].isFishOfBreedingAge() + blockGhostArray[i + 1][j + 1].isFishOfBreedingAge() + blockGhostArray[i - 1][j - 1].isFishOfBreedingAge() + blockGhostArray[i - 1][j].isFishOfBreedingAge() + blockGhostArray[i][j - 1].isFishOfBreedingAge() + blockGhostArray[i + 1][j - 1].isFishOfBreedingAge()+blockGhostArray[i-1][j + 1].isFishOfBreedingAge();
           sharksOfBreedingAge =  blockGhostArray[i + 1][j].isSharkOfBreedingAge() + blockGhostArray[i][j + 1].isSharkOfBreedingAge() + blockGhostArray[i + 1][j + 1].isSharkOfBreedingAge() + blockGhostArray[i - 1][j - 1].isSharkOfBreedingAge() + blockGhostArray[i - 1][j].isSharkOfBreedingAge() + blockGhostArray[i][j - 1].isSharkOfBreedingAge() + blockGhostArray[i + 1][j - 1].isSharkOfBreedingAge()+blockGhostArray[i-1][j + 1].isSharkOfBreedingAge();

            //fish rules
            if(blockGhostArray[i][j].isFish==true){
                if(sharkNeighbours>=5){
                    blockArrayNext[i][j].setDead();
                    numberOfFish--;
                }
                else if(fishNeighbours==8){
                    blockArrayNext[i][j].setDead();
                    numberOfFish--;
                }
                else if(blockGhostArray[i][j].blockAge>=10){
                    blockArrayNext[i][j].setDead();
                    numberOfFish--;
                }

                else{
                    blockArrayNext[i][j] = blockGhostArray[i][j];
                }


            }
            //shark rules

            else if(blockGhostArray[i][j].isShark==true){
                if(sharkNeighbours>=6 && fishNeighbours==0){
                    blockArrayNext[i][j].setDead();
                    numberOfSharks--;
                }
                else if(blockGhostArray[i][j].blockAge>=20){
                    blockArrayNext[i][j].setDead();
                        numberOfSharks--;
                }
                //1 in 32 chance of death
                else if(rand()%32 +1 ==20){
                    blockArrayNext[i][j].setDead();
                        numberOfSharks--;
                }

                else{
                    blockArrayNext[i][j] = blockGhostArray[i][j];
                }

            }
            //empty rules
            else if(blockGhostArray[i][j].isDead==true){

                if(fishNeighbours>=4 && fishOfBreedingAge>=3 && sharkNeighbours<4){
                    blockArrayNext[i][j].setFish();
                    numberOfFish++;
                }
                else if(sharkNeighbours>=4&&sharksOfBreedingAge>=3 && fishNeighbours<4){
                    blockArrayNext[i][j].setShark();
                    numberOfSharks++;

                }
                else{
                    blockArrayNext[i][j] = blockGhostArray[i][j];
                }
             }


            }

        }
    //display new array
    for (int i = mapSize; i < 2*mapSize; ++i){
        for (int j = mapSize; j < 2*mapSize; ++j){

            blockArray[i-mapSize][j-mapSize]=blockArrayNext[i][j];


        }

    }



 frame2 = cv::Mat(mapSize, mapSize, CV_8UC3, cv::Vec3b(0,0,0));


    //for frame2
    for (int i = 0; i < mapSize; ++i){
        for (int j = 0; j < mapSize; ++j){

    frame2.at<cv::Vec3b>(cv::Point(i,j)) = blockArray[i][j].getColour();
        }
    }

 QImage img2 = QImage((uchar*) frame2.data,frame2.cols,frame2.rows,frame2.step, QImage::Format_RGB888);
 QPixmap pix2 = QPixmap::fromImage(img2);

 ui->label_2->setPixmap(pix2.scaled(400, 400, Qt::IgnoreAspectRatio, Qt::FastTransformation));

qDebug()<<count;

//ui->label->update();
count++;

//increment age of all blocks

for(int i=0; i<mapSize;i++){
    for(int j = 0; j<mapSize;j++){
        blockArray[i][j].blockAge ++;
    }
}

 QThread::msleep(100);

}
void MainWindow::on_pushButton_clicked()
{
    numberOfFish=0;
    numberOfSharks =0;

    int randArray[mapSize][mapSize];
    for(int i=0;i<mapSize;i++){
       for(int j=0; j<mapSize;j++){
           randArray[i][j] = rand()&100;
       }
    }

    for (int i =0; i<mapSize;++i){
        for(int j=0; j<mapSize; ++j){
            if(randArray[i][j] >50){

            blockArray[i][j].setFish();
            numberOfFish++;
            }
            else if(randArray[i][j]<25){
                blockArray[i][j].setShark();

                numberOfSharks++;

            }
            else{
                blockArray[i][j].setDead();

            }
        }
    }




}


void MainWindow::on_pushButton_2_clicked()
{
    shark= new Shark(this);
    shark->show();
}
