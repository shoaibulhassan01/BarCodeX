#include "pos_barcode_scanner.h"

#include <QMediaDevices>

namespace Pos
{
// Owner: Zain Ul Abideen. Reuse the existing camera scanner.
bool startCameraScan(PosBarcodeScanner &scanner, QString &error)
{
    QCameraDevice camera = QMediaDevices::defaultVideoInput();
    if (camera.isNull())
    {
        error = QStringLiteral("No default camera is available.");
        return false;
    }
    return scanner.start(camera, error);
}

// Owner: Zain Ul Abideen. Forward the stop request to the existing scanner.
void stopCameraScan(PosBarcodeScanner &scanner)
{
    scanner.stop();
}
}
