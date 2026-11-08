// ui/ui.cpp  -  BarcodeX (Code 39) user interface
// Single window with two tabs: Scan (open an image) and Generate (6-digit input).
// This file knows nothing about the encoder/decoder. It only calls the two
// functions that main.cpp hands over through UiBackend (see ui.h).
// No Q_OBJECT is used, so no moc step is needed.

#include "ui.h"

#include <functional>

#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QButtonGroup>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QFileInfo>
#include <QClipboard>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>
#include <QPainter>
#include <QPixmap>
#include <QImage>
#include <QIcon>
#include <QStyle>
#include <QTimer>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

// UI rule: the first digit of the 6-digit input must not be 0.
// Set to false to allow inputs such as 004281 again.
static const bool kBlockLeadingZero = true;

// Colours used by the UI (change them here, the stylesheet picks them up).
static const char* kAccent = "#4F46E5";
static const char* kAccentHover = "#4338CA";
static const char* kAccentLight = "#EEF2FF";
static const char* kBackground = "#F5F7FB";
static const char* kBorder = "#E2E8F0";
static const char* kText = "#172033";
static const char* kMuted = "#667085";
static const char* kSoft = "#A5B4FC";
static const char* kOk = "#16804A";
static const char* kOkBg = "#ECF8F1";
static const char* kOkBorder = "#B7E2C8";
static const char* kBad = "#B42318";
static const char* kBadBg = "#FEF3F2";
static const char* kBadBorder = "#F2C2BE";

// =====================================================================
//  App icon: a few bars on a rounded square
// =====================================================================
static QPixmap makeLogo(int s)
{
    QPixmap pm(s, s);
    pm.fill(Qt::transparent);
    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(kAccent));
    p.drawRoundedRect(QRectF(0, 0, s, s), s * 0.2, s * 0.2);

    static const int units[] = { 2, 1, 1, 1, 3, 1, 1, 1, 2, 1, 3, 1, 1, 1, 2 };
    int total = 0;
    for (int u : units) total += u;
    const double unit = (s * 0.56) / total;
    double x = s * 0.22;
    bool bar = true;
    p.setBrush(Qt::white);
    for (int u : units) {
        if (bar) p.drawRect(QRectF(x, s * 0.25, u * unit, s * 0.50));
        x += u * unit;
        bar = !bar;
    }
    return pm;
}

// =====================================================================
//  Custom widgets
// =====================================================================

// Shows an image (scanned file or generated barcode) on a white sheet
class PreviewFrame : public QWidget
{
public:
    explicit PreviewFrame(const QString& placeholderText, QWidget* parent = nullptr)
        : QWidget(parent), placeholder(placeholderText)
    {
        setMinimumHeight(180);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }
    void setImage(const QImage& i) { img = i; update(); }
    void clear() { img = QImage(); update(); }
    void setPlaceholder(const QString& t) { placeholder = t; update(); }

protected:
    void paintEvent(QPaintEvent*) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        p.setRenderHint(QPainter::SmoothPixmapTransform);

        const QRectF r = QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5);
        p.setPen(QPen(QColor(kBorder), 1));
        p.setBrush(QColor("#F1EEFB"));
        p.drawRoundedRect(r, 8, 8);

        const QRectF inner = r.adjusted(18, 18, -18, -18);
        if (img.isNull()) {
            p.setPen(QColor(kMuted));
            QFont f = font();
            f.setPixelSize(14);
            p.setFont(f);
            p.drawText(inner, Qt::AlignCenter | Qt::TextWordWrap, placeholder);
        }
        else {
            QSizeF s = img.size();
            s.scale(inner.size(), Qt::KeepAspectRatio);
            QRectF t(0, 0, s.width(), s.height());
            t.moveCenter(inner.center());
            p.setPen(QPen(QColor(kBorder), 1));
            p.setBrush(Qt::white);
            p.drawRect(t.adjusted(-4, -4, 4, 4));
            p.drawImage(t, img);
        }
    }

