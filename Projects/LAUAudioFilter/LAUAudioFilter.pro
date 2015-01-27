#-------------------------------------------------
#
# Project created by QtCreator 2015-01-02T19:26:56
#
#-------------------------------------------------

QT      += core gui widgets multimedia opengl serialport
TEMPLATE = app
TARGET   = LAUAudioFilter

SOURCES += main.cpp\
           lauaudiowidget.cpp \
           lauaudioobject.cpp \
           lauaudioglwidget.cpp \
           lauaudiofilterobject.cpp

HEADERS += lauaudiowidget.h \
           lauaudioobject.h \
           lauaudioglwidget.h \
           lauaudiofilterobject.h

OTHER_FILES += LAUAudioGLWidget.vert \
               LAUAudioGLWidget.frag

RESOURCES   += LAUAudioGLWidget.qrc \
               LAUAudioFilterObject.qrc

DEFINES += RECORDDEBUGDATA
