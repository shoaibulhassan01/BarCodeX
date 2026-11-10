#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <vector>
using namespace std;

class Image {
public:
    int width;
    int height;
    vector<int> pixels;

    Image() {
        width = 0;
        height = 0;
    }
};

#endif 