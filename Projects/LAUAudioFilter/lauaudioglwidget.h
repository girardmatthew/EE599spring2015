#ifndef LAUAUDIOGLWIDGET_H
#define LAUAUDIOGLWIDGET_H

#include <QScreen>
#include <QGLWidget>
#include <QGLFormat>
#include <QApplication>
#include <QOpenGLBuffer>
#include <QDesktopWidget>
#include <QGuiApplication>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>

#include "lauaudioobject.h"

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
class LAUAudioGLWidget : public QGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit LAUAudioGLWidget(const QGLFormat& format, QWidget *parent = NULL) : QGLWidget(format, parent), sampleCounter(0), threshold(0.5f) { ; }
    ~LAUAudioGLWidget() { ; }

protected:
    void resizeGL(int w, int h)
    {
        // Get the Desktop Widget so that we can get information about multiple monitors connected to the system.
        QDesktopWidget *dkWidget = QApplication::desktop();
        QList<QScreen*> screenList = QGuiApplication::screens();
        int devicePixelRatio = screenList[dkWidget->screenNumber(this)]->devicePixelRatio();  // Get the devicePixel Ratio for the Current Screen
        glViewport(0, 0, w*devicePixelRatio, h*devicePixelRatio);
    }
    void initializeGL();
    void paintGL();

public slots:
    void onSetThreshold(int val) { onSetThreshold((float)val/100.0f); }
    void onSetThreshold(float val) { threshold = val; }
    void onUpdateBuffer(float *buffer, int samples);

private:
    int sampleCounter;
    int hiDPIScaleFactor;
    float threshold;
    QOpenGLBuffer indiceBuffer, vertexBuffer;
    QOpenGLVertexArrayObject vertexArrayObject;
    QOpenGLShaderProgram program;

signals:
    void emitBuffer(float *buffer, int samples);
};

#endif // LAUAUDIOGLWIDGET_H
