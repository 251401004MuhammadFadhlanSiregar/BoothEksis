#include "camerapage.h"

#include <QPainter>
#include <QPainterPath>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QCamera>
#include <QVideoWidget>
#include <QImageCapture>
#include <QMediaDevices>
#include <QMediaCaptureSession>
#include <QFileDialog>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFont>
#include <QTemporaryFile>
#include <QDir>
#include <QApplication>
#include <QDebug>

// ── palette (same as menu) ───────────────────────────────────────────
static const QColor CAM_BG     { "#EEEEEE" };
static const QColor CAM_STRIP  { "#A89BC8" };
static const QColor CAM_FRAME  { "#FAFAC8" };  // yellow slot bg
static const QColor CAM_TITLE  { "#FF69B4" };
static const QColor CAM_STAR1  { "#FFD700" };
static const QColor CAM_STAR2  { "#FFB6C1" };
static const QColor CAM_STAR3  { "#B8FFD0" };

void CameraPage::drawSparkle(QPainter &p, QPointF c, float sz, QColor col)
{
    p.save();
    p.setPen(Qt::NoPen);
    p.setBrush(col);
    QPainterPath path;
    const float s = sz * 0.18f;
    path.moveTo(c.x(),       c.y() - sz);
    path.lineTo(c.x() + s,   c.y() - s);
    path.lineTo(c.x() + sz,  c.y());
    path.lineTo(c.x() + s,   c.y() + s);
    path.lineTo(c.x(),       c.y() + sz);
    path.lineTo(c.x() - s,   c.y() + s);
    path.lineTo(c.x() - sz,  c.y());
    path.lineTo(c.x() - s,   c.y() - s);
    path.closeSubpath();
    p.drawPath(path);
    p.restore();
}

// ── constructor ──────────────────────────────────────────────────────
CameraPage::CameraPage(QWidget *parent)
    : QWidget(parent)
    , m_camera(nullptr)
    , m_imageCapture(nullptr)
    , m_timer(nullptr)
    , m_countdownValue(3)
    , m_currentSlot(0)
    , m_uploadMode(false)
    , m_capturing(false)
{
    m_photos.resize(2);
    setupUI();
    setupCamera();
}

void CameraPage::setupUI()
{
    const int W = 820;
    const int H = 900;

    // ── Strip dimensions (centred) ──
    int stripW  = 500;
    int stripH  = 600;
    int stripX  = (W - stripW) / 2;
    int stripY  = (H - stripH) / 2;
    m_stripRect = QRect(stripX, stripY, stripW, stripH);

    // Two photo slots inside strip
    int slotW = 380;
    int slotH = 210;
    int slotX = stripX + (stripW - slotW) / 2;
    m_slot0Rect = QRect(slotX, stripY + 70,  slotW, slotH);
    m_slot1Rect = QRect(slotX, stripY + 310, slotW, slotH);

    // ── Camera viewfinder (fills active slot) ──
    m_viewfinderContainer = new QWidget(this);
    m_viewfinderContainer->setGeometry(m_slot0Rect);
    m_viewfinderContainer->setStyleSheet("background:black;");

    m_viewfinderContainer = new QVideoWidget(m_viewfinderContainer);
    m_viewfinderContainer->setGeometry(0, 0, slotW, slotH);

    // ── Countdown overlay ──
    m_countdownOverlay = new QLabel(this);
    m_countdownOverlay->setAlignment(Qt::AlignCenter);
    m_countdownOverlay->setGeometry(W / 2 - 40, stripY + 30, 80, 50);
    m_countdownOverlay->setStyleSheet(
        "QLabel {"
        "  background: rgba(40,40,40,200);"
        "  color: white;"
        "  font-size: 28px;"
        "  font-weight: bold;"
        "  border-radius: 12px;"
        "}");
    m_countdownOverlay->hide();

    // ── Frame indicator label ──
    m_frameLabel = new QLabel("Photo 1 / 2", this);
    m_frameLabel->setAlignment(Qt::AlignCenter);
    m_frameLabel->setGeometry(0, stripY + stripH + 10, W, 24);
    m_frameLabel->setStyleSheet("color:#555; font-size:14px; font-weight:bold;");

    // ── Capture / Start button ──
    m_captureBtn = new QPushButton("Start Session", this);
    m_captureBtn->setGeometry(W / 2 - 110, stripY + stripH + 46, 220, 52);
    m_captureBtn->setCursor(Qt::PointingHandCursor);
    m_captureBtn->setStyleSheet(
        "QPushButton {"
        "  background: #FAFAC8;"
        "  border: 2px solid #C8C850;"
        "  border-radius: 18px;"
        "  font-size: 17px;"
        "  font-weight: bold;"
        "  color: #333;"
        "}"
        "QPushButton:hover  { background: #F0F090; }"
        "QPushButton:pressed{ background: #E0E070; }"
        "QPushButton:disabled{ background: #E0E0E0; color: #888; border-color:#C0C0C0; }"
    );
    connect(m_captureBtn, &QPushButton::clicked,
            this, &CameraPage::onCaptureButtonClicked);

    // ── Back button ──
    m_backBtn = new QPushButton("← Back", this);
    m_backBtn->setGeometry(20, 20, 100, 36);
    m_backBtn->setCursor(Qt::PointingHandCursor);
    m_backBtn->setStyleSheet(
        "QPushButton {"
        "  background: rgba(255,255,255,150);"
        "  border: 2px solid #999;"
        "  border-radius: 10px;"
        "  font-size: 13px;"
        "  color: #333;"
        "}"
        "QPushButton:hover { background: white; }"
    );
    connect(m_backBtn, &QPushButton::clicked,
            this, &CameraPage::backClicked);
}

