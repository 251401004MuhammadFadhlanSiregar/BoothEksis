#include "stickerpage.h"

#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QPushButton>
#include <QLabel>
#include <QFileDialog>
#include <QClipboard>
#include <QGuiApplication>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDir>
#include <QDateTime>
#include <QDesktopServices>
#include <QUrl>
#include <QDebug>

// ── palette ──────────────────────────────────────────────────────────
static const QColor S_BG      { "#A89BC8" };
static const QColor S_TITLE   { "#FF69B4" };
static const QColor S_SLOT    { "#FAFAC8" };
static const QColor S_STAR1   { "#FFD700" };
static const QColor S_STAR2   { "#FFB6C1" };
static const QColor S_STAR3   { "#B8FFD0" };

// ─────────────────────────────────────────────────────────────────────
// Sticker drawing helper (index 0-4)
// 0=Alien  1=Meteor  2=Earth  3=Saturn  4=Rocket
// ─────────────────────────────────────────────────────────────────────
QPixmap StickerCanvas::makeStickerPixmap(int type, int sz)
{
    QPixmap pm(sz, sz);
    pm.fill(Qt::transparent);
    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing);

    const int cx = sz / 2;
    const int cy = sz / 2;
    const int r  = sz / 2 - 4;

    switch (type) {
    // ── 0: Alien ──────────────────────────────────────────────────
    case 0: {
        // Face
        p.setPen(QPen(QColor("#3A8A3A"), 1.5));
        p.setBrush(QColor("#90EE90"));
        p.drawEllipse(cx - r, cy - r + 10, r * 2, r * 2 - 5);
        // Antennas
        p.setPen(QPen(QColor("#3A8A3A"), 2));
        p.drawLine(cx - 8, cy - r + 12, cx - 14, cy - r - 6);
        p.drawLine(cx + 8, cy - r + 12, cx + 14, cy - r - 6);
        p.setBrush(QColor("#90EE90")); p.setPen(Qt::NoPen);
        p.drawEllipse(cx - 17, cy - r - 10, 7, 7);
        p.drawEllipse(cx + 10, cy - r - 10, 7, 7);
        // Eyes
        p.setBrush(Qt::white); p.setPen(QPen(Qt::black, 1));
        p.drawEllipse(cx - 13, cy - r + 20, 10, 14);
        p.drawEllipse(cx + 3,  cy - r + 20, 10, 14);
        p.setBrush(Qt::black); p.setPen(Qt::NoPen);
        p.drawEllipse(cx - 11, cy - r + 23, 6, 8);
        p.drawEllipse(cx + 5,  cy - r + 23, 6, 8);
        // Smile
        p.setPen(QPen(QColor("#3A8A3A"), 2));
        p.drawArc(cx - 8, cy + 5, 16, 9, 0, -180 * 16);
        break;
    }
    // ── 1: Meteor ─────────────────────────────────────────────────
    case 1: {
        QPainterPath path;
        path.moveTo(cx, cy - r);
        path.quadTo(cx + r - 3, cy - r + 6, cx + r, cy + 2);
        path.quadTo(cx + r + 2, cy + r - 4, cx + 2, cy + r);
        path.quadTo(cx - r + 2, cy + r + 2, cx - r, cy + 2);
        path.quadTo(cx - r - 2, cy - r + 4, cx, cy - r);
        p.setPen(QPen(QColor("#7B3513"), 1.5));
        p.setBrush(QColor("#CD853F"));
        p.drawPath(path);
        // craters
        p.setPen(Qt::NoPen);
        p.setBrush(QColor("#8B4513"));
        p.drawEllipse(cx + 4, cy - 6, 9, 7);
        p.drawEllipse(cx - 10, cy + 7, 7, 5);
        p.drawEllipse(cx - 3, cy - 10, 6, 5);
        break;
    }
    // ── 2: Earth ──────────────────────────────────────────────────
    case 2: {
        p.setPen(QPen(QColor("#1050A0"), 1.5));
        p.setBrush(QColor("#4090D0"));
        p.drawEllipse(cx - r, cy - r, r * 2, r * 2);
        p.setPen(Qt::NoPen);
        p.setBrush(QColor("#50B850"));
        p.drawEllipse(cx - 4, cy - r + 6, 16, 20);
        p.drawEllipse(cx - r + 6, cy,      14, 18);
        p.setBrush(QColor("#70D070"));
        p.drawEllipse(cx + 6, cy - r + 10, 10, 12);
        // Cloud swirl
        p.setPen(QPen(Qt::white, 1.5f));
        p.setBrush(Qt::NoBrush);
        QPainterPath sw;
        sw.moveTo(cx - r + 10, cy - 4);
        sw.cubicTo(cx - 4, cy - r + 14, cx + 8, cy - 4, cx + r - 6, cy + 6);
        p.drawPath(sw);
        break;
    }
    // ── 3: Saturn ─────────────────────────────────────────────────
    case 3: {
        // Ring (drawn behind planet)
        p.setPen(QPen(QColor("#E8A060"), 3));
        p.setBrush(Qt::NoBrush);
        p.drawEllipse(cx - r - 2, cy - 8, (r + 2) * 2, 16);
        // Planet body
        p.setPen(QPen(QColor("#C06020"), 1.5));
        p.setBrush(QColor("#FFA040"));
        p.drawEllipse(cx - r + 6, cy - r + 8, (r - 6) * 2, (r - 8) * 2);
        // Stripes
        p.setPen(QPen(QColor("#E08030"), 1.5));
        int pb = cy - r + 8;
        int ph = (r - 8) * 2;
        p.drawArc(cx - r + 6, pb + ph/4,     (r-6)*2, ph/4, 0, 180*16);
        p.drawArc(cx - r + 6, pb + ph*2/4,   (r-6)*2, ph/4, 0, 180*16);
        break;
    }
    // ── 4: Rocket ─────────────────────────────────────────────────
    case 4: {
        // Body
        p.setPen(Qt::NoPen);
        p.setBrush(QColor("#E8E8F0"));
        p.drawRoundedRect(cx - 10, cy - r + 14, 20, 44, 5, 5);
        // Nose cone
        QPainterPath nose;
        nose.moveTo(cx - 10, cy - r + 14);
        nose.lineTo(cx, cy - r);
        nose.lineTo(cx + 10, cy - r + 14);
        p.setBrush(QColor("#E83030")); p.drawPath(nose);
        // Window
        p.setBrush(QColor("#90C8F8"));
        p.drawEllipse(cx - 7, cy - r + 20, 14, 14);
        p.setBrush(QColor(255,255,255,120));
        p.drawEllipse(cx - 5, cy - r + 22, 6, 6);
        // Fins
        QPainterPath fl;
        fl.moveTo(cx - 10, cy + 12); fl.lineTo(cx - 22, cy + 30); fl.lineTo(cx - 10, cy + 30);
        QPainterPath fr;
        fr.moveTo(cx + 10, cy + 12); fr.lineTo(cx + 22, cy + 30); fr.lineTo(cx + 10, cy + 30);
        p.setBrush(QColor("#E83030")); p.drawPath(fl); p.drawPath(fr);
        // Flames
        p.setBrush(QColor("#FF8C00")); p.drawEllipse(cx - 8, cy + 28, 16, 14);
        p.setBrush(QColor("#FFD700")); p.drawEllipse(cx - 5, cy + 31, 10, 9);
        break;
    }
    default: break;
    }

    return pm;
}

