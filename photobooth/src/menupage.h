#pragma once

#include <QWidget>
#include <QPushButton>

class MenuPage : public QWidget
{
    Q_OBJECT
public:
    explicit MenuPage(QWidget *parent = nullptr);

signals:
    void takePhotosClicked();
    void uploadPhotosClicked();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void setupUI();
    void drawSparkle(class QPainter &p, QPointF center, float size, QColor color);
    void drawBootheksisLogo(class QPainter &p, QRect area);

    QPushButton *m_takeBtn;
    QPushButton *m_uploadBtn;
};
