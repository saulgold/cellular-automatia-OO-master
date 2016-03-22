#include "shark.h"
#include "ui_shark.h"


Shark::Shark(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Shark)
{

    ui->setupUi(this);
    newWorld = new world(100,100,25,50);

   QTimer *timerShark = new QTimer(this);;


   connect(timerShark,SIGNAL(timeout()),this,SLOT(updateGUIShark()));
     timerShark->start(20);


}

void Shark::updateGUIShark(){


}

Shark::~Shark()
{
    delete ui;
}
