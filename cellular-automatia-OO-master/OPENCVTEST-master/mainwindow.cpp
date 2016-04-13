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
int const mapSize = 1000;
QTime runTime;
block blockArray[mapSize][mapSize];
block blockArrayNext[3*mapSize][3*mapSize];
block blockGhostArray[3*mapSize][3*mapSize];
int fishPercent = 50;
int sharkPercent = 25;
int frameRate = 0;
int threadNumber=1;
int currentThreads;
int iterationsTest;
int userMapSize;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{


    speedData << "No of iterations,"<<"time taken,"<<"number of cores,"<<"visuals"<<"map size"<<std::endl;


    qDebug()<< ID;

    //initialise bolckArrayNExt as all dead
    for (int i = 0; i < 3*userMapSize; ++i){
        for (int j = 0; j < 3*userMapSize; ++j){

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
//#pragma omp parallel
//    {
//    ui->textBrowser->append(QString::number(omp_get_thread_num()));
//}
//display data
   // int numThreads =omp_get_max_threads();
   // ui->textBrowser->append(QString::number(numThreads));


    //set ghoast array to current array
            omp_set_dynamic(0);
int MaxthreadNumber = ui->spinBox->value();
omp_set_num_threads(threadNumber);
#pragma omp parallel num_threads(threadNumber)
    {



     currentThreads = omp_get_num_threads();


#pragma omp for

    for (int i = 0; i < userMapSize; ++i){
        for (int j = 0; j < userMapSize; ++j){

        blockGhostArray[i][j]=blockArray[i][j];
        }
    }

    //10-2*mapSize, 0-10
    #pragma omp for
    for (int i = userMapSize; i < 2*userMapSize; ++i){
        for (int j = 0; j < userMapSize; ++j){

        blockGhostArray[i][j]=blockArray[i-userMapSize][j];
        }
    }
    //0-3*mapSize, 0-10
    #pragma omp for
    for (int i = userMapSize; i < 2*userMapSize; ++i){
        for (int j = 0; j < userMapSize; ++j){

        blockGhostArray[i][j]=blockArray[i-userMapSize][j];
        }
    }
    //2*mapSize-3*mapSize, 0-10
    #pragma omp for
    for (int i = 2*userMapSize; i < 3*userMapSize; ++i){
        for (int j = 0; j < userMapSize; ++j){

        blockGhostArray[i][j]=blockArray[i-2*userMapSize][j];
        }
    }
    //0-10, 10-2*mapSize
    #pragma omp for
    for (int i = 0; i < userMapSize; ++i){
        for (int j = userMapSize; j < 2*userMapSize; ++j){

        blockGhostArray[i][j]=blockArray[i][j-userMapSize];
        }
    }
    //0-10, 2*mapSize-3*mapSize
    #pragma omp for
    for (int i = 0; i < userMapSize; ++i){
        for (int j = 2*userMapSize; j < 3*userMapSize; ++j){

            blockGhostArray[i][j]=blockArray[i][j-2*userMapSize];
        }
    }
    //2*mapSize-3*mapSize, 2*mapSize-3*mapSize
    #pragma omp for
    for (int i = 2*userMapSize; i < 3*userMapSize; ++i){
        for (int j = 2*userMapSize; j < 3*userMapSize; ++j){

            blockGhostArray[i][j]=blockArray[i-2*userMapSize][j-2*userMapSize];
        }
    }
    //10-2*mapSize, 10-2*mapSize
    #pragma omp for
    for (int i = userMapSize; i < 2*userMapSize; ++i){
        for (int j = userMapSize; j < 2*userMapSize; ++j){

        blockGhostArray[i][j]=blockArray[i-userMapSize][j-userMapSize];
        }

    }
    //10-2*mapSize, 2*mapSize-3*mapSize
    #pragma omp for
    for (int i = userMapSize; i < 2*userMapSize; ++i){
        for (int j = 2*userMapSize; j < 3*userMapSize; ++j){

        blockGhostArray[i][j]=blockArray[i-userMapSize][j-2*userMapSize];
        }

    }
    //2*mapSize-3*mapSize, 2*mapSize-3*mapSize
    #pragma omp for
    for (int i = 2*userMapSize; i < 3*userMapSize; ++i){
        for (int j = 2*userMapSize; j < 3*userMapSize; ++j){

        blockGhostArray[i][j]=blockArray[i-2*userMapSize][j-2*userMapSize];
        }

    }
    //2*mapSize-3*mapSize, mapSize-2*mapSize
    #pragma omp for
    for (int i = 2*userMapSize; i < 3*userMapSize; ++i){
        for (int j = userMapSize; j < 2*userMapSize; ++j){

        blockGhostArray[i][j]=blockArray[i-2*userMapSize][j-userMapSize];
        }

    }




    //apply rules

    int sharkNeighbours, fishNeighbours,fishOfBreedingAge,sharksOfBreedingAge;

    #pragma omp for
      for (int i = userMapSize; i <2*userMapSize; ++i){

        for (int j = userMapSize; j <2*userMapSize; ++j){
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
      #pragma omp for
    for (int i = userMapSize; i < 2*userMapSize; ++i){

        for (int j = userMapSize; j < 2*userMapSize; ++j){

            blockArray[i-userMapSize][j-userMapSize]=blockArrayNext[i][j];


        }

    }

}
//turn visuals on
if(ui->radioButton->isChecked()){
 frame2 = cv::Mat(userMapSize, userMapSize, CV_8UC3, cv::Vec3b(0,0,0));


    //for frame2
    for (int i = 0; i < userMapSize; ++i){
        for (int j = 0; j < userMapSize; ++j){

    frame2.at<cv::Vec3b>(cv::Point(i,j)) = blockArray[i][j].getColour();
        }
    }

 QImage img2 = QImage((uchar*) frame2.data,frame2.cols,frame2.rows,frame2.step, QImage::Format_RGB888);
 QPixmap pix2 = QPixmap::fromImage(img2);

 ui->label_2->setPixmap(pix2.scaled(400, 400, Qt::IgnoreAspectRatio, Qt::FastTransformation));
 //save image after a certain iterations

 switch(count){
        case 0:
 {
            QFile file("0iterations_size"+QString::number(userMapSize)+"_"+QString::number(currentThreads)+"threads.png");
            file.open(QIODevice::WriteOnly);
            pix2.scaled(400,400,Qt::IgnoreAspectRatio,Qt::FastTransformation).save(&file,"PNG");
            file.close();
            break;
 }
        case 100:
 {
            QFile file1("100iterations_size"+QString::number(userMapSize)+"_"+QString::number(currentThreads)+"threads.png");
            file1.open(QIODevice::WriteOnly);
            pix2.scaled(400,400,Qt::IgnoreAspectRatio,Qt::FastTransformation).save(&file1,"PNG");
            file1.close();
            break;
 }
        case 500:
 {
            QFile file2("500iterations_size"+QString::number(userMapSize)+"_"+QString::number(currentThreads)+"threads.png");
            file2.open(QIODevice::WriteOnly);
            pix2.scaled(400,400,Qt::IgnoreAspectRatio,Qt::FastTransformation).save(&file2,"PNG");
            file2.close();
            break;
 }
        case 1000:
 {
             QFile file3("1000iterations_size"+QString::number(userMapSize)+"_"+QString::number(currentThreads)+"threads.png");
             file3.open(QIODevice::WriteOnly);
             pix2.scaled(400,400,Qt::IgnoreAspectRatio,Qt::FastTransformation).save(&file3,"PNG");
             file3.close();
            break;
 }
        case 10000:
{
      QFile file4("10000iterations_size"+QString::number(userMapSize)+"_"+QString::number(currentThreads)+"threads.png");
      file4.open(QIODevice::WriteOnly);
      pix2.scaled(400,400,Qt::IgnoreAspectRatio,Qt::FastTransformation).save(&file4,"PNG");
      file4.close();
     break;
}
            default:
            break;
    }


}
//qDebug()<<count;


//ui->textBrowser->append(QString::number(ID));
ui->label->update();
count++;

//increment age of all blocks

for(int i=0; i<userMapSize;i++){
    for(int j = 0; j<userMapSize;j++){
        blockArray[i][j].blockAge ++;
    }
}

QThread::msleep(frameRate);
//display elapsed time
int timeInMs = runTime.elapsed();
ui->lcdNumber_6->display(QString::number(timeInMs));
iterationsTest = ui->IterationsSpinBox->value();
if(ui->radioButton_3->isChecked() && runs <=5&&threadNumber<=4){


    if(count==iterationsTest){
        ui->lcdNumber_7->display(QString::number(timeInMs));
        //speedData << "No of iterations,"<<"time taken,"<<"number of cores,"<<"visuals"<<"map size"<<std::endl;
        speedData<<iterationsTest<<","<<ui->lcdNumber_6->value()<<","<<ui->spinBox->value()<<","<<ui->radioButton->isChecked()<<userMapSize<<std::endl;
        ui->textBrowser_3->append(QString::number(ui->lcdNumber_6->value()));
         ui->textBrowser->append(QString::number(currentThreads));
         ui->textBrowser->append(QString::number(threadNumber));
         ui->textBrowser_4->append("mapsize: "+QString::number(userMapSize));
        runs++;
        count =0;
        ui->pushButton->clicked();


        }
    //change map size
    if(threadNumber==MaxthreadNumber+1 &&runs >2){
        userMapSize = userMapSize*2;
        ui->gridSizeSpinBox->setValue(userMapSize);
    }
    //reset runs
    if(runs>2){

        threadNumber++;
        runs=0;

    }
//reset threads
    if(threadNumber==MaxthreadNumber+1){
        ui->textBrowser_4->setText("finished speed test with mapsize: "+QString::number(userMapSize/2));
        threadNumber =1;


        //speedData.close();
    }

    if(userMapSize>=1000){
        speedData.close();
        ui->textBrowser_4->setText("testing finished");
        userMapSize = 0;
    }
    }
}

void MainWindow::on_pushButton_clicked()
{
    //start timer
    //const double startTime = omp_get_wtime();
    userMapSize = ui->gridSizeSpinBox->value();
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
    int startingFish = (userMapSize*userMapSize)*(fishPercent/100.0);
    int startingShark = ((userMapSize*userMapSize)*(sharkPercent/100.0));
    int totalGridSize = userMapSize*userMapSize;
      int a[mapSize][mapSize];
  if(ui->radioButton_3->isChecked()){


    //create an array with values 1 to 100
    for(int i=0;i<userMapSize;i++){
       for(int j=0; j<userMapSize;j++){
           randArray[i][j] = arrayCount;
           arrayCount++;
       }
       arrayCount++;
    }
    //swap array values randomly
    for(int i=0;i<userMapSize;i++){
       for(int j=0; j<userMapSize;j++){
           iRand = rand() % userMapSize;
           jRand = rand() % userMapSize;
           temp =  randArray[i][j];
          randArray[i][j] = randArray[iRand][jRand];
          randArray[iRand][jRand]=temp;

       }

    }



    for (int i =0; i<userMapSize;++i){
        for(int j=0; j<userMapSize; ++j){


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
        for(int i=0;i<userMapSize;i++){
            for(int j=0;j<userMapSize;j++){
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






