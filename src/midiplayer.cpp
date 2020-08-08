#include "midiplayer.h"
#include <QThread>
#include <QDebug>

MidiPlayer::MidiPlayer(QSettings* settings, const QString& _settingsGroup, QObject *parent)
    : QObject(parent), _settings(settings), _settingsGroup(_settingsGroup)
{
    connect(&_midiOut, &QMidiOut::disconnected, this, &MidiPlayer::onDeviceDisconnected);

    //Trying to connect to last opened device
    const QList<DeviceInfo>& devices = MidiPlayer::devices();

    if (_settings)
    {
        QString lastDeviceId   = _settings->value(_settingsGroup + "/" + _settingKeyLastDeviceId  ).toString();
        QString lastDeviceName = _settings->value(_settingsGroup + "/" + _settingKeyLastDeviceName).toString();

        if (!lastDeviceId.isEmpty() || !lastDeviceName.isEmpty())
        {
            QString foundDeviceId = searchDevice(lastDeviceId, lastDeviceName);

            if (!foundDeviceId.isEmpty())
            {
                setDevice(foundDeviceId);
            }
            else
            {
                if (!devices.empty())
                {
                    setDevice(devices.first().id);
                }
            }
        }
        else
        {
            if (!devices.empty())
            {
                setDevice(devices.first().id);
            }
        }
    }
    else
    {
        if (!devices.empty())
        {
            setDevice(devices.first().id);
        }
    }

    //Setting of update device timer
    connect(&_timerDevicesUpdate, &QTimer::timeout, this, &MidiPlayer::onUpdateDevices);
    _timerDevicesUpdate.setInterval(1000);
    _timerDevicesUpdate.start();
}

MidiPlayer::~MidiPlayer()
{
    QMutexLocker locker(&_mutex);
    _enableSignalonDeviceDisconnected = false;
    _midiOut.stopAll();
    _midiOut.disconnect();

    //emit devicesChanged();
}

bool MidiPlayer::isPlaying()
{
    return _composition && _composition->midi && !_needStop && !_pause;
}

void MidiPlayer::run()
{
    while (true)
    {
        _mutex.lock();
        _midiOut.stopAll();
        _currentTime = 0;
        _amendmentTime = 0;
        _mutex.unlock();

        if (_composition && _composition->midi)
        {
            _mutex.lock();
            qDebug(QString("Start Playing. Events: %1, time: %2 sec")
                   .arg(_events.count()).arg(double(_maxTime) / 1000.0).toUtf8());
            _mutex.unlock();

            for (_currentPosition = 0; _currentPosition < _events.count(); _currentPosition += 1)
            {
                _mutex.lock();

                if (_needExit){
                    _mutex.unlock();
                    return;
                }

                if (!_composition || !_composition->midi || _needStop)
                {
                    _mutex.unlock();
                    break;
                }

                if (_pause || !_midiOut.isConnected())
                {
                    _currentPosition -= 1;
                    _mutex.unlock();
                    QThread::msleep(50);
                    continue;
                }

                QMidiEvent* e = _events.at(_currentPosition);

                if (e->type() != QMidiEvent::Meta)
                {
                    _currentTime = qint64(_composition->midi->timeFromTick(e->tick()) * 1000.0f);

                    qint32 waitTime = _currentTime - _elapsedTime.elapsed() - _amendmentTime;
                    if (waitTime > 0)
                    {
                        //ToDo: предусмотреть выход, если задержка слишком большая
                        _mutex.unlock();
                        QThread::msleep(uint32_t(waitTime));
                        _mutex.lock();
                    }
                    if (e->type() == QMidiEvent::SysEx)
                    {
                        // TODO: sysex
                    } else
                    {
                        qint32 message = e->message();
                        _midiOut.sendMsg(message);
                    }

                    emit updateProgress(_currentTime, _maxTime);
                }

                _mutex.unlock();
            }

            _mutex.lock();

            qDebug("End Playing");

            if (_needStop){
                _needStop = false;
            }
            else{
                emit stopPlayingSignal();
                _composition = nullptr;
            }

            _mutex.unlock();
        }

        QThread::msleep(50);

        _mutex.lock();
        if (_needExit)
        {
            _currentTime = 0;
            _amendmentTime = 0;
            _mutex.unlock();
            return;
        }
        _mutex.unlock();
    }
}

QList<MidiPlayer::DeviceInfo> MidiPlayer::devices() const
{
    QList<DeviceInfo> devicesInfo;
    const QMap<QString, QString>& devices = QMidiOut::devices();

    for (const QString deviceId : devices.keys())
    {
        DeviceInfo info;

        info.isValid = !deviceId.isEmpty();
        info.id = deviceId;
        info.name = devices.value(deviceId);

        if (!_currentDevice.id.isEmpty() && info.id == _currentDevice.id)
        {
            info.isConnected = true;
        }

        devicesInfo.append(info);
    }

    return devicesInfo;
}

MidiPlayer::DeviceInfo MidiPlayer::currentDevice() const
{
    return _currentDevice;
}

