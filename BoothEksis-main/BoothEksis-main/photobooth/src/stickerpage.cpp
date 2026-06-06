#include "stickerpage.h"

#include <QWidget>
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
#include <QPrinter>
#include <QPrintDialog>
#include <QStyle>
#include <QApplication>

static const QColor S_BG      { "#A89BC8" };
static const QColor S_TITLE   { "#FF69B4" };
static const QColor S_SLOT    { "#FAFAC8" };
static const QColor S_STAR1   { "#FFD700" };
static const QColor S_STAR2   { "#FFB6C1" };
static const QColor S_STAR3   { "#B8FFD0" };

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
    case 0: { // Alien
        p.setPen(QPen(QColor("#3A8A3A"), 1.5));
        p.setBrush(QColor("#90EE90"));
        p.drawEllipse(cx - r, cy - r + 10, r * 2, r * 2 - 5);
        p.setPen(QPen(QColor("#3A8A3A"), 2));
        p.drawLine(cx - 8, cy - r + 12, cx - 14, cy - r - 6);
        p.drawLine(cx + 8, cy - r + 12, cx + 14, cy - r - 6);
        p.setBrush(QColor("#90EE90")); p.setPen(Qt::NoPen);
        p.drawEllipse(cx - 17, cy - r - 10, 7, 7);
        p.drawEllipse(cx + 10, cy - r - 10, 7, 7);
        p.setBrush(Qt::white); p.setPen(QPen(Qt::black, 1));
        p.drawEllipse(cx - 13, cy - r + 20, 10, 14);
        p.drawEllipse(cx + 3,  cy - r + 20, 10, 14);
        p.setBrush(Qt::black); p.setPen(Qt::NoPen);
        p.drawEllipse(cx - 11, cy - r + 23, 6, 8);
        p.drawEllipse(cx + 5,  cy - r + 23, 6, 8);
        p.setPen(QPen(QColor("#3A8A3A"), 2));
        p.drawArc(cx - 8, cy + 5, 16, 9, 0, -180 * 16);
        break;
    }
    case 1: { // Meteor
        QPainterPath path;
        path.moveTo(cx, cy - r);
        path.quadTo(cx + r - 3, cy - r + 6, cx + r, cy + 2);
        path.quadTo(cx + r + 2, cy + r - 4, cx + 2, cy + r);
        path.quadTo(cx - r + 2, cy + r + 2, cx - r, cy + 2);
        path.quadTo(cx - r - 2, cy - r + 4, cx, cy - r);
        p.setPen(QPen(QColor("#7B3513"), 1.5));
        p.setBrush(QColor("#CD853F"));
        p.drawPath(path);
        p.setPen(Qt::NoPen);
        p.setBrush(QColor("#8B4513"));
        p.drawEllipse(cx + 4, cy - 6, 9, 7);
        p.drawEllipse(cx - 10, cy + 7, 7, 5);
        p.drawEllipse(cx - 3, cy - 10, 6, 5);
        break;
    }
    case 2: { // Earth
        p.setPen(QPen(QColor("#1050A0"), 1.5));
        p.setBrush(QColor("#4090D0"));
        p.drawEllipse(cx - r, cy - r, r * 2, r * 2);
        p.setPen(Qt::NoPen);
        p.setBrush(QColor("#50B850"));
        p.drawEllipse(cx - 4, cy - r + 6, 16, 20);
        p.drawEllipse(cx - r + 6, cy,      14, 18);
        p.setBrush(QColor("#70D070"));
        p.drawEllipse(cx + 6, cy - r + 10, 10, 12);
        p.setPen(QPen(Qt::white, 1.5f));
        p.setBrush(Qt::NoBrush);
        QPainterPath sw;
        sw.moveTo(cx - r + 10, cy - 4);
        sw.cubicTo(cx - 4, cy - r + 14, cx + 8, cy - 4, cx + r - 6, cy + 6);
        p.drawPath(sw);
        break;
    }
    case 3: { // Saturn
        p.setPen(QPen(QColor("#E8A060"), 3));
        p.setBrush(Qt::NoBrush);
        p.drawEllipse(cx - r - 2, cy - 8, (r + 2) * 2, 16);
        p.setPen(QPen(QColor("#C06020"), 1.5));
        p.setBrush(QColor("#FFA040"));
        p.drawEllipse(cx - r + 6, cy - r + 8, (r - 6) * 2, (r - 8) * 2);
        p.setPen(QPen(QColor("#E08030"), 1.5));
        int pb = cy - r + 8;
        int ph = (r - 8) * 2;
        p.drawArc(cx - r + 6, pb + ph/4,     (r-6)*2, ph/4, 0, 180*16);
        p.drawArc(cx - r + 6, pb + ph*2/4,   (r-6)*2, ph/4, 0, 180*16);
        break;
    }
    case 4: { // Rocket
        p.setPen(Qt::NoPen);
        p.setBrush(QColor("#E8E8F0"));
        p.drawRoundedRect(cx - 10, cy - r + 14, 20, 44, 5, 5);
        QPainterPath nose;
        nose.moveTo(cx - 10, cy - r + 14);
        nose.lineTo(cx, cy - r);
        nose.lineTo(cx + 10, cy - r + 14);
        p.setBrush(QColor("#E83030")); p.drawPath(nose);
        p.setBrush(QColor("#90C8F8"));
        p.drawEllipse(cx - 7, cy - r + 20, 14, 14);
        p.setBrush(QColor(255,255,255,120));
        p.drawEllipse(cx - 5, cy - r + 22, 6, 6);
        QPainterPath fl;
        fl.moveTo(cx - 10, cy + 12); fl.lineTo(cx - 22, cy + 30); fl.lineTo(cx - 10, cy + 30);
        QPainterPath fr;
        fr.moveTo(cx + 10, cy + 12); fr.lineTo(cx + 22, cy + 30); fr.lineTo(cx + 10, cy + 30);
        p.setBrush(QColor("#E83030")); p.drawPath(fl); p.drawPath(fr);
        p.setBrush(QColor("#FF8C00")); p.drawEllipse(cx - 8, cy + 28, 16, 14);
        p.setBrush(QColor("#FFD700")); p.drawEllipse(cx - 5, cy + 31, 10, 9);
        break;
    }
    default: break;
    }

    return pm;
}

