#include "camera_scanner.h"

#include "../decoder/decoder.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <utility>
#include <vector>

#include <QCamera>
#include <QCameraFormat>
#include <QMediaCaptureSession>
#include <QMediaDevices>
#include <QMetaObject>
#include <QPointer>
#include <QThreadPool>
#include <QTimer>
#include <QVideoFrame>
#include <QVideoSink>
#include <QTransform>

namespace
{
    constexpr int kThresholdCount = 5;
    constexpr int kPreviewIntervalMs = 50;
    // Largest side used for decoding. The camera is asked for ~720p, so live
    // frames are scanned at full resolution: shrinking them made small
    // barcodes (and the short side of portrait frames) too thin to read.
    constexpr int kLiveMaxDim = 1280;
    constexpr int kStillMaxDim = 1600;
    using Thresholds = std::array<int, kThresholdCount>;

    // ---------------------------------------------------------------
    // 1-D decoding
    // ---------------------------------------------------------------
    // Cheap pre-check: do the 9 runs at `at` look like the '*' start/stop
    // symbol (forward or reversed)? '*' has both narrow and wide bars and
    // spaces, so it can be classified on its own. This rejects almost every
    // window before the full 79-run classification, which keeps full
    // resolution scanning fast.
    bool looksLikeStartStop(const std::vector<int>& runs, size_t at)
    {
        int minBar = std::numeric_limits<int>::max();
        int maxBar = 0;
        int minSpace = std::numeric_limits<int>::max();
        int maxSpace = 0;
        for (size_t i = 0; i < 9; ++i)
        {
            const int run = runs[at + i];
            if (i % 2 == 0)
            {
                minBar = std::min(minBar, run);
                maxBar = std::max(maxBar, run);
            }
            else
            {
                minSpace = std::min(minSpace, run);
                maxSpace = std::max(maxSpace, run);
            }
        }
        char symbol[9];
        for (size_t i = 0; i < 9; ++i)
        {
            const int cutoff = (i % 2 == 0) ? minBar + maxBar : minSpace + maxSpace;
            symbol[i] = runs[at + i] * 2 <= cutoff ? 'n' : 'w';
        }
        return std::equal(symbol, symbol + 9, "nwnnwnwnn") ||
            std::equal(symbol, symbol + 9, "nnwnwnnwn");
    }

    bool decodeRunsOnLine(int width, const std::vector<bool>& dark, QString& digits)
    {
        int first = 0;
        while (first < width && !dark[static_cast<size_t>(first)])
            ++first;
        if (first == width)
            return false;

        int last = width - 1;
        while (last > first && !dark[static_cast<size_t>(last)])
            --last;

        std::vector<int> runs;
        runs.reserve(static_cast<size_t>(std::min(width, 256)));
        bool current = dark[static_cast<size_t>(first)];
        int runWidth = 0;
        for (int x = first; x <= last; ++x)
        {
            const bool pixelIsDark = dark[static_cast<size_t>(x)];
            if (pixelIsDark == current)
            {
                ++runWidth;
            }
            else
            {
                runs.push_back(runWidth);
                if (runs.size() > 512)
                    return false;
                runWidth = 1;
                current = pixelIsDark;
            }
        }
        runs.push_back(runWidth);

        if (runs.size() < 79)
            return false;

        constexpr size_t elementsPerBarcode = 79;
        // '*' start/stop symbol, and the same symbol read backwards. A line read
        // right-to-left (barcode upside down, rotated the "other" way by 90
        // degrees, or a mirrored webcam image) shows the reversed form at both
        // ends; decodeRuns() then reverses the whole pattern itself.
        constexpr char startStop[] = "nwnnwnwnn";
        constexpr char startStopReversed[] = "nnwnwnnwn";
        std::string pattern;
        pattern.reserve(elementsPerBarcode);

        for (size_t start = 0; start + elementsPerBarcode <= runs.size(); start += 2)
        {
            if (!looksLikeStartStop(runs, start) ||
                !looksLikeStartStop(runs, start + elementsPerBarcode - 9))
                continue;

            int minBar = runs[start];
            int maxBar = runs[start];
            int minSpace = width;
            int maxSpace = 0;
            for (size_t i = 0; i < elementsPerBarcode; ++i)
            {
                const int run = runs[start + i];
                if (i % 2 == 0)
                {
                    minBar = std::min(minBar, run);
                    maxBar = std::max(maxBar, run);
                }
                else
                {
                    minSpace = std::min(minSpace, run);
                    maxSpace = std::max(maxSpace, run);
                }
            }

            pattern.clear();
            for (size_t i = 0; i < elementsPerBarcode; ++i)
            {
                const int cutoff = (i % 2 == 0)
                    ? minBar + maxBar
                    : minSpace + maxSpace;
                pattern.push_back(runs[start + i] * 2 <= cutoff ? 'n' : 'w');
            }

            const bool forward = pattern.compare(0, 9, startStop) == 0 &&
                pattern.compare(70, 9, startStop) == 0;
            const bool backward = pattern.compare(0, 9, startStopReversed) == 0 &&
                pattern.compare(70, 9, startStopReversed) == 0;
            if (!forward && !backward)
                continue;

            std::string decoded;
            std::string error;
            if (decodeRuns(pattern, decoded, error))
            {
                digits = QString::fromStdString(decoded);
                return true;
            }
        }

        return false;
    }

