#pragma once

#include <QMainWindow>
#include <QStackedWidget>
#include <QVector>
#include <QPixmap>

class MenuPage;
class CameraPage;
class StickerPage;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

public slots:
    void showMenu();
    void showCamera(bool uploadMode = false);
    void showStickers(const QVector<QPixmap> &photos);

private:
    QStackedWidget *m_stack;
    MenuPage       *m_menuPage;
    CameraPage     *m_cameraPage;
    StickerPage    *m_stickerPage;
};
