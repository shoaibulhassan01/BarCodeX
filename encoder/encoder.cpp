#include "../common/common.h"
#include "encoder.h"

vector<string> encodeText(string text) {
    const string digitPatterns[] = {
        "nnnwwnwnn", "wnnwnnnnw", "nnwwnnnnw", "wnwwnnnnn", "nnnwwnnnw",
        "wnnwwnnnn", "nnwwwnnnn", "nnnwnnwnw", "wnnwnnwnn", "nnwwnnwnn"
    };
    const string startStopPattern = "nwnnwnwnn";

    if (text.empty()) {
        return {};
    }

    vector<string> patterns;
    patterns.push_back(startStopPattern);

    for (char character : text) {
        if (character < '0' || character > '9') {
            return {};
        }
        patterns.push_back(digitPatterns[character - '0']);
    }

    patterns.push_back(startStopPattern);
    return patterns;
}