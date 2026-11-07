#ifndef SCANNER_H
#define SCANNER_H

#include "../common/common.h"
#include <QImage>

// Load an image from a file and convert it to grayscale
Image loadImage(string filename);

// Convert a grayscale image to black and white using a threshold of 128
Image makeBlackWhite(Image img);

// Convert a grayscale image to a QImage for display in the GUI
QImage toQImage(const Image& img);

// Barcode wali line (row number) dhoondo. Na mile to -1.
int findLine(Image img);

// Us line par kale/safaid runs ki chorai (pixels mein).
// Rule: pehla number hamesha BAR (kala) ka hoga.
// Example: { 2, 2, 6, 2, 2, 6, 6, 2, 2 }
vector<int> getRuns(Image img, int line);

Image takePhoto();

#endif