private:
    QImage  img;
    QString placeholder;
};

// Area that accepts a dragged image file (also clickable)
class DropZone : public QFrame
{
public:
    std::function<void(const QString&)> onFile;
    std::function<void()>                onClick;

    explicit DropZone(QWidget* parent = nullptr) : QFrame(parent)
    {
        setObjectName("dropZone");
        setAcceptDrops(true);
        setCursor(Qt::PointingHandCursor);
    }

protected:
    static QString firstLocalFile(const QMimeData* m)
    {
        if (!m->hasUrls()) return QString();
        for (const QUrl& u : m->urls())
            if (u.isLocalFile()) return u.toLocalFile();
        return QString();
    }
    void setDrag(bool on)
    {
        setProperty("drag", on);
        style()->unpolish(this);
        style()->polish(this);
        update();
    }
    void dragEnterEvent(QDragEnterEvent* e) override
    {
        if (!firstLocalFile(e->mimeData()).isEmpty()) { e->acceptProposedAction(); setDrag(true); }
        else e->ignore();
    }
    void dragLeaveEvent(QDragLeaveEvent* e) override { setDrag(false); e->accept(); }
    void dropEvent(QDropEvent* e) override
    {
        setDrag(false);
        const QString path = firstLocalFile(e->mimeData());
        if (!path.isEmpty()) {
            e->acceptProposedAction();
            if (onFile) onFile(path);
        }
    }
    void mousePressEvent(QMouseEvent* e) override
    {
        if (e->button() == Qt::LeftButton && onClick) onClick();
        QFrame::mousePressEvent(e);
    }
};

// =====================================================================
//  Main window
// =====================================================================
class MainWindow : public QMainWindow
{
public:
    explicit MainWindow(const UiBackend& b) : backend(b)
    {
        setWindowTitle("BarcodeX");
        setWindowIcon(QIcon(makeLogo(128)));
        setMinimumSize(940, 640);
        buildUi();
        applyStyle();
        setScanState(ScanState::Idle);
        setGenStatus(GenState::Ready);
        setStatus("Ready.", false);
    }

protected:
    bool eventFilter(QObject* obj, QEvent* e) override
    {
        const int idx = indexOfBox(obj);
        if (idx >= 0) {
            if (e->type() == QEvent::FocusIn) {
                QLineEdit* box = boxes[idx];
                QTimer::singleShot(0, box, &QLineEdit::selectAll);
            }
            else if (e->type() == QEvent::KeyPress) {
                auto* ke = static_cast<QKeyEvent*>(e);
                if (ke->matches(QKeySequence::Paste)) { pasteDigits(); return true; }
                if (ke->key() == Qt::Key_Backspace && boxes[idx]->text().isEmpty() && idx > 0) {
                    boxes[idx - 1]->clear();
                    boxes[idx - 1]->setFocus();
                    return true;
                }
                if (ke->key() == Qt::Key_Left && idx > 0) { boxes[idx - 1]->setFocus(); return true; }
                if (ke->key() == Qt::Key_Right && idx < 5) { boxes[idx + 1]->setFocus(); return true; }
                if (ke->key() == Qt::Key_Return || ke->key() == Qt::Key_Enter) { onGenerate(); return true; }
            }
        }
        return QMainWindow::eventFilter(obj, e);
    }

private:
    enum class ScanState { Idle, Ok, Bad };
    enum class GenState { Ready, Done, Error };

    UiBackend backend;   // functions supplied by main.cpp

    // tabs / pages
    QStackedWidget* stack = nullptr;
    QPushButton* tabScan = nullptr, * tabGen = nullptr;

    // scan page
    PreviewFrame* scanPreview = nullptr;
    QFrame* valueBox = nullptr;
    QLabel* valueTitle = nullptr, * valueDigits = nullptr, * scanStatusValue = nullptr;
    QPushButton* copyBtn = nullptr;

