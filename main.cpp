// main.cpp  -  BarcodeX (Code 39) UI
// Single window, two tabs: "Scan Barcode" (image upload / drag & drop)
// and "Generate Barcode" (6-digit input). No Q_OBJECT used, so no moc needed.

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
#include <QPainterPath>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QPixmap>
#include <QImage>
#include <QIcon>
#include <QStyle>
#include <QTimer>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

// =====================================================================
//  TEAM INTERFACE  -  teammates will replace the bodies of these two
//  functions with calls to encoder/generator and scanner/decoder.
//  The UI does not care what happens inside them.
// =====================================================================

// Input: 6-digit string. Output: barcode image (null QImage = failure).
// NOTE: PLACEHOLDER renderer so the UI can be tested.
static QImage encodeToBarcodeImage(const QString &digits)
{
    // n = narrow, w = wide  (9 elements: bar,space,bar,space,...)
    static const char *pat[10] = {
        "nnnwwnwnn", "wnnwnnnnw", "nnwwnnnnw", "wnwwnnnnn", "nnnwwnnnw",
        "wnnwwnnnn", "nnwwwnnnn", "nnnwnnwnw", "wnnwnnwnn", "nnwwnnwnn"
    };
    const char *star = "nwnnwnwnn"; // start/stop '*'

    QStringList chars;
    chars << QString::fromLatin1(star);
    for (QChar c : digits) {
        if (c < QLatin1Char('0') || c > QLatin1Char('9')) return QImage();
        chars << QString::fromLatin1(pat[c.digitValue()]);
    }
    chars << QString::fromLatin1(star);

    const int narrow = 3, wide = 9, quiet = 10 * narrow, height = 140, margin = 20;

    int width = quiet * 2;
    for (const QString &p : chars) {
        for (QChar e : p) width += (e == 'w') ? wide : narrow;
        width += narrow; // inter-character gap
    }
    width -= narrow;

    QImage img(width, height + margin * 2, QImage::Format_RGB32);
    img.fill(Qt::white);
    QPainter painter(&img);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);

    int x = quiet;
    for (const QString &p : chars) {
        bool bar = true;
        for (QChar e : p) {
            int w = (e == 'w') ? wide : narrow;
            if (bar) painter.drawRect(x, margin, w, height);
            x += w;
            bar = !bar;
        }
        x += narrow;
    }
    return img;
}

// Input: uploaded image. Output: true + digitsOut if decoded,
// otherwise false + a message in errorOut.
// NOTE: PLACEHOLDER - the real scanner + decoder goes here.
static bool decodeBarcodeImage(const QImage &img, QString &digitsOut, QString &errorOut)
{
    Q_UNUSED(img);
    Q_UNUSED(digitsOut);
    errorOut = "Decoder is not connected yet";
    return false;
}

// =====================================================================
//  Small drawing helpers
// =====================================================================
static void drawCorners(QPainter &p, const QRectF &r, double len, const QPen &pen)
{
    p.setPen(pen);
    p.setBrush(Qt::NoBrush);
    const double x1 = r.left(), y1 = r.top(), x2 = r.right(), y2 = r.bottom();
    QPainterPath path;
    path.moveTo(x1, y1 + len); path.lineTo(x1, y1); path.lineTo(x1 + len, y1);
    path.moveTo(x2 - len, y1); path.lineTo(x2, y1); path.lineTo(x2, y1 + len);
    path.moveTo(x1, y2 - len); path.lineTo(x1, y2); path.lineTo(x1 + len, y2);
    path.moveTo(x2 - len, y2); path.lineTo(x2, y2); path.lineTo(x2, y2 - len);
    p.drawPath(path);
}

// App logo: barcode inside four corner brackets
static QPixmap makeLogo(int s)
{
    QPixmap pm(s, s);
    pm.fill(Qt::transparent);
    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing);

    QLinearGradient g(0, 0, s, s);
    g.setColorAt(0, QColor("#22d3ee"));
    g.setColorAt(1, QColor("#6366f1"));
    QPen pen(QBrush(g), s * 0.06, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    drawCorners(p, QRectF(s * 0.07, s * 0.07, s * 0.86, s * 0.86), s * 0.27, pen);

    static const int units[] = {2, 1, 1, 1, 3, 1, 1, 1, 2, 1, 3, 1, 1, 1, 2};
    int total = 0;
    for (int u : units) total += u;
    const double unit = (s * 0.52) / total;
    double x = s * 0.24;
    bool bar = true;
    p.setPen(Qt::NoPen);
    p.setBrush(Qt::white);
    for (int u : units) {
        if (bar) p.drawRect(QRectF(x, s * 0.24, u * unit, s * 0.52));
        x += u * unit;
        bar = !bar;
    }
    return pm;
}

