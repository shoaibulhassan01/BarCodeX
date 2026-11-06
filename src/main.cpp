#include <QApplication>
#include <QFileDialog>
#include <QFileInfo>
#include <QFrame>
#include <QImage>
#include <QImageReader>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QResizeEvent>
#include <QStringList>
#include <QVBoxLayout>
#include <QWidget>

#include <utility>

class ImagePreview final : public QLabel
{
public:
    explicit ImagePreview(QWidget *parent = nullptr)
        : QLabel(parent)
    {
        setAlignment(Qt::AlignCenter);
        setMinimumSize(320, 220);
        setFrameShape(QFrame::StyledPanel);
        setText("Choose an image to preview");
    }

    bool loadImage(const QString &path, QString *errorMessage)
    {
        QImageReader reader(path);
        reader.setAutoTransform(true);
        QImage loadedImage = reader.read();

        if (loadedImage.isNull()) {
            if (errorMessage != nullptr) {
                *errorMessage = reader.errorString();
            }
            return false;
        }

        image_ = std::move(loadedImage);
        updatePreview();
        return true;
    }

    QSize imageSize() const
    {
        return image_.size();
    }

protected:
    void resizeEvent(QResizeEvent *event) override
    {
        QLabel::resizeEvent(event);
        updatePreview();
    }

private:
    void updatePreview()
    {
        if (!image_.isNull()) {
            setPixmap(QPixmap::fromImage(image_).scaled(
                contentsRect().size(),
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation));
        }
    }

    QImage image_;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QWidget window;
    window.setWindowTitle("BarCodeX - Image Upload");
    window.resize(720, 520);

    auto *layout = new QVBoxLayout(&window);
    auto *uploadButton = new QPushButton("Choose Image...", &window);
    auto *statusLabel = new QLabel("No image selected.", &window);
    auto *imagePreview = new ImagePreview(&window);

    layout->addWidget(uploadButton);
    layout->addWidget(statusLabel);
    layout->addWidget(imagePreview, 1);

    QStringList imagePatterns;
    for (const QByteArray &format : QImageReader::supportedImageFormats()) {
        imagePatterns.append("*." + QString::fromLatin1(format));
    }
    const QString imageFilter = QString("Images (%1);;All files (*)")
                                    .arg(imagePatterns.join(' '));

    QObject::connect(uploadButton, &QPushButton::clicked, &window, [&]() {
        const QString filePath = QFileDialog::getOpenFileName(
            &window,
            "Choose an image",
            QString(),
            imageFilter);

        if (filePath.isEmpty()) {
            return;
        }

        QString errorMessage;
        if (!imagePreview->loadImage(filePath, &errorMessage)) {
            statusLabel->setText(
                QString("Could not load %1: %2")
                    .arg(QFileInfo(filePath).fileName(), errorMessage));
            return;
        }

        const QSize size = imagePreview->imageSize();
        statusLabel->setText(
            QString("Loaded %1 (%2 x %3)")
                .arg(QFileInfo(filePath).fileName())
                .arg(size.width())
                .arg(size.height()));
    });

    window.show();
    return app.exec();
}
