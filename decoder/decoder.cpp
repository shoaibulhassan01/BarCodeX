
#include "../common/common.h"
#include "decoder.h"

#include <iostream>
#include <map>
#include <algorithm>
#include <cmath>
#include <limits>

using namespace std;

namespace
{
    // Code 39 constants
    const int ELEMENTS_PER_CHARACTER = 9;
    const int REQUIRED_DIGITS = 6;
    const int TOTAL_CHARACTERS = REQUIRED_DIGITS + 2;

    // Code 39 numeric lookup table
    const map<string, char> CODE39_TABLE =
    {
        {"nnnwwnwnn", '0'},
        {"wnnwnnnnw", '1'},
        {"nnwwnnnnw", '2'},
        {"wnwwnnnnn", '3'},
        {"nnnwwnnnw", '4'},
        {"wnnwwnnnn", '5'},
        {"nnwwwnnnn", '6'},
        {"nnnwnnwnw", '7'},
        {"wnnwnnwnn", '8'},
        {"nnwwnnwnn", '9'}
    };

    const string START_STOP_PATTERN = "nwnnwnwnn";

    // Automatically identify narrow and wide widths
    bool findWidthGroups(
        const vector<int>& runs,
        double& narrowWidth,
        double& wideWidth,
        string& error)
    {
        int minimum = numeric_limits<int>::max();
        int maximum = 0;

        for (int width : runs)
        {
            if (width <= 0)
            {
                error = "Invalid run width.";
                return false;
            }

            minimum = min(minimum, width);
            maximum = max(maximum, width);
        }

        if (minimum == maximum)
        {
            error = "Cannot distinguish narrow and wide widths.";
            return false;
        }

        double center1 = minimum;
        double center2 = maximum;

        // Two-group clustering
        for (int iteration = 0; iteration < 50; iteration++)
        {
            double sum1 = 0;
            double sum2 = 0;

            int count1 = 0;
            int count2 = 0;

            for (int width : runs)
            {
                if (abs(width - center1) <=
                    abs(width - center2))
                {
                    sum1 += width;
                    count1++;
                }
                else
                {
                    sum2 += width;
                    count2++;
                }
            }

            if (count1 == 0 || count2 == 0)
            {
                error = "Unable to classify run widths.";
                return false;
            }

            double newCenter1 = sum1 / count1;
            double newCenter2 = sum2 / count2;

            if (abs(newCenter1 - center1) < 0.001 &&
                abs(newCenter2 - center2) < 0.001)
            {
                center1 = newCenter1;
                center2 = newCenter2;
                break;
            }

            center1 = newCenter1;
            center2 = newCenter2;
        }

        narrowWidth = min(center1, center2);
        wideWidth = max(center1, center2);

        double ratio = wideWidth / narrowWidth;

        if (ratio < 1.7 || ratio > 3.5)
        {
            error = "Invalid narrow/wide width ratio.";
            return false;
        }

        return true;
    }

    // Convert nine run widths into an N/W pattern
    bool createPattern(
        const vector<int>& runs,
        int startIndex,
        double narrowWidth,
        double wideWidth,
        string& pattern,
        string& error)
    {
        pattern.clear();

        int wideCount = 0;

        for (int i = 0; i < ELEMENTS_PER_CHARACTER; i++)
        {
            int width = runs[startIndex + i];

            if (abs(width - narrowWidth) <
                abs(width - wideWidth))
            {
                pattern += 'n';
            }
            else
            {
                pattern += 'w';
                wideCount++;
            }
        }

        // Every Code 39 character has three wide elements
        if (wideCount != 3)
        {
            error = "Invalid Code 39 character pattern.";
            return false;
        }

        return true;
    }
}

// Main Decoder function
bool decodeRuns(vector<int> runs, string &text, string &error)
{
    cout << "[decoder] hello, runs = "
         << runs.size() << endl;

    text.clear();
    error.clear();

    // Start + 6 digits + Stop
    // 8 characters * 9 elements + 7 gaps = 79 runs
    const int expectedRuns =
        TOTAL_CHARACTERS * ELEMENTS_PER_CHARACTER
        + (TOTAL_CHARACTERS - 1);

    if (static_cast<int>(runs.size()) != expectedRuns)
    {
        error = "Invalid barcode run count.";
        return false;
    }

    // Find narrow and wide widths
    double narrowWidth = 0;
    double wideWidth = 0;

    if (!findWidthGroups(
            runs,
            narrowWidth,
            wideWidth,
            error))
    {
        return false;
    }

    string decoded;

    // Decode all eight Code 39 characters
    for (int characterIndex = 0;
         characterIndex < TOTAL_CHARACTERS;
         characterIndex++)
    {
        int startIndex =
            characterIndex * (ELEMENTS_PER_CHARACTER + 1);

        string pattern;

        if (!createPattern(
                runs,
                startIndex,
                narrowWidth,
                wideWidth,
                pattern,
                error))
        {
            return false;
        }

        // Verify start symbol
        if (characterIndex == 0)
        {
            if (pattern != START_STOP_PATTERN)
            {
                error = "Invalid barcode start symbol.";
                return false;
            }
        }

        // Verify stop symbol
        else if (characterIndex == TOTAL_CHARACTERS - 1)
        {
            if (pattern != START_STOP_PATTERN)
            {
                error = "Invalid barcode stop symbol.";
                return false;
            }
        }

        // Decode numeric characters
        else
        {
            auto found = CODE39_TABLE.find(pattern);

            if (found == CODE39_TABLE.end())
            {
                error = "Unknown Code 39 pattern.";
                return false;
            }

            decoded += found->second;
        }

        // Check inter-character gap
        if (characterIndex < TOTAL_CHARACTERS - 1)
        {
            int gapIndex =
                startIndex + ELEMENTS_PER_CHARACTER;

            if (runs[gapIndex] >
                (narrowWidth + wideWidth) / 2.0)
            {
                error = "Invalid character spacing.";
                return false;
            }
        }
    }

    // Validate final decoded result
    if (decoded.size() != REQUIRED_DIGITS)
    {
        error = "Decoded result must contain exactly six digits.";
        return false;
    }

    for (char digit : decoded)
    {
        if (digit < '0' || digit > '9')
        {
            error = "Decoded result contains invalid characters.";
            return false;
        }
    }

    text = decoded;
    error.clear();

    return true;
}
