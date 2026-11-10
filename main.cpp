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

#include "common/common.h"
#include "decoder/decoder.h"
#include "encoder/encoder.h"
#include "generator/generator.h"
#include "scanner/scanner.h"
#include "ui/ui.h"

// digits: exactly 6 digits, first digit is 1-9.
// Return true and fill imgOut on success, otherwise return false and set errorOut.
static bool generateBarcodeForUi(const QString &digits, QImage &imgOut, QString &errorOut)
{
    const vector<string> patterns = encodeText(digits.toStdString());
    if (patterns.empty()) {
        errorOut = "The encoder could not create barcode patterns.";
        return false;
    }

    const Image barcode = generateBarcode(patterns);
    if (barcode.width <= 0 || barcode.height <= 0 ||
        barcode.pixels.size() != static_cast<size_t>(barcode.width) * barcode.height) {
        errorOut = "The barcode generator returned an invalid image.";
        return false;
    }

    imgOut = QImage(barcode.width, barcode.height, QImage::Format_Grayscale8);
    if (imgOut.isNull()) {
        errorOut = "Could not allocate memory for the barcode image.";
        return false;
    }

    for (int y = 0; y < barcode.height; ++y) {
        uchar *row = imgOut.scanLine(y);
        for (int x = 0; x < barcode.width; ++x) {
            row[x] = static_cast<uchar>(barcode.pixels[y * barcode.width + x]);
        }
    }

    errorOut.clear();
    return true;
}

// img: the image the user opened.
// Return true and fill digitsOut (6 digits) on success, otherwise return false and set errorOut.
static bool decodeBarcode(const QImage &img, QString &digitsOut, QString &errorOut)
{
    const string pattern = imageToString(img);
    if (pattern.empty()) {
        errorOut = "No valid Code 39 barcode pattern was found in the image.";
        return false;
    }

    string digits;
    string decodeError;
    if (!decodeRuns(pattern, digits, decodeError)) {
        errorOut = QString::fromStdString(decodeError);
        return false;
    }

    digitsOut = QString::fromStdString(digits);
    errorOut.clear();
    return true;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    UiBackend backend;
    backend.generate = generateBarcodeForUi;
    backend.decode   = decodeBarcode;

    std::unique_ptr<QWidget> window(createMainWindow(backend));
    window->show();

    return app.exec();
}