StickerCanvas::StickerCanvas(QWidget *parent)
    : QWidget(parent)
    , m_dragIdx(-1)
    , m_selectedIdx(-1)
    , m_dragMode(None)
    , m_resizeStartSize(80)
    , m_resizeStartDist(1)
{
    setMouseTracking(true);
    setFixedSize(500, 640);
    m_stripRect = QRect(0, 0, 500, 640);

    int slotW = 380, slotH = 210;
    int slotX = (500 - slotW) / 2;
    m_slot0Rect = QRect(slotX, 70,  slotW, slotH);
    m_slot1Rect = QRect(slotX, 310, slotW, slotH);
}

void StickerCanvas::setPhotos(const QVector<QPixmap> &photos)
{
    m_photos = photos;
    m_stickers.clear();

    const int canvasW   = 500;
    const int canvasH   = 620;
    const int titleH    = 58;
    const int padBottom = 52;
    const int gap       = 14;

    int availH   = canvasH - titleH - padBottom - gap;
    int slotMaxH = availH / 2;
    int slotMaxW = canvasW - 120;

    QSize s0 = (!photos.isEmpty() && !photos[0].isNull()) ? photos[0].size() : QSize(380, 210);
    QSize s1 = (photos.size() > 1 && !photos[1].isNull()) ? photos[1].size() : QSize(380, 210);

    QSize slot0 = s0.scaled(slotMaxW, slotMaxH, Qt::KeepAspectRatio);
    QSize slot1 = s1.scaled(slotMaxW, slotMaxH, Qt::KeepAspectRatio);

    int x0 = (canvasW - slot0.width()) / 2;
    m_slot0Rect = QRect(x0, titleH, slot0.width(), slot0.height());

    int x1   = (canvasW - slot1.width()) / 2;
    int top1 = m_slot0Rect.bottom() + gap;
    m_slot1Rect = QRect(x1, top1, slot1.width(), slot1.height());

    m_stripRect = QRect(0, 0, canvasW, canvasH);
    setFixedSize(canvasW, canvasH);
    update();
}

