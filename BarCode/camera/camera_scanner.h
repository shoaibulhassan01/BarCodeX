#pragma once

#include <functional>

#include <QCameraDevice>
#include <QElapsedTimer>
#include <QImage>
#include <QObject>
#include <QString>

class QCamera;
class QMediaCaptureSession;
class QTimer;
class QVideoFrame;
class QVideoSink;

class CameraBarcodeScanner final : public QObject
{
public:
    using PreviewHandler = std::function<void(const QImage&)>;
    using ResultHandler = std::function<void(const QString&)>;
    using ErrorHandler = std::function<void(const QString&)>;

    explicit CameraBarcodeScanner(QObject* parent = nullptr);
    ~CameraBarcodeScanner() override;

    static QList<QCameraDevice> availableCameras();

    // Full (thorough) decode of a still image. Safe to call from any thread.
    static bool decodeImage(const QImage& image, QString& digits);

    // Quick decode used for live camera frames. Safe to call from any thread.
    static bool decodeImageFast(const QImage& image, QString& digits);

    void setHandlers(PreviewHandler preview, ResultHandler result, ErrorHandler error);
    bool start(const QCameraDevice& device, QString& error);
    void stop();
    bool isRunning() const;

private:
    void processFrame(const QVideoFrame& frame);

    QCamera* camera;
    QMediaCaptureSession* captureSession;
    QTimer* frameTimeoutTimer;
    QVideoSink* videoSink;
    PreviewHandler previewHandler;
    ResultHandler resultHandler;
    ErrorHandler errorHandler;
    QElapsedTimer previewTimer;
    bool running = false;
    bool receivedFrame = false;
    bool resultFound = false;
    bool decoding = false;   // a worker thread is decoding a frame right now
    int failedScans = 0;     // every Nth failed scan runs the thorough pass
};