    std::vector<uchar> smoothLine(const uchar* pixels, int width)
    {
        std::vector<uchar> smoothed(static_cast<size_t>(width));
        for (int x = 0; x < width; ++x)
        {
            const int left = pixels[std::max(0, x - 1)];
            const int center = pixels[x] * 2;
            const int right = pixels[std::min(width - 1, x + 1)];
            smoothed[static_cast<size_t>(x)] =
                static_cast<uchar>((left + center + right + 2) / 4);
        }
        return smoothed;
    }

    bool tryThreshold(const std::vector<uchar>& pixels, int threshold, QString& digits)
    {
        const int width = static_cast<int>(pixels.size());
        std::vector<bool> dark(static_cast<size_t>(width));
        for (int x = 0; x < width; ++x)
            dark[static_cast<size_t>(x)] = pixels[static_cast<size_t>(x)] < threshold;
        return decodeRunsOnLine(width, dark, digits);
    }

    bool tryAdaptiveThreshold(const std::vector<uchar>& pixels, QString& digits)
    {
        const int width = static_cast<int>(pixels.size());
        std::vector<int> prefix(static_cast<size_t>(width + 1), 0);
        for (int x = 0; x < width; ++x)
            prefix[static_cast<size_t>(x + 1)] =
                prefix[static_cast<size_t>(x)] + pixels[static_cast<size_t>(x)];

        const int radius = std::max(4, width / 48);
        std::vector<bool> dark(static_cast<size_t>(width));
        for (int x = 0; x < width; ++x)
        {
            const int left = std::max(0, x - radius);
            const int right = std::min(width, x + radius + 1);
            const int localMean = (prefix[static_cast<size_t>(right)] -
                prefix[static_cast<size_t>(left)]) / (right - left);
            const int threshold = std::clamp(localMean - 12, 72, 200);
            dark[static_cast<size_t>(x)] =
                pixels[static_cast<size_t>(x)] < threshold;
        }
        return decodeRunsOnLine(width, dark, digits);
    }

    bool scanPixels(const std::vector<uchar>& pixels,
        const Thresholds& thresholds, QString& digits)
    {
        if (pixels.size() < 80)
            return false;

        // Quick reject: a line without real contrast can't hold a barcode.
        // This skips most of the useless lines in a camera frame.
        const auto mm = std::minmax_element(pixels.begin(), pixels.end());
        if (*mm.second - *mm.first < 40)
            return false;

        if (tryAdaptiveThreshold(pixels, digits))
            return true;
        for (const int threshold : thresholds)
        {
            if (tryThreshold(pixels, threshold, digits))
                return true;
        }
        return false;
    }