void StickerCanvas::addSticker(const QPixmap &px, int sz)
{
    StickerItem item;
    item.pixmap    = px;
    item.size      = sz;
    item.rotation  = 0.0;
    item.hovered   = false;
    item.selected  = false;
    item.pos = QPointF(
        60 + (m_stickers.size() % 5) * 60,
        120 + (m_stickers.size() / 5) * 60
    );
    m_stickers.append(item);

    if (m_selectedIdx >= 0 && m_selectedIdx < m_stickers.size() - 1)
        m_stickers[m_selectedIdx].selected = false;
    m_selectedIdx = m_stickers.size() - 1;
    m_stickers[m_selectedIdx].selected = true;
    update();
}

void StickerCanvas::clearStickers()
{
    m_stickers.clear();
    update();
}

QPointF StickerCanvas::toLocalSpace(int idx, QPointF pt) const
{
    const StickerItem &s = m_stickers[idx];
    QPointF center = s.pos + QPointF(s.size / 2.0, s.size / 2.0);
    QTransform t;
    t.translate(center.x(), center.y());
    t.rotate(-s.rotation);
    t.translate(-center.x(), -center.y());
    return t.map(pt);
}

bool StickerCanvas::hitResizeHandle(int idx, QPointF pt) const
{
    QPointF local  = toLocalSpace(idx, pt);
    const StickerItem &s = m_stickers[idx];
    QPointF handle = s.pos + QPointF(s.size, s.size);
    return QLineF(handle, local).length() < 13.0;
}

bool StickerCanvas::hitRotateHandle(int idx, QPointF pt) const
{
    QPointF local  = toLocalSpace(idx, pt);
    const StickerItem &s = m_stickers[idx];
    QPointF handle = s.pos + QPointF(s.size / 2.0, -20.0);
    return QLineF(handle, local).length() < 13.0;
}

void StickerCanvas::mousePressEvent(QMouseEvent *e)
{
    if (e->button() != Qt::LeftButton) return;
    const QPointF pt = e->position();

    if (m_selectedIdx >= 0 && m_selectedIdx < m_stickers.size()) {
        if (hitResizeHandle(m_selectedIdx, pt)) {
            m_dragIdx  = m_selectedIdx;
            m_dragMode = Resizing;
            StickerItem &s = m_stickers[m_selectedIdx];
            QPointF center = s.pos + QPointF(s.size / 2.0, s.size / 2.0);
            m_resizeStartSize = s.size;
            m_resizeStartDist = QLineF(center, pt).length();
            if (m_resizeStartDist < 1.0) m_resizeStartDist = 1.0;
            update();
            return;
        }
        if (hitRotateHandle(m_selectedIdx, pt)) {
            m_dragIdx  = m_selectedIdx;
            m_dragMode = Rotating;
            update();
            return;
        }
    }

    for (int i = m_stickers.size() - 1; i >= 0; --i) {
        QPointF local = toLocalSpace(i, pt);
        const StickerItem &s = m_stickers[i];
        if (QRectF(s.pos, QSizeF(s.size, s.size)).contains(local)) {
            if (m_selectedIdx >= 0 && m_selectedIdx < m_stickers.size())
                m_stickers[m_selectedIdx].selected = false;

            m_dragOffset = pt - s.pos;
            m_stickers.move(i, m_stickers.size() - 1);
            m_dragIdx     = m_stickers.size() - 1;
            m_dragMode    = Moving;
            m_selectedIdx = m_dragIdx;
            m_stickers[m_selectedIdx].selected = true;
            update();
            return;
        }
    }

    if (m_selectedIdx >= 0 && m_selectedIdx < m_stickers.size())
        m_stickers[m_selectedIdx].selected = false;
    m_selectedIdx = -1;
    m_dragIdx     = -1;
    m_dragMode    = None;
    update();
}

