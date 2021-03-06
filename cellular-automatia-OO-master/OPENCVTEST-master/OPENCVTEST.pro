#-------------------------------------------------
#
# Project created by QtCreator 2016-03-05T18:53:09
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OPENCVTEST
TEMPLATE = app

QMAKE_LFLAGS +=/STACK:32000000
QMAKE_CXXFLAGS += -openmp -arch:AVX -D "_CRT_SECURE_NO_WARNINGS"
QMAKE_CXXFLAGS_RELEASE *= -O2
QMAKE_CC = mpicc
QMAKE_CXX = mpic++


SOURCES += main.cpp\
        mainwindow.cpp \
    shark.cpp \
    block.cpp \
    world.cpp

HEADERS  += mainwindow.h \
    block.h \
    world.h \
    shark.h

FORMS    += mainwindow.ui \
    shark.ui

INCLUDEPATH+=C:\\opencv\\build\\install\\include \
           C:\MPI\Include

LIBS+=-LC:\\opencv\\mybuild\\lib\\Debug\
    -lopencv_imgcodecs300d \
    -lopencv_imgproc300d \
    -lopencv_ml300d \
    -lopencv_objdetect300d \
    -lopencv_photo300d \
    -lopencv_shape300d \
    -lopencv_stitching300d \
    -lopencv_superres300d \
    -lopencv_ts300d \
    -lopencv_video300d \
    -lopencv_videoio300d \
    -lopencv_videostab300d \
    -lopencv_calib3d300d \
    -lopencv_core300d \
    -lopencv_features2d300d \
    -lopencv_flann300d \
    -lopencv_hal300d \
    -lopencv_highgui300d \
    C:\MPI\Lib\x86\msmpi.lib
