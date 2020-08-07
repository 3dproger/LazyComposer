#ifndef MIDIPLAYER_H
#define MIDIPLAYER_H

#include <QObject>
#include <QRunnable >
#include <QMidiFile.h>
#include <QMidiOut.h>
#include <QElapsedTimer>
#include "global.h"
#include <QMutex>

class MidiPlayer : public QObject, public QRunnable
{
    Q_OBJECT
public:
    struct DeviceInfo {
        QString id;
        QString name;
        bool isConnected = false;
        bool isValid = false;
    };

    explicit MidiPlayer(QObject *parent = nullptr);
    ~MidiPlayer();
    bool isPlaying();
    void run();
    QList<DeviceInfo> devices() const;
    DeviceInfo currentDevice() const;
    bool setDevice(const QString& deviceId);

signals:
    void stopPlayingSignal();
    void updateProgress(int currentTime, int maxTime);
    void playStarted(Composition*);
    void devicesChanged();

public slots:
    void play(Composition *composition);
    void setPause(bool pause);
    void changePosition(float percentage);
    void destroy();

private:
    quint64 calcTime(QMidiFile* _midiFile);

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

    DeviceInfo _currentDevice;
};



#endif // MIDIPLAYER_H