    int otsuThreshold(const QImage& image)
    {
        std::array<quint64, 256> histogram = {};
        const int sampleStep = std::max(1, std::max(image.width(), image.height()) / 256);
        quint64 total = 0;
        quint64 sum = 0;

        for (int y = 0; y < image.height(); y += sampleStep)
        {
            const uchar* row = image.constScanLine(y);
            for (int x = 0; x < image.width(); x += sampleStep)
            {
                ++histogram[row[x]];
                ++total;
                sum += static_cast<quint64>(row[x]);
            }
        }

        quint64 backgroundWeight = 0;
        quint64 backgroundSum = 0;
        double bestVariance = -1.0;
        int bestThreshold = 128;
        for (int threshold = 0; threshold < 256; ++threshold)
        {
            backgroundWeight += histogram[static_cast<size_t>(threshold)];
            backgroundSum += static_cast<quint64>(threshold) *
                histogram[static_cast<size_t>(threshold)];
            if (backgroundWeight == 0 || backgroundWeight == total)
                continue;

            const quint64 foregroundWeight = total - backgroundWeight;
            const double backgroundMean =
                static_cast<double>(backgroundSum) / backgroundWeight;
            const double foregroundMean =
                static_cast<double>(sum - backgroundSum) / foregroundWeight;
            const double difference = backgroundMean - foregroundMean;
            const double variance = static_cast<double>(backgroundWeight) *
                foregroundWeight * difference * difference;
            if (variance > bestVariance)
            {
                bestVariance = variance;
                bestThreshold = threshold;
            }
        }
        return bestThreshold;
    }

    Thresholds thresholdsFor(const QImage& image)
    {
        const int threshold = otsuThreshold(image);
        return {
            threshold,
            std::clamp(threshold - 24, 48, 224),
            std::clamp(threshold + 24, 48, 224),
            std::clamp(threshold - 48, 40, 224),
            128
        };
    }

    // ---------------------------------------------------------------
    // Scanning strategies
    // ---------------------------------------------------------------

    // Scan horizontal rows starting from the CENTER of the image and moving
    // outwards, because the user almost always aims the barcode at the middle.
    bool scanOrientation(const QImage& image,
        const Thresholds& thresholds, int rowDivisor, QString& digits)
    {
        const int height = image.height();
        const int rowStep = std::max(1, height / rowDivisor);
        const int mid = height / 2;

        for (int k = 0; k * rowStep <= mid; ++k)
        {
            const int up = mid - k * rowStep;
            const int down = mid + k * rowStep;

            if (up >= 0)
            {
                const auto pixels = smoothLine(image.constScanLine(up), image.width());
                if (scanPixels(pixels, thresholds, digits))
                    return true;
            }
            if (k > 0 && down < height)
            {
                const auto pixels = smoothLine(image.constScanLine(down), image.width());
                if (scanPixels(pixels, thresholds, digits))
                    return true;
            }
        }
        return false;
    }

    bool sampleLine(const QImage& image, double angleDegrees, double offset,
        const Thresholds& thresholds, QString& digits)
    {
        const double radians = angleDegrees * 3.14159265358979323846 / 180.0;
        const double dx = std::cos(radians);
        const double dy = std::sin(radians);
        const double nx = -dy;
        const double ny = dx;
        const double halfWidth = (image.width() - 1) / 2.0;
        const double halfHeight = (image.height() - 1) / 2.0;

        double halfLength = std::numeric_limits<double>::max();
        if (std::abs(dx) > 1e-6)
            halfLength = std::min(halfLength, halfWidth / std::abs(dx));
        if (std::abs(dy) > 1e-6)
            halfLength = std::min(halfLength, halfHeight / std::abs(dy));
        halfLength *= 0.94;
        if (halfLength < 40.0)
            return false;

        const double centerX = halfWidth + 0.5;
        const double centerY = halfHeight + 0.5;
        const int sampleCount = static_cast<int>(halfLength * 2.0) + 1;
        std::vector<uchar> line(static_cast<size_t>(sampleCount));
        for (int x = 0; x < sampleCount; ++x)
        {
            const double along = x - halfLength;
            const double sourceX = centerX + along * dx + offset * nx;
            const double sourceY = centerY + along * dy + offset * ny;
            const int x0 = std::clamp(static_cast<int>(sourceX), 0, image.width() - 1);
            const int y0 = std::clamp(static_cast<int>(sourceY), 0, image.height() - 1);
            const int x1 = std::min(x0 + 1, image.width() - 1);
            const int y1 = std::min(y0 + 1, image.height() - 1);
            const double fx = sourceX - x0;
            const double fy = sourceY - y0;
            const double top = image.constScanLine(y0)[x0] * (1.0 - fx) +
                image.constScanLine(y0)[x1] * fx;
            const double bottom = image.constScanLine(y1)[x0] * (1.0 - fx) +
                image.constScanLine(y1)[x1] * fx;
            line[static_cast<size_t>(x)] =
                static_cast<uchar>(std::clamp(
                    static_cast<int>(top * (1.0 - fy) + bottom * fy + 0.5),
                    0, 255));
        }

        const std::vector<uchar> smoothed = smoothLine(line.data(), sampleCount);
        return scanPixels(smoothed, thresholds, digits);
    }

