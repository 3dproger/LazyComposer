#include "midi.h"


Midi::Midi(QObject *parent) : QObject(parent){

    connect(&_timer, SIGNAL(timeout()), this, SLOT(timeout()));
    _timer.start(_durationFullms);

    QMap<QString, QString> devices = QMidiOut::devices();

    qDebug("Midi devices:");
    for (const QString& key : devices.keys()){
        qDebug(QString("%1: %2").arg(key).arg(devices.value(key)).toUtf8());
    }
    qDebug(QString("Total midi devices: %1").arg(devices.keys().count()).toUtf8());

    if (!devices.isEmpty()){
        if (!_out.connect(devices.first())){
            qCritical(QString("Failed to open MIDI device \"%1\"").arg(devices.first()).toUtf8());
        }
    }

    //_out.controlChange(36, 10, 10);
    //_out.channelAftertouch(36, 10);
    //_out.setInstrument(0, 36);
    //_out.setInstrument(_voice, 36);
    //_out.sendMsg(153);
}

Midi::~Midi(){
    _out.disconnect();
}

void Midi::playNote(int note, NoteDuration duration){

    //_out.setInstrument(0, -36);

    _out.noteOn(note + _transposition, _voice, 64);
}

void Midi::playNote(const Note &note){
    _out.setInstrument(0, -36);
    _out.noteOn(note.pitch, _voice, 64);
}


void Midi::setTransposition(int transposition){
    _transposition = transposition;
}

void Midi::timeout(){
    playNote(-36);
    _timer.start(_durationFullms / 4);
}

void Midi::setTempo(int tempo){
    _tempo = tempo;
    _durationFullms = 60000 / _tempo * 4;
}
