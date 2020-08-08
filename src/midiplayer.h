#ifndef MIDIPLAYER_H
#define MIDIPLAYER_H

#include <QObject>
#include <QRunnable >
#include <QMidiFile.h>
#include <QMidiOut.h>
#include <QElapsedTimer>
#include "global.h"
#include <QMutex>
#include <QSettings>
#include <QTimer>

class MidiPlayer : public QObject, public QRunnable
{
    Q_OBJECT
public:
    struct DeviceInfo {
        QString id;
        QString name;
        bool isConnected = false;
        bool isValid = false;

        bool operator==(const DeviceInfo& other)
        {
            return id == other.id &&
                    name == other.name &&
                    isConnected == other.isConnected &&
                    isValid == other.isValid;
        }
    };

    explicit MidiPlayer(QSettings* settings, const QString& _settingsGroup, QObject *parent = nullptr);
    ~MidiPlayer();
    bool isPlaying();
    void run();
    QList<DeviceInfo> devices() const;
    DeviceInfo currentDevice() const;
    bool setDevice(const QString& deviceId);

    Composition *currentComposition() const;

    bool isPause() const;

signals:
    void stopPlayingSignal();
    void updateProgress(int currentTime, int maxTime);
    void playStarted(Composition*);
    void devicesChanged();

public slots:
    void play(Composition *currentComposition);
    void setPause(bool isPause);
    void changePosition(float percentage);
    void destroy();

private slots:
    void onUpdateDevices();

private:
    static quint64 calcTime(QMidiFile* _midiFile);
    QString searchDevice(const QString& deviceId, const QString& deviceName);

    QMidiOut _midiOut;
    Composition* _composition = nullptr;
    bool _needExit = false;
    bool _needStop = false;
    bool _pause    = false;
    QList<QMidiEvent*> _events;
    QMutex _mutex;

    int _currentPosition = 0;
    int _maxPosition = 0;

    QElapsedTimer _elapsedTime;
    quint64 _maxTime = 0;
    quint64 _currentTime = 0;
    quint64 _amendmentTime = 0;

    QTimer _timerDevicesUpdate;
    QList<DeviceInfo> _devices;
    DeviceInfo _currentDevice;

    QSettings* _settings = nullptr;
    QString _settingsGroup;
    const QString _settingKeyLastDeviceId   = "last_device/device_id";
    const QString _settingKeyLastDeviceName = "last_device/device_name";
};



#endif // MIDIPLAYER_H
