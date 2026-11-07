#ifndef SCANNER_H
#define SCANNER_H

#include "../common/common.h"
#include <QImage>

Image loadImage(string filename);

Image makeBlackWhite(Image img);

Image takePhoto();

// Barcode wali line (row number) dhoondo. Na mile to -1.
int findLine(Image img);

// Us line par kale/safaid runs ki chorai (pixels mein).
// Rule: pehla number hamesha BAR (kala) ka hoga.
// Example: { 2, 2, 6, 2, 2, 6, 6, 2, 2 }
vector<int> getRuns(Image img, int line);

QImage toQImage(const Image& img);

#endif // SCANNER_H