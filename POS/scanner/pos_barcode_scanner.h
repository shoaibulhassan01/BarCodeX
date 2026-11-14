#pragma once

#include "../../BarCode/camera/camera_scanner.h"

#include <QString>

namespace Pos
{
// Use the camera scanner that was completed in Increment 1.
using PosBarcodeScanner = CameraBarcodeScanner;

// Owner: Zain Ul Abideen. Start scanning with the computer's default camera.
bool startCameraScan(PosBarcodeScanner &scanner, QString &error);

// Owner: Zain Ul Abideen. Stop scanning when the user leaves the scan page.
void stopCameraScan(PosBarcodeScanner &scanner);
}
