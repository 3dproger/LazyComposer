#ifndef GLOBAL_H
#define GLOBAL_H

#include <QList>
#include "QMidiFile.h"

enum NoteDuration {ND_1, ND_1_2, ND_1_4, ND_1_8, ND_1_16, ND_1_32, ND_1_64};
enum TimeSignature {TS_4_4};
struct Note{
    int pitch = 60;
    NoteDuration duration = ND_1_8;

};

struct Tick{
    QList<Note> notes;
    int intervalToNext = 0;
};

enum AccompanimentType{ARP_8};

struct Accompaniment{
    AccompanimentType type = ARP_8;
};

struct Signature{
    int first  = 4;
    int second = 4;
};

struct Composition
{
    QMidiFile* midi;
    QString title;
};

#endif // GLOBAL_H
