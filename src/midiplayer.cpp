#include "midiplayer.h"
#include <QThread>
#include <QDebug>

MidiPlayer::MidiPlayer(QObject *parent) : QObject(parent)
{
    const QList<DeviceInfo>& devices = MidiPlayer::devices();

    qDebug("Midi devices:");
    for (const DeviceInfo& device : devices)
    {
        qDebug(QString("%1: \"%2\"").arg(device.id).arg(device.name).toUtf8());
    }
    qDebug(QString("Total midi devices: %1").arg(devices.count()).toUtf8());

    setDevice(devices.first().id);
}

MidiPlayer::~MidiPlayer()
{
    //_mutex.lock();
    _midiOut.stopAll();
    _midiOut.disconnect();
    //_mutex.unlock();

    emit devicesChanged();
}

bool MidiPlayer::isPlaying()
{
    return _composition && _composition->midi && !_needStop && !_pause;
}

void MidiPlayer::run()
{
    while (true)
    {
        //_mutex.lock();
        _midiOut.stopAll();
        //_mutex.unlock();

        if (_composition && _composition->midi) {
            qDebug(QString("Start Playing. Events: %1, time: %2 sec").arg(_events.count()).arg(double(_maxTime) / 1000.0).toUtf8());

            for (_currentPosition = 0; _currentPosition < _events.count(); _currentPosition += 1){
                //_mutex.lock();

                if (_needExit){
                    //_mutex.unlock();
                    return;
                }

                if (!_composition || !_composition->midi || _needStop)
                {
                    //_mutex.unlock();
                    break;
                }

                if (_pause)
                {
                    _currentPosition -= 1;
                    //_mutex.unlock();
                    QThread::msleep(50);
                    continue;
                }

                QMidiEvent* e = _events.at(_currentPosition);

                if (e->type() != QMidiEvent::Meta) {
                    quint64 eventTime = qint64(_composition->midi->timeFromTick(e->tick()) * 1000.0f);

                    qint32 waitTime = eventTime - _elapsedTime.elapsed();
                    if (waitTime > 0) {
                        //ToDo: предусмотреть выход, если пауза слишком большая
                        //_mutex.unlock();
                        QThread::msleep(uint32_t(waitTime));
                        //_mutex.lock();
                    }
                    if (e->type() == QMidiEvent::SysEx) {
                        // TODO: sysex
                    } else {
                        qint32 message = e->message();
                        _midiOut.sendMsg(message);
                    }

                    emit updateProgress(eventTime, _maxTime);
                }
            }

            qDebug(QString("End Playing").toUtf8());

            if (_needStop){
                _needStop = false;
            }
            else{
                emit stopPlayingSignal();
                _composition = nullptr;
            }

            //_mutex.unlock();
        }

        QThread::msleep(50);

        //_mutex.lock();
        if (_needExit)
        {
            //_mutex.unlock();
            return;
        }
    }
}

QList<MidiPlayer::DeviceInfo> MidiPlayer::devices() const
{
    QList<DeviceInfo> devicesInfo;
    const QMap<QString, QString>& devices = QMidiOut::devices();

    for (const QString deviceId : devices)
    {
        DeviceInfo info;

        info.isValid = !deviceId.isEmpty();
        info.id = deviceId;
        info.name = devices.value(deviceId);

        if (!_currentDevice.id.isEmpty() && info.id ==_currentDevice.id)
        {
            info.isConnected = true;
        }

        if (info.name.isEmpty())
        {
            info.name = info.id;
        }

        devicesInfo.append(info);
    }

    return devicesInfo;
}

MidiPlayer::DeviceInfo MidiPlayer::currentDevice() const
{
    return _currentDevice;
}

bool MidiPlayer::setDevice(const QString &deviceId)
{
    _mutex.lock();

    if (_midiOut.isConnected())
    {
        _midiOut.stopAll();
        _midiOut.disconnect();
    }

    _currentDevice = DeviceInfo();

    bool found = false;
    bool connected = false;
    const auto& devices = MidiPlayer::devices();
    for (const auto& device : devices)
    {
        if (deviceId == device.id)
        {
            found = true;
            connected = _midiOut.connect(deviceId);
            _currentDevice = device;
            _currentDevice.isConnected = true;
            break;
        }
    }

    if (!found)
    {
        qCritical() << Q_FUNC_INFO << ": device with id \"" + deviceId + "\" not found";
    }

    if (connected)
    {
        qDebug() << Q_FUNC_INFO << ": device with id \"" + deviceId + "\" successfully connected";
    }
    else
    {
        qCritical() << Q_FUNC_INFO << ": failed to connect device with id \"" + deviceId + "\"";
    }

    emit devicesChanged();

    _mutex.unlock();

    return connected;
}

void MidiPlayer::play(Composition *composition)
{
    if (!composition || !composition->midi)
    {
        qDebug("MidiPlayer::play(): !composition || !composition->midi");
        return;
    }

    _midiOut.stopAll();

    if (_composition != composition){
        _maxTime = calcTime(composition->midi);
        _composition = composition;

        emit updateProgress(0, _maxTime);

        _elapsedTime.start();
        _events = _composition->midi->events();

        _currentPosition = 0;
        _maxPosition = _events.count() - 1;

        _needStop = false;

        emit playStarted(_composition);
    }
    else{

    }
    _pause = false;
}

void MidiPlayer::setPause(bool pause)
{
    _pause = pause;
    if (_pause){
        _midiOut.stopAll();
    }
}

void MidiPlayer::changePosition(float percentage)
{
    _currentPosition = int(float(_maxPosition) * percentage / 100.0f);
}

void MidiPlayer::destroy()
{
    _composition = nullptr;
    _needExit = true;
}

quint64 MidiPlayer::calcTime(QMidiFile *midiFile)
{
    if (!midiFile){
        return 0;
    }

    quint64 time = 0;

    QList<QMidiEvent*> events = midiFile->events();

    for (QMidiEvent* e : events) {
        if (e->type() != QMidiEvent::Meta) {
            quint64 eventTime = quint64(midiFile->timeFromTick(e->tick()) * 1000.0f);
            if (eventTime > time){
                time = eventTime;
            }
        }
    }

    return time;
}
