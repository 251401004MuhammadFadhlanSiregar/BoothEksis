#include "menupage.h"

#include <QPainter>
#include <QPainterPath>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFont>
#include <QMouseEvent>

// ── colour palette ──────────────────────────────────────────────────
static const QColor C_BG        { "#9B8EC4" };   // medium purple
static const QColor C_INNER     { "#B5ACD8" };   // lighter inner bg
static const QColor C_BUTTON    { "#FAFAC8" };   // butter yellow
static const QColor C_TITLE     { "#FF69B4" };   // hot pink
static const QColor C_TITLE_OUT { "#FFFFFF" };
static const QColor C_STAR1     { "#FFD700" };   // gold
static const QColor C_STAR2     { "#FFB6C1" };   // light pink
static const QColor C_STAR3     { "#B8FFD0" };   // mint green
static const QColor C_BROWN     { "#A0522D" };   // bootheksis text

void MenuPage::drawSparkle(QPainter &p, QPointF c, float sz, QColor col)
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

void MenuPage::drawBootheksisLogo(QPainter &p, QRect area)
{
    p.save();
    p.setRenderHint(QPainter::Antialiasing);

    int cx = area.center().x();
    int cy = area.center().y();
    int r  = area.height() / 2;

    // Helmet (white circle)
    p.setPen(QPen(QColor("#CCCCCC"), 2));
    p.setBrush(QColor("#F5F5F5"));
    p.drawEllipse(cx - r, cy - r, r * 2, r * 2);

    // Visor highlight
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(255, 255, 255, 90));
    p.drawEllipse(cx - r + 6, cy - r + 4, r, r - 4);

    // Cat face
    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#FFFFFF"));
    p.drawEllipse(cx - r + 8, cy - r + 12, (r - 8) * 2, (r - 8) * 2);

    // Eyes
    p.setBrush(QColor("#333333"));
    p.drawEllipse(cx - 8, cy - 4, 6, 7);
    p.drawEllipse(cx + 2, cy - 4, 6, 7);

    // Blush
    p.setBrush(QColor(255, 182, 193, 180));
    p.drawEllipse(cx - 14, cy + 4, 8, 5);
    p.drawEllipse(cx + 6,  cy + 4, 8, 5);

    // Antenna
    p.setPen(QPen(QColor("#AAAAAA"), 2));
    p.drawLine(cx, cy - r + 2, cx, cy - r - 10);
    p.setBrush(QColor("#AAAAAA"));
    p.setPen(Qt::NoPen);
    p.drawEllipse(cx - 3, cy - r - 13, 6, 6);

    p.restore();
}

MenuPage::MenuPage(QWidget *parent) : QWidget(parent)
{
    setupUI();
}

void MenuPage::setupUI()
{
    // --- Take Photos button ---
    m_takeBtn = new QPushButton("📷  Take Photos", this);
    m_takeBtn->setGeometry(175, 360, 470, 58);
    m_takeBtn->setCursor(Qt::PointingHandCursor);
    m_takeBtn->setStyleSheet(
        "QPushButton {"
        "  background: #FAFAC8;"
        "  border: none;"
        "  border-radius: 18px;"
        "  font-size: 18px;"
        "  font-weight: bold;"
        "  color: #333;"
        "}"
        "QPushButton:hover { background: #F0F090; }"
        "QPushButton:pressed { background: #E0E070; }"
    );
    connect(m_takeBtn, &QPushButton::clicked,
            this, &MenuPage::takePhotosClicked);

    // --- Upload Photos button ---
    m_uploadBtn = new QPushButton("⬆  Upload Photos", this);
    m_uploadBtn->setGeometry(175, 435, 470, 58);
    m_uploadBtn->setCursor(Qt::PointingHandCursor);
    m_uploadBtn->setStyleSheet(m_takeBtn->styleSheet());
    connect(m_uploadBtn, &QPushButton::clicked,
            this, &MenuPage::uploadPhotosClicked);

    // --- Settings button ---
    m_settingsBtn = new QPushButton("⚙️  Pengaturan App", this);
    m_settingsBtn->setGeometry(175, 510, 470, 58);
    m_settingsBtn->setCursor(Qt::PointingHandCursor);
    m_settingsBtn->setStyleSheet(m_takeBtn->styleSheet());
    connect(m_settingsBtn, &QPushButton::clicked,
            this, &MenuPage::settingsClicked);
}

