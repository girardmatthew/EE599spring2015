#version 410 core

uniform vec3 qt_markerColorA; // HOLDS THE CURRENT COLOR FOR LOW VOLUME SAMPLES
uniform vec3 qt_markerColorB; // HOLDS THE CURRENT COLOR FOR HIGH VOLUME SAMPLES
in float qt_flag;             // INDICATES IF VOLUME EXCEEDS THRESHOLD

layout(location = 0, index = 0) out vec4 qt_fragColor;

void main()
{
    qt_fragColor = vec4(qt_flag * qt_markerColorB + (1.0f - qt_flag) * qt_markerColorA, 1.0f);
}
