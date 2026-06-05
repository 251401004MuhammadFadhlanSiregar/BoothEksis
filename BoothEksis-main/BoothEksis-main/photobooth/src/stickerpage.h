#pragma once

#include <QWidget>
#include <QVector>
#include <QPixmap>
#include <QPointF>
#include <QScrollArea>

class QPushButton;
class QLabel;

// ── One sticker placed on the canvas ────────────────────────────────
struct StickerItem {
    QPixmap pixmap;
    QPointF pos;      // top-left in canvas coordinates
    int     size;     // rendered size (square)
    qreal   rotation; // rotation in degrees (0 = upright)
    bool    hovered;
    bool    selected;
};

// ── StickerCanvas: custom widget with drag-and-drop sticker support ──
class StickerCanvas : public QWidget
{
    Q_OBJECT
public:
    // Mode drag yang sedang aktif
    enum DragMode { None, Moving, Resizing, Rotating };
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

    int      m_dragIdx;        // -1 = none
    int      m_selectedIdx;    // sticker yang sedang dipilih (tampilkan handle)
    DragMode m_dragMode;       // mode operasi saat ini

    QPointF  m_dragOffset;
    qreal    m_resizeStartSize; // ukuran awal saat mulai resize
    qreal    m_resizeStartDist; // jarak mouse ke center saat mulai resize

    // Helper functions
    QPointF toLocalSpace(int idx, QPointF pt) const;
    bool    hitResizeHandle(int idx, QPointF pt) const;
    bool    hitRotateHandle(int idx, QPointF pt) const;
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

protected:
    void resizeEvent(QResizeEvent *e) override;

private slots:
    void onAddSticker(int type);
    void onDownload();
    void onShare();
    void onPrint();

private:
    void setupUI();
    void repositionBottomButtons();

    StickerCanvas *m_canvas;

    // Sticker type pixmaps (pre-generated)
    QVector<QPixmap> m_stickerPreviews;

    QPushButton *m_downloadBtn;
    QPushButton *m_homeBtn;
    QPushButton *m_shareBtn;
    QPushButton *m_printBtn;
    QPushButton *m_clearBtn;
    QLabel      *m_hintLabel;
    QScrollArea *m_scrollArea;
    QWidget     *m_scrollContent;
};
