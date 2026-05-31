#pragma once

#include <QWidget>
#include <QVector>
#include <QPixmap>
#include <QPointF>

class QPushButton;
class QLabel;

// ── One sticker placed on the canvas ────────────────────────────────
struct StickerItem {
    QPixmap pixmap;
    QPointF pos;    // top-left in canvas coordinates
    int     size;   // rendered size (square)
    bool    hovered;
};

// ── StickerCanvas: custom widget with drag-and-drop sticker support ──
class StickerCanvas : public QWidget
{
    Q_OBJECT
public:
    static QPixmap makeStickerPixmap(int type, int size);
    explicit StickerCanvas(QWidget *parent = nullptr);

    void setPhotos(const QVector<QPixmap> &photos);
    void addSticker(const QPixmap &px, int sz = 72);
    void clearStickers();

    /** Render the strip (background + photos + stickers) to an off-screen pixmap */
    QPixmap renderToPixmap() const;

protected:
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;

private:
    void drawBackground(QPainter &p) const;
    void drawSparkle(QPainter &p, QPointF c, float sz, QColor col) const;   // type 0-4

    // Layout (fixed inside the canvas widget)
    QRect m_stripRect;
    QRect m_slot0Rect;
    QRect m_slot1Rect;

    QVector<QPixmap>    m_photos;
    QVector<StickerItem> m_stickers;

    int    m_dragIdx;      // -1 = none
    QPointF m_dragOffset;
};

// ── StickerPage: wraps StickerCanvas + UI controls ───────────────────
class StickerPage : public QWidget
{
    Q_OBJECT
public:
    explicit StickerPage(QWidget *parent = nullptr);
    void setPhotos(const QVector<QPixmap> &photos);

signals:
    void homeClicked();

private slots:
    void onAddSticker(int type);
    void onDownload();
    void onShare();

private:
    void setupUI();

    StickerCanvas *m_canvas;

    // Sticker type pixmaps (pre-generated)
    QVector<QPixmap> m_stickerPreviews;

    QPushButton *m_downloadBtn;
    QPushButton *m_homeBtn;
    QPushButton *m_shareBtn;
    QPushButton *m_clearBtn;
    QLabel      *m_hintLabel;
};
