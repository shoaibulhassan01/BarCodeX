#ifndef DECODER_H
#define DECODER_H

#include <string>

// Decode an N/W Code 39 pattern into text.
// Returns true on success and false with an error message on failure.
bool decodeRuns(std::string pattern, std::string &text, std::string &error);

#endif