// ─────────────────────────────────────────────────────────────────────
// StickerCanvas
// ─────────────────────────────────────────────────────────────────────
StickerCanvas::StickerCanvas(QWidget *parent)
    : QWidget(parent)
    , m_dragIdx(-1)
{
    setMouseTracking(true);

    // Fixed canvas size: 500 x 640
    setFixedSize(500, 640);

    // Strip fills the canvas
    m_stripRect = QRect(0, 0, 500, 640);

    // Photo slots inside strip
    int slotW = 380, slotH = 210;
    int slotX = (500 - slotW) / 2;
    m_slot0Rect = QRect(slotX, 70,  slotW, slotH);
    m_slot1Rect = QRect(slotX, 310, slotW, slotH);
}

void StickerCanvas::setPhotos(const QVector<QPixmap> &photos)
{
    m_photos = photos;
    m_stickers.clear();
    update();
}

void StickerCanvas::addSticker(const QPixmap &px, int sz)
{
    StickerItem item;
    item.pixmap  = px;
    item.size    = sz;
    item.hovered = false;
    // Default: random position near center
    item.pos = QPointF(
        60 + (m_stickers.size() % 5) * 60,
        120 + (m_stickers.size() / 5) * 60
    );
    m_stickers.append(item);
    update();
}

void StickerCanvas::clearStickers()
{
    m_stickers.clear();
    update();
}

