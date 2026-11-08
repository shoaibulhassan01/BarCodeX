#ifndef SCANNER_H
#define SCANNER_H

#include "../common/common.h"
#include <QImage>

// Convert a loaded image to grayscale.
Image makeGrayScale(const QImage& file);

// Convert a grayscale image to black and white using a threshold of 128.
Image makeBlackWhite(Image img);

// Count changes between adjacent pixels on a row.
int countTransitions(const Image& img, int line);

// Find the first row in the image that has exactly 80 transitions (first row of barcode).
int findRow(const Image& img);

// Calculate the widths of black bars and white spaces on the first row of barcode.
// Note: This function assumes a valid Code-39 barcode image.
vector<int> getRuns(const Image& img);

// Convert the runs from getRuns() into a string ("nwnwwnw...") to be passed to the decoder.
string runsToString(const vector<int>& runs);

// Check if the uploaded image generates a valid Code-39 N/W string pattern.
bool validPattern(const string& pattern);  

// A single callable function for the decoder modue (Anas) that directly converts a QImage object
// to an N/W string to be decoded into a 6-digit number.
string imageToString(const QImage& file);

// To scan a barcode directly from a desktop/laptop camera.
Image scanFromCamera();

#endif