    // generate page
    QLineEdit* boxes[6] = {};
    QLabel* digitError = nullptr;
    PreviewFrame* genPreview = nullptr;
    QPushButton* generateBtn = nullptr, * saveBtn = nullptr;
    QLabel* genStatusValue = nullptr;
    QImage        currentBarcode;

    // footer
    QLabel* footMsg = nullptr;

    // ---------------- helpers ----------------
    int indexOfBox(QObject* o) const
    {
        for (int i = 0; i < 6; ++i) if (boxes[i] == o) return i;
        return -1;
    }

    QString currentInput() const
    {
        QString s;
        for (int i = 0; i < 6; ++i) s += boxes[i]->text();
        return s;
    }

    void setStatus(const QString& msg, bool isError)
    {
        footMsg->setText(msg);
        footMsg->setStyleSheet(QString("color:%1;").arg(isError ? kBad : kMuted));
    }

    static QFrame* makeCard()
    {
        auto* c = new QFrame;
        c->setObjectName("card");
        return c;
    }

    static void addCardTitle(QVBoxLayout* lay, const QString& title, const QString& hint = QString())
    {
        auto* t = new QLabel(title);
        t->setObjectName("cardTitle");
        lay->addWidget(t);
        if (!hint.isEmpty()) {
            auto* h = new QLabel(hint);
            h->setObjectName("muted");
            lay->addWidget(h);
        }
    }

    static void addDivider(QVBoxLayout* lay)
    {
        auto* l = new QFrame;
        l->setObjectName("line");
        l->setFixedHeight(1);
        lay->addWidget(l);
    }

    // "Format ........ Code 39" style row; returns the value label
    static QLabel* addInfoRow(QVBoxLayout* lay, const QString& key, const QString& value)
    {
        auto* row = new QHBoxLayout;
        auto* k = new QLabel(key);
        k->setObjectName("infoKey");
        auto* v = new QLabel(value);
        v->setObjectName("infoVal");
        v->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        row->addWidget(k);
        row->addStretch();
        row->addWidget(v);
        lay->addLayout(row);
        return v;
    }

    // ---------------- UI construction ----------------
    void buildUi()
    {
        auto* central = new QFrame;
        central->setObjectName("central");
        setCentralWidget(central);
        auto* root = new QVBoxLayout(central);
        root->setContentsMargins(0, 0, 0, 0);
        root->setSpacing(0);

        auto* body = new QVBoxLayout;
        body->setContentsMargins(30, 24, 30, 18);
        body->setSpacing(16);
        root->addLayout(body, 1);

        body->addLayout(buildHeader());
        body->addLayout(buildTabs());
        addDivider(body);

        stack = new QStackedWidget;
        stack->addWidget(buildScanPage());
        stack->addWidget(buildGeneratePage());
        body->addWidget(stack, 1);

        root->addWidget(buildFooter());
    }

    QHBoxLayout* buildHeader()
    {
        auto* row = new QHBoxLayout;
        row->setSpacing(14);

        auto* logo = new QLabel;
        logo->setPixmap(makeLogo(40));
        logo->setFixedSize(40, 40);
        row->addWidget(logo);

        auto* titleBlock = new QVBoxLayout;
        titleBlock->setSpacing(2);

        auto* title = new QLabel("BarcodeX");
        title->setObjectName("appTitle");
        titleBlock->addWidget(title);

        auto* sub = new QLabel("Barcode generation and image scanning");
        sub->setObjectName("muted");
        titleBlock->addWidget(sub);
        row->addLayout(titleBlock);
        row->addStretch();

        auto* badge = new QLabel("CODE 39 STANDARD");
        badge->setObjectName("headerBadge");
        badge->setAlignment(Qt::AlignCenter);
        row->addWidget(badge, 0, Qt::AlignVCenter);
        return row;
    }

