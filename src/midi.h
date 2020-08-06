#ifndef MIDI_H
#define MIDI_H

#include <QObject>
#include <QMidiOut.h>
#include <QMidiFile.h>
#include "global.h"
#include <QTimer>

class Midi : public QObject
{
    Q_OBJECT
public:
    explicit Midi(QObject *parent = nullptr);
    ~Midi();
    void setTransposition(int transposition);

    void setTempo(int tempo);

public slots:
    void playNote(int pitch, NoteDuration duration = ND_1);
    void playNote(const Note& note);

signals:

public slots:

private slots:
    void timeout();

protected:
     QMidiOut _out;

     int _tempo = 120;
     int _durationFullms = 60000 / _tempo * 4;
     int _transposition = 0;
     TimeSignature _signature = TS_4_4;

     int _voice = 0;
     Note _currentNote;

     QTimer _timer;
};

#endif // MIDI_H