MidiPlayer::Error MidiPlayer::setDevice(const QString &deviceId)
{
    QMutexLocker locker(&_mutex);

    Error e;

    _currentDevice = DeviceInfo();

    if (_midiOut.isConnected())
    {
        _midiOut.stopAll();
        _midiOut.disconnect();
    }

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
            _currentDevice.isConnected = connected;
            break;
        }
    }

    if (!found)
    {
        qWarning(QString("%1: Device %2 not found!").arg(Q_FUNC_INFO).arg(deviceId).toUtf8());

        e = Error(ErrorType::DeviceByIdNotFound,
                  tr("Device with id \"%1\" not found").arg(deviceId));
    }

    if (connected)
    {
        if (_settings)
        {
            _settings->setValue(_settingsGroup + "/" + _settingKeyLastDeviceId,   _currentDevice.id);
            _settings->setValue(_settingsGroup + "/" + _settingKeyLastDeviceName, _currentDevice.name);
        }

        qDebug(QString("Device %1: \"%2\" successfully connected!")
               .arg(_currentDevice.id).arg(_currentDevice.name).toUtf8());
    }
    else
    {
        if (_settings)
        {
            _settings->setValue(_settingsGroup + "/" + _settingKeyLastDeviceId,   "");
            _settings->setValue(_settingsGroup + "/" + _settingKeyLastDeviceName, "");
        }

        qWarning(QString("%1: Failed to connect device %2: \"%3\"")
               .arg(Q_FUNC_INFO).arg(_currentDevice.name).arg(_currentDevice.id).toUtf8());

        e = Error(ErrorType::FailedToConnectDevice,
                  tr("Failed to connect device %1: \"%2\"").arg(_currentDevice.name).arg(_currentDevice.id));
    }

    emit devicesChanged();

    return e;
}

MidiPlayer::Error MidiPlayer::play(Composition *composition)
{
    QMutexLocker locker(&_mutex);

    if (!composition)
    {
        qDebug("!composition->midi");
        return Error(NoComposition,
                     tr("No composition specified"));
    }

    if (!composition || !composition->midi)
    {
        qDebug("!composition->midi");
        return Error(InvalidComposition,
                     tr("Invalid composition"));
    }

    if (!_midiOut.isConnected())
    {
        return Error(DeviceNotConnected,
                     tr("MIDI device not connected"));
    }

    _midiOut.stopAll();

    if (_composition != composition){
        _maxTime = calcTime(composition->midi);
        _composition = composition;

        emit updateProgress(0, _maxTime);

        _elapsedTime.start();
        _events = _composition->midi->events();
        _amendmentTime = 0;

        _currentPosition = 0;
        _maxPosition = _events.count() - 1;

        _needStop = false;

        emit playStarted(_composition);
    }

    _pause = false;

    return Error();
}

MidiPlayer::Error MidiPlayer::setPause(bool pause)
{
    QMutexLocker locker(&_mutex);

    if (_pause != pause)
    {
        _pause = pause;
        if (_pause)
        {
            _midiOut.stopAll();
            _amendmentTime += _elapsedTime.elapsed();
        }
        else
        {
            if (_midiOut.isConnected())
            {
                _elapsedTime.restart();
            }
            else
            {
                _pause = true;
                return Error(DeviceNotConnected,
                             tr("MIDI device not connected"));
            }
        }
    }

    return Error();
}

void MidiPlayer::changePosition(float percentage)
{
    //QMutexLocker locker(&_mutex);
    //_currentPosition = int(float(_maxPosition) * percentage / 100.0f);
}

void MidiPlayer::destroy()
{
    QMutexLocker locker(&_mutex);
    _composition = nullptr;
    _needExit = true;
}

void MidiPlayer::onUpdateDevices()
{
    const auto& devices = MidiPlayer::devices();
    if (devices != _devices)
    {
        _devices = devices;
        emit devicesChanged();
    }
}

void MidiPlayer::onDeviceDisconnected(QString deviceId)
{
    //TODO: Maybe need mutex

    if (_currentDevice.id == deviceId)
    {
        //setPause(true);
        _currentDevice.isConnected = false;

        if (_enableSignalonDeviceDisconnected)
        {
            emit error(Error(ErrorType::CurrentDeviceDisconnected,
                             tr("The current MIDI-device is disconnected. Playback stopped. Select another MIDI-device to continue playback")));
        }
    }
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

QString MidiPlayer::searchDevice(const QString &deviceId, const QString &deviceName)
{
    if (deviceId.isEmpty() && deviceName.isEmpty())
    {
        return QString();
    }

    const QList<DeviceInfo>& devices = MidiPlayer::devices();

    QList<DeviceInfo> candidates;

    //Search all candidates
    for (const auto& device : devices)
    {
        if (deviceId == device.id || deviceName == device.name)
        {
            candidates.append(device);
        }
    }

    //Finding the most suitable
    for (const auto& device : candidates)
    {
        if (deviceId == device.id && deviceName == device.name)
        {
            return device.id;
        }
    }

    //The first with the same name
    for (const auto& device : candidates)
    {
        if (deviceName == device.name)
        {
            return device.id;
        }
    }

    //The first with the same id
    for (const auto& device : candidates)
    {
        if (deviceId == device.id)
        {
            return device.id;
        }
    }

    //Not found
    return QString();
}

bool MidiPlayer::isPause() const
{
    return _pause;
}

Composition *MidiPlayer::currentComposition() const
{
    return _composition;
}

