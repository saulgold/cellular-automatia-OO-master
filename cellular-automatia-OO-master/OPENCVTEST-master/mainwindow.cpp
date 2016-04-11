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
int runs=0;
int maxThreads = omp_get_max_threads();
int ID = omp_get_thread_num();

std::ofstream speedData("speed_data.csv");

cv::Mat  frame2;
int const mapSize = 50;
QTime runTime;
block blockArray[mapSize][mapSize];
block blockArrayNext[3*mapSize][3*mapSize];
block blockGhostArray[3*mapSize][3*mapSize];
int fishPercent = 50;
int sharkPercent = 25;
int frameRate = 0;
int threadNumber=1;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    speedData << "No of iterations,"<<"time taken,"<<"number of cores,"<<"visuals"<<std::endl;


    qDebug()<< ID;

    //initialise bolckArrayNExt as all dead
    for (int i = 0; i < 3*mapSize; ++i){
        for (int j = 0; j < 3*mapSize; ++j){

            blockArrayNext[i][j].setDead();
        }

    }



    ui->setupUi(this);
    timer = new QTimer(this);
   connect(timer,SIGNAL(timeout()),this,SLOT(updateGUI()));
     timer->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::updateGUI(){


//#pragma omp parallel
//    {
//    ui->textBrowser->append(QString::number(omp_get_thread_num()));
//}
//display data
    int numThreads =omp_get_max_threads();
    ui->textBrowser->append(QString::number(numThreads));

    ui->lcdNumber->setPalette(QColor(200,124,130));
    ui->lcdNumber_2->setPalette(QColor(100,0,100));
    ui->lcdNumber_3->setPalette(Qt::black);
    ui->lcdNumber_4->setPalette(Qt::black);
    ui->lcdNumber_5->setPalette(Qt::black);
    ui->lcdNumber_6->setPalette(Qt::black);
    ui->lcdNumber_7->setPalette(Qt::black);
    ui->lcdNumber_8->setPalette(Qt::black);
    ui->lcdNumber->display(QString::number(numberOfFish));
    ui->lcdNumber_2->display(QString::number(numberOfSharks));
    ui->lcdNumber_3->display(QString::number(count));

    //set ghoast array to current array

#pragma omp parallel num_threads(4)
    {
#pragma omp for

    for (int i = 0; i < mapSize; ++i){
        for (int j = 0; j < mapSize; ++j){

        blockGhostArray[i][j]=blockArray[i][j];
        }
    }

    //10-2*mapSize, 0-10
    #pragma omp for
    for (int i = mapSize; i < 2*mapSize; ++i){
        for (int j = 0; j < mapSize; ++j){

        blockGhostArray[i][j]=blockArray[i-mapSize][j];
        }
    }
    //0-3*mapSize, 0-10
    #pragma omp for
    for (int i = mapSize; i < 2*mapSize; ++i){
        for (int j = 0; j < mapSize; ++j){

        blockGhostArray[i][j]=blockArray[i-mapSize][j];
        }
    }
    //2*mapSize-3*mapSize, 0-10
    #pragma omp for
    for (int i = 2*mapSize; i < 3*mapSize; ++i){
        for (int j = 0; j < mapSize; ++j){

        blockGhostArray[i][j]=blockArray[i-2*mapSize][j];
        }
    }
    //0-10, 10-2*mapSize
    #pragma omp for
    for (int i = 0; i < mapSize; ++i){
        for (int j = mapSize; j < 2*mapSize; ++j){

        blockGhostArray[i][j]=blockArray[i][j-mapSize];
        }
    }
    //0-10, 2*mapSize-3*mapSize
    #pragma omp for
    for (int i = 0; i < mapSize; ++i){
        for (int j = 2*mapSize; j < 3*mapSize; ++j){

            blockGhostArray[i][j]=blockArray[i][j-2*mapSize];
        }
    }
    //2*mapSize-3*mapSize, 2*mapSize-3*mapSize
    #pragma omp for
    for (int i = 2*mapSize; i < 3*mapSize; ++i){
        for (int j = 2*mapSize; j < 3*mapSize; ++j){

            blockGhostArray[i][j]=blockArray[i-2*mapSize][j-2*mapSize];
        }
    }
    //10-2*mapSize, 10-2*mapSize
    #pragma omp for
    for (int i = mapSize; i < 2*mapSize; ++i){
        for (int j = mapSize; j < 2*mapSize; ++j){

        blockGhostArray[i][j]=blockArray[i-mapSize][j-mapSize];
        }

    }
    //10-2*mapSize, 2*mapSize-3*mapSize
    #pragma omp for
    for (int i = mapSize; i < 2*mapSize; ++i){
        for (int j = 2*mapSize; j < 3*mapSize; ++j){

        blockGhostArray[i][j]=blockArray[i-mapSize][j-2*mapSize];
        }

    }
    //2*mapSize-3*mapSize, 2*mapSize-3*mapSize
    #pragma omp for
    for (int i = 2*mapSize; i < 3*mapSize; ++i){
        for (int j = 2*mapSize; j < 3*mapSize; ++j){

        blockGhostArray[i][j]=blockArray[i-2*mapSize][j-2*mapSize];
        }

    }
    //2*mapSize-3*mapSize, mapSize-2*mapSize
    #pragma omp for
    for (int i = 2*mapSize; i < 3*mapSize; ++i){
        for (int j = mapSize; j < 2*mapSize; ++j){

        blockGhostArray[i][j]=blockArray[i-2*mapSize][j-mapSize];
        }

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

//turn visuals on
if(ui->radioButton->isChecked()){
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
}
//qDebug()<<count;


//ui->textBrowser->append(QString::number(ID));
ui->label->update();
count++;

//increment age of all blocks

for(int i=0; i<mapSize;i++){
    for(int j = 0; j<mapSize;j++){
        blockArray[i][j].blockAge ++;
    }
}

QThread::msleep(frameRate);
//display elapsed time
int timeInMs = runTime.elapsed();
ui->lcdNumber_6->display(QString::number(timeInMs));

if(ui->radioButton_3->isChecked() && runs <=5&&threadNumber<=4){

    ui->spinBox->setValue(threadNumber);
    if(count==100){
        ui->lcdNumber_7->display(QString::number(timeInMs));
        speedData<<100<<","<<ui->lcdNumber_6->value()<<","<<ui->spinBox->value()<<","<<ui->radioButton->isChecked()<<std::endl;
        ui->textBrowser_3->append(QString::number(ui->lcdNumber_6->value()));

        runs++;
        count =0;
        ui->pushButton->clicked();


        }
    if(runs>5){

        threadNumber++;
        runs=0;

    }
    if(threadNumber==5){
        ui->textBrowser_4->setText("finished speed test");
        speedData.close();
    }
    }
}

void MainWindow::on_pushButton_clicked()
{
    //start timer
    //const double startTime = omp_get_wtime();
    int numberOfThreads = ui->spinBox->value();
    omp_set_num_threads(numberOfThreads);
    runTime.start();
    frameRate = ui->verticalSlider_3->value();
    count =0;
    numberOfFish=0;
    numberOfSharks =0;
    fishPercent = ui->verticalSlider->value();
    sharkPercent = ui->verticalSlider_2->value();
//    if((ui->verticalSlider->value()+ui->verticalSlider_2->value())>100){
//            fishPercent = 50;
//            fishPercent = 50;
//            ui->verticalSlider->setValue(50);
//            ui->verticalSlider_2->setValue(50);
//            ui->lcdNumber_4->display(50);
//            ui->lcdNumber_5->display(50);

//}


    int randArray[mapSize][mapSize];
    int arrayCount =0;
    int iRand,jRand, temp;
    int startingFish = (mapSize*mapSize)*(fishPercent/100.0);
    int startingShark = ((mapSize*mapSize)*(sharkPercent/100.0));
    int totalGridSize = mapSize*mapSize;
      int a[mapSize][mapSize];
  if(ui->radioButton_3->isChecked()){


    //create an array with values 1 to 100
    for(int i=0;i<mapSize;i++){
       for(int j=0; j<mapSize;j++){
           randArray[i][j] = arrayCount;
           arrayCount++;
       }
       arrayCount++;
    }
    //swap array values randomly
    for(int i=0;i<mapSize;i++){
       for(int j=0; j<mapSize;j++){
           iRand = rand() % mapSize;
           jRand = rand() % mapSize;
           temp =  randArray[i][j];
          randArray[i][j] = randArray[iRand][jRand];
          randArray[iRand][jRand]=temp;

       }

    }

//    for(int i=0;i<mapSize;i++){
//       for(int j=0; j<mapSize;j++){
//          randArray[i][j] = rand() %100;

//       }

//    }


    for (int i =0; i<mapSize;++i){
        for(int j=0; j<mapSize; ++j){


            if(randArray[i][j] >totalGridSize -startingFish){

            blockArray[i][j].setFish();
            a[i][j]=1;
            numberOfFish++;
            }
            else if(randArray[i][j]<startingShark){
                blockArray[i][j].setShark();
                numberOfSharks++;
                a[i][j]=2;

            }
            else{
                blockArray[i][j].setDead();
                a[i][j]=0;
            }
        }
    }


}
    else{
        ui->textBrowser_2->append(QString::number((std::rand()&100)));
       int arrayPlace = 0;
        for(int i=0;i<mapSize;i++){
            for(int j=0;j<mapSize;j++){
                if(arrayPlace > totalGridSize - startingFish ){
                    blockArray[i][j].setFish();
                    numberOfFish++;
                }
                else if(arrayPlace < startingShark){
                    blockArray[i][j].setShark();
                    numberOfSharks++;
                }
                else{
                    blockArray[i][j].setDead();
                }
                arrayPlace++;
            }
       }


    }

}