    QHBoxLayout* buildTabs()
    {
        auto* row = new QHBoxLayout;
        row->setSpacing(0);

        tabScan = new QPushButton("Scan");
        tabGen = new QPushButton("Generate");
        for (QPushButton* b : { tabScan, tabGen }) {
            b->setObjectName("tab");
            b->setCheckable(true);
            b->setCursor(Qt::PointingHandCursor);
            b->setFlat(true);
        }
        auto* grp = new QButtonGroup(this);
        grp->setExclusive(true);
        grp->addButton(tabScan);
        grp->addButton(tabGen);
        tabScan->setChecked(true);

        connect(tabScan, &QPushButton::clicked, this, [this] { stack->setCurrentIndex(0); });
        connect(tabGen, &QPushButton::clicked, this, [this] {
            stack->setCurrentIndex(1);
            boxes[0]->setFocus();
            });

        row->addWidget(tabScan);
        row->addWidget(tabGen);
        row->addStretch();
        return row;
    }

    QWidget* buildFooter()
    {
        auto* footer = new QFrame;
        footer->setObjectName("footer");
        footer->setFixedHeight(42);
        auto* h = new QHBoxLayout(footer);
        h->setContentsMargins(28, 0, 28, 0);

        footMsg = new QLabel;
        footMsg->setObjectName("footMsg");
        auto* ver = new QLabel("BARCODEX  ·  1.0");
        ver->setObjectName("muted");

        h->addWidget(footMsg, 1);
        h->addWidget(ver);
        return footer;
    }

    // ---------------- Scan page ----------------
    QWidget* buildScanPage()
    {
        auto* page = new QWidget;
        auto* row = new QHBoxLayout(page);
        row->setContentsMargins(0, 6, 0, 0);
        row->setSpacing(20);
        row->addWidget(buildUploadCard(), 1);
        row->addWidget(buildResultCard(), 1);
        return page;
    }

    QFrame* buildUploadCard()
    {
        QFrame* card = makeCard();
        auto* lay = new QVBoxLayout(card);
        lay->setContentsMargins(24, 22, 24, 24);
        lay->setSpacing(14);
        addCardTitle(lay, "Open an image", "Pick a photo or screenshot that contains a barcode.");

        auto* zone = new DropZone;
        zone->onFile = [this](const QString& p) { loadImageFile(p); };
        zone->onClick = [this] { onChooseImage(); };

        auto* dz = new QVBoxLayout(zone);
        dz->setContentsMargins(24, 24, 24, 24);
        dz->setSpacing(8);
        dz->addStretch();

        auto* dzTitle = new QLabel("Drop an image here");
        dzTitle->setObjectName("dzTitle");
        dzTitle->setAlignment(Qt::AlignCenter);
        dz->addWidget(dzTitle);

        auto* orLbl = new QLabel("or");
        orLbl->setObjectName("muted");
        orLbl->setAlignment(Qt::AlignCenter);
        dz->addWidget(orLbl);

        auto* choose = new QPushButton("Choose file...");
        choose->setObjectName("primaryBtn");
        choose->setCursor(Qt::PointingHandCursor);
        choose->setFixedSize(160, 44);
        connect(choose, &QPushButton::clicked, this, [this] { onChooseImage(); });
        dz->addWidget(choose, 0, Qt::AlignHCenter);

        auto* sup = new QLabel("JPG, PNG or BMP");
        sup->setObjectName("muted");
        sup->setAlignment(Qt::AlignCenter);
        dz->addWidget(sup);
        dz->addStretch();

        lay->addWidget(zone, 1);
        return card;
    }

