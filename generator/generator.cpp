#include "../common/common.h"
#include "generator.h"
#include <iostream>

Image generateBarcode(vector<string> patterns) {
    cout << "[generator] hello, patterns = " << patterns.size() << endl;

    // TODO(Zain): bars aur spaces bana kar image banao
    Image img;
    return img;
}

bool saveImage(Image img, string filename) {
    cout << "[generator] save: " << filename << endl;

    // TODO(Zain): image file mein likho
    return true;
}