#include "../common/common.h"
#include "decoder.h"

#include <algorithm>
#include <cctype>
#include <map>
#include <string>

using namespace std;

namespace
{
    const int ELEMENTS_PER_CHARACTER = 9;
    const int REQUIRED_DIGITS = 6;
    const int TOTAL_CHARACTERS = REQUIRED_DIGITS + 2;

    // 8 characters * 9 elements + 7 gaps = 79
    const int PATTERN_WITH_GAPS =
        TOTAL_CHARACTERS * ELEMENTS_PER_CHARACTER + (TOTAL_CHARACTERS - 1);

    // 8 characters * 9 elements = 72 (agar scanner gaps na bheje)
    const int PATTERN_WITHOUT_GAPS =
        TOTAL_CHARACTERS * ELEMENTS_PER_CHARACTER;

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

    // Sirf n aur w rakhta hai (spaces, commas, capital letters theek kar deta hai)
    string cleanPattern(const string& raw)
    {
        string cleaned;

        for (char c : raw)
        {
            char lower = static_cast<char>(
                tolower(static_cast<unsigned char>(c)));

            if (lower == 'n' || lower == 'w')
            {
                cleaned += lower;
            }
        }

        return cleaned;
    }

    // Saaf string ko digits mein badalta hai
    bool decodeCleanPattern(
        const string& pattern,
        string& decoded,
        string& error)
    {
        decoded.clear();

        int length = static_cast<int>(pattern.size());
        bool hasGaps = false;

        if (length == PATTERN_WITH_GAPS)
        {
            hasGaps = true;
        }
        else if (length == PATTERN_WITHOUT_GAPS)
        {
            hasGaps = false;
        }
        else
        {
            error = "Invalid barcode pattern length.";
            return false;
        }

        int step = hasGaps
            ? ELEMENTS_PER_CHARACTER + 1
            : ELEMENTS_PER_CHARACTER;

        for (int characterIndex = 0;
             characterIndex < TOTAL_CHARACTERS;
             characterIndex++)
        {
            int startIndex = characterIndex * step;

            string symbol =
                pattern.substr(startIndex, ELEMENTS_PER_CHARACTER);

            // Har Code 39 character mein theek 3 wide hote hain
            int wideCount = static_cast<int>(
                count(symbol.begin(), symbol.end(), 'w'));

            if (wideCount != 3)
            {
                error = "Invalid Code 39 character pattern.";
                return false;
            }

            if (characterIndex == 0)
            {
                if (symbol != START_STOP_PATTERN)
                {
                    error = "Invalid barcode start symbol.";
                    return false;
                }
            }
            else if (characterIndex == TOTAL_CHARACTERS - 1)
            {
                if (symbol != START_STOP_PATTERN)
                {
                    error = "Invalid barcode stop symbol.";
                    return false;
                }
            }
            else
            {
                auto found = CODE39_TABLE.find(symbol);

                if (found == CODE39_TABLE.end())
                {
                    error = "Unknown Code 39 pattern.";
                    return false;
                }

                decoded += found->second;
            }

            // Characters ke beech ka gap patla (n) hona chahiye
            if (hasGaps && characterIndex < TOTAL_CHARACTERS - 1)
            {
                if (pattern[startIndex + ELEMENTS_PER_CHARACTER] != 'n')
                {
                    error = "Invalid character spacing.";
                    return false;
                }
            }
        }

        return true;
    }
}

// Main Decoder function: N/W string se 6 digits nikalta hai
bool decodeRuns(string pattern, string &text, string &error)
{
    text.clear();
    error.clear();

    string cleaned = cleanPattern(pattern);

    string decoded;
    string firstError;

    // Pehli koshish: seedha parho
    if (decodeCleanPattern(cleaned, decoded, firstError))
    {
        text = decoded;
        return true;
    }

    // Doosri koshish: barcode ulta scan hua ho toh string ulta kar ke parho
    reverse(cleaned.begin(), cleaned.end());

    string secondError;

    if (decodeCleanPattern(cleaned, decoded, secondError))
    {
        text = decoded;
        return true;
    }

    error = firstError;
    return false;
}