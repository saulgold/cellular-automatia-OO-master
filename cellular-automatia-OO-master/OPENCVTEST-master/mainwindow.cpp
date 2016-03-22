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
//   timer = new QTimer(this);
//   connect(timer,SIGNAL(timeout()),this,SLOT(on_pushButton_clicked());
//cv::namedWindow("game of life",cv::WINDOW_AUTOSIZE);
//cv::namedWindow("video",cv::WINDOW_AUTOSIZE);
// cv::Mat frame;

cv::Scalar pixelScalar;
//initialise arrayNExt with zeros

//for (int i = 0; i < 30; ++i){
//    for (int j = 0; j < 30; ++j){
//        arrayNext[i][j] = 0;
//    }

//}
cv::Mat frame, frame2;
int const mapSize = 50;
int array[mapSize][mapSize];
block blockArray[mapSize][mapSize];

cv::Scalar arrayScalar[mapSize][mapSize];



    //initialise arrayNExt with zeros
    int arrayNext[3*mapSize][3*mapSize];
    block blockArrayNext[3*mapSize][3*mapSize];
    //int goastArray[30][30];
    //for (int i = 0; i < 30; ++i){
    //	for (int j = 0; j < 30; ++){
    //		if (i/3 =10)
    //		goastArray[i][j] = array[i][j]
    //	}
    //}
    //0-10, 0-10
    int ghostArray[3*mapSize][3*mapSize];
    block blockGhostArray[3*mapSize][3*mapSize];


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    for (int i = 0; i < 3*mapSize; ++i){
        for (int j = 0; j < 3*mapSize; ++j){
            arrayNext[i][j] = 0;
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

    //set ghoast array to current array

    for (int i = 0; i < mapSize; ++i){
        for (int j = 0; j < mapSize; ++j){
            ghostArray[i][j] = array[i][j];
        blockGhostArray[i][j]=blockArray[i][j];
        }
    }
    //10-2*mapSize, 0-10
    for (int i = mapSize; i < 2*mapSize; ++i){
        for (int j = 0; j < mapSize; ++j){
            ghostArray[i][j] = array[i-mapSize][j];
        blockGhostArray[i][j]=blockArray[i-mapSize][j];
        }
    }
    //0-3*mapSize, 0-10
    for (int i = mapSize; i < 2*mapSize; ++i){
        for (int j = 0; j < mapSize; ++j){
            ghostArray[i][j] = array[i - mapSize][j];
        blockGhostArray[i][j]=blockArray[i-mapSize][j];
        }
    }
    //2*mapSize-3*mapSize, 0-10
    for (int i = 2*mapSize; i < 3*mapSize; ++i){
        for (int j = 0; j < mapSize; ++j){
            ghostArray[i][j] = array[i - 2*mapSize][j];
        blockGhostArray[i][j]=blockArray[i-2*mapSize][j];
        }
    }
    //0-10, 10-2*mapSize
    for (int i = 0; i < mapSize; ++i){
        for (int j = mapSize; j < 2*mapSize; ++j){
            ghostArray[i][j] = array[i][j - mapSize];
        blockGhostArray[i][j]=blockArray[i][j-mapSize];
        }
    }
    //0-10, 2*mapSize-3*mapSize
    for (int i = 0; i < mapSize; ++i){
        for (int j = 2*mapSize; j < 3*mapSize; ++j){
            ghostArray[i][j] = array[i][j - 2*mapSize];
            blockGhostArray[i][j]=blockArray[i][j-2*mapSize];
        }
    }
    //2*mapSize-3*mapSize, 2*mapSize-3*mapSize
    for (int i = 2*mapSize; i < 3*mapSize; ++i){
        for (int j = 2*mapSize; j < 3*mapSize; ++j){
            ghostArray[i][j] = array[i-2*mapSize][j - 2*mapSize];
            blockGhostArray[i][j]=blockArray[i-2*mapSize][j-2*mapSize];
        }
    }
    //10-2*mapSize, 10-2*mapSize
    for (int i = mapSize; i < 2*mapSize; ++i){
        for (int j = mapSize; j < 2*mapSize; ++j){
            ghostArray[i][j] = array[i - mapSize][j - mapSize];
        blockGhostArray[i][j]=blockArray[i-mapSize][j-mapSize];
        }

    }
    //10-2*mapSize, 2*mapSize-3*mapSize
    for (int i = mapSize; i < 2*mapSize; ++i){
        for (int j = 2*mapSize; j < 3*mapSize; ++j){
            ghostArray[i][j] = array[i - mapSize][j - 2*mapSize];
        blockGhostArray[i][j]=blockArray[i-mapSize][j-2*mapSize];
        }

    }
    //2*mapSize-3*mapSize, 2*mapSize-3*mapSize
    for (int i = 2*mapSize; i < 3*mapSize; ++i){
        for (int j = 2*mapSize; j < 3*mapSize; ++j){
            ghostArray[i][j] = array[i - 2*mapSize][j - 2*mapSize];
        blockGhostArray[i][j]=blockArray[i-2*mapSize][j-2*mapSize];
        }

    }
    //2*mapSize-3*mapSize, mapSize-2*mapSize
    for (int i = 2*mapSize; i < 3*mapSize; ++i){
        for (int j = mapSize; j < 2*mapSize; ++j){
            ghostArray[i][j] = array[i - 2*mapSize][j - mapSize];
        blockGhostArray[i][j]=blockArray[i-2*mapSize][j-mapSize];
        }

    }





    //apply rules
    int neighbours;
    int sharkNeighbours, fishNeighbours;
      for (int i = mapSize; i <2*mapSize; ++i){
        for (int j = mapSize; j <2*mapSize; ++j){
            neighbours = ghostArray[i + 1][j] + ghostArray[i][j + 1] + ghostArray[i + 1][j + 1] + ghostArray[i - 1][j - 1] + ghostArray[i - 1][j] + ghostArray[i][j - 1] + ghostArray[i + 1][j - 1]+ghostArray[i-1][j + 1];
            sharkNeighbours =  blockGhostArray[i + 1][j].isShark + blockGhostArray[i][j + 1].isShark + blockGhostArray[i + 1][j + 1].isShark + blockGhostArray[i - 1][j - 1].isShark + blockGhostArray[i - 1][j].isShark + blockGhostArray[i][j - 1].isShark + blockGhostArray[i + 1][j - 1].isShark+blockGhostArray[i-1][j + 1].isShark;


            //lonely rule
            if (neighbours <2){

                arrayNext[i][j] =0;
            }
            else if(neighbours > 3)
            {
                arrayNext[i][j] = 0;
            }
            else if (neighbours == 3){
                arrayNext[i][j] =1;
            }
            else if (neighbours==2)
            {
                arrayNext[i][j] = ghostArray[i][j];
                //arrayNext[i][j] =0;
            }
            }

        }
    //display new array
    for (int i = mapSize; i < 2*mapSize; ++i){
        for (int j = mapSize; j < 2*mapSize; ++j){


            array[i-mapSize][j-mapSize] = arrayNext[i][j];

        }

    }


//    for (int i = mapSize; i < 2*mapSize; ++i){
//        for (int j = mapSize; j < 2*mapSize; ++j){
//            if(arrayNext[i][j]==1){
//                arrayNext[i][j]=255;
//                arrayScalar[i-mapSize][j-mapSize] = (255,0,255);
//            }
//            else if(arrayNext[i][j]==0){
//                 arrayNext[i][j]=0;
//                // arrayScalar[i-mapSize][j-mapSize] = (0,255,0);
//            }
//             else if(arrayNext[i][j]==2)
//                  arrayNext[i][j]=0;
//        }

//    }


   frame = cv::Mat(mapSize, mapSize, CV_8UC3, cv::Vec3b(0,0,0));
  // frame =  cv::Mat(3*mapSize,3*mapSize,CV_16UC1, arrayNext);
 frame2 = cv::Mat(mapSize, mapSize, CV_8UC3, cv::Vec3b(0,0,0));

    for (int i = 0; i < mapSize; ++i){
        for (int j = 0; j < mapSize; ++j){
            cv::Vec3b colour = frame.at<cv::Vec3b>(cv::Point(i,j));
            colour[0] = 0;
            colour[1]=200*array[i][j];
            colour[2]=0;

    frame.at<cv::Vec3b>(cv::Point(i,j)) = colour;
        }}
    //for frame2
    for (int i = 0; i < mapSize; ++i){
        for (int j = 0; j < mapSize; ++j){

    frame2.at<cv::Vec3b>(cv::Point(i,j)) = blockArray[i][j].getColour();
        }}


//   cv::Mat mask;
//   cv::inRange(frame, cv::Scalar(0,0,255), cv::Scalar(0,0,255), mask);
//   frame.setTo(cv::Scalar(100,0,100), mask);



//    std::string filename = "filename" +std::to_string(count)+".jpg";

//    cv::imwrite(filename,frame);

 QImage img = QImage((uchar*) frame.data,frame.cols,frame.rows,frame.step, QImage::Format_RGB888);
   QPixmap pix = QPixmap::fromImage(img);

  ui->label->setPixmap(pix.scaled(400, 400, Qt::IgnoreAspectRatio, Qt::FastTransformation));

  QImage img2 = QImage((uchar*) frame2.data,frame2.cols,frame2.rows,frame2.step, QImage::Format_RGB888);
    QPixmap pix2 = QPixmap::fromImage(img2);

   ui->label_2->setPixmap(pix2.scaled(400, 400, Qt::IgnoreAspectRatio, Qt::FastTransformation));

qDebug()<<count;

//ui->label->update();
count++;

 QThread::msleep(100);

}
void MainWindow::on_pushButton_clicked()
{
   // cv::VideoCapture cap;
    //cap.open(0);
    //cv::Mat vid;
    int randArray[mapSize][mapSize];
    for(int i=0;i<mapSize;i++){
       for(int j=0; j<mapSize;j++){
           randArray[i][j] = rand()&100;
       }
    }

    for (int i =0; i<mapSize;++i){
        for(int j=0; j<mapSize; ++j){
            if(randArray[i][j] >50){
            array[i][j]=1;
            blockArray[i][j].setFish();
            }
            else if(randArray[i][j]<25){
                blockArray[i][j].setShark();
                array[i][j]=0;

            }
            else{array[i][j]=0;
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
