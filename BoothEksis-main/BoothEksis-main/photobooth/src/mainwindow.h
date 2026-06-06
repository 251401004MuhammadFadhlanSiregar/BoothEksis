#pragma once

#include <QMainWindow>
#include <QStackedWidget>
#include <QVector>
#include <QPixmap>
#include <QSize>
#include <QMediaPlayer>
#include <QAudioOutput>

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
    void openSettings(); // Added slot to show the settings dialog

private:
    QStackedWidget *m_stack;
    MenuPage       *m_menuPage;
    CameraPage     *m_cameraPage;
    StickerPage    *m_stickerPage;

    // Active preferences
    QSize   m_cameraResolution;
    int     m_musicVolume;
    QString m_selectedCameraId;

    // Background Audio Player
    QMediaPlayer *m_mediaPlayer;
    QAudioOutput *m_audioOutput;
};