    QFrame* buildResultCard()
    {
        QFrame* card = makeCard();
        auto* lay = new QVBoxLayout(card);
        lay->setContentsMargins(24, 22, 24, 24);
        lay->setSpacing(14);
        addCardTitle(lay, "Result");

        scanPreview = new PreviewFrame("The image you open will be shown here.");
        lay->addWidget(scanPreview, 1);

        valueBox = new QFrame;
        valueBox->setObjectName("valueBox");
        auto* vl = new QVBoxLayout(valueBox);
        vl->setContentsMargins(16, 10, 12, 12);
        vl->setSpacing(2);

        auto* top = new QHBoxLayout;
        valueTitle = new QLabel("Decoded number");
        valueTitle->setObjectName("valueTitle");
        copyBtn = new QPushButton("Copy");
        copyBtn->setObjectName("secondaryBtn");
        copyBtn->setCursor(Qt::PointingHandCursor);
        copyBtn->setFixedHeight(32);
        connect(copyBtn, &QPushButton::clicked, this, [this] {
            const QString d = valueDigits->property("rawDigits").toString();
            if (!d.isEmpty()) {
                QApplication::clipboard()->setText(d);
                setStatus("Copied " + d + " to the clipboard.", false);
            }
            });
        top->addWidget(valueTitle, 1);
        top->addWidget(copyBtn);
        vl->addLayout(top);

        valueDigits = new QLabel;
        valueDigits->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        vl->addWidget(valueDigits);
        lay->addWidget(valueBox);

        addInfoRow(lay, "Format", "Code 39");
        scanStatusValue = addInfoRow(lay, "Status", "Waiting");
        return card;
    }

    // ---------------- Generate page ----------------
    QWidget* buildGeneratePage()
    {
        auto* page = new QWidget;
        auto* row = new QHBoxLayout(page);
        row->setContentsMargins(0, 6, 0, 0);
        row->setSpacing(20);
        row->addWidget(buildInputCard(), 1);
        row->addWidget(buildPreviewCard(), 1);
        return page;
    }

    QFrame* buildInputCard()
    {
        QFrame* card = makeCard();
        auto* lay = new QVBoxLayout(card);
        lay->setContentsMargins(24, 22, 24, 24);
        lay->setSpacing(14);
        addCardTitle(lay, "Enter a number", "Exactly 6 digits. The first digit can't be 0.");

        auto* row = new QHBoxLayout;
        row->setSpacing(8);
        auto* validator = new QRegularExpressionValidator(QRegularExpression("[0-9]"), this);
        for (int i = 0; i < 6; ++i) {
            boxes[i] = new QLineEdit;
            boxes[i]->setObjectName("digitBox");
            boxes[i]->setMaxLength(1);
            boxes[i]->setAlignment(Qt::AlignCenter);
            boxes[i]->setValidator(validator);
            boxes[i]->setFixedHeight(64);
            boxes[i]->setMinimumWidth(48);
            boxes[i]->installEventFilter(this);
            row->addWidget(boxes[i]);

            connect(boxes[i], &QLineEdit::textChanged, this, [this] { updateGenerateState(); });

            const int next = i + 1;
            connect(boxes[i], &QLineEdit::textEdited, this, [this, next](const QString& t) {
                if (!t.isEmpty() && next < 6) boxes[next]->setFocus();
                });
        }
        lay->addLayout(row);

        digitError = new QLabel;
        digitError->setObjectName("errText");
        digitError->setFixedHeight(20);
        lay->addWidget(digitError);

        generateBtn = new QPushButton("Generate");
        generateBtn->setObjectName("primaryBtn");
        generateBtn->setCursor(Qt::PointingHandCursor);
        generateBtn->setFixedSize(160, 44);
        connect(generateBtn, &QPushButton::clicked, this, [this] { onGenerate(); });
        lay->addWidget(generateBtn, 0, Qt::AlignLeft);

        lay->addStretch(1);
        auto* tip = new QLabel("The * start and stop characters are added for you.");
        tip->setObjectName("muted");
        tip->setWordWrap(true);
        lay->addWidget(tip);
        return card;
    }

