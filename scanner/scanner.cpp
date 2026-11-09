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
    for (int i = 0; i < (int)img.pixels.size(); i++) {
        img.pixels[i] = (img.pixels[i] < 128) ? 0 : 255;
    }
    return img;
}

vector<int> getRuns(const Image& img, int row) {
    vector<int> runs;

    if (row < 0 || row >= img.height)
        return runs;

    int start = -1;
    int end = -1;
    for (int x = 0; x < img.width; x++) {
        if (img.pixels[row * img.width + x] == 0) {
            if (start == -1)
                start = x;
            end = x;
        }
    }
    if (start == -1)
        return runs;  // no black pixel on this row
    
    int count = 1;
    for (int x = start + 1; x <= end; x++) {
        if (img.pixels[row * img.width + x] == img.pixels[row * img.width + x - 1])
            count++;
        else {
            runs.push_back(count);
            count = 1;
        }
    }
    runs.push_back(count);

    return runs;
}

string runsToString(const vector<int>& runs) {
    string s;

    if (runs.empty())
        return s;

   // smallest and largest run, separately for bars (even index) and spaces (odd index).
    int minBar = runs[0];
    int maxBar = runs[0];
    int minSpace = -1;
    int maxSpace = -1;

    for (int i = 0; i < (int)runs.size(); i++) {
        if (i % 2 == 0) {
            if (runs[i] < minBar)
                minBar = runs[i];
            if (runs[i] > maxBar)
                maxBar = runs[i];
        }
        else {
            if (minSpace == -1 || runs[i] < minSpace)
                minSpace = runs[i];
            if (runs[i] > maxSpace)
                maxSpace = runs[i];
        }
    }

    // The threshold for a narrow bar/space is the average of the smallest and largest run of that type.
    int barCut = minBar + maxBar;
    int spaceCut = minSpace + maxSpace;

    for (int i = 0; i < (int)runs.size(); i++) {
        int cut = (i % 2 == 0) ? barCut : spaceCut;

        if (runs[i] * 2 <= cut)  // narrow <= (min + max) / 2
            s.push_back('n');
        else 
            s.push_back('w');
    }
    return s;
}

bool validPattern(const string& pattern) {
    if (pattern.size() != 79)  // 79 alternating bars and spaces in Code-39
        return false;

    int size = pattern.size();
    int count = 0;
    int wides = 0;

    for (int i = 0; i < size; i++) {
        count++;
        if (count == 10) {  // check for 3-wides for each group of 9
            if (wides != 3 || pattern[i] != 'n')
                return false;
            else {
                count = 0;
                wides = 0;
                continue;
            }
        }
        if (pattern[i] == 'w')
            wides++;
    }

    // check for start and stop combintions ("nwnnwnwnn" for '*')
    string asterisk = "nwnnwnwnn";
    if (pattern.substr(0, 9) != asterisk || pattern.substr(size - 9) !=  asterisk)
        return false;

    return true;
}

// Try one row. Returns a valid 79-letter N/W string, or "" if this row fails.
static string tryRow(const Image& img, int row) {
    vector<int> runs = getRuns(img, row);
    if (runs.size() != 79)  // cheap early exit, real validation by validPattern()
        return "";

    string s = runsToString(runs);
    if (validPattern(s))
        return s;

    return "";
}

string imageToString(const QImage& qimg) {
    Image img = makeBlackWhite(makeGrayScale(qimg));

    if (img.width == 0 || img.height == 0) {
        return "";
    }

    int middle = img.height / 2;
    int limit = img.height / 5;

    for (int offset = 0; offset <= limit; offset++) {
        string s = tryRow(img, middle + offset);
        if (s != "")
            return s;

        if (offset > 0) {
            s = tryRow(img, middle - offset);
            if (s != "")
                return s;
        }
    }

    return "";
}

Image scanFromCamera() {
    // TODO: Scan barcode directly from the camera
    Image img;
    return img;
}