// =====================================================================
//  Custom widgets
// =====================================================================

// Background gradient with soft purple / blue glow
class GradientBg : public QWidget
{
protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        QLinearGradient g(0, 0, width(), height());
        g.setColorAt(0.0, QColor("#060a22"));
        g.setColorAt(0.55, QColor("#0f1238"));
        g.setColorAt(1.0, QColor("#09102e"));
        p.fillRect(rect(), g);

        QRadialGradient r1(width() * 0.92, height() * 0.30, width() * 0.45);
        r1.setColorAt(0, QColor(124, 58, 237, 110));
        r1.setColorAt(1, QColor(124, 58, 237, 0));
        p.fillRect(rect(), r1);

        QRadialGradient r2(width() * 0.04, height() * 0.85, width() * 0.40);
        r2.setColorAt(0, QColor(37, 99, 235, 95));
        r2.setColorAt(1, QColor(37, 99, 235, 0));
        p.fillRect(rect(), r2);
    }
};

// Barcode preview: image on a white card with scanner-style corner brackets
class PreviewFrame : public QWidget
{
public:
    explicit PreviewFrame(const QString &placeholderText, QWidget *parent = nullptr)
        : QWidget(parent), placeholder(placeholderText)
    {
        setMinimumHeight(190);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }
    void setImage(const QImage &i) { img = i; update(); }
    void clear() { img = QImage(); update(); }
    void setPlaceholder(const QString &t) { placeholder = t; update(); }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        p.setRenderHint(QPainter::SmoothPixmapTransform);

        const QRectF r = QRectF(rect()).adjusted(1, 1, -1, -1);
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(255, 255, 255, 10));
        p.drawRoundedRect(r, 16, 16);

        const QRectF inner = r.adjusted(34, 30, -34, -30);
        if (img.isNull()) {
            p.setPen(QColor("#8c93c4"));
            QFont f = font();
            f.setPixelSize(16);
            p.setFont(f);
            p.drawText(inner, Qt::AlignCenter | Qt::TextWordWrap, placeholder);
        } else {
            QSizeF s = img.size();
            s.scale(inner.size(), Qt::KeepAspectRatio);
            QRectF t(0, 0, s.width(), s.height());
            t.moveCenter(inner.center());
            p.setPen(Qt::NoPen);
            p.setBrush(Qt::white);
            p.drawRoundedRect(t.adjusted(-6, -6, 6, 6), 8, 8);
            p.drawImage(t, img);
        }

        drawCorners(p, r.adjusted(8, 8, -8, -8), 28,
                    QPen(QColor("#7c6cff"), 4, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    }

private:
    QImage  img;
    QString placeholder;
};

// Drag & drop area for image files (also clickable)
class DropZone : public QFrame
{
public:
    std::function<void(const QString &)> onFile;
    std::function<void()>                onClick;