    QFrame* buildPreviewCard()
    {
        QFrame* card = makeCard();
        auto* lay = new QVBoxLayout(card);
        lay->setContentsMargins(24, 22, 24, 24);
        lay->setSpacing(14);
        addCardTitle(lay, "Barcode");

        genPreview = new PreviewFrame("Your barcode will be shown here.");
        lay->addWidget(genPreview, 1);

        saveBtn = new QPushButton("Save image...");
        saveBtn->setObjectName("secondaryBtn");
        saveBtn->setCursor(Qt::PointingHandCursor);
        saveBtn->setFixedHeight(38);
        saveBtn->setEnabled(false);
        connect(saveBtn, &QPushButton::clicked, this, [this] { onSave(); });
        lay->addWidget(saveBtn, 0, Qt::AlignLeft);

        addDivider(lay);
        addInfoRow(lay, "Format", "Code 39");
        genStatusValue = addInfoRow(lay, "Status", "Ready");
        return card;
    }

    // ---------------- State display ----------------
    void setScanState(ScanState s, const QString& digits = QString(), const QString& msg = QString())
    {
        QString bg, border, titleColor, digitColor, title = "Decoded number", status, statusColor;
        QString shown = "- - - - - -", raw;
        switch (s) {
        case ScanState::Idle:
            bg = "#F8FAFC"; border = kBorder; titleColor = kMuted; digitColor = "#AAB4C5";
            status = "Waiting"; statusColor = kMuted;
            break;
        case ScanState::Ok:
            bg = kOkBg; border = kOkBorder; titleColor = kMuted; digitColor = kOk;
            status = "Valid"; statusColor = kOk;
            shown = digits; raw = digits;
            break;
        case ScanState::Bad:
            bg = kBadBg; border = kBadBorder; titleColor = kBad; digitColor = "#D5A5A1";
            title = msg.isEmpty() ? QString("No barcode found") : msg;
            status = "Invalid"; statusColor = kBad;
            break;
        }

        valueBox->setStyleSheet(QString("QFrame#valueBox{background:%1;border:1px solid %2;border-radius:8px;}")
            .arg(bg, border));
        valueTitle->setText(title);
        valueTitle->setStyleSheet(QString("color:%1;font-weight:600;").arg(titleColor));
        valueDigits->setText(shown);
        valueDigits->setProperty("rawDigits", raw);
        valueDigits->setStyleSheet(QString("font-family:Consolas,'Courier New',monospace;"
            "font-size:38px;font-weight:600;color:%1;").arg(digitColor));
        copyBtn->setEnabled(s == ScanState::Ok);
        scanStatusValue->setText(status);
        scanStatusValue->setStyleSheet(QString("color:%1;font-weight:600;").arg(statusColor));
    }

    void setGenStatus(GenState g)
    {
        QString text = "Ready", col = kMuted;
        if (g == GenState::Done) { text = "Generated"; col = kOk; }
        else if (g == GenState::Error) { text = "Failed";    col = kBad; }
        genStatusValue->setText(text);
        genStatusValue->setStyleSheet(QString("color:%1;font-weight:600;").arg(col));
    }

    // Blocks the Generate button and shows a message while the first digit is 0
    void updateGenerateState()
    {
        const bool bad = kBlockLeadingZero && boxes[0]->text() == "0";
        digitError->setText(bad ? "The first digit can't be 0. Start with 1-9." : QString());
        boxes[0]->setProperty("bad", bad);
        boxes[0]->style()->unpolish(boxes[0]);
        boxes[0]->style()->polish(boxes[0]);
        if (generateBtn) generateBtn->setEnabled(!bad);
    }

