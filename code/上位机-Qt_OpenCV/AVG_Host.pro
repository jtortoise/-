QT -= gui
QT += bluetooth

CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += /usr/local/include/opencv
LIBS += -L/usr/local/lib64/     \
        -lopencv_core           \
        -lopencv_imgproc        \
        -lopencv_aruco          \
        -lopencv_highgui        \
        -lopencv_calib3d        \
        -lopencv_videoio

SOURCES += main.cpp \
    Enginner.cpp \
    MarkerDetect.cpp \
    RobotVehicle.cpp \
    pid/PID_v1.cpp \
    blecom.cpp \
    commanager.cpp \
    motor.cpp

DISTFILES += \
    config.ini

HEADERS += \
    Enginner.h \
    MarkerDetect.h \
    RobotVehicle.h \
    ini/INIReader.h \
    pid/PID_v1.h \
    blecom.h \
    commanager.h \
    motor.h

RESOURCES += \
    resource.qrc