// ── Mouse events for drag-and-drop ───────────────────────────────────
void StickerCanvas::mousePressEvent(QMouseEvent *e)
{
    if (e->button() != Qt::LeftButton) return;
    const QPointF pt = e->position();

    // Check in reverse order (topmost first)
    for (int i = m_stickers.size() - 1; i >= 0; --i) {
        const auto &s = m_stickers[i];
        QRectF r(s.pos, QSizeF(s.size, s.size));
        if (r.contains(pt)) {
            m_dragIdx    = i;
            m_dragOffset = pt - s.pos;
            // Bring to top
            m_stickers.move(i, m_stickers.size() - 1);
            m_dragIdx = m_stickers.size() - 1;
            update();
            return;
        }
    }
    m_dragIdx = -1;
}

void StickerCanvas::mouseMoveEvent(QMouseEvent *e)
{
    const QPointF pt = e->position();

    if (m_dragIdx >= 0 && (e->buttons() & Qt::LeftButton)) {
        m_stickers[m_dragIdx].pos = pt - m_dragOffset;
        // Clamp inside canvas
        auto &pos = m_stickers[m_dragIdx].pos;
        int   sz  = m_stickers[m_dragIdx].size;
        pos.setX(qBound(0.0, pos.x(), double(width()  - sz)));
        pos.setY(qBound(0.0, pos.y(), double(height() - sz)));
        update();
        return;
    }

    // Hover detection
    bool needUpdate = false;
    for (auto &s : m_stickers) {
        bool now = QRectF(s.pos, QSizeF(s.size,s.size)).contains(pt);
        if (now != s.hovered) { s.hovered = now; needUpdate = true; }
    }
    setCursor((m_dragIdx >= 0) ? Qt::ClosedHandCursor :
              needUpdate       ? Qt::OpenHandCursor   : Qt::ArrowCursor);
    if (needUpdate) update();
}

void StickerCanvas::mouseReleaseEvent(QMouseEvent *)
{
    m_dragIdx = -1;
    setCursor(Qt::ArrowCursor);
    update();
}

// ── paintEvent ───────────────────────────────────────────────────────
void StickerCanvas::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::SmoothPixmapTransform);

    drawBackground(p);

    // Draw photos into slots
    const QRect photoSlots[2] = { m_slot0Rect, m_slot1Rect };
    for (int i = 0; i < 2 && i < m_photos.size(); ++i) {
        if (m_photos[i].isNull()) continue;
        QPixmap scaled = m_photos[i].scaled(
            photoSlots[i].size(), Qt::KeepAspectRatioByExpanding,
            Qt::SmoothTransformation);
        QRect src((scaled.width() - photoSlots[i].width()) / 2,
                  (scaled.height() - photoSlots[i].height()) / 2,
                  photoSlots[i].width(), photoSlots[i].height());
        p.save();
        p.setClipRect(photoSlots[i]);
        p.drawPixmap(photoSlots[i], scaled, src);
        p.restore();
    }


    // Draw stickers
    for (const auto &s : std::as_const(m_stickers)) {
        if (s.hovered)
            p.setOpacity(0.85);
        p.drawPixmap(QRectF(s.pos, QSizeF(s.size, s.size)).toRect(), s.pixmap);
        p.setOpacity(1.0);
    }
}

