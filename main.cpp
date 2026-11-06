#include "common/common.h"
#include "encoder/encoder.h"
#include "generator/generator.h"
#include "scanner/scanner.h"
#include "decoder/decoder.h"

#include <QApplication>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>
#include <iostream>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    cout << "Hello World" << endl;

    cout << "--- GENERATE ---" << endl;
    vector<string> patterns = encodeText("ABC");
    Image barcode = generateBarcode(patterns);
    saveImage(barcode, "samples/test.png");

    cout << "--- SCAN ---" << endl;
    Image img = loadImage("samples/test.png");
    Image bw = makeBlackWhite(img);
    int line = findLine(bw);
    vector<int> runs = getRuns(bw, line);

    string text, error;
    bool decoded = decodeRuns(runs, text, error);
    if (decoded) {
        cout << "Decoded: " << text << endl;
    } else {
        cout << "Error: " << error << endl;
    }

    QWidget window;
    window.setWindowTitle("BarCodeX");
    window.resize(600, 400);
    window.setStyleSheet("background-color: blue;");

    auto *label = new QLabel(
        decoded ? QString("Hello World!\nDecoded: %1").arg(QString::fromStdString(text))
                : QString("Hello World!\nError: %1").arg(QString::fromStdString(error)),
        &window);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet(
        "color: white;"
        "font-size: 30px;"
        "font-weight: bold;"
    );

    auto *layout = new QVBoxLayout(&window);
    layout->addWidget(label);

    window.show();
    return app.exec();
}