    // Tilted barcodes at any angle. Lines are tried from the center outwards.
    // Only 0..180 degrees is needed: a line at angle+180 is the same line read
    // backwards, and reversed reads are decoded too. Angles closest to
    // horizontal/vertical are tried first because they are the most common.
    std::vector<double> slantAngles(int angleStep)
    {
        std::vector<double> angles;
        for (int delta = angleStep; delta <= 45; delta += angleStep)
        {
            angles.push_back(delta);            // just below horizontal
            angles.push_back(180 - delta);      // just above horizontal
            if (delta < 45)
            {
                angles.push_back(90 - delta);   // either side of vertical
                angles.push_back(90 + delta);
            }
        }
        return angles;
    }

    bool scanSlantedAngle(const QImage& image, double angle,
        const Thresholds& thresholds, double lineSpacing, QString& digits)
    {
        const double halfWidth = (image.width() - 1) / 2.0;
        const double halfHeight = (image.height() - 1) / 2.0;
        const double radians = angle * 3.14159265358979323846 / 180.0;
        const double dx = std::cos(radians);
        const double dy = std::sin(radians);
        double halfLength = std::numeric_limits<double>::max();
        if (std::abs(dx) > 1e-6)
            halfLength = std::min(halfLength, halfWidth / std::abs(dx));
        if (std::abs(dy) > 1e-6)
            halfLength = std::min(halfLength, halfHeight / std::abs(dy));
        halfLength *= 0.94;

        const double remainingX = halfWidth - halfLength * std::abs(dx);
        const double remainingY = halfHeight - halfLength * std::abs(dy);
        const double nx = std::abs(dy);
        const double ny = std::abs(dx);
        double halfOffset = std::numeric_limits<double>::max();
        if (nx > 1e-6)
            halfOffset = std::min(halfOffset, remainingX / nx);
        if (ny > 1e-6)
            halfOffset = std::min(halfOffset, remainingY / ny);
        halfOffset = std::max(0.0, halfOffset - 2.0);

        const int steps = std::clamp(
            static_cast<int>(halfOffset / lineSpacing), 0, 60);
        for (int k = 0; k <= steps; ++k)
        {
            const double offset = k * lineSpacing;
            if (sampleLine(image, angle, offset, thresholds, digits))
                return true;
            if (k > 0 && sampleLine(image, angle, -offset, thresholds, digits))
                return true;
        }
        return false;
    }

    bool scanSlanted(const QImage& image, const Thresholds& thresholds,
        int angleStep, double lineSpacing, QString& digits)
    {
        for (const double angle : slantAngles(angleStep))
        {
            if (scanSlantedAngle(image, angle, thresholds, lineSpacing, digits))
                return true;
        }
        return false;
    }

