#ifndef SHARK_H
#define SHARK_H

#include <QMainWindow>
#include<QTime>
#include<QTimer>
#include"world.h"
#include"block.h"

namespace Ui {
class Shark;
}

class Shark : public QMainWindow
{
    Q_OBJECT

public:
    explicit Shark(QWidget *parent = 0);
   void updateGUIShark();
   world *newWorld;
    ~Shark();

private:
    Ui::Shark *ui;
    QTimer *timerShark;
};

#endif // SHARK_H