// ── drawBackground ───────────────────────────────────────────────────
void StickerCanvas::drawBackground(QPainter &p) const
{
    // Strip bg
    p.save();
    p.setPen(Qt::NoPen);
    p.setBrush(S_BG);
    p.drawRect(m_stripRect);

    // Stars / sparkles
    struct Sp { float x,y,sz; int c; };
    const Sp sps[] = {
        {0.05f,0.04f,7,0},{0.90f,0.03f,6,1},{0.50f,0.02f,5,2},
        {0.92f,0.15f,7,0},{0.03f,0.30f,6,2},{0.95f,0.50f,7,1},
        {0.04f,0.60f,6,0},{0.92f,0.72f,7,2},{0.08f,0.88f,8,1},
        {0.88f,0.90f,7,0},{0.45f,0.96f,5,2},{0.20f,0.07f,6,1},
        {0.70f,0.92f,6,0},
    };
    const QColor cols[3]={ S_STAR1, S_STAR2, S_STAR3 };
    for (const auto &s : sps)
        drawSparkle(p,
            { s.x * m_stripRect.width(), s.y * m_stripRect.height() },
            s.sz, cols[s.c]);

    // Title "SPACE ⊕ ADVENTURE"
    QFont tf("Arial", 18, QFont::Black);
    p.setFont(tf);
    p.setPen(Qt::white);
    for (int dx=-2;dx<=2;dx++) for(int dy=-2;dy<=2;dy++) if(dx||dy)
        p.drawText(QRect(0,4,500,54).translated(dx,dy),
                   Qt::AlignHCenter|Qt::AlignVCenter, "SPACE ⊕ ADVENTURE");
    p.setPen(S_TITLE);
    p.drawText(QRect(0,4,500,54), Qt::AlignHCenter|Qt::AlignVCenter,
               "SPACE ⊕ ADVENTURE");

    // Slot backgrounds
    p.setPen(Qt::NoPen);
    p.setBrush(S_SLOT);
    p.drawRoundedRect(m_slot0Rect, 8, 8);
    p.drawRoundedRect(m_slot1Rect, 8, 8);

    // Saturn (top-left)
    {
        int px=12, py=52;
        p.setBrush(QColor("#FFA040")); p.drawEllipse(px,py,34,34);
        p.setPen(QPen(QColor("#E08030"),2.5)); p.setBrush(Qt::NoBrush);
        p.drawEllipse(px-8,py+9,50,16);
        p.setPen(Qt::NoPen);
    }
    // Earth (top-right)
    {
        int px=454, py=48;
        p.setBrush(QColor("#4090D0")); p.drawEllipse(px,py,32,32);
        p.setBrush(QColor("#50B850"));
        p.drawEllipse(px+4,py+8,12,14);
        p.drawEllipse(px+14,py+2,10,12);
    }
    // Meteor (left-middle)
    {
        int px=8, py=290;
        p.setBrush(QColor("#CD853F")); p.drawEllipse(px,py,28,28);
        p.setBrush(QColor("#8B4513"));
        p.drawEllipse(px+6,py+5,8,6);
        p.drawEllipse(px+4,py+16,5,5);
    }
    // Rocket (right-bottom)
    {
        int rx=455, ry=530;
        p.setBrush(QColor("#E8E8F0")); p.drawRoundedRect(rx+4,ry+6,20,38,4,4);
        QPainterPath nose; nose.moveTo(rx+4,ry+6); nose.lineTo(rx+14,ry); nose.lineTo(rx+24,ry+6);
        p.setBrush(QColor("#E83030")); p.drawPath(nose);
        p.setBrush(QColor("#90C8F8")); p.drawEllipse(rx+6,ry+12,12,12);
        QPainterPath fl; fl.moveTo(rx+4,ry+28); fl.lineTo(rx-4,ry+44); fl.lineTo(rx+4,ry+44);
        QPainterPath fr; fr.moveTo(rx+24,ry+28); fr.lineTo(rx+32,ry+44); fr.lineTo(rx+24,ry+44);
        p.setBrush(QColor("#E83030")); p.drawPath(fl); p.drawPath(fr);
        p.setBrush(QColor("#FF8C00")); p.drawEllipse(rx+6,ry+42,12,10);
        p.setBrush(QColor("#FFD700")); p.drawEllipse(rx+8,ry+44,8,7);
    }
    // Wave at bottom
    {
        int wy = 592;
        QPainterPath wave;
        wave.moveTo(8, wy + 8);
        for (int i=0;i<5;i++){
            float x0=8+i*96.0f, x1=x0+48, x2=x0+96;
            wave.cubicTo(x0+18,wy-2, x1+6,wy+22, x2,wy+10);
        }
        QPainterPath wave2 = wave;
        wave2.translate(0,10);
        p.setPen(QPen(QColor("#A0C8F0"),2.5f,Qt::SolidLine,Qt::RoundCap));
        p.setBrush(Qt::NoBrush);
        p.drawPath(wave);
        p.drawPath(wave2);
    }

    p.restore();
}