    QImage prepareImage(const QImage& source, int maxDim)
    {
        QImage image = source.format() == QImage::Format_Grayscale8
            ? source
            : source.convertToFormat(QImage::Format_Grayscale8);
        // Smooth (area-averaging) scaling: nearest-neighbour drops whole
        // pixel columns and makes thin bars vanish, which hurt portrait
        // frames most because their short side was shrunk the most.
        if (image.width() > maxDim || image.height() > maxDim)
            image = image.scaled(maxDim, maxDim, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        return image;
    }

    // Fast pass (used for every live camera frame):
    //   horizontal rows (center first) -> vertical rows (center first).
    // Each line is decoded left-to-right and right-to-left, so this covers
    // 0, 90, 180 and 270 degrees and mirrored webcam images.
    bool decodeFast(const QImage& source, QString& digits)
    {
        if (source.isNull())
            return false;

        const QImage image = prepareImage(source, kLiveMaxDim);
        const Thresholds thresholds = thresholdsFor(image);
        if (scanOrientation(image, thresholds, 90, digits))
            return true;

        const QImage rotated = image.transformed(QTransform().rotate(90), Qt::FastTransformation);
        return scanOrientation(rotated, thresholdsFor(rotated), 90, digits);
    }

    // Thorough pass: everything the fast pass does + barcodes tilted at any angle.
    bool decodeThorough(const QImage& source, QString& digits, bool coarse)
    {
        if (source.isNull())
            return false;

        const QImage image = prepareImage(source, coarse ? kLiveMaxDim : kStillMaxDim);
        const Thresholds thresholds = thresholdsFor(image);
        if (scanOrientation(image, thresholds, coarse ? 90 : 180, digits))
            return true;

        const QImage rotated = image.transformed(QTransform().rotate(90), Qt::FastTransformation);
        if (scanOrientation(rotated, thresholdsFor(rotated), coarse ? 90 : 180, digits))
            return true;

        const int angleStep = coarse ? 5 : 3;
        const double spacing = coarse ? 10.0 : 4.0;
        return scanSlanted(image, thresholds, angleStep, spacing, digits);
    }

    // ---------------------------------------------------------------
    // Camera format: 720p-ish at 30 fps is the sweet spot. Higher
    // resolutions only make frame conversion slower.
    // ---------------------------------------------------------------
    QCameraFormat pickFormat(const QCameraDevice& device)
    {
        QCameraFormat best;
        long long bestScore = std::numeric_limits<long long>::max();
        for (const QCameraFormat& format : device.videoFormats())
        {
            if (format.maxFrameRate() < 24.0f)
                continue;
            const long long pixels =
                static_cast<long long>(format.resolution().width()) * format.resolution().height();
            if (pixels < 640LL * 480LL)
                continue;
            long long score = std::llabs(pixels - 1280LL * 720LL);
            // Prefer higher fps on equal resolution.
            score = score * 100 - static_cast<long long>(format.maxFrameRate());
            if (score < bestScore)
            {
                bestScore = score;
                best = format;
            }
        }
        return best;
    }
}

CameraBarcodeScanner::CameraBarcodeScanner(QObject* parent)
    : QObject(parent),
      camera(new QCamera(this)),
      captureSession(new QMediaCaptureSession(this)),
      frameTimeoutTimer(new QTimer(this)),
      videoSink(new QVideoSink(this))
{
    captureSession->setCamera(camera);
    captureSession->setVideoSink(videoSink);

    connect(videoSink, &QVideoSink::videoFrameChanged, this,
        [this](const QVideoFrame& frame) { processFrame(frame); });
    connect(camera, &QCamera::errorOccurred, this,
        [this](QCamera::Error, const QString& message) {
            if (!running)
                return;
            running = false;
            frameTimeoutTimer->stop();
            if (errorHandler)
                errorHandler(message);
        });
    frameTimeoutTimer->setSingleShot(true);
    connect(frameTimeoutTimer, &QTimer::timeout, this, [this] {
        if (running && !receivedFrame)
        {
            const bool wasActive = camera->isActive();
            const QString message = wasActive
                ? "The camera opened but produced no video frames. Check Windows camera privacy settings and close other apps using the camera."
                : "The camera did not start. Check camera permissions and device settings.";
            running = false;
            camera->stop();
            if (errorHandler)
                errorHandler(message);
        }
    });
}

CameraBarcodeScanner::~CameraBarcodeScanner()
{
    stop();
}

QList<QCameraDevice> CameraBarcodeScanner::availableCameras()
{
    return QMediaDevices::videoInputs();
}

bool CameraBarcodeScanner::decodeImage(const QImage& image, QString& digits)
{
    return decodeThorough(image, digits, false);
}

bool CameraBarcodeScanner::decodeImageFast(const QImage& image, QString& digits)
{
    return decodeFast(image, digits);
}

void CameraBarcodeScanner::setHandlers(
    PreviewHandler preview, ResultHandler result, ErrorHandler error)
{
    previewHandler = std::move(preview);
    resultHandler = std::move(result);
    errorHandler = std::move(error);
}

bool CameraBarcodeScanner::start(const QCameraDevice& device, QString& error)
{
    error.clear();
    if (device.isNull())
    {
        error = "The selected camera is unavailable.";
        return false;
    }

    stop();
    resultFound = false;
    receivedFrame = false;
    failedScans = 0;
    previewTimer.invalidate();
    camera->setCameraDevice(device);

    const QCameraFormat format = pickFormat(device);
    if (!format.isNull())
        camera->setCameraFormat(format);

    // Continuous autofocus, biased to near range when the camera offers it:
    // barcodes are held close to the lens.
    if (camera->isFocusModeSupported(QCamera::FocusModeAutoNear))
        camera->setFocusMode(QCamera::FocusModeAutoNear);
    else if (camera->isFocusModeSupported(QCamera::FocusModeAuto))
        camera->setFocusMode(QCamera::FocusModeAuto);
    if (camera->isExposureModeSupported(QCamera::ExposureBarcode))
        camera->setExposureMode(QCamera::ExposureBarcode);

    running = true;
    camera->start();
    if (camera->error() != QCamera::NoError)
    {
        running = false;
        error = camera->errorString();
        return false;
    }
    frameTimeoutTimer->start(4000);
    return true;
}

void CameraBarcodeScanner::stop()
{
    running = false;
    frameTimeoutTimer->stop();
    if (camera->isActive())
        camera->stop();
}

bool CameraBarcodeScanner::isRunning() const
{
    return running;
}

void CameraBarcodeScanner::processFrame(const QVideoFrame& frame)
{
    if (!running)
        return;

    if (!frame.isValid())
        return;

    if (!receivedFrame)
    {
        receivedFrame = true;
        frameTimeoutTimer->stop();
    }

    const bool updatePreview = previewHandler &&
        (!previewTimer.isValid() || previewTimer.elapsed() >= kPreviewIntervalMs);
    if (!updatePreview && (resultFound || decoding))
        return;

    const QImage image = frame.toImage();
    if (image.isNull())
    {
        running = false;
        camera->stop();
        if (errorHandler)
            errorHandler("Camera frames arrived, but Qt could not convert them to images.");
        return;
    }

    // Limit preview repaints; decoding still uses the freshest frame when ready.
    if (updatePreview)
    {
        previewHandler(image);
        previewTimer.restart();
    }

    // Decode on a worker thread. While a decode is still running, new frames
    // are simply dropped (we always scan the freshest frame that arrives
    // after the worker is free), so there is no queue and no lag.
    if (resultFound || decoding)
        return;
    decoding = true;

    // Most attempts are the fast pass (rows and columns, both read
    // directions). Every 4th failed attempt runs the thorough pass, which
    // also covers barcodes tilted at any angle, so a phone held portrait,
    // upside down or diagonally is still picked up within a fraction of a
    // second.
    const bool thorough = (++failedScans % 4 == 0);

    QPointer<CameraBarcodeScanner> self(this);
    QThreadPool::globalInstance()->start([self, image, thorough] {
        QString digits;
        const bool ok = thorough
            ? decodeThorough(image, digits, true)
            : decodeFast(image, digits);

        if (!self)
            return;
        QMetaObject::invokeMethod(self.data(), [self, ok, digits] {
            if (!self)
                return;
            self->decoding = false;
            if (ok && self->running && !self->resultFound)
            {
                self->resultFound = true;
                if (self->resultHandler)
                    self->resultHandler(digits);
            }
        }, Qt::QueuedConnection);
    });
}