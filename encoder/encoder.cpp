#include "../common/common.h"
#include "encoder.h"
#include <iostream>

vector<string> encodeText(string text) {
    cout << "[encoder] hello, text = " << text << endl;

    // TODO(Leader): asli table se patterns banao
    vector<string> patterns;
    patterns.push_back("nwnnwnwnn");   // start *
    patterns.push_back("nwnnwnwnn");   // stop *
    return patterns;
}