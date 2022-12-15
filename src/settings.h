#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>

struct Settings {
    std::string sceneFilePath;
    int shapeParameter1 = 25;
    int shapeParameter2 = 25;
    float nearPlane = 0.1;
    float farPlane = 100.f;
    bool perPixelFilter = false;
    bool kernelBasedFilter = false;
    bool extraCredit1 = false;
    bool extraCredit2 = false;
    bool extraCredit3 = false;
    bool extraCredit4 = false;

    bool toonShade = false;
    bool glowRim = false;
    bool movingLight = false;

    int toon1 = 0;
    int toon2 = 1;
    double toon3 = 0.f;

    double toon4_blend = 0.01;
};


// The global Settings object, will be initialized by MainWindow
extern Settings settings;

#endif // SETTINGS_H