    explicit DropZone(QWidget *parent = nullptr) : QFrame(parent)
    {
        setObjectName("dropZone");
        setAcceptDrops(true);
        setCursor(Qt::PointingHandCursor);
    }

protected:
    static QString firstLocalFile(const QMimeData *m)
    {
        if (!m->hasUrls()) return QString();
        for (const QUrl &u : m->urls())
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
    void dragEnterEvent(QDragEnterEvent *e) override
    {
        if (!firstLocalFile(e->mimeData()).isEmpty()) { e->acceptProposedAction(); setDrag(true); }
        else e->ignore();
    }
    void dragLeaveEvent(QDragLeaveEvent *e) override { setDrag(false); e->accept(); }
    void dropEvent(QDropEvent *e) override
    {
        setDrag(false);
        const QString path = firstLocalFile(e->mimeData());
        if (!path.isEmpty()) {
            e->acceptProposedAction();
            if (onFile) onFile(path);
        }
    }
    void mousePressEvent(QMouseEvent *e) override
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
    MainWindow()
    {
        setWindowTitle("BarcodeX");
        setWindowIcon(QIcon(makeLogo(128)));
        setMinimumSize(1080, 740);
        buildUi();
        applyStyle();
        setScanState(ScanState::Idle);
        setGenStatus(false);
        setStatus("Scan any Code 39 barcode with ease!", false);
    }

protected:
    bool eventFilter(QObject *obj, QEvent *e) override
    {
        const int idx = indexOfBox(obj);
        if (idx >= 0) {
            if (e->type() == QEvent::FocusIn) {
                QLineEdit *box = boxes[idx];
                QTimer::singleShot(0, box, &QLineEdit::selectAll);
            } else if (e->type() == QEvent::KeyPress) {
                auto *ke = static_cast<QKeyEvent *>(e);
                if (ke->matches(QKeySequence::Paste)) { pasteDigits(); return true; }
                if (ke->key() == Qt::Key_Backspace && boxes[idx]->text().isEmpty() && idx > 0) {
                    boxes[idx - 1]->clear();
                    boxes[idx - 1]->setFocus();
                    return true;
                }
                if (ke->key() == Qt::Key_Left && idx > 0)  { boxes[idx - 1]->setFocus(); return true; }
                if (ke->key() == Qt::Key_Right && idx < 5) { boxes[idx + 1]->setFocus(); return true; }
                if (ke->key() == Qt::Key_Return || ke->key() == Qt::Key_Enter) { onGenerate(); return true; }
            }
        }
        return QMainWindow::eventFilter(obj, e);
    }

private:
    enum class ScanState { Idle, Ok, Bad };

    struct Tile { QFrame *frame; QLabel *icon; QLabel *value; };

    // tabs / pages
    QStackedWidget *stack = nullptr;
    QPushButton    *tabScan = nullptr, *tabGen = nullptr;

    // scan page
    DropZone     *dropZone = nullptr;
    PreviewFrame *scanPreview = nullptr;
    QLabel       *scanHdrIcon = nullptr, *valueIcon = nullptr, *valueTitle = nullptr, *valueDigits = nullptr;
    QFrame       *valueBox = nullptr;
    QPushButton  *copyBtn = nullptr;
    Tile          scanStatusTile{};

    // generate page
    QLineEdit    *boxes[6] = {};
    PreviewFrame *genPreview = nullptr;
    QPushButton  *generateBtn = nullptr, *saveBtn = nullptr;
    Tile          genStatusTile{};
    QImage        currentBarcode;

    // footer
    QLabel *footMsg = nullptr;

    // ---------------- helpers ----------------
    int indexOfBox(QObject *o) const
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

    void setStatus(const QString &msg, bool isError)
    {
        footMsg->setText(msg);
        footMsg->setStyleSheet(isError ? "color:#ff7b7b;" : "color:#c9d0ff;");
    }

    static QString circleStyle(const QString &bg, const QString &fg, int size, int fontPx)
    {
        return QString("background:%1;color:%2;border-radius:%3px;font-size:%4px;font-weight:bold;")
            .arg(bg, fg).arg(size / 2).arg(fontPx);
    }

    static QLabel *circleLabel(const QString &text, int size, const QString &bg, const QString &fg, int fontPx)
    {
        auto *l = new QLabel(text);
        l->setFixedSize(size, size);
        l->setAlignment(Qt::AlignCenter);
        l->setStyleSheet(circleStyle(bg, fg, size, fontPx));
        return l;
    }

    static QFrame *makeCard()
    {
        auto *c = new QFrame;
        c->setObjectName("card");
        return c;
    }

    // Header row: round icon + title. Returns the icon label.
    static QLabel *addCardHeader(QVBoxLayout *lay, const QString &icon, const QString &title)
    {
        auto *row = new QHBoxLayout;
        row->setSpacing(14);
        QLabel *ic = circleLabel(icon, 40, "#7c3aed", "white", 20);
        auto *t = new QLabel(title);
        t->setObjectName("cardTitle");
        row->addWidget(ic);
        row->addWidget(t);
        row->addStretch();
        lay->addLayout(row);
        return ic;
    }

    static Tile makeTile(const QString &iconText, const QString &caption, const QString &value)
    {
        Tile t;
        t.frame = new QFrame;
        t.frame->setObjectName("tile");
        auto *h = new QHBoxLayout(t.frame);
        h->setContentsMargins(18, 12, 18, 12);
        h->setSpacing(14);
        t.icon = circleLabel(iconText, 34, "transparent", "#cfd6ff", 24);
        auto *col = new QVBoxLayout;
        col->setSpacing(0);
        auto *cap = new QLabel(caption);
        cap->setObjectName("tileCap");
        t.value = new QLabel(value);
        t.value->setObjectName("tileVal");
        col->addWidget(cap);
        col->addWidget(t.value);
        h->addWidget(t.icon);
        h->addLayout(col);
        h->addStretch();
        return t;
    }

    // ---------------- UI construction ----------------
    void buildUi()
    {
        auto *central = new GradientBg;
        setCentralWidget(central);
        auto *root = new QVBoxLayout(central);
        root->setContentsMargins(0, 0, 0, 0);
        root->setSpacing(0);

        auto *body = new QVBoxLayout;
        body->setContentsMargins(30, 22, 30, 16);
        body->setSpacing(16);
        root->addLayout(body, 1);

        body->addLayout(buildHeader());
        body->addLayout(buildTabs());

        stack = new QStackedWidget;
        stack->addWidget(buildScanPage());
        stack->addWidget(buildGeneratePage());
        body->addWidget(stack, 1);

        root->addWidget(buildFooter());
    }

    QHBoxLayout *buildHeader()
    {
        auto *row = new QHBoxLayout;
        row->setSpacing(22);

        auto *logo = new QLabel;
        logo->setPixmap(makeLogo(96));
        logo->setFixedSize(96, 96);
        row->addWidget(logo);

        auto *col = new QVBoxLayout;
        col->setSpacing(0);
        auto *title = new QLabel("Barcode<span style='color:#8b5cf6;'>X</span>");
        title->setTextFormat(Qt::RichText);
        title->setObjectName("appTitle");
        auto *sub = new QLabel("Code 39 Generator & Scanner");
        sub->setObjectName("appSub");
        col->addStretch();
        col->addWidget(title);
        col->addWidget(sub);
        col->addStretch();
        row->addLayout(col);
        row->addStretch();

        auto *badge = new QFrame;
        badge->setObjectName("badge");
        auto *bl = new QHBoxLayout(badge);
        bl->setContentsMargins(22, 10, 26, 10);
        bl->setSpacing(12);
        auto *shield = new QLabel("🛡");
        shield->setStyleSheet("font-size:24px;");
        auto *bt = new QLabel("Code 39  ·  Simple & Reliable");
        bt->setObjectName("badgeText");
        bl->addWidget(shield);
        bl->addWidget(bt);
        row->addWidget(badge, 0, Qt::AlignTop);
        return row;
    }

    QHBoxLayout *buildTabs()
    {
        auto *row = new QHBoxLayout;
        row->setSpacing(12);

        tabScan = new QPushButton("⬆   Scan Barcode");
        tabGen  = new QPushButton("▦   Generate Barcode");
        for (QPushButton *b : {tabScan, tabGen}) {
            b->setObjectName("tab");
            b->setCheckable(true);
            b->setCursor(Qt::PointingHandCursor);
            b->setFixedHeight(46);
            b->setMinimumWidth(210);
        }
        auto *grp = new QButtonGroup(this);
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

    QWidget *buildFooter()
    {
        auto *footer = new QFrame;
        footer->setObjectName("footer");
        footer->setFixedHeight(54);
        auto *h = new QHBoxLayout(footer);
        h->setContentsMargins(30, 0, 30, 0);
        h->setSpacing(12);

        auto *info = new QLabel("i");
        info->setFixedSize(24, 24);
        info->setAlignment(Qt::AlignCenter);
        info->setStyleSheet("border:2px solid #8f9bff;border-radius:12px;color:#8f9bff;"
                            "font-size:14px;font-weight:bold;");
        footMsg = new QLabel;
        footMsg->setObjectName("footMsg");
        auto *ver = new QLabel("BarcodeX v1.0");
        ver->setObjectName("footVer");

        h->addWidget(info);
        h->addWidget(footMsg, 1);
        h->addWidget(ver);
        return footer;
    }

    // ---------------- Scan page ----------------
    QWidget *buildScanPage()
    {
        auto *page = new QWidget;
        auto *row = new QHBoxLayout(page);
        row->setContentsMargins(0, 0, 0, 0);
        row->setSpacing(22);
        row->addWidget(buildUploadCard(), 1);
        row->addWidget(buildResultCard(), 1);
        return page;
    }

    QFrame *buildUploadCard()
    {
        QFrame *card = makeCard();
        auto *lay = new QVBoxLayout(card);
        lay->setContentsMargins(26, 22, 26, 22);
        lay->setSpacing(16);
        addCardHeader(lay, "⬆", "Upload Image");

        dropZone = new DropZone;
        dropZone->onFile  = [this](const QString &p) { loadImageFile(p); };
        dropZone->onClick = [this] { onChooseImage(); };

        auto *dz = new QVBoxLayout(dropZone);
        dz->setContentsMargins(24, 24, 24, 24);
        dz->setSpacing(10);
        dz->addStretch();

        auto *pic = new QLabel("🖼");
        pic->setAlignment(Qt::AlignCenter);
        pic->setStyleSheet("font-size:68px;");
        dz->addWidget(pic);

        auto *dzTitle = new QLabel("Drag & Drop an Image Here");
        dzTitle->setObjectName("dzTitle");
        dzTitle->setAlignment(Qt::AlignCenter);
        dz->addWidget(dzTitle);

        auto *orRow = new QHBoxLayout;
        auto *l1 = new QFrame; l1->setObjectName("line");
        auto *l2 = new QFrame; l2->setObjectName("line");
        auto *orLbl = new QLabel("or");
        orLbl->setObjectName("muted");
        orRow->addSpacing(40);
        orRow->addWidget(l1, 1);
        orRow->addWidget(orLbl);
        orRow->addWidget(l2, 1);
        orRow->addSpacing(40);
        dz->addLayout(orRow);

        auto *choose = new QPushButton("📂   Choose Image");
        choose->setObjectName("gradBtn");
        choose->setCursor(Qt::PointingHandCursor);
        choose->setFixedSize(280, 54);
        connect(choose, &QPushButton::clicked, this, [this] { onChooseImage(); });
        dz->addWidget(choose, 0, Qt::AlignHCenter);

        auto *sup = new QLabel("Supports: JPG, PNG, BMP");
        sup->setObjectName("muted");
        sup->setAlignment(Qt::AlignCenter);
        dz->addWidget(sup);
        dz->addStretch();

        lay->addWidget(dropZone, 1);
        return card;
    }

    QFrame *buildResultCard()
    {
        QFrame *card = makeCard();
        auto *lay = new QVBoxLayout(card);
        lay->setContentsMargins(26, 22, 26, 22);
        lay->setSpacing(14);
        scanHdrIcon = addCardHeader(lay, "•", "Scan Result");

        scanPreview = new PreviewFrame("The uploaded barcode image\nwill appear here");
        lay->addWidget(scanPreview, 1);

        // Decoded data box
        valueBox = new QFrame;
        valueBox->setObjectName("valueBox");
        auto *vl = new QVBoxLayout(valueBox);
        vl->setContentsMargins(18, 12, 18, 14);
        vl->setSpacing(4);

        auto *top = new QHBoxLayout;
        top->setSpacing(10);
        valueIcon = circleLabel("•", 28, "#2c3170", "#9aa3d8", 16);
        valueTitle = new QLabel("Decoded Data:");
        valueTitle->setObjectName("valueTitle");
        copyBtn = new QPushButton("⧉");
        copyBtn->setObjectName("copyBtn");
        copyBtn->setToolTip("Copy number");
        copyBtn->setCursor(Qt::PointingHandCursor);
        copyBtn->setFixedSize(42, 36);
        connect(copyBtn, &QPushButton::clicked, this, [this] {
            const QString d = valueDigits->property("rawDigits").toString();
            if (!d.isEmpty()) {
                QApplication::clipboard()->setText(d);
                setStatus("Copied to clipboard: " + d, false);
            }
        });
        top->addWidget(valueIcon);
        top->addWidget(valueTitle, 1);
        top->addWidget(copyBtn);
        vl->addLayout(top);

        valueDigits = new QLabel;
        valueDigits->setAlignment(Qt::AlignCenter);
        vl->addWidget(valueDigits);
        lay->addWidget(valueBox);

        // Type / Status tiles
        auto *tiles = new QHBoxLayout;
        tiles->setSpacing(14);
        Tile type = makeTile("▦", "Type:", "Code 39");
        scanStatusTile = makeTile("•", "Status:", "Waiting");
        tiles->addWidget(type.frame, 1);
        tiles->addWidget(scanStatusTile.frame, 1);
        lay->addLayout(tiles);
        return card;
    }

    // ---------------- Generate page ----------------
    QWidget *buildGeneratePage()
    {
        auto *page = new QWidget;
        auto *row = new QHBoxLayout(page);
        row->setContentsMargins(0, 0, 0, 0);
        row->setSpacing(22);
        row->addWidget(buildInputCard(), 1);
        row->addWidget(buildPreviewCard(), 1);
        return page;
    }

    QFrame *buildInputCard()
    {
        QFrame *card = makeCard();
        auto *lay = new QVBoxLayout(card);
        lay->setContentsMargins(26, 22, 26, 22);
        lay->setSpacing(16);
        addCardHeader(lay, "⚡", "Generate Barcode");

        auto *hint = new QLabel("Enter exactly 6 digits");
        hint->setObjectName("muted");
        lay->addWidget(hint);

        auto *row = new QHBoxLayout;
        row->setSpacing(10);
        auto *validator = new QRegularExpressionValidator(QRegularExpression("[0-9]"), this);
        for (int i = 0; i < 6; ++i) {
            boxes[i] = new QLineEdit;
            boxes[i]->setObjectName("digitBox");
            boxes[i]->setMaxLength(1);
            boxes[i]->setAlignment(Qt::AlignCenter);
            boxes[i]->setValidator(validator);
            boxes[i]->setFixedHeight(70);
            boxes[i]->setMinimumWidth(50);
            boxes[i]->installEventFilter(this);
            row->addWidget(boxes[i]);

            const int next = i + 1;
            connect(boxes[i], &QLineEdit::textEdited, this, [this, next](const QString &t) {
                if (!t.isEmpty() && next < 6) boxes[next]->setFocus();
            });
        }
        lay->addLayout(row);

        generateBtn = new QPushButton("⚡   Generate Barcode");
        generateBtn->setObjectName("gradBtn");
        generateBtn->setCursor(Qt::PointingHandCursor);
        generateBtn->setFixedHeight(54);
        connect(generateBtn, &QPushButton::clicked, this, [this] { onGenerate(); });
        lay->addWidget(generateBtn);

        lay->addStretch(1);
        auto *tip = new QLabel("Tip: the * start and stop symbols are added automatically.\n"
                               "Leading zeros are kept (for example 004281).");
        tip->setObjectName("muted");
        tip->setWordWrap(true);
        lay->addWidget(tip);
        return card;
    }

    QFrame *buildPreviewCard()
    {
        QFrame *card = makeCard();
        auto *lay = new QVBoxLayout(card);
        lay->setContentsMargins(26, 22, 26, 22);
        lay->setSpacing(14);
        addCardHeader(lay, "▦", "Barcode Preview");

        genPreview = new PreviewFrame("Your generated barcode\nwill appear here");
        lay->addWidget(genPreview, 1);

        saveBtn = new QPushButton("⬇   Save Image");
        saveBtn->setObjectName("ghostBtn");
        saveBtn->setCursor(Qt::PointingHandCursor);
        saveBtn->setFixedHeight(46);
        saveBtn->setEnabled(false);
        connect(saveBtn, &QPushButton::clicked, this, [this] { onSave(); });
        lay->addWidget(saveBtn, 0, Qt::AlignHCenter);

        auto *tiles = new QHBoxLayout;
        tiles->setSpacing(14);
        Tile type = makeTile("▦", "Type:", "Code 39");
        genStatusTile = makeTile("•", "Status:", "Ready");
        tiles->addWidget(type.frame, 1);
        tiles->addWidget(genStatusTile.frame, 1);
        lay->addLayout(tiles);
        return card;
    }

    // ---------------- State display ----------------
    void setScanState(ScanState s, const QString &digits = QString(), const QString &msg = QString())
    {
        QString border, boxBg, valColor, circleBg, icon, title, statusText;
        switch (s) {
        case ScanState::Idle:
            border = "rgba(120,130,255,0.35)"; boxBg = "rgba(20,26,80,0.50)";
            valColor = "#7d84b8"; circleBg = "#2c3170"; icon = "•";
            title = "Decoded Data:"; statusText = "Waiting";
            break;
        case ScanState::Ok:
            border = "#22c55e"; boxBg = "rgba(16,72,54,0.55)";
            valColor = "#34e08a"; circleBg = "#22c55e"; icon = "✓";
            title = "Decoded Data:"; statusText = "Valid";
            break;
        case ScanState::Bad:
            border = "#ef4444"; boxBg = "rgba(80,22,36,0.55)";
            valColor = "#ff7b7b"; circleBg = "#ef4444"; icon = "!";
            title = msg.isEmpty() ? QString("No barcode found") : msg;
            statusText = "Invalid";
            break;
        }

        valueBox->setStyleSheet(QString("QFrame#valueBox{background:%1;border:2px solid %2;border-radius:14px;}")
                                    .arg(boxBg, border));

        QString shown = "— — — — — —", raw;
        if (s == ScanState::Ok) {
            shown.clear();
            for (QChar c : digits) shown += QString(c) + " ";
            shown = shown.trimmed();
            raw = digits;
        }
        valueDigits->setText(shown);
        valueDigits->setProperty("rawDigits", raw);
        valueDigits->setStyleSheet(QString("font-size:46px;font-weight:700;color:%1;").arg(valColor));
        valueTitle->setText(title);
        copyBtn->setEnabled(s == ScanState::Ok);

        scanHdrIcon->setText(icon);
        scanHdrIcon->setStyleSheet(circleStyle(s == ScanState::Idle ? "#7c3aed" : circleBg, "white", 40, 20));
        valueIcon->setText(icon);
        valueIcon->setStyleSheet(circleStyle(circleBg, s == ScanState::Idle ? "#9aa3d8" : "white", 28, 16));

        scanStatusTile.icon->setText(icon);
        scanStatusTile.icon->setStyleSheet(circleStyle(circleBg, s == ScanState::Idle ? "#9aa3d8" : "white", 34, 18));
        scanStatusTile.value->setText(statusText);
        scanStatusTile.value->setStyleSheet(QString("color:%1;").arg(s == ScanState::Idle ? "#9aa3d8" : valColor));
    }

    void setGenStatus(bool generated)
    {
        genStatusTile.icon->setText(generated ? "✓" : "•");
        genStatusTile.icon->setStyleSheet(circleStyle(generated ? "#22c55e" : "#2c3170",
                                                      generated ? "white" : "#9aa3d8", 34, 18));
        genStatusTile.value->setText(generated ? "Generated" : "Ready");
        genStatusTile.value->setStyleSheet(QString("color:%1;").arg(generated ? "#34e08a" : "#9aa3d8"));
    }

    // ---------------- Actions ----------------
    void pasteDigits()
    {
        const QString t = QApplication::clipboard()->text().trimmed();
        static const QRegularExpression re("^[0-9]{6}$");
        if (!re.match(t).hasMatch()) {
            setStatus("Paste failed: clipboard must contain exactly 6 digits", true);
            return;
        }
        for (int i = 0; i < 6; ++i) boxes[i]->setText(QString(t[i]));
        boxes[5]->setFocus();
        setStatus("Digits pasted", false);
    }

    void onGenerate()
    {
        const QString input = currentInput();
        if (input.length() != 6) {
            setStatus("Please enter exactly 6 digits", true);
            for (int i = 0; i < 6; ++i)
                if (boxes[i]->text().isEmpty()) { boxes[i]->setFocus(); break; }
            return;
        }
        const QImage img = encodeToBarcodeImage(input);   // <-- team function
        if (img.isNull()) {
            setStatus("Could not generate the barcode", true);
            return;
        }
        currentBarcode = img;
        genPreview->setImage(currentBarcode);
        saveBtn->setEnabled(true);
        setGenStatus(true);
        setStatus("Barcode generated for " + input, false);
    }

    void onSave()
    {
        if (currentBarcode.isNull()) return;
        const QString path = QFileDialog::getSaveFileName(this, "Save Barcode",
                                 "barcode_" + currentInput() + ".png",
                                 "PNG Image (*.png);;BMP Image (*.bmp)");
        if (path.isEmpty()) return;
        if (currentBarcode.save(path)) setStatus("Saved: " + path, false);
        else setStatus("Could not save the file", true);
    }

    void onChooseImage()
    {
        const QString path = QFileDialog::getOpenFileName(this, "Select Barcode Image", QString(),
                                 "Images (*.png *.jpg *.jpeg *.bmp)");
        if (!path.isEmpty()) loadImageFile(path);
    }

    void loadImageFile(const QString &path)
    {
        const QString ext = QFileInfo(path).suffix().toLower();
        if (ext != "png" && ext != "jpg" && ext != "jpeg" && ext != "bmp") {
            scanPreview->clear();
            setScanState(ScanState::Bad, QString(), "Unsupported file type");
            setStatus("Please use a JPG, PNG or BMP image", true);
            return;
        }

        const QImage img(path);
        if (img.isNull()) {
            scanPreview->clear();
            setScanState(ScanState::Bad, QString(), "Unreadable image file");
            setStatus("Could not open the image", true);
            return;
        }
        scanPreview->setImage(img);

        const QString info = QString("%1 (%2 × %3)").arg(QFileInfo(path).fileName())
                                 .arg(img.width()).arg(img.height());

        QString digits, err;
        if (decodeBarcodeImage(img, digits, err)) {       // <-- team function
            static const QRegularExpression re("^[0-9]{6}$");
            if (re.match(digits).hasMatch()) {
                setScanState(ScanState::Ok, digits);
                setStatus("Decoded successfully: " + info, false);
            } else {
                setScanState(ScanState::Bad, QString(), "Decoded data is not a 6-digit number");
                setStatus("Invalid barcode pattern: " + info, true);
            }
        } else {
            const QString m = err.isEmpty() ? QString("No barcode found") : err;
            setScanState(ScanState::Bad, QString(), m);
            setStatus(m + " - " + info, true);
        }
    }

    // ---------------- Styling ----------------
    void applyStyle()
    {
        setStyleSheet(R"(
            * { font-family:'Segoe UI', Arial, sans-serif; }
            QLabel { background:transparent; color:#e6e9ff; font-size:15px; }

            QLabel#appTitle { font-size:48px; font-weight:700; color:white; }
            QLabel#appSub   { font-size:22px; color:#8f9bff; }
            QFrame#badge    { background:rgba(20,30,90,0.60); border:1px solid rgba(120,130,255,0.40); border-radius:26px; }
            QLabel#badgeText{ font-size:16px; color:#dfe3ff; }

            QFrame#card     { background:rgba(22,26,78,0.62); border:1px solid rgba(120,130,255,0.30); border-radius:20px; }
            QLabel#cardTitle{ font-size:26px; font-weight:600; color:white; }
            QLabel#muted    { color:#8f96c9; font-size:15px; }
            QLabel#dzTitle  { font-size:22px; font-weight:600; color:white; }

            QFrame#dropZone { background:rgba(10,14,50,0.45); border:2px dashed rgba(130,140,255,0.55); border-radius:16px; }
            QFrame#dropZone[drag="true"] { background:rgba(124,58,237,0.22); border:2px dashed #a78bfa; }
            QFrame#line     { background:rgba(130,140,255,0.30); border:none; min-height:1px; max-height:1px; }

            QPushButton#tab { color:#b9c0ff; background:rgba(30,36,100,0.55); border:1px solid rgba(120,130,255,0.30);
                              border-radius:23px; padding:0 28px; font-size:18px; font-weight:600; }
            QPushButton#tab:hover:!checked { background:rgba(60,70,160,0.60); }
            QPushButton#tab:checked { color:white; border:none;
                              background:qlineargradient(x1:0,y1:0,x2:1,y2:0, stop:0 #7c3aed, stop:1 #3b6cf5); }

            QPushButton#gradBtn { color:white; font-size:20px; font-weight:600; border:none; border-radius:27px; padding:0 30px;
                              background:qlineargradient(x1:0,y1:0,x2:1,y2:0, stop:0 #7c3aed, stop:1 #3b6cf5); }
            QPushButton#gradBtn:hover { background:qlineargradient(x1:0,y1:0,x2:1,y2:0, stop:0 #8b5cf6, stop:1 #5b86ff); }
            QPushButton#gradBtn:pressed { background:qlineargradient(x1:0,y1:0,x2:1,y2:0, stop:0 #6d28d9, stop:1 #2f58d6); }

            QPushButton#ghostBtn { color:#e6e9ff; background:rgba(60,70,160,0.45); border:1px solid rgba(120,130,255,0.40);
                              border-radius:23px; padding:0 40px; font-size:17px; }
            QPushButton#ghostBtn:hover    { background:rgba(80,90,190,0.60); }
            QPushButton#ghostBtn:disabled { color:#6a70a8; background:rgba(40,46,110,0.35); border:1px solid rgba(120,130,255,0.15); }

            QPushButton#copyBtn { color:#e6e9ff; background:rgba(60,70,160,0.50); border:1px solid rgba(120,130,255,0.35);
                              border-radius:10px; font-size:20px; padding:0; }
            QPushButton#copyBtn:hover    { background:rgba(80,90,190,0.65); }
            QPushButton#copyBtn:disabled { color:#5f6598; background:rgba(40,46,110,0.30); }

            QLineEdit#digitBox { background:rgba(10,14,50,0.60); border:1px solid rgba(120,130,255,0.40); border-radius:12px;
                              font-size:32px; color:white; selection-background-color:#7c3aed; }
            QLineEdit#digitBox:focus { border:2px solid #8b5cf6; }

            QFrame#valueBox { background:rgba(20,26,80,0.50); border:2px solid rgba(120,130,255,0.35); border-radius:14px; }
            QLabel#valueTitle { font-size:17px; color:white; }

            QFrame#tile     { background:rgba(15,22,70,0.55); border:1px solid rgba(120,130,255,0.30); border-radius:14px; }
            QLabel#tileCap  { color:#9aa3d8; font-size:14px; }
            QLabel#tileVal  { font-size:21px; color:#cfd6ff; }

            QFrame#footer   { background:rgba(6,8,34,0.60); border-top:1px solid rgba(120,130,255,0.25); }
            QLabel#footMsg  { font-size:15px; color:#c9d0ff; }
            QLabel#footVer  { font-size:15px; color:#9b7bff; }
        )");
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow w;
    w.resize(1160, 780);
    w.show();
    return app.exec();
}