void CameraPage::setupCamera()
{
    if (QMediaDevices::videoInputs().isEmpty()) {
        qWarning() << "No camera found - upload-mode only.";
        m_uploadMode = true;
        return;
    }

    m_camera         = new QCamera(this);
    m_captureSession = new QMediaCaptureSession(this);
    m_imageCapture   = new QImageCapture(this);

    m_captureSession->setCamera(m_camera);
    m_captureSession->setImageCapture(m_imageCapture);
    m_captureSession->setVideoOutput(m_viewfinderContainer);  // pakai m_viewfinder yang sudah dibuat di setupUI()

    connect(m_imageCapture, &QImageCapture::imageCaptured,
            this, &CameraPage::onImageCaptured);

    m_camera->start();
}

// ── startSession ─────────────────────────────────────────────────────
void CameraPage::startSession(bool uploadMode)
{
    m_uploadMode     = uploadMode;
    m_currentSlot    = 0;
    m_capturing      = false;
    m_countdownValue = 3;
    m_photos[0]      = QPixmap();
    m_photos[1]      = QPixmap();
    m_viewfinderContainer->setGeometry(m_slot0Rect);
    m_frameLabel->setText("Photo 1 / 2");
    m_countdownOverlay->hide();

    if (m_uploadMode) {
        m_captureBtn->setText("Select Photos");
        if (m_camera) m_camera->stop();
        m_viewfinderContainer->hide();
        update();
        // Go immediately to file picker
        QTimer::singleShot(200, this, &CameraPage::handleUploadMode);
    } else {
        m_captureBtn->setText("Capture");
        m_captureBtn->setEnabled(true);
        if (m_camera) {
            m_viewfinderContainer->show();
            m_camera->start();
        } else {
            m_viewfinderContainer->hide();
            m_captureBtn->setText("No Camera – Upload instead");
        }
        update();
    }
}

void CameraPage::stopCamera()
{
    if (m_camera && m_camera->isActive())
        m_camera->stop();
    if (m_timer && m_timer->isActive())
        m_timer->stop();
    m_countdownOverlay->hide();
}

// ── Countdown logic ──────────────────────────────────────────────────
void CameraPage::startCountdown()
{
    m_countdownValue = 3;
    m_countdownOverlay->setText(QString::number(m_countdownValue));
    m_countdownOverlay->show();
    m_captureBtn->setEnabled(false);

    if (!m_timer) {
        m_timer = new QTimer(this);
        m_timer->setInterval(1000);
        connect(m_timer, &QTimer::timeout, this, &CameraPage::onCountdownTick);
    }
    m_timer->start();
}

void CameraPage::onCountdownTick()
{
    m_countdownValue--;
    if (m_countdownValue > 0) {
        m_countdownOverlay->setText(QString::number(m_countdownValue));
    } else {
        m_timer->stop();
        m_countdownOverlay->hide();
        triggerCapture();
    }
}