    // ---------------- Actions ----------------
    void pasteDigits()
    {
        const QString t = QApplication::clipboard()->text().trimmed();
        static const QRegularExpression re("^[0-9]{6}$");
        if (!re.match(t).hasMatch()) {
            setStatus("The clipboard doesn't contain exactly 6 digits.", true);
            return;
        }
        for (int i = 0; i < 6; ++i) boxes[i]->setText(QString(t[i]));
        boxes[5]->setFocus();
        setStatus("Pasted " + t + ".", false);
    }

    void onGenerate()
    {
        if (kBlockLeadingZero && boxes[0]->text() == "0") {
            setStatus("The first digit can't be 0.", true);
            boxes[0]->setFocus();
            return;
        }
        const QString input = currentInput();
        if (input.length() != 6) {
            setStatus("Please enter all 6 digits.", true);
            for (int i = 0; i < 6; ++i)
                if (boxes[i]->text().isEmpty()) { boxes[i]->setFocus(); break; }
            return;
        }

        QImage img;
        QString err;
        bool ok = false;
        if (backend.generate) ok = backend.generate(input, img, err);   // <-- team function
        else err = "Generator not connected yet";
        if (!ok || img.isNull()) {
            const QString m = err.isEmpty() ? QString("Could not generate the barcode") : err;
            currentBarcode = QImage();
            genPreview->clear();
            genPreview->setPlaceholder(m);
            saveBtn->setEnabled(false);
            setGenStatus(GenState::Error);
            setStatus(m, true);
            return;
        }
        currentBarcode = img;
        genPreview->setImage(currentBarcode);
        saveBtn->setEnabled(true);
        setGenStatus(GenState::Done);
        setStatus("Generated a barcode for " + input + ".", false);
    }

    void onSave()
    {
        if (currentBarcode.isNull()) return;
        const QString path = QFileDialog::getSaveFileName(this, "Save barcode",
            "barcode_" + currentInput() + ".png",
            "PNG image (*.png);;BMP image (*.bmp)");
        if (path.isEmpty()) return;
        if (currentBarcode.save(path)) setStatus("Saved to " + path, false);
        else setStatus("Couldn't save the file.", true);
    }

    void onChooseImage()
    {
        const QString path = QFileDialog::getOpenFileName(this, "Choose a barcode image", QString(),
            "Images (*.png *.jpg *.jpeg *.bmp)");
        if (!path.isEmpty()) loadImageFile(path);
    }

    void loadImageFile(const QString& path)
    {
        const QString ext = QFileInfo(path).suffix().toLower();
        if (ext != "png" && ext != "jpg" && ext != "jpeg" && ext != "bmp") {
            scanPreview->clear();
            setScanState(ScanState::Bad, QString(), "Unsupported file type");
            setStatus("Please choose a JPG, PNG or BMP image.", true);
            return;
        }

        const QImage img(path);
        if (img.isNull()) {
            scanPreview->clear();
            setScanState(ScanState::Bad, QString(), "Couldn't open that image");
            setStatus("The file couldn't be read as an image.", true);
            return;
        }
        scanPreview->setImage(img);

        const QString info = QString("%1 (%2 x %3 px)").arg(QFileInfo(path).fileName())
            .arg(img.width()).arg(img.height());

        QString digits, err;
        bool decoded = false;
        if (backend.decode) decoded = backend.decode(img, digits, err);   // <-- team function
        else err = "Decoder not connected yet";
        if (decoded) {
            static const QRegularExpression re("^[0-9]{6}$");
            if (re.match(digits).hasMatch()) {
                setScanState(ScanState::Ok, digits);
                setStatus("Read " + digits + " from " + info + ".", false);
            }
            else {
                setScanState(ScanState::Bad, QString(), "That isn't a 6-digit number");
                setStatus("The barcode in " + info + " doesn't hold a 6-digit number.", true);
            }
        }
        else {
            const QString m = err.isEmpty() ? QString("No barcode found") : err;
            setScanState(ScanState::Bad, QString(), m);
            setStatus(m + " - " + info, true);
        }
    }

