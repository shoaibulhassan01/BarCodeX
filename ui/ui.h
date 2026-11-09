// ui/ui.h  -  BarcodeX user interface (Qt Widgets)
// Owner: Abdul Ahad. Other team members do not need to edit the UI files.
#pragma once

#include <functional>
#include <QImage>
#include <QString>

class QWidget;

// The two things the UI needs from the rest of the project.
// main.cpp fills these in (that is where encoder/generator and
// scanner/decoder get connected). If one is left empty, the UI shows
// "... not connected yet" instead of crashing.
struct UiBackend
{
    // Input : exactly 6 digits (the UI already checked length and first digit).
    // Output: true + imgOut = the barcode image, or false + errorOut = message.
    std::function<bool(const QString &digits, QImage &imgOut, QString &errorOut)> generate;

    // Input : the image the user opened.
    // Output: true + digitsOut = the 6 digits, or false + errorOut = message.
    std::function<bool(const QImage &img, QString &digitsOut, QString &errorOut)> decode;
};

// Creates the BarcodeX window. The caller owns it and calls show().
QWidget *createMainWindow(const UiBackend &backend);