void StickerCanvas::drawSparkle(QPainter &p, QPointF c, float sz, QColor col) const
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

// ── renderToPixmap: composites everything for export ─────────────────
QPixmap StickerCanvas::renderToPixmap() const
{
    QPixmap result(size());
    result.fill(Qt::transparent);
    QPainter p(&result);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::SmoothPixmapTransform);

    drawBackground(p);

    const QRect photoSlots[2] = { m_slot0Rect, m_slot1Rect };
    for (int i = 0; i < 2 && i < m_photos.size(); ++i) {
        if (m_photos[i].isNull()) continue;
        QPixmap scaled = m_photos[i].scaled(
            photoSlots[i].size(), Qt::KeepAspectRatioByExpanding,
            Qt::SmoothTransformation);
        QRect src((scaled.width() - photoSlots[i].width()) / 2,
                  (scaled.height() - photoSlots[i].height()) / 2,
                  photoSlots[i].width(), photoSlots[i].height());
        p.save();
        p.setClipRect(photoSlots[i]);
        p.drawPixmap(photoSlots[i], scaled, src);
        p.restore();
    }

    for (const auto &s : m_stickers)
        p.drawPixmap(QRectF(s.pos, QSizeF(s.size,s.size)).toRect(), s.pixmap);

    return result;
}

// ─────────────────────────────────────────────────────────────────────
// StickerPage
// ─────────────────────────────────────────────────────────────────────
StickerPage::StickerPage(QWidget *parent)
    : QWidget(parent)
{
    // Pre-generate sticker pixmaps
    for (int i = 0; i < 5; ++i)
        m_stickerPreviews.append(StickerCanvas::makeStickerPixmap(i, 64));

    setupUI();
}

void StickerPage::setPhotos(const QVector<QPixmap> &photos)
{
    m_canvas->setPhotos(photos);
}

