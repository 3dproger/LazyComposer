#ifndef MIDIPLAYER_H
#define MIDIPLAYER_H

#include <QObject>
#include <QRunnable>
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
    enum ErrorType {
        NoError,
        CommonError,
        CurrentDeviceDisconnected,
        FailedToConnectDevice,
        DeviceByIdNotFound,
        DevicesNotFound,
        DeviceNotConnected,
        NoComposition,
        InvalidComposition
    };

    class Error{
    public:
        friend class MidiPlayer;

        Error(ErrorType type, QString text) :
            _type(type), _text(text) { }
        ErrorType type() const { return _type; }
        QString   text() const { return _text; }

    private:
        Error(){}
        ErrorType _type = ErrorType::NoError;
        QString _text = tr("No Error");
    };

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
    Error setDevice(const QString& deviceId);

    Composition *currentComposition() const;

    bool isPause() const;
    Error play(Composition *currentComposition);
    Error setPause(bool isPause);

signals:
    void stopped();
    void updateProgress(int currentTime, int maxTime);
    void playStarted(Composition*);
    void devicesChanged();
    void error(Error error);

public slots:
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

    QTimer* _timerUpdateDevices = new QTimer(this);
    QList<DeviceInfo> _devices;
    DeviceInfo _currentDevice;

    QSettings* _settings = nullptr;
    QString _settingsGroup;
    const QString _settingKeyLastDeviceId   = "last_device/device_id";
    const QString _settingKeyLastDeviceName = "last_device/device_name";
};



#endif // MIDIPLAYER_H
