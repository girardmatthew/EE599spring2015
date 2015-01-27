#include "lauaudioglwidget.h"

#ifdef Q_OS_WIN
//#define NAN 0.0f/0.0f;
#else
#include <math.h>
#endif

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
void LAUAudioGLWidget::initializeGL()
{
    // MAP EMBEDDED OPENGL FUNCTIONS TO DESKTOP FUNCTIONS
    initializeOpenGLFunctions();

    // SET THE BACKGROUND COLOR TO BLACK
    qglClearColor(QColor(0,0,0));

    // INITIALIZE OUR SHADER PROGRAM FOR DISPLAYING AUDIO SAMPLES ON SCREEN
    setlocale(LC_NUMERIC, "C");
    if (!program.addShaderFromSourceFile(QOpenGLShader::Vertex,   ":/shaders/LAUAudioGLWidget.vert")) close();
    if (!program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/LAUAudioGLWidget.frag")) close();
    if (!program.link()) close();
    setlocale(LC_ALL, "");

    // CREATE THE VERTEX ARRAY OBJECT FOR FEEDING VERTICES TO OUR SHADER PROGRAMS
    vertexArrayObject.create();
    vertexArrayObject.bind();

    // CREATE VERTEX BUFFER TO HOLD CORNERS OF QUADRALATERAL
    vertexBuffer = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    vertexBuffer.create();
    vertexBuffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    if (vertexBuffer.bind()){
        // ALLOCATE THE VERTEX BUFFER FOR HOLDING THE AUDIO DATA
        vertexBuffer.allocate(AUDIOBUFFERLENGTH*sizeof(float));
        float *buffer = (float*)vertexBuffer.map(QOpenGLBuffer::WriteOnly);
        if (buffer){
            for (int n=0; n<AUDIOBUFFERLENGTH; n++){
                buffer[n] = NAN;
            }
            vertexBuffer.unmap();
        }
        vertexBuffer.release();
    }

    // CREATE INDEX BUFFER TO ORDERINGS OF VERTICES FORMING POLYGON
    indiceBuffer = QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    indiceBuffer.create();
    indiceBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    if (indiceBuffer.bind()){
        indiceBuffer.allocate(AUDIOBUFFERLENGTH*sizeof(unsigned int));
        unsigned int *indices = (unsigned int*)indiceBuffer.map(QOpenGLBuffer::WriteOnly);
        if (indices){
            for (int n=0; n<AUDIOBUFFERLENGTH; n++){
                indices[n] = n;
            }
            indiceBuffer.unmap();
        } else {
            qDebug() << QString("indiceBuffer buffer mapped from GPU.");
        }
        indiceBuffer.release();
    }
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
void LAUAudioGLWidget::onUpdateBuffer(float *buffer, int samples)
{
    // MAKE SURE WE HAVE ATLEAST ONE VALID SAMPLE
    if (samples > 0) {
        // MAKE THIS CONTEXT THE CURRENT CONTEXT TO RECIEVE OUR OPENGL COMMANDS
        makeCurrent();

        // BIND THE VERTEX BUFFER SO WE CAN COPY SAMPLES INTO IT
        if (vertexBuffer.bind()){
            float *vertices = (float*)vertexBuffer.map(QOpenGLBuffer::ReadWrite);
            if (vertices){
                if ((sampleCounter + samples) > AUDIOBUFFERLENGTH){
                    // FILL UP REMAINING SPACE WITH INCOMING SAMPLES
                    int samplesAvailable = AUDIOBUFFERLENGTH - sampleCounter;
                    memcpy(&vertices[sampleCounter], buffer, samplesAvailable*sizeof(float));

                    // COPY THE REMAINING SAMPLES TO THE BEGINNING OF THE BUFFER
                    sampleCounter = samples - samplesAvailable;
                    memcpy(&vertices[0], &buffer[samplesAvailable], sampleCounter*sizeof(float));
                } else {
                    // COPY THE ENTIRE INCOMING BUFFER TO THE GRAPHICS CARD
                    memcpy(&vertices[sampleCounter], buffer, samples*sizeof(float));
                    sampleCounter += samples;
                }
                vertexBuffer.unmap();
            }
            vertexBuffer.release();
        }
        // TELL THE WIDGET TO UPDATE ITSELF ON SCREEN
        update();
    }
    // EMIT THE BUFFER FOR FURTHER PROCESSING
    emit emitBuffer(buffer, samples);
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
void LAUAudioGLWidget::paintGL()
{
    // BIND THE GLSL PROGRAMS RESPONSIBLE FOR CONVERTING OUR FRAME BUFFER
    // OBJECT TO AN XYZ+TEXTURE POINT CLOUD FOR DISPLAY ON SCREEN
    if (program.bind()){
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glPointSize(2.0f);

        // BIND VBOS FOR DRAWING TRIANGLES ON SCREEN
        vertexBuffer.bind();
        indiceBuffer.bind();

        // SET THE PROJECTION MATRIX IN THE SHADER PROGRAM
        program.setUniformValue("qt_width", AUDIOBUFFERLENGTH);
        program.setUniformValue("qt_height", height());
        program.setUniformValue("qt_anchor", sampleCounter);
        program.setUniformValue("qt_threshold", threshold);
        program.setUniformValue("qt_markerColorA", QVector3D(1.0f, 0.0f, 0.0f));
        program.setUniformValue("qt_markerColorB", QVector3D(1.0f, 1.0f, 0.0f));

        // Tell OpenGL programmable pipeline how to locate vertex position data
        glVertexAttribPointer(program.attributeLocation("qt_sample"), 1, GL_FLOAT, GL_FALSE, sizeof(float), 0);
        program.enableAttributeArray("qt_sample");

        // DRAW AUDIO SAMPLES ON SCREEN AS POINTS
        glDrawElements(GL_POINTS, AUDIOBUFFERLENGTH, GL_UNSIGNED_INT, 0);

        // RELEASE THE FRAME BUFFER OBJECT AND ITS ASSOCIATED GLSL PROGRAMS
        vertexBuffer.release();
        indiceBuffer.release();
        program.release();
    }
}
