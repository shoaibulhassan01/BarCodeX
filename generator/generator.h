#ifndef GENERATOR_H
#define GENERATOR_H

// OWNER: Zain
// Patterns deta ho, barcode ki image wapas milti hai.
// Narrow bar = 2 pixel, wide bar = 6 pixel.
// Image ke dono taraf quiet zone (safaid jagah) hogi.
Image generateBarcode(vector<string> patterns);

// Image ko file mein save karta hai. Kamyab ho to true.
bool saveImage(Image img, string filename);

#endif GENERATOR_H