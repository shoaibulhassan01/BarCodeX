#ifndef SCANNER_H
#define SCANNER_H

#include "../common/common.h"
#include <QImage>

// Convert a loaded image to grayscale.
Image makeGrayScale(const QImage& file);

// Convert a grayscale image to black and white using a threshold of 128.
Image makeBlackWhite(Image img);

// Run widths along one row, from the first black pixel to the last black pixel.
// First run is a bar, last run is a bar. Empty vector if the row is bad or has no black.
std::vector<int> getRuns(const Image& img, int row);

// Convert the runs from getRuns() into a string ("nwnwwnw...") to be passed to the decoder.
std::string runsToString(const std::vector<int>& runs);

// Check if the uploaded image generates a valid Code-39 N/W string pattern.
bool validPattern(const std::string& pattern);  

// A single callable function for the decoder module that directly converts a QImage object
// to an N/W string to be decoded into a 6-digit number.
std::string imageToString(const QImage& file);

// To scan a barcode directly from a desktop/laptop camera.
Image scanFromCamera();

#endif