#include "../common/common.h"
#include "generator.h"
#include <iostream>
#include <fstream>
#include <cctype>

// Returns the pixel width of one element:
// 'n' = narrow, 'w' = wide, anything else = 0 (ignored).
// Upper case 'N' and 'W' are also accepted.
int getElementWidth(char c, int narrow, int wide)
{
    c = tolower((unsigned char)c);

    if (c == 'n')
    {
        return narrow;
    }
    if (c == 'w')
    {
        return wide;
    }
    return 0;
}

Image generateBarcode(vector<string> patterns)
{
    Image img;

    int narrow = 2;        // narrow bar or space = 2 pixels (as in generator.h)
    int wide = 6;          // wide bar or space = 6 pixels (3 times narrow)
    int quietZone = 20;    // white margin on left and right (10 narrow widths)
    int height = 100;

    // Step 1: calculate the total width of the image
    int totalWidth = quietZone * 2;

    for (int i = 0; i < patterns.size(); i++)
    {
        int count = 0;     // number of valid elements in this pattern

        for (int j = 0; j < patterns[i].length(); j++)
        {
            int w = getElementWidth(patterns[i][j], narrow, wide);

            if (w > 0)
            {
                totalWidth += w;
                count++;
            }
        }

        // A pattern with an odd number of elements ends with a bar.
        // Add a narrow white gap so it does not touch the next bar.
        if (count % 2 == 1)
        {
            totalWidth += narrow;
        }
    }

    // Nothing valid to draw: return an empty image
    if (totalWidth == quietZone * 2)
    {
        cout << "[generator] error: no valid pattern data" << endl;
        return img;
    }

    // Step 2: create a white image (255 = white)
    img.width = totalWidth;
    img.height = height;
    img.pixels.resize(img.width * img.height, 255);

    // Step 3: draw the bars (0 = black)
    int x = quietZone;

    for (int i = 0; i < patterns.size(); i++)
    {
        int count = 0;     // position inside the pattern (0 = first bar)

        for (int j = 0; j < patterns[i].length(); j++)
        {
            int barWidth = getElementWidth(patterns[i][j], narrow, wide);

            if (barWidth == 0)
            {
                continue;  // skip invalid characters
            }

            // even positions are black bars, odd positions are white spaces
            if (count % 2 == 0)
            {
                for (int y = 0; y < height; y++)
                {
                    for (int k = 0; k < barWidth; k++)
                    {
                        img.pixels[y * img.width + x + k] = 0;
                    }
                }
            }

            x += barWidth;
            count++;
        }

        // Same gap as in Step 1 (the two steps must match)
        if (count % 2 == 1)
        {
            x += narrow;
        }
    }

    return img;
}


bool saveImage(Image img, string filename)
{
    // Check that the image is valid
    if (img.width <= 0 || img.height <= 0)
    {
        return false;
    }

    if (img.pixels.size() != img.width * img.height)
    {
        return false;
    }

    ofstream file(filename, ios::binary);

    if (!file)
    {
        return false;
    }

    // PGM header: format (P5 = grayscale), size, max pixel value
    file << "P5\n";
    file << img.width << " " << img.height << "\n";
    file << "255\n";

    // Pixel data: one byte per pixel
    for (int i = 0; i < img.pixels.size(); i++)
    {
        unsigned char pixel = (unsigned char)img.pixels[i];
        file.write((char*)&pixel, 1);
    }

    file.close();

    return true;
}