    // ---------------- Styling ----------------
    void applyStyle()
    {
        QString css = R"(
            * { font-family:'Segoe UI', Arial, sans-serif; }
            QFrame#central { background:@bg; }
            QLabel { background:transparent; color:@text; font-size:14px; }

            QLabel#appTitle { font-size:24px; font-weight:700; }
            QLabel#muted    { color:@muted; font-size:13px; }
            QLabel#headerBadge { color:@accent; background:@accentLight; border:1px solid #DDE3FF;
                                 border-radius:12px; padding:7px 12px; font-size:11px; font-weight:700; }
            QLabel#cardTitle{ font-size:17px; font-weight:600; }
            QLabel#dzTitle  { font-size:17px; font-weight:600; }
            QLabel#infoKey  { color:@muted; font-size:14px; }
            QLabel#infoVal  { font-size:14px; }
            QLabel#errText  { color:@bad; font-size:13px; }
            QLabel#valueTitle { font-size:14px; }

            QFrame#card { background:white; border:1px solid @border; border-radius:12px; }
            QFrame#line { background:@border; border:none; }

            QFrame#dropZone { background:#FAFBFE; border:2px dashed #C8D0E0; border-radius:10px; }
            QFrame#dropZone:hover { background:#F5F7FF; border:2px dashed @soft; }
            QFrame#dropZone[drag="true"] { background:@accentLight; border:2px dashed @accent; }

            QPushButton#tab { background:transparent; border:none; border-bottom:2px solid transparent;
                              color:@muted; font-size:14px; padding:10px 5px; margin-right:24px; }
            QPushButton#tab:hover { color:@text; }
            QPushButton#tab:checked { color:@accent; font-weight:700; border-bottom:2px solid @accent; }
            QPushButton#tab:focus { outline:none; }

            QPushButton#primaryBtn { background:@accent; color:white; border:none; border-radius:8px;
                                     font-size:14px; font-weight:600; }
            QPushButton#primaryBtn:hover    { background:@accentHover; }
            QPushButton#primaryBtn:disabled { background:#D8D0F5; color:#F8F7FF; }
            QPushButton#primaryBtn:focus { border:2px solid #A5B4FC; }

            QPushButton#secondaryBtn { background:white; color:@text; border:1px solid #D5DCE8; border-radius:8px;
                                       padding:0 16px; font-size:13px; font-weight:600; }
            QPushButton#secondaryBtn:hover    { background:#F8FAFF; border-color:#AAB6CC; }
            QPushButton#secondaryBtn:disabled { color:#98A2B3; background:#F8FAFC; border:1px solid #E8ECF2; }
            QPushButton#secondaryBtn:focus { border:2px solid #A5B4FC; }

            QLineEdit#digitBox { background:white; border:1px solid #D5DCE8; border-radius:8px; color:@text;
                                 font-family:Consolas,'Courier New',monospace; font-size:27px; font-weight:600;
                                 selection-background-color:@accent; }
            QLineEdit#digitBox:hover { border-color:#AAB6CC; }
            QLineEdit#digitBox:focus { border:2px solid @accent; }
            QLineEdit#digitBox[bad="true"] { border:2px solid @bad; background:#FFFBFA; }

            QFrame#footer  { background:white; border-top:1px solid @border; }
            QFrame#footer QLabel#muted { color:@muted; font-size:11px; font-weight:600; }
            QLabel#footMsg { font-size:13px; }
        )";
        css.replace("@accentLight", kAccentLight).replace("@accentHover", kAccentHover)
            .replace("@accent", kAccent).replace("@bg", kBackground).replace("@border", kBorder)
            .replace("@text", kText).replace("@muted", kMuted).replace("@bad", kBad)
            .replace("@soft", kSoft);
        setStyleSheet(css);
    }
};

QWidget* createMainWindow(const UiBackend& backend)
{
    auto* w = new MainWindow(backend);
    w->resize(1080, 720);
    return w;
}
