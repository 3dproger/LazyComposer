#include "midiplayer.h"
#include <QThread>

MidiPlayer::MidiPlayer(QObject *parent) : QObject(parent)
{


    QMap<QString, QString> devices = QMidiOut::devices();

    qDebug("Midi devices:");
    for (const QString& key : devices.keys())
    {
        qDebug(QString("%1: %2").arg(key).arg(devices.value(key)).toUtf8());
    }
    qDebug(QString("Total midi devices: %1").arg(devices.keys().count()).toUtf8());

    if (!devices.isEmpty())
    {
        if (!_midiOut.connect(devices.first()))
        {
            qCritical(QString("Failed to open MIDI device \"%1\"").arg(devices.first()).toUtf8());
        }
        else
        {
            qDebug(QString("Connected \"%1\"").arg(devices.first()).toUtf8());
        }
    }
}

MidiPlayer::~MidiPlayer()
{
    _midiOut.disconnect();
}

bool MidiPlayer::isPlaying()
{
    return _composition && _composition->midi && !_needStop && !_pause;
}

void MidiPlayer::run()
{
    while (true){
        _midiOut.stopAll();
        if (_composition && _composition->midi){
            qDebug(QString("Start Playing. Events: %1, time: %2 sec").arg(_events.count()).arg(double(_maxTime) / 1000.0).toUtf8());

            for (_currentPosition = 0; _currentPosition < _events.count(); _currentPosition += 1){

                if (_needExit){
                    return;
                }

                if (!_composition || !_composition->midi || _needStop){
                    break;
                }

                if (_pause){
                    _currentPosition -= 1;
                    QThread::msleep(10);
                    continue;
                }

                QMidiEvent* e = _events.at(_currentPosition);

                if (e->type() != QMidiEvent::Meta) {
                    quint64 eventTime = qint64(_composition->midi->timeFromTick(e->tick()) * 1000.0f);

                    qint32 waitTime = eventTime - _elapsedTime.elapsed();
                    if (waitTime > 0) {
                        //ToDo: предусмотреть выход, если пауза слишком большая
                        QThread::msleep(uint32_t(waitTime));
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
        }
        QThread::msleep(10);

        if (_needExit){
            return;
        }
    }
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