void MenuPage::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::TextAntialiasing);

    const int W = width();
    const int H = height();

    // ── outer border (grey) ──
    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#C8C8C8"));
    p.drawRoundedRect(0, 0, W, H, 18, 18);

    // ── inner bg ──
    const int margin = 18;
    QRect inner(margin, margin, W - margin * 2, H - margin * 2);
    p.setBrush(C_BG);
    p.drawRoundedRect(inner, 12, 12);

    // ── scattered sparkles ──
    struct Spark { float x, y, sz; int col; };
    const Spark sparks[] = {
        {0.12f, 0.10f, 9, 0}, {0.85f, 0.08f, 7, 1},
        {0.75f, 0.25f, 11,2}, {0.08f, 0.40f, 8, 1},
        {0.90f, 0.45f, 9, 0}, {0.15f, 0.65f, 7, 2},
        {0.82f, 0.70f, 10,1}, {0.55f, 0.18f, 6, 2},
        {0.30f, 0.80f, 8, 0}, {0.65f, 0.85f, 7, 1},
        {0.05f, 0.82f, 9, 2}, {0.92f, 0.80f, 8, 0},
    };
    const QColor cols[3] = { C_STAR1, C_STAR2, C_STAR3 };
    for (const auto &s : sparks)
        drawSparkle(p, { s.x * W, s.y * H }, s.sz, cols[s.col]);

    // ── Bootheksis logo (cat astronaut) ──
    int logoSz = 72;
    int logoX  = W / 2 - logoSz / 2;
    int logoY  = 55;
    drawBootheksisLogo(p, QRect(logoX, logoY, logoSz, logoSz));

    // "Bootheksis" text
    QFont bFont("Arial", 14, QFont::Bold);
    p.setFont(bFont);
    p.setPen(C_BROWN);
    p.drawText(QRect(0, logoY + logoSz + 4, W, 26),
               Qt::AlignHCenter, "Bootheksis");

    // ── "SPACE ⊕ ADVENTURE" title ──
    QFont titleFont("Arial Rounded MT Bold", 42, QFont::Black);
    titleFont.setItalic(false);
    if (!titleFont.exactMatch())
        titleFont.setFamily("Arial");
    p.setFont(titleFont);

    auto drawOutlinedText = [&](const QString &txt, QRect r, QColor fill, QColor outline, int outW = 3)
    {
        p.setPen(outline);
        for (int dx = -outW; dx <= outW; ++dx)
            for (int dy = -outW; dy <= outW; ++dy)
                if (dx || dy)
                    p.drawText(r.translated(dx, dy), Qt::AlignHCenter | Qt::AlignVCenter, txt);
        p.setPen(fill);
        p.drawText(r, Qt::AlignHCenter | Qt::AlignVCenter, txt);
    };

    drawOutlinedText("SPACE ⊕ ADVENTURE",
                     QRect(30, 160, W - 60, 80),
                     C_TITLE, Qt::white);

    // ── decorative camera icon left of take-photos button ──
    p.save();
    p.translate(115, 370);
    p.setRenderHint(QPainter::Antialiasing);
    // camera body
    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#F4A0A0"));
    p.drawRoundedRect(0, 12, 55, 38, 6, 6);
    // lens bump
    p.setBrush(QColor("#E07070"));
    p.drawRoundedRect(5, 8, 20, 14, 3, 3);
    // lens
    p.setBrush(QColor("#663300"));
    p.drawEllipse(12, 18, 26, 26);
    p.setBrush(QColor("#442200"));
    p.drawEllipse(17, 23, 16, 16);
    p.setBrush(QColor(255, 255, 255, 90));
    p.drawEllipse(17, 23, 8, 8);
    // flash
    p.setBrush(QColor("#FFFFD0"));
    p.drawRoundedRect(38, 11, 10, 6, 2, 2);
    p.restore();

    // ── decorative upload icon right of upload button ──
    p.save();
    p.translate(W - 130, 438);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#E08090"));
    p.drawEllipse(0, 0, 52, 52);
    // arrow up
    p.setBrush(Qt::white);
    QPainterPath arrow;
    arrow.moveTo(26, 8);
    arrow.lineTo(40, 25);
    arrow.lineTo(32, 25);
    arrow.lineTo(32, 44);
    arrow.lineTo(20, 44);
    arrow.lineTo(20, 25);
    arrow.lineTo(12, 25);
    arrow.closeSubpath();
    p.drawPath(arrow);
    // underline
    p.drawRect(12, 44, 28, 4);
    p.restore();
}
