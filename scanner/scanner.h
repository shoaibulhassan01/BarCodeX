#ifndef SCANNER_H
#define SCANNER_H

#include "../common/common.h"

// OWNER: Umar
// File se image kholo
Image loadImage(string filename);

// Camera se photo lo
Image takePhoto();

// Image ko kala/safaid banao (128 se kam = 0, baki = 255)
Image makeBlackWhite(Image img);

// Barcode wali line (row number) dhoondo. Na mile to -1.
int findLine(Image img);

// Us line par kale/safaid runs ki chorai (pixels mein).
// Rule: pehla number hamesha BAR (kala) ka hoga.
// Example: { 2, 2, 6, 2, 2, 6, 6, 2, 2 }
vector<int> getRuns(Image img, int line);

#endif