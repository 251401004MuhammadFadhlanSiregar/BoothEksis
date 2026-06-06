#include "mainwindow.h"
#include "menupage.h"
#include "camerapage.h"
#include "stickerpage.h"
#include "settingsdialog.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_cameraResolution(1280, 720) // Default HD
    , m_musicVolume(50)             // Default volume index 50%
    , m_selectedCameraId("default")
{
    setWindowTitle("Bootheksis – Space Adventure");

    m_stack = new QStackedWidget(this);
    setCentralWidget(m_stack);

    m_menuPage    = new MenuPage(this);
    m_cameraPage  = new CameraPage(this);
    m_stickerPage = new StickerPage(this);

    m_stack->addWidget(m_menuPage);    // index 0
    m_stack->addWidget(m_cameraPage);  // index 1
    m_stack->addWidget(m_stickerPage); // index 2

    // Menu signals
    connect(m_menuPage, &MenuPage::takePhotosClicked,
            this, [this](){ showCamera(false); });
    connect(m_menuPage, &MenuPage::uploadPhotosClicked,
            this, [this](){ showCamera(true); });
    connect(m_menuPage, &MenuPage::settingsClicked,
            this, &MainWindow::openSettings);

    // Camera signals
    connect(m_cameraPage, &CameraPage::photosReady,
            this, &MainWindow::showStickers);
    connect(m_cameraPage, &CameraPage::backClicked,
            this, &MainWindow::showMenu);

    // Sticker page signals
    connect(m_stickerPage, &StickerPage::homeClicked,
            this, &MainWindow::showMenu);

    // Initialize background audio player with Space ambient soundtrack
    m_mediaPlayer = new QMediaPlayer(this);
    m_audioOutput = new QAudioOutput(this);
    m_mediaPlayer->setAudioOutput(m_audioOutput);
    m_audioOutput->setVolume(m_musicVolume / 100.0f);
    m_mediaPlayer->setSource(QUrl("https://mp3tourl.com/audio/1780756808263-889345e7-c567-4e5c-bd77-664a9bf69abf.mp3"));
    m_mediaPlayer->setLoops(QMediaPlayer::Infinite);

    connect(m_mediaPlayer, &QMediaPlayer::errorOccurred, this, [](QMediaPlayer::Error error, const QString &errorString) {
        qWarning() << "Media Player Error:" << error << "-" << errorString;
    });

    m_mediaPlayer->play();

    showMenu();
}

void MainWindow::showMenu()
{
    m_cameraPage->stopCamera();
    m_stack->setCurrentIndex(0);
}

void MainWindow::showCamera(bool uploadMode)
{
    // Apply preferences to camera session dynamically before starting!
    m_cameraPage->updateCameraDeviceAndResolution(m_selectedCameraId, m_cameraResolution);
    m_cameraPage->startSession(uploadMode);
    m_stack->setCurrentIndex(1);
}

void MainWindow::showStickers(const QVector<QPixmap> &photos)
{
    m_stickerPage->setPhotos(photos);
    m_stack->setCurrentIndex(2);
}

void MainWindow::openSettings()
{
    SettingsDialog dialog(this);
    dialog.setCurrentSettings(m_cameraResolution, m_musicVolume, m_selectedCameraId);

    if (dialog.exec() == QDialog::Accepted) {
        m_cameraResolution = dialog.selectedResolution();
        m_musicVolume      = dialog.selectedVolume();
        m_selectedCameraId = dialog.selectedCameraId();

        qDebug() << "Pengaturan Berhasil Diterapkan:";
        qDebug() << "  Resolusi Frame: " << m_cameraResolution.width() << "x" << m_cameraResolution.height();
        qDebug() << "  Volume Music:    " << m_musicVolume << "%";
        qDebug() << "  Device Kamera:   " << m_selectedCameraId;

        // Apply background music volume physically
        if (m_audioOutput) {
            m_audioOutput->setVolume(m_musicVolume / 100.0f);
        }

        // Propagate preferences dynamically to widgets
        m_cameraPage->updateCameraDeviceAndResolution(m_selectedCameraId, m_cameraResolution);
    }
}
