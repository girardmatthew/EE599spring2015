#version 410 core

uniform int qt_width;       // KEEPS TRACK OF THE WIDGET'S WIDTH IN PIXELS
uniform int qt_height;      // KEEPS TRACK OF THE WIDGET'S HEIGHT IN PIXELS
uniform int qt_anchor;      // KEEPS TRACK OF THE STARTING POINT ON SCREEN
uniform float qt_threshold; // KEEPS TRACK OF THE VOLUME THRESHOLD

 in float qt_sample;         // FLOATING POINT SAMPLE VALUE BETWEEN -1.0 AND +1.0
out float qt_flag;           // INDICATES IF VOLUME EXCEEDS THRESHOLD

void main(void)
{
    float lambda = float(gl_VertexID + qt_width - qt_anchor)/float(qt_width);
    float xCoord = 2.0*(lambda - floor(lambda)) - 1.0;
    float yCoord = qt_sample;

    // SIMPLY PASS THE INCOMING VERTEX TO THE SCREEN
    qt_flag = float(abs(yCoord) > qt_threshold);
    gl_Position = vec4(xCoord, yCoord, 0.0, 1.0);
}
