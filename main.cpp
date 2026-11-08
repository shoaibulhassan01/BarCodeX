// main.cpp  -  BarcodeX entry point
// Starts Qt, connects the UI to the barcode code, and shows the window.
//
// The UI itself lives in ui/ui.cpp. The only job of this file is to hand it
// the two functions it needs (generate and decode).

#include <memory>

#include <QApplication>
#include <QImage>
#include <QString>
#include <QWidget>

#include "ui/ui.h"

// ---------------------------------------------------------------------
//  Connection points for the team.
//  Replace the body of each function with calls to the real modules:
//    generate :  encoder (digits -> patterns)  ->  generator (patterns -> image)
//    decode   :  scanner (image -> run widths) ->  decoder   (run widths -> digits)
// ---------------------------------------------------------------------

// digits: exactly 6 digits, first digit is 1-9.
// Return true and fill imgOut on success, otherwise return false and set errorOut.
static bool generateBarcode(const QString &digits, QImage &imgOut, QString &errorOut)
{
    Q_UNUSED(digits);
    Q_UNUSED(imgOut);
    errorOut = "Generator not connected yet";   // PLACEHOLDER
    return false;
}

// img: the image the user opened.
// Return true and fill digitsOut (6 digits) on success, otherwise return false and set errorOut.
static bool decodeBarcode(const QImage &img, QString &digitsOut, QString &errorOut)
{
    Q_UNUSED(img);
    Q_UNUSED(digitsOut);
    errorOut = "Decoder not connected yet";     // PLACEHOLDER
    return false;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    UiBackend backend;
    backend.generate = generateBarcode;
    backend.decode   = decodeBarcode;

    std::unique_ptr<QWidget> window(createMainWindow(backend));
    window->show();

    return app.exec();
}
