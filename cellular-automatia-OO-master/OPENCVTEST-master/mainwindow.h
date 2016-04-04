#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include "opencv2/imgproc/types_c.h"
#include <QPicture>
#include<iostream>
#include <QMainWindow>
#include <QTime>
#include <omp.h>
#include <shark.h>
#include <fstream>
#include <sstream>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    //int mapSize = 100;
    int numberOfSharks;
    int numberOfFish;
    //int getMapSize(void){return mapSize;}
private slots:
    void on_pushButton_clicked();
    void updateGUI();




private:
    Ui::MainWindow *ui;
    QTimer *timer;
    Shark *shark;



};

#endif // MAINWINDOW_H
