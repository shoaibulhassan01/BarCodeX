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

Image takePhoto() {
    cout << "[scanner] camera photo" << endl;
    // TODO(Umar): camera se photo lo
    Image img;
    return img;
}

Image makeBlackWhite(Image img) {
    for (int &pixel : img.pixels) {
        pixel = pixel < 128 ? 0 : 255;
    }
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