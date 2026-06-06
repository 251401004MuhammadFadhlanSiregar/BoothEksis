#include "settingsdialog.h"
#include <QMediaDevices>
#include <QCameraDevice>

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Pengaturan Bootheksis");
    resize(420, 320); // slightly larger for nicer padding
    setStyleSheet(
        "QDialog { background-color: #FAFAC8; border: 3px solid #8878B8; border-radius: 12px; }"
        "QLabel { font-weight: bold; color: #333; font-size: 13px; }"
        "QComboBox { background-color: white; color: #333; padding: 6px; border: 1.5px solid #8878B8; border-radius: 6px; font-weight: normal; font-size: 13px; }"
        "QComboBox QAbstractItemView { background-color: white; color: #333; selection-background-color: #8878B8; selection-color: white; border: 1.5px solid #8878B8; }"
        "QPushButton { background-color: #8878B8; color: white; border-radius: 8px; font-weight: bold; padding: 6px 14px; }"
        "QPushButton:hover { background-color: #7869A8; }"
        );

    setupUI();
    loadCameras();
}

void SettingsDialog::setupUI()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(16, 16, 16, 16);
    mainLayout->setSpacing(14);

    // 1. Resolution
    auto *lblRes = new QLabel("Pengaturan Resolusi Frame:", this);
    m_resolutionCombo = new QComboBox(this);
    m_resolutionCombo->addItem("Full HD (1920x1080)", QSize(1920, 1080));
    m_resolutionCombo->addItem("HD Classic (1280x720) [Default]", QSize(1280, 720));
    m_resolutionCombo->addItem("SD Stream (640x480)", QSize(640, 480));
    m_resolutionCombo->setCurrentIndex(1); // Default to HD

    mainLayout->addWidget(lblRes);
    mainLayout->addWidget(m_resolutionCombo);

    // 2. Volume Music
    auto *lblVol = new QLabel("Slider Volume Music:", this);
    auto *volLayout = new QHBoxLayout();
    m_volumeSlider = new QSlider(Qt::Horizontal, this);
    m_volumeSlider->setRange(0, 100);
    m_volumeSlider->setValue(50); // Default to 50%
    m_volumeSlider->setTickPosition(QSlider::TicksBelow);
    m_volumeSlider->setTickInterval(10);

    auto *lblVolVal = new QLabel("50%", this);
    connect(m_volumeSlider, &QSlider::valueChanged, this, [lblVolVal](int val) {
        lblVolVal->setText(QString::number(val) + "%");
    });

    volLayout->addWidget(m_volumeSlider);
    volLayout->addWidget(lblVolVal);

    mainLayout->addWidget(lblVol);
    mainLayout->addLayout(volLayout);

    // 3. Camera Selection
    auto *lblCam = new QLabel("Pilih Device Kamera Windows yang Bisa Dipakai:", this);
    m_cameraCombo = new QComboBox(this);
    mainLayout->addWidget(lblCam);
    mainLayout->addWidget(m_cameraCombo);

    // Spacer
    mainLayout->addSpacing(10);

    // Actions Footer Button Row
    auto *actionsLayout = new QHBoxLayout();
    m_btnCancel = new QPushButton("Batal", this);
    m_btnCancel->setStyleSheet("background-color: white; color: #333; border: 1.5px solid #bbb;");
    m_btnSave = new QPushButton("Simpan & Terapkan", this);

    actionsLayout->addWidget(m_btnCancel);
    actionsLayout->addWidget(m_btnSave);
    mainLayout->addLayout(actionsLayout);

    // Connect trigger signals
    connect(m_btnCancel, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_btnSave,   &QPushButton::clicked, this, &QDialog::accept);
}

void SettingsDialog::loadCameras()
{
    m_cameraCombo->clear();
    m_cameraCombo->addItem("Kamera Default System", "default");

    // Retrieve active windows multimedia video input devices
    const QList<QCameraDevice> cameras = QMediaDevices::videoInputs();
    for (const auto &camera : cameras) {
        m_cameraCombo->addItem(camera.description(), camera.id());
    }
}

QSize SettingsDialog::selectedResolution() const
{
    return m_resolutionCombo->currentData().toSize();
}

int SettingsDialog::selectedVolume() const
{
    return m_volumeSlider->value();
}

QString SettingsDialog::selectedCameraId() const
{
    return m_cameraCombo->currentData().toString();
}

void SettingsDialog::setCurrentSettings(const QSize &resolution, int volume, const QString &cameraId)
{
    // Find resolution
    int resIdx = m_resolutionCombo->findData(resolution);
    if (resIdx != -1) {
        m_resolutionCombo->setCurrentIndex(resIdx);
    }

    // Set volume
    m_volumeSlider->setValue(volume);

    // Find camera
    int camIdx = -1;
    for (int i = 0; i < m_cameraCombo->count(); ++i) {
        if (m_cameraCombo->itemData(i).toString() == cameraId) {
            camIdx = i;
            break;
        }
    }
    if (camIdx != -1) {
        m_cameraCombo->setCurrentIndex(camIdx);
    }
}
