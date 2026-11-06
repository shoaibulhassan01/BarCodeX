#include "../common/common.h"
#include "scanner.h"
#include <iostream>

Image loadImage(string filename) {
    cout << "[scanner] load: " << filename << endl;
    // TODO(Umar): file se image parho
    Image img;
    return img;
}

Image takePhoto() {
    cout << "[scanner] camera photo" << endl;
    // TODO(Umar): camera se photo lo
    Image img;
    return img;
}

Image makeBlackWhite(Image img) {
    cout << "[scanner] black white" << endl;
    // TODO(Umar): threshold lagao
    return img;
}

int findLine(Image img) {
    cout << "[scanner] find line" << endl;
    // TODO(Umar): sahi row dhoondo
    return 0;
}

vector<int> getRuns(Image img, int line) {
    cout << "[scanner] get runs, line = " << line << endl;
    // TODO(Umar): runs naapo
    vector<int> runs;
    return runs;
}