void StickerPage::setupUI()
{
    setStyleSheet("background: white;");

    const int PAGE_W = 820;

    // ── Bootheksis header ──
    auto *headerLabel = new QLabel("Bootheksis", this);
    headerLabel->setAlignment(Qt::AlignHCenter);
    headerLabel->setGeometry(0, 10, PAGE_W, 28);
    headerLabel->setStyleSheet("color:#A0522D; font-weight:bold; font-size:14px;");

    // ── Hint label ──
    m_hintLabel = new QLabel("✨  Add Stickers!  (drag to reposition)", this);
    m_hintLabel->setAlignment(Qt::AlignHCenter);
    m_hintLabel->setGeometry(0, 38, PAGE_W, 24);
    m_hintLabel->setStyleSheet("color:#555; font-size:13px;");

    // ── Sticker buttons row ──
    const int btnSz   = 56;
    const int numBtns = 5 + 1; // 5 stickers + 1 clear
    int totalW = numBtns * btnSz + (numBtns - 1) * 10;
    int bx     = (PAGE_W - totalW) / 2;
    int by     = 66;

    const QString stickerNames[5] = { "Alien","Meteor","Earth","Saturn","Rocket" };
    for (int i = 0; i < 5; ++i) {
        auto *btn = new QPushButton(this);
        btn->setGeometry(bx + i * (btnSz + 10), by, btnSz, btnSz);
        btn->setIcon(QIcon(m_stickerPreviews[i]));
        btn->setIconSize(QSize(44, 44));
        btn->setToolTip(stickerNames[i]);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setStyleSheet(
            "QPushButton {"
            "  background: #F8F8E8;"
            "  border: 2px solid #C8C890;"
            "  border-radius: 28px;"
            "}"
            "QPushButton:hover  { background: #FFFFF0; border-color:#A8A860; }"
            "QPushButton:pressed{ background: #FFFFF8; }"
        );
        int idx = i;
        connect(btn, &QPushButton::clicked, this, [this,idx](){ onAddSticker(idx); });
    }

    // Reset / clear stickers button
    m_clearBtn = new QPushButton("↺", this);
    m_clearBtn->setGeometry(bx + 5 * (btnSz + 10), by, btnSz, btnSz);
    m_clearBtn->setToolTip("Clear all stickers");
    m_clearBtn->setCursor(Qt::PointingHandCursor);
    m_clearBtn->setStyleSheet(
        "QPushButton {"
        "  background: #F8F8E8;"
        "  border: 2px solid #C8C890;"
        "  border-radius: 28px;"
        "  font-size: 22px;"
        "}"
        "QPushButton:hover { background: #FFFFF0; }"
    );
    connect(m_clearBtn, &QPushButton::clicked,
            m_canvas, &StickerCanvas::clearStickers);
    // ── Canvas ──
    m_canvas = new StickerCanvas(this);
    int canvasX = (PAGE_W - m_canvas->width()) / 2;
    m_canvas->setGeometry(canvasX, by + btnSz + 14, m_canvas->width(), m_canvas->height());

    // Now connect clear button
    connect(m_clearBtn, &QPushButton::clicked,
            m_canvas, &StickerCanvas::clearStickers);

    // ── Bottom buttons ──
    int bottomY = m_canvas->y() + m_canvas->height() + 18;
    int btnW    = 160;
    int spacing = 16;
    int totalBW = 3 * btnW + 2 * spacing;
    int startX  = (PAGE_W - totalBW) / 2;

    auto mkBtn = [&](const QString &label, int x) {
        auto *b = new QPushButton(label, this);
        b->setGeometry(x, bottomY, btnW, 48);
        b->setCursor(Qt::PointingHandCursor);
        b->setStyleSheet(
            "QPushButton {"
            "  background: white;"
            "  border: 2px solid #AAAAAA;"
            "  border-radius: 16px;"
            "  font-size: 15px;"
            "  font-weight: bold;"
            "  color: #333;"
            "}"
            "QPushButton:hover  { background:#F5F5F5; border-color:#888; }"
            "QPushButton:pressed{ background:#EEEEEE; }"
        );
        return b;
    };

    m_downloadBtn = mkBtn("⬇  Download", startX);
    m_shareBtn    = mkBtn("🔗  Share",   startX + btnW + spacing);
    m_homeBtn     = mkBtn("🏠  Home",    startX + (btnW + spacing) * 2);

    connect(m_downloadBtn, &QPushButton::clicked, this, &StickerPage::onDownload);
    connect(m_shareBtn,    &QPushButton::clicked, this, &StickerPage::onShare);
    connect(m_homeBtn,     &QPushButton::clicked, this, &StickerPage::homeClicked);
}

void StickerPage::onAddSticker(int type)
{
    QPixmap px = StickerCanvas::makeStickerPixmap(type, 80);
    m_canvas->addSticker(px, 80);
}

void StickerPage::onDownload()
{
    const QString defaultName =
        QDir::homePath() + "/space_adventure_" +
        QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss") + ".png";

    QString path = QFileDialog::getSaveFileName(
        this, "Save Photo Strip", defaultName,
        "PNG Image (*.png);;JPEG Image (*.jpg)");

    if (path.isEmpty()) return;

    QPixmap pm = m_canvas->renderToPixmap();

    // Scale up 2× for better print quality
    QPixmap hd = pm.scaled(pm.size() * 2, Qt::KeepAspectRatio,
                            Qt::SmoothTransformation);

    if (hd.save(path)) {
        QMessageBox::information(this, "Saved!",
            QString("Photo strip saved to:\n%1").arg(path));
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
    } else {
        QMessageBox::warning(this, "Error", "Could not save the image.");
    }
}

void StickerPage::onShare()
{
    QPixmap pm = m_canvas->renderToPixmap();
    QGuiApplication::clipboard()->setPixmap(pm);
    QMessageBox::information(this, "Copied!",
        "Your photo strip has been copied to the clipboard!\n"
        "Paste it anywhere (Ctrl+V) to share.");
}
