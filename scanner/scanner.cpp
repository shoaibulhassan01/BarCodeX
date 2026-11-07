#include "../common/common.h"
#include "scanner.h"
#include <QImage>

#include <iostream>

// Load an image from a file and convert it to grayscale
Image loadImage(string filename) {
    Image img;
    QImage file;
    if (!file.load(QString::fromStdString(filename))) {
        return img;
    }

    img.width = file.width();
    img.height = file.height();
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            // push the grayscale value into the pixels array.
            img.pixels.push_back(qGray(file.pixel(x, y)));
        }
    }
    return img;
}

// Convert a grayscale image to black and white using a threshold of 128
Image makeBlackWhite(Image img) {
    for (int i = 0; i < img.pixels.size(); i++) {
        img.pixels[i] = (img.pixels[i] < 128) ? 0 : 255;
    }
    return img;
}

Image takePhoto() {
    cout << "[scanner] camera photo" << endl;
    // TODO(Umar): camera se photo lo
    Image img;
    return img;
}

int findLine(Image img) {
    cout << "[scanner] find line" << endl;
    // TODO(Umar): sahi row dhoondo
    return 0;
}

vector<int> getRuns(Image img, int line) {
    cout << "[scanner] get runs, line = " << line << endl;
    // TODO(Umar): runs naapo
    vector<int> runs;
    return runs;
}

// Convert a grayscale image to a QImage for display in the GUI
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