void StickerCanvas::mouseMoveEvent(QMouseEvent *e)
{
    const QPointF pt = e->position();

    if (m_dragIdx >= 0 && (e->buttons() & Qt::LeftButton)) {
        StickerItem &s = m_stickers[m_dragIdx];
        QPointF center = s.pos + QPointF(s.size / 2.0, s.size / 2.0);

        if (m_dragMode == Moving) {
            s.pos = pt - m_dragOffset;
            s.pos.setX(qBound(0.0, s.pos.x(), double(width()  - s.size)));
            s.pos.setY(qBound(0.0, s.pos.y(), double(height() - s.size)));
        }
        else if (m_dragMode == Resizing) {
            qreal dist    = QLineF(center, pt).length();
            qreal newSize = m_resizeStartSize * dist / m_resizeStartDist;
            s.size = (int)qBound(24.0, newSize, 260.0);
        }
        else if (m_dragMode == Rotating) {
            qreal angle = qRadiansToDegrees(atan2(pt.y() - center.y(), pt.x() - center.x())) + 90.0;
            s.rotation = angle;
        }
        update();
        return;
    }

    bool needUpdate = false;
    for (int i = 0; i < m_stickers.size(); ++i) {
        QPointF local = toLocalSpace(i, pt);
        bool now = QRectF(m_stickers[i].pos, QSizeF(m_stickers[i].size, m_stickers[i].size)).contains(local);
        if (now != m_stickers[i].hovered) { m_stickers[i].hovered = now; needUpdate = true; }
    }

    Qt::CursorShape cursor = Qt::ArrowCursor;
    if (m_selectedIdx >= 0 && m_selectedIdx < m_stickers.size()) {
        if      (hitResizeHandle(m_selectedIdx, pt)) cursor = Qt::SizeFDiagCursor;
        else if (hitRotateHandle(m_selectedIdx, pt)) cursor = Qt::CrossCursor;
        else if (m_stickers[m_selectedIdx].hovered)  cursor = Qt::OpenHandCursor;
    } else {
        for (const auto &s : m_stickers)
            if (s.hovered) { cursor = Qt::OpenHandCursor; break; }
    }
    setCursor(cursor);
    if (needUpdate) update();
}

void StickerCanvas::mouseReleaseEvent(QMouseEvent *)
{
    m_dragIdx  = -1;
    m_dragMode = None;
    setCursor(Qt::ArrowCursor);
    update();
}

void StickerCanvas::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::SmoothPixmapTransform);

    drawBackground(p);

    const QRect photoSlots[2] = { m_slot0Rect, m_slot1Rect };
    for (int i = 0; i < 2 && i < m_photos.size(); ++i) {
        if (m_photos[i].isNull()) continue;
        QPixmap scaled = m_photos[i].scaled(photoSlots[i].size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        QRect src((scaled.width() - photoSlots[i].width()) / 2,
                  (scaled.height() - photoSlots[i].height()) / 2,
                  photoSlots[i].width(), photoSlots[i].height());
        p.save();
        p.setClipRect(photoSlots[i]);
        p.drawPixmap(photoSlots[i], scaled, src);
        p.restore();
    }

    for (int i = 0; i < m_stickers.size(); ++i) {
        const StickerItem &s = m_stickers[i];
        QPointF center = s.pos + QPointF(s.size / 2.0, s.size / 2.0);

        p.save();
        p.translate(center);
        p.rotate(s.rotation);
        p.translate(-center);

        if (s.hovered && i != m_selectedIdx) p.setOpacity(0.80);
        p.drawPixmap(QRectF(s.pos, QSizeF(s.size, s.size)).toRect(), s.pixmap);
        p.setOpacity(1.0);

        if (i == m_selectedIdx) {
            p.setPen(QPen(Qt::white, 1.5, Qt::DashLine));
            p.setBrush(Qt::NoBrush);
            p.drawRect(QRectF(s.pos, QSizeF(s.size, s.size)));

            QPointF rh = s.pos + QPointF(s.size, s.size);
            p.setPen(QPen(QColor("#6633FF"), 2));
            p.setBrush(Qt::white);
            p.drawEllipse(rh, 9, 9);
            p.setPen(QPen(QColor("#6633FF"), 1.5));
            p.drawLine(rh + QPointF(-3, -3), rh + QPointF(3, 3));
            p.drawLine(rh + QPointF(0, 3),   rh + QPointF(3, 3));
            p.drawLine(rh + QPointF(3, 0),   rh + QPointF(3, 3));

            QPointF topCenter = s.pos + QPointF(s.size / 2.0, 0);
            QPointF roth      = s.pos + QPointF(s.size / 2.0, -22.0);
            p.setPen(QPen(Qt::white, 1.5));
            p.drawLine(topCenter, roth);
            p.setPen(QPen(QColor("#FF6633"), 2));
            p.setBrush(QColor("#FF6633"));
            p.drawEllipse(roth, 9, 9);
            p.setPen(QPen(Qt::white, 1.5));
            p.setBrush(Qt::NoBrush);
            p.drawArc(QRectF(roth + QPointF(-5, -5), QSizeF(10, 10)), 40*16, 280*16);
        }

        p.restore();
    }
}