void CameraPage::triggerCapture()
{
    if (m_imageCapture) {
        m_imageCapture->capture();
    } else {
        // Fallback: generate a solid-color placeholder
        QPixmap placeholder(380, 210);
        placeholder.fill(m_currentSlot == 0 ? QColor("#D0C0E8") : QColor("#C0D0E8"));
        onImageCaptured(-1, placeholder.toImage());
    }
}

void CameraPage::onCaptureButtonClicked()
{
    if (m_uploadMode) {
        handleUploadMode();
    } else {
        startCountdown();
    }
}

void CameraPage::onImageCaptured(int /*id*/, const QImage &img)
{
    m_photos[m_currentSlot] = QPixmap::fromImage(img);
    update();

    if (m_currentSlot == 0) {
        // Move viewfinder to slot 1
        m_currentSlot = 1;
        m_viewfinderContainer->setGeometry(m_slot1Rect);
        m_frameLabel->setText("Photo 2 / 2");
        m_captureBtn->setEnabled(true);
        m_captureBtn->setText("Capture  (Photo 2)");
    } else {
        // Both slots done
        m_viewfinderContainer->hide();
        if (m_camera) m_camera->stop();
        emit photosReady(m_photos);
    }
}

void CameraPage::onImageSaved(int, const QString &) {}

// ── Upload mode ──────────────────────────────────────────────────────
void CameraPage::handleUploadMode()
{
    QStringList files = QFileDialog::getOpenFileNames(
        this,
        "Select 2 Photos",
        QDir::homePath(),
        "Images (*.png *.jpg *.jpeg *.bmp *.webp)"
    );

    if (files.isEmpty()) {
        emit backClicked();
        return;
    }

    for (int i = 0; i < qMin(files.size(), 2); ++i)
        m_photos[i] = QPixmap(files[i]);

    // If only 1 file selected, use it for both slots
    if (files.size() == 1)
        m_photos[1] = m_photos[0];

    emit photosReady(m_photos);
}

// ── paintEvent – draws the strip preview background ──────────────────
void CameraPage::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // White page bg
    p.fillRect(rect(), Qt::white);

    // "Bootheksis" header area
    p.setPen(QColor("#A0522D"));
    QFont hf("Arial", 13, QFont::Bold);
    p.setFont(hf);
    p.drawText(QRect(0, 18, width(), 28), Qt::AlignHCenter, "Bootheksis");

    // Draw the strip background (decorative frame)
    drawSpaceStrip(p);

    // // Draw captured photos into their slots
    const QRect photoSlots[2] = { m_slot0Rect, m_slot1Rect };
    for (int i = 0; i < 2; ++i) {
        if (!m_photos[i].isNull()) {
            QPixmap scaled = m_photos[i].scaled(
                photoSlots[i].size(), Qt::KeepAspectRatioByExpanding,
                Qt::SmoothTransformation);
            QRect src((scaled.width() - photoSlots[i].width()) / 2,
                      (scaled.height() - photoSlots[i].height()) / 2,
                      photoSlots[i].width(), photoSlots[i].height());
            p.drawPixmap(photoSlots[i], scaled, src);
        }
    }
}

