#include "scanner/scanner.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QImage>
#include <QLabel>
#include <QPixmap>
#include <QVBoxLayout>
#include <QWidget>
#include <iostream>

QImage toQImage(const Image& img) {
    QImage result(img.width, img.height, QImage::Format_RGB32);
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            int gray = img.pixels[y * img.width + x];
            result.setPixel(x, y, qRgb(gray, gray, gray));
        }
    }
    return result;
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    const string imagePath = "tests/cat.jpeg";
    QImage original(QString::fromStdString(imagePath));
    if (original.isNull()) {
        std::cerr << "Could not load " << imagePath << std::endl;
        return 1;
    }

    Image gray = loadImage(imagePath);
    if (gray.width == 0 || gray.height == 0) {
        std::cerr << "Scanner could not read " << imagePath << std::endl;
        return 1;
    }
    Image bw = makeBlackWhite(gray);

    QWidget window;
    window.setWindowTitle("Color, Grayscale, and Black-and-White Comparison");

    auto* comparison = new QHBoxLayout;
    const QImage images[] = {original, toQImage(gray), toQImage(bw)};
    const QString titles[] = {"Original (color)", "Grayscale", "Black and white"};

    for (int i = 0; i < 3; ++i) {
        auto* panel = new QVBoxLayout;
        auto* title = new QLabel(titles[i]);
        auto* preview = new QLabel;
        preview->setAlignment(Qt::AlignCenter);
        preview->setPixmap(QPixmap::fromImage(images[i]).scaled(
            360, 280, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        preview->setFixedSize(370, 290);
        panel->addWidget(title);
        panel->addWidget(preview);
        comparison->addLayout(panel);
    }

    window.setLayout(comparison);
    window.resize(1170, 350);
    window.show();

    return app.exec();
}
