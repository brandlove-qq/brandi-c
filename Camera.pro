#-------------------------------------------------
#
# Project created by QtCreator 2016-11-28T12:01:42
#
#-------------------------------------------------

QT       -= gui
QT       += network
QT       += core
QT       += widgets
QT       += multimedia
QT       += multimediawidgets

TARGET = Camera
TEMPLATE = lib
CONFIG +=dll thread C++11

DEFINES += CAMERA_LIBRARY

SOURCES += \
    camera.cpp

HEADERS += camera.h\
    ../../include/concurrentqueue.h \
    ../../include/icameramodule.h \
    ../../include/qzskjdatastream.hpp \
        camera_global.h \
    ../../include/er_types.h \

#opencv
INCLUDEPATH += "../../../../3rdLibrary/opencv-4.1.0/vc14-x64/include" \
                "../../../../3rdLibrary/opencv-4.1.0/vc14-x64/include/opencv2"

CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../3rdLibrary/opencv-4.1.0/vc14-x64/x64/vc14/lib/ -lopencv_world410
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../3rdLibrary/opencv-4.1.0/vc14-x64/x64/vc14/lib/ -lopencv_world410d

#other path
INCLUDEPATH += "../../include"

unix {
    target.path = /usr/lib
    INSTALLS += target
}
