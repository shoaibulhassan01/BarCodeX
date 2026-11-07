#include "../common/common.h"
#include "scanner.h"
#include <QImage>

Image loadImage(string filename) {
    Image img;
    QImage file;
    if (!file.load(QString::fromStdString(filename))) {
        return img;  // caller will report erorr
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

Image makeBlackWhite(Image img) {
    for (int i = 0; i < (int)img.pixels.size(); i++) {
        img.pixels[i] = (img.pixels[i] < 128) ? 0 : 255;
    }
    return img;
}

QImage toQImage(const Image& img) {
    QImage result(img.width, img.height, QImage::Format_RGB32);  // 32-bit RGB format with opaque alpha byte (0xFF)

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            int gray = img.pixels[y * img.width + x];
            result.setPixel(x, y, qRgb(gray, gray, gray));  // set pixels of the grayscale image
        }
    }
    return result;
}

int findLine(Image img) {
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            if (img.pixels[y * img.width + x] == 0) {  // black pixel found
                return y;
            }
        }
    }
    return -1;  // no black pixel found
}

vector<int> getRuns(Image img, int line) {
    vector<int> runs;

    if (line < 0 || line >= img.height)
        return runs;  // return empty vector

    int count = 0;
    bool isBlack = false;
    bool first = false;

    for (int x = 0; x < img.width; x++) {
        if (!first) {  // first pixel should be black
            if (img.pixels[line * img.width + x] == 0) {
                isBlack = true;
                count++;
                first = true;
                continue;
            }
            continue;
        }

        if (img.pixels[line * img.width + x] == 0) {
            if (isBlack) {
                count++;
            }
            else {
                isBlack = true;
                runs.push_back(count);  // push old run first
                count = 1;
            }
        }
        else {
            if (!isBlack) {
                count++;
            }
            else {
                isBlack = false;
                runs.push_back(count);
                count = 1;
            }
        }
    }

    if (isBlack)
        runs.push_back(count);  // push last run only if black

    return runs;
}

Image takePhoto() {
    //cout << "[scanner] camera photo" << endl;
    // TODO(Umar): camera se photo lo
    Image img;
    return img;
}