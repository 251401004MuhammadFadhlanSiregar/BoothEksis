#include "mainwindow.h"
#include "menupage.h"
#include "camerapage.h"
#include "stickerpage.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
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

    // Camera signals
    connect(m_cameraPage, &CameraPage::photosReady,
            this, &MainWindow::showStickers);
    connect(m_cameraPage, &CameraPage::backClicked,
            this, &MainWindow::showMenu);

    // Sticker page signals
    connect(m_stickerPage, &StickerPage::homeClicked,
            this, &MainWindow::showMenu);

    showMenu();
}

void MainWindow::showMenu()
{
    m_cameraPage->stopCamera();
    m_stack->setCurrentIndex(0);
}

void MainWindow::showCamera(bool uploadMode)
{
    m_cameraPage->startSession(uploadMode);
    m_stack->setCurrentIndex(1);
}

void MainWindow::showStickers(const QVector<QPixmap> &photos)
{
    m_stickerPage->setPhotos(photos);
    m_stack->setCurrentIndex(2);
}