void CameraPage::drawSpaceStrip(QPainter &p)
{
    // Strip bg
    p.save();
    p.setPen(QPen(QColor("#8878B8"), 3));
    p.setBrush(CAM_STRIP);
    p.drawRoundedRect(m_stripRect, 14, 14);

    // Title "SPACE ⊕ ADVENTURE"
    QFont tf("Arial", 20, QFont::Black);
    p.setFont(tf);
    p.setPen(Qt::white);
    for (int dx=-2;dx<=2;dx++) for (int dy=-2;dy<=2;dy++) if(dx||dy)
        p.drawText(m_stripRect.adjusted(0,0,0,0).translated(dx,dy), Qt::AlignHCenter | Qt::AlignTop | Qt::TextSingleLine,
                   "  SPACE ⊕ ADVENTURE  ");
    p.setPen(CAM_TITLE);
    p.drawText(m_stripRect, Qt::AlignHCenter | Qt::AlignTop | Qt::TextSingleLine,
               "  SPACE ⊕ ADVENTURE  ");

    // Slot backgrounds
    p.setPen(Qt::NoPen);
    p.setBrush(CAM_FRAME);
    p.drawRoundedRect(m_slot0Rect, 8, 8);
    p.drawRoundedRect(m_slot1Rect, 8, 8);

    // Sparkles
    const QColor cols[3] = { CAM_STAR1, CAM_STAR2, CAM_STAR3 };
    struct Sp { float x,y,sz; int c; };
    const Sp sps[] = {
        {0.04f,0.08f,7,0},{0.92f,0.06f,6,1},{0.88f,0.55f,8,2},
        {0.05f,0.55f,7,1},{0.95f,0.30f,6,0},{0.10f,0.92f,7,2},
        {0.85f,0.92f,8,0},{0.50f,0.05f,5,1},
    };
    for (const auto &s : sps)
        drawSparkle(p,
            {m_stripRect.x() + s.x * m_stripRect.width(),
             m_stripRect.y() + s.y * m_stripRect.height()},
            s.sz, cols[s.c]);

    // Saturn planet (left side)
    {
        int px = m_stripRect.x() + 22, py = m_stripRect.y() + 50;
        p.setBrush(QColor("#FFC0A0")); p.setPen(Qt::NoPen);
        p.drawEllipse(px,py,36,36);
        p.setPen(QPen(QColor("#E09070"),3));
        p.drawEllipse(px-8,py+10,52,16);
    }
    // Earth planet (right side, lower)
    {
        int px = m_stripRect.x() + m_stripRect.width() - 52;
        int py = m_stripRect.y() + 45;
        p.setBrush(QColor("#80B8E8")); p.setPen(Qt::NoPen);
        p.drawEllipse(px,py,32,32);
        p.setBrush(QColor("#70C870"));
        p.drawEllipse(px+4,py+8,12,14);
        p.drawEllipse(px+14,py+2,10,12);
    }
    // Brown meteor (left, mid)
    {
        int px = m_stripRect.x() + 8, py = m_stripRect.y() + m_stripRect.height()/2 - 12;
        p.setBrush(QColor("#CD853F")); p.setPen(Qt::NoPen);
        p.drawEllipse(px,py,28,28);
        p.setBrush(QColor("#8B4513"));
        p.drawEllipse(px+7,py+5,8,6);
        p.drawEllipse(px+4,py+14,5,5);
    }
    // Rocket (right, bottom)
    {
        int rx = m_stripRect.x() + m_stripRect.width() - 48;
        int ry = m_stripRect.y() + m_stripRect.height() - 80;
        // body
        p.setBrush(QColor("#E8E8E8")); p.setPen(Qt::NoPen);
        p.drawRoundedRect(rx+8,ry+8,24,48,6,6);
        // nose
        QPainterPath nose; nose.moveTo(rx+8,ry+8); nose.lineTo(rx+20,ry); nose.lineTo(rx+32,ry+8);
        p.setBrush(QColor("#F44336")); p.drawPath(nose);
        // window
        p.setBrush(QColor("#90CAF9")); p.drawEllipse(rx+12,ry+16,16,16);
        // fins
        QPainterPath fl; fl.moveTo(rx+8,ry+40); fl.lineTo(rx,ry+56); fl.lineTo(rx+8,ry+56);
        QPainterPath fr; fr.moveTo(rx+32,ry+40); fr.lineTo(rx+40,ry+56); fr.lineTo(rx+32,ry+56);
        p.setBrush(QColor("#F44336")); p.drawPath(fl); p.drawPath(fr);
        // flame
        p.setBrush(QColor("#FF9800")); p.drawEllipse(rx+10,ry+54,20,14);
        p.setBrush(QColor("#FFEB3B")); p.drawEllipse(rx+14,ry+57,12,10);
    }
    // Wave decoration at bottom
    {
        int wy = m_stripRect.y() + m_stripRect.height() - 42;
        QPainterPath wave;
        wave.moveTo(m_stripRect.x() + 10, wy + 10);
        for (int i = 0; i < 5; ++i) {
            float x0 = m_stripRect.x() + 10 + i * (m_stripRect.width()-20)/5.0f;
            float x1 = x0 + (m_stripRect.width()-20)/10.0f;
            float x2 = x0 + (m_stripRect.width()-20)/5.0f;
            wave.cubicTo(x0+15, wy, x1+5, wy+22, x2, wy+12);
        }
        p.setPen(QPen(QColor("#B0D0F8"), 2.5f, Qt::SolidLine, Qt::RoundCap));
        p.setBrush(Qt::NoBrush);
        p.drawPath(wave);
        wave.translate(0, 10);
        p.drawPath(wave);
    }

    p.restore();
}