void StickerCanvas::drawBackground(QPainter &p) const
{
    p.save();
    p.setPen(Qt::NoPen);
    p.setBrush(S_BG);
    p.drawRect(m_stripRect);

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
        drawSparkle(p, { s.x * m_stripRect.width(), s.y * m_stripRect.height() }, s.sz, cols[s.c]);

    QFont tf("Arial", 18, QFont::Black);
    p.setFont(tf);
    p.setPen(Qt::white);
    for (int dx=-2;dx<=2;dx++) for(int dy=-2;dy<=2;dy++) if(dx||dy)
        p.drawText(QRect(0,4,500,54).translated(dx,dy), Qt::AlignHCenter|Qt::AlignVCenter, "SPACE ⊕ ADVENTURE");
    p.setPen(S_TITLE);
    p.drawText(QRect(0,4,500,54), Qt::AlignHCenter|Qt::AlignVCenter, "SPACE ⊕ ADVENTURE");

    p.setPen(Qt::NoPen);
    p.setBrush(S_SLOT);
    p.drawRoundedRect(m_slot0Rect, 8, 8);
    p.drawRoundedRect(m_slot1Rect, 8, 8);

    // Saturn
    {
        int px=12, py=52;
        p.setBrush(QColor("#FFA040")); p.drawEllipse(px,py,34,34);
        p.setPen(QPen(QColor("#E08030"),2.5)); p.setBrush(Qt::NoBrush);
        p.drawEllipse(px-8,py+9,50,16);
        p.setPen(Qt::NoPen);
    }
    // Earth
    {
        int px=454, py=48;
        p.setBrush(QColor("#4090D0")); p.drawEllipse(px,py,32,32);
        p.setBrush(QColor("#50B850"));
        p.drawEllipse(px+4,py+8,12,14);
        p.drawEllipse(px+14,py+2,10,12);
    }
    // Meteor
    {
        int px=8, py=290;
        p.setBrush(QColor("#CD853F")); p.drawEllipse(px,py,28,28);
        p.setBrush(QColor("#8B4513"));
        p.drawEllipse(px+6,py+5,8,6);
        p.drawEllipse(px+4,py+16,5,5);
    }
    // Rocket
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
    // Wave
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
        QPixmap scaled = m_photos[i].scaled(photoSlots[i].size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        QRect src((scaled.width() - photoSlots[i].width()) / 2,
                  (scaled.height() - photoSlots[i].height()) / 2,
                  photoSlots[i].width(), photoSlots[i].height());
        p.save();
        p.setClipRect(photoSlots[i]);
        p.drawPixmap(photoSlots[i], scaled, src);
        p.restore();
    }

    for (const auto &s : m_stickers) {
        QPointF center = s.pos + QPointF(s.size / 2.0, s.size / 2.0);
        p.save();
        p.translate(center);
        p.rotate(s.rotation);
        p.translate(-center);
        p.drawPixmap(QRectF(s.pos, QSizeF(s.size, s.size)).toRect(), s.pixmap);
        p.restore();
    }

    return result;
}

StickerPage::StickerPage(QWidget *parent)
    : QWidget(parent)
{
    for (int i = 0; i < 5; ++i)
        m_stickerPreviews.append(StickerCanvas::makeStickerPixmap(i, 64));

    setupUI();
}

void StickerPage::setPhotos(const QVector<QPixmap> &photos)
{
    m_canvas->setPhotos(photos);

    const int PAGE_W = 820;
    int canvasX = (PAGE_W - m_canvas->width()) / 2;
    m_canvas->move(canvasX, m_canvas->y());

    repositionBottomButtons();
}

