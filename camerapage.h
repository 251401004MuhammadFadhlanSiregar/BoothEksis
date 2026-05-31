#pragma once

#include <QWidget>
#include <QCamera>
#include <QImageCapture>
#include <QMediaCaptureSession>
#include <QMediaDevices>
#include <QPixmap>
#include <QImage>
#include<QPainter>

class QLabel;
class QPushButton;
class QTimer;
class QCamera;
class QImageCapture;

/**
 * CameraPage handles two flows:
 *  1. Live camera → countdown (3 s per slot) → capture 2 photos.
 *  2. Upload mode → open file dialog → place chosen images in slots.
 *
 * After both slots are filled, emits photosReady(QVector<QPixmap>).
 */
class CameraPage : public QWidget
{
    Q_OBJECT
public:
    explicit CameraPage(QWidget *parent = nullptr);

    void startSession(bool uploadMode = false);
    void stopCamera();

signals:
    void photosReady(const QVector<QPixmap> &photos);
    void backClicked();

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onCountdownTick();
    void onCaptureButtonClicked();
    void onImageCaptured(int id, const QImage &img);
    void onImageSaved(int id, const QString &filename);

private:
    void setupUI();
    void setupCamera();
    void startCountdown();
    void triggerCapture();
    void handleUploadMode();
    void drawSpaceStrip(QPainter &p);
    void drawSparkle(QPainter &p, QPointF c, float sz, QColor col);

    // Widgets
    QLabel          *m_logoLabel;
    QWidget         *m_viewfinderContainer;
    QLabel          *m_countdownOverlay;
    QLabel          *m_frameLabel;
    QPushButton     *m_captureBtn;
    QPushButton     *m_backBtn;

    // Camera
    QCamera              *m_camera;
    QImageCapture *m_imageCapture;
    QMediaCaptureSession *m_captureSession;

    // State
    QTimer          *m_timer;
    int              m_countdownValue;
    int              m_currentSlot;     // 0 or 1
    bool             m_uploadMode;
    bool             m_capturing;
    QVector<QPixmap> m_photos;

    // Strip layout (in widget coords)
    QRect m_stripRect;
    QRect m_slot0Rect;
    QRect m_slot1Rect;
};
