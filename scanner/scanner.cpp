#include "../common/common.h"
#include "scanner.h"
#include <QImage>

Image makeGrayScale(const QImage& file) {
    Image img;
    img.width = file.width();
    img.height = file.height();

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            // push the grayscale value into the pixels array
            img.pixels.push_back(qGray(file.pixel(x, y)));
        }
    }
    return img;
}

Image makeBlackWhite(Image img) {
    // TODO: Improve this function (probably use Otsu's method of local thresholding)
    for (int i = 0; i < (int)img.pixels.size(); i++) {
        img.pixels[i] = (img.pixels[i] < 128) ? 0 : 255;
    }
    return img;
}

bool validImage(const Image& img) {
    return true;
}

// Helper for findRow()
int countTransitions(const Image& img, int row) {
    if (row < 0 || row >= img.height)
        return 0;

    int count = 0;
     for (int x = 1; x < img.width; x++) {
        if (img.pixels[row * img.width + (x - 1)] != img.pixels[row * img.width + x])
            count++;
     }
     return count;
}

// Helper for getRuns()
int findRow(const Image& img) {
    int count = 0;
    int startRow = 0;
    bool found = false;

     for (int y = 0; y < img.height; y++) {
        count = countTransitions(img, y);

        if (count == 80) {
            startRow = y;
            found = true;
            break;
        }
    } 

    return found ? startRow : -1;
}

vector<int> getRuns(const Image& img) {
    vector<int> runs;
    int row = findRow(img);

    if (row < 0 || row >= img.height)
        return runs;  // return empty vector
 
    int count = 0;
    bool isBlack = false;
    bool first = false;

    for (int x = 0; x < img.width; x++) {
        if (!first) {  // first pixel should be black
            if (img.pixels[row * img.width + x] == 0) {
                isBlack = true;
                count++;
                first = true;
                continue;
            }
            continue;
        }

        if (img.pixels[row * img.width + x] == 0) {
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

string runsToString(const vector<int>& runs) {
    string s;

    if (runs.empty()) {
        return s;
    }

    int narrowBar = runs[0];  // first run is a narrow black bar

    for (int i = 0; i < (int)runs.size(); i++) {
         if (runs[i] * 2 <= narrowBar * 3)   // up to 1.5 times the narrow bar counts as narrow 
            s.push_back('n');
        else
            s.push_back('w');
    }
    return s;
}

string imageToString(const QImage& file) {
    Image img = makeGrayScale(file);
    img = makeBlackWhite(img);

    if (!validImage(img)) {
        return "";
    }

    vector<int> runs = getRuns(img);
    string result = runsToString(runs);

    return result;
}

Image scanFromCamera() {
    // TODO(Umar): Scan barcode directly from the camera
    Image img;
    return img;
}