void StickerPage::repositionBottomButtons()
{
    const int PAGE_W  = 820;
    const int btnW    = 140;
    const int spacing = 12;

    int totalBW = 4 * btnW + 3 * spacing;
    int startX  = (PAGE_W - totalBW) / 2;
    int btnY    = m_canvas->y() + m_canvas->height() + 20;

    m_downloadBtn->setGeometry(startX, btnY, btnW, 48);
    m_shareBtn   ->setGeometry(startX + (btnW + spacing), btnY, btnW, 48);
    m_printBtn   ->setGeometry(startX + (btnW + spacing) * 2, btnY, btnW, 48);
    m_homeBtn    ->setGeometry(startX + (btnW + spacing) * 3, btnY, btnW, 48);

    int contentH = btnY + 48 + 25;
    m_scrollContent->setFixedSize(PAGE_W, contentH);
}

void StickerPage::setupUI()
{
    setStyleSheet("background: white;");
    const int PAGE_W = 820;

    auto *headerLabel = new QLabel("Bootheksis", this);
    headerLabel->setAlignment(Qt::AlignHCenter);
    headerLabel->setGeometry(0, 10, PAGE_W, 28);
    headerLabel->setStyleSheet("color:#A0522D; font-weight:bold; font-size:14px;");

    const int btnSz   = 56;
    const int numBtns = 6;
    int totalW = numBtns * btnSz + (numBtns - 1) * 10;
    int bx     = (PAGE_W - totalW) / 2;
    int by     = 42;

    m_hintLabel = new QLabel("✨  Add Stickers! (Drag & Drop Sticker)", this);
    m_hintLabel->setAlignment(Qt::AlignHCenter);
    m_hintLabel->setGeometry(0, by, PAGE_W, 24);
    m_hintLabel->setStyleSheet("color:#555; font-size:13px;");
    by += 28;

    const QString stickerNames[5] = {"Alien","Meteor","Earth","Saturn","Rocket"};
    for (int i = 0; i < 5; ++i) {
        auto *btn = new QPushButton(this);
        btn->setGeometry(bx + i * (btnSz + 10), by, btnSz, btnSz);
        btn->setIcon(QIcon(m_stickerPreviews[i]));
        btn->setIconSize(QSize(44, 44));
        btn->setToolTip(stickerNames[i]);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setStyleSheet(
            "QPushButton {"
            "  background:#F8F8E8; border:2px solid #C8C890;"
            "  border-radius:28px;}"
            "QPushButton:hover{background:#FFFFF0;border-color:#A8A860;}"
            "QPushButton:pressed{background:#FFFFF8;}");
        int idx = i;
        connect(btn, &QPushButton::clicked, this, [this,idx](){ onAddSticker(idx); });
    }

    m_clearBtn = new QPushButton("↺", this);
    m_clearBtn->setGeometry(bx + 5*(btnSz+10), by, btnSz, btnSz);
    m_clearBtn->setToolTip("Clear all stickers");
    m_clearBtn->setCursor(Qt::PointingHandCursor);
    m_clearBtn->setStyleSheet(
        "QPushButton {"
        "  background:#F8F8E8; border:2px solid #C8C890;"
        "  border-radius:28px; font-size:22px;}"
        "QPushButton:hover{background:#FFFFF0;}");

    int stickerRowBottom = by + btnSz + 10;

    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setFrameShape(QFrame::NoFrame);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setWidgetResizable(false);
    m_scrollArea->setStyleSheet("background:white;");
    m_scrollArea->setGeometry(0, stickerRowBottom, PAGE_W, height() - stickerRowBottom);

    m_scrollContent = new QWidget;
    m_scrollContent->setStyleSheet("background:white;");

    m_canvas = new StickerCanvas(m_scrollContent);
    int canvasX = (PAGE_W - m_canvas->width()) / 2;
    m_canvas->move(canvasX, 10);

    auto mkBtn = [&](const QString &label, const QString &customStyle) {
        auto *b = new QPushButton(label, m_scrollContent);
        b->setCursor(Qt::PointingHandCursor);
        if (customStyle.isEmpty()) {
            b->setStyleSheet(
                "QPushButton {"
                "  background:white; border:2px solid #AAAAAA;"
                "  border-radius:16px; font-size:14px;"
                "  font-weight:bold; color:#333;}"
                "QPushButton:hover{background:#F5F5F5;border-color:#888;}"
                "QPushButton:pressed{background:#EEEEEE;}");
        } else {
            b->setStyleSheet(customStyle);
        }
        return b;
    };

    QString printStyle =
        "QPushButton {"
        "  background-color: #C1F0D1;"
        "  border: 2px solid #2D5A27;"
        "  border-radius: 16px;"
        "  color: #1B331A;"
        "  font-size: 14px;"
        "  font-weight: bold;"
        "}"
        "QPushButton:hover { background-color: #A8E6B9; }"
        "QPushButton:pressed { background-color: #92D5A4; }";

    m_downloadBtn = mkBtn("⬇  Download", "");
    m_shareBtn    = mkBtn("🔗  Share", "");
    m_printBtn    = mkBtn("🖨  PRINT", printStyle);
    m_printBtn->setIcon(style()->standardIcon(QStyle::SP_ComputerIcon));
    m_printBtn->setIconSize(QSize(20, 20));
    m_homeBtn     = mkBtn("🏠  Home", "");

    repositionBottomButtons();
    m_scrollArea->setWidget(m_scrollContent);

    connect(m_clearBtn,    &QPushButton::clicked, m_canvas,      &StickerCanvas::clearStickers);
    connect(m_downloadBtn, &QPushButton::clicked, this,          &StickerPage::onDownload);
    connect(m_shareBtn,    &QPushButton::clicked, this,          &StickerPage::onShare);
    connect(m_printBtn,    &QPushButton::clicked, this,          &StickerPage::onPrint);
    connect(m_homeBtn,     &QPushButton::clicked, this,          &StickerPage::homeClicked);
}

