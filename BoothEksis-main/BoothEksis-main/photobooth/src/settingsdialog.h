#pragma once

#include <QDialog>
#include <QComboBox>
#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

class SettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget *parent = nullptr);

    void setCurrentSettings(const QSize &resolution, int volume, const QString &cameraId);

    // Getters for selected preferences
    QSize selectedResolution() const;
    int selectedVolume() const;
    QString selectedCameraId() const;

private:
    void setupUI();
    void loadCameras();

    QComboBox *m_resolutionCombo;
    QSlider   *m_volumeSlider;
    QComboBox *m_cameraCombo;

    QPushButton *m_btnSave;
    QPushButton *m_btnCancel;
};