void StickerPage::onAddSticker(int type)
{
    QPixmap px = StickerCanvas::makeStickerPixmap(type, 80);
    m_canvas->addSticker(px, 80);
}

void StickerPage::onDownload()
{
    const QString defaultName = QDir::homePath() + "/space_adventure_" + QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss") + ".png";
    QString path = QFileDialog::getSaveFileName(this, "Save Photo Strip", defaultName, "PNG Image (*.png);;JPEG Image (*.jpg)");
    if (path.isEmpty()) return;

    QPixmap pm = m_canvas->renderToPixmap();
    QPixmap hd = pm.scaled(pm.size() * 2, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    if (hd.save(path)) {
        QMessageBox::information(this, "Saved!", QString("Photo strip saved to:\n%1").arg(path));
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
    } else {
        QMessageBox::warning(this, "Error", "Could not save the image.");
    }
}

void StickerPage::onShare()
{
    QPixmap pm = m_canvas->renderToPixmap();
    QGuiApplication::clipboard()->setPixmap(pm);
    QMessageBox::information(this, "Copied!", "Your photo strip has been copied to the clipboard!\nPaste it anywhere (Ctrl+V) to share.");
}

void StickerPage::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);
    if (m_scrollArea) {
        int stickerRowBottom = m_hintLabel->y() + 28 + 56 + 10;
        m_scrollArea->setGeometry(0, stickerRowBottom, width(), height() - stickerRowBottom);
    }
}

void StickerPage::onPrint()
{
    QPixmap pm = m_canvas->renderToPixmap();
    if (pm.isNull()) {
        QMessageBox::warning(this, "Error", "Tidak ada gambar untuk dicetak.");
        return;
    }

    QPrinter printer(QPrinter::HighResolution);
    printer.setPageOrientation(QPageLayout::Portrait);
    printer.setDocName("BoothEksis_PhotoStrip");

    QPrintDialog dialog(&printer, this);
    dialog.setWindowTitle("Cetak Photo Strip");

    if (dialog.exec() == QDialog::Accepted) {
        QPainter painter(&printer);
        if (!painter.isActive()) {
            QMessageBox::warning(this, "Error", "Gagal memulai proses pencetakan.");
            return;
        }

        QRect pageRect = printer.pageRect(QPrinter::DevicePixel).toRect();
        QPixmap scaledPm = pm.scaled(pageRect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

        int x = pageRect.left() + (pageRect.width() - scaledPm.width()) / 2;
        int y = pageRect.top() + (pageRect.height() - scaledPm.height()) / 2;

        painter.drawPixmap(x, y, scaledPm);
        painter.end();

        QMessageBox::information(this, "Sukses", "Dokumen berhasil dikirim ke printer!");
    }
}
