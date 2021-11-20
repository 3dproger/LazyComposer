#ifndef COMPOSER_H
#define COMPOSER_H

#include "global.h"
#include <QMidiFile.h>
#include <QObject>
#include <QRunnable>

/*
Piano:
0 Acoustic Grand Piano
1 Bright Acoustic Piano
2 Electric Grand Piano
3 Honky-tonk Piano
4 Electric Piano 1
5 Electric Piano 2
6 Harpsichord
7 Clavinet

Chromatic Percussion:
8 Celesta
9 Glockenspiel
10 Music Box
11 Vibraphone
12 Marimba
13 Xylophone
14 Tubular Bells
15 Dulcimer

Organ:
16 Drawbar Organ
17 Percussive Organ
18 Rock Organ
19 Church Organ
20 Reed Organ
21 Accordion
22 Harmonica
23 Tango Accordion

Guitar:
24 Acoustic Guitar (nylon)
25 Acoustic Guitar (steel)
26 Electric Guitar (jazz)
27 Electric Guitar (clean)
28 Electric Guitar (muted)
29 Overdriven Guitar
30 Distortion Guitar
31 Guitar harmonics

Bass:
32 Acoustic Bass
33 Electric Bass (finger)
34 Electric Bass (pick)
35 Fretless Bass
36 Slap Bass 1
37 Slap Bass 2
38 Synth Bass 1
39 Synth Bass 2

Strings:
40 Violin
41 Viola
42 Cello
43 Contrabass
44 Tremolo Strings
45 Pizzicato Strings
46 Orchestral Harp
47 Timpani

Strings (continued):
48 String Ensemble 1
49 String Ensemble 2
50 Synth Strings 1
51 Synth Strings 2
52 Choir Aahs
53 Voice Oohs
54 Synth Voice
55 Orchestra Hit

Brass:
56 Trumpet
57 Trombone
58 Tuba
59 Muted Trumpet
60 French Horn
61 Brass Section
62 Synth Brass 1
63 Synth Brass 2

Reed:
64 Soprano Sax
65 Alto Sax
66 Tenor Sax
67 Baritone Sax
68 Oboe
69 English Horn
70 Bassoon
71 Clarinet

Pipe:
72 Piccolo
73 Flute
74 Recorder
75 Pan Flute
76 Blown Bottle
77 Shakuhachi
78 Whistle
79 Ocarina

Synth Lead:
80 Lead 1 (square)
81 Lead 2 (sawtooth)
82 Lead 3 (calliope)
83 Lead 4 (chiff)
84 Lead 5 (charang)
85 Lead 6 (voice)
86 Lead 7 (fifths)
87 Lead 8 (bass + lead)

Synth Pad:
88 Pad 1 (new age)
89 Pad 2 (warm)
90 Pad 3 (polysynth)
91 Pad 4 (choir)
92 Pad 5 (bowed)
93 Pad 6 (metallic)
94 Pad 7 (halo)
95 Pad 8 (sweep)

Synth Effects:
96 FX 1 (rain)
97 FX 2 (soundtrack)
98 FX 3 (crystal)
99 FX 4 (atmosphere)
100 FX 5 (brightness)
101 FX 6 (goblins)
102 FX 7 (echoes)
103 FX 8 (sci-fi)

Ethnic:
104 Sitar
105 Banjo
106 Shamisen
107 Koto
108 Kalimba
109 Bag pipe
110 Fiddle
111 Shanai

Percussive:
112 Tinkle Bell
113 Agogo
114 Steel Drums
115 Woodblock
116 Taiko Drum
117 Melodic Tom
118 Synth Drum

Sound effects:
119 Reverse Cymbal
120 Guitar Fret Noise
121 Breath Noise
122 Seashore
123 Bird Tweet
124 Telephone Ring
125 Helicopter
126 Applause
127 Gunshot
*/

template<typename T>
static T randFromArray(const QList<T>& list);

class Composer : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit Composer(QObject *parent = nullptr);
    void run();

signals:
    void result(Composition* composition);
    void updateProgress(int percentage);

protected:
    static QString generateTitle();

private:
    QMidiFile* compose();

    int _noteStart = 55;
    int _transposition = 0;
    float _tempo = 120;

    float _duration1_1 = 240.0f / _tempo;
    float _duration1_2 = 120.0f / _tempo;
    float _duration1_4 = 60.0f  / _tempo;
    float _duration1_6 = 40.0f  / _tempo;
    float _duration1_8 = 30.0f  / _tempo;

    const float _tempoBase = 140;
    const float _tempoDispersion = 60;

    //{0, 2, 4, 5, 7, 9, 10} major
    //{0, 1, 3, 5, 6, 8, 10} Natural minor
    //{0, 1, 3, 5, 6, 9, 10} Harmonic minor
    const QList<int> _harmony = {0, 2, 3, 5, 7, 9, 10};

    const QList<QList<int>> _progressionsDrop1 =
    {
        {5, 3, 7, 4},
        {5, 5, 3, 2},
        {1, 3, 5, 4},
        {5, 7, 3, 2},
        {1, 7, 4, 3},
        {7, 4, 5, 3},
        {5, 7, 3, 4},
        {1, 5, 1, 5}
    };

    const QList<QList<int>> _progressionsDrop2 =
    {
        {5, 3, 7, 4},
        {5, 5, 3, 2},
        {1, 3, 5, 4},
        {5, 7, 3, 2},
        {1, 7, 4, 3},
        {7, 4, 5, 3},
        {5, 7, 3, 4},
        {1, 5, 1, 5}
    };

    const QList<QList<int>> _progressionsHollow1 =
    {
        {5, 3, 7, 4},
        {5, 5, 3, 2},
        {1, 3, 5, 4},
        {5, 7, 3, 2},
        {1, 7, 4, 3},
        {7, 4, 5, 3},
        {5, 7, 3, 4},
        {1, 5, 1, 5}
    };

    const QList<QList<int>> _progressionsHollow2 =
    {
        {5, 3, 7, 4},
        {5, 5, 3, 2},
        {1, 3, 5, 4},
        {5, 7, 3, 2},
        {1, 2, 3, 4},
        {1, 7, 4, 3},
        {7, 4, 5, 3},
        {5, 7, 3, 4},
        {1, 5, 1, 5}
    };

    const QList<int> _instrumentsSolo =
    {
        0, 1, 2, 3, 4, 5, 6, 7, 29, 30,
        40, 41, 42, 43, 56, 57, 58,
        64, 65, 66, 67, 68, 69, 70, 71,
        72, 73, 74,
        80, 81


        /*0,  1, 2, 3, 4, 5, 6, 7, 8, 11, 12, 13,
        21, 22, 23, 24, 25, 27, 29, 30, 40, 41, 42, 46, 56,
        57, 61, 62, 63, 64, 65, 66, 67, 71, 72,
        73, 74, 75, 79, 80, 81, 82, 83, 85, 85,
        96, 104, 105, 108, 109, 110, 114*/
    };

    const QList<int> _instrumentsAccomp =
    {
        0, 1, 2, 3, 4, 5, 6, 7, 24, 25, 26, 27, 28, 105
        //0, 1, 2, 4, 5, 6, 7, 24, 25, 26, 27, 28, 29, 30, 104, 105
    };

    const QList<int> _instrumentsBass =
    {
        32, 33, 34
        //32, 33, 34, 35, 36, 37, 38, 39, 80, 81, 87
    };

    void makeBass(QMidiFile& midi, const float& time, const int& track, const QList<int>& progression, const int& progressionStep, const QList<float>& rithm, const int& octave);
    void makeAccomp(QMidiFile& midi, const float& time, const int& track, const QList<int>& progression, const int& progressionStep, const QList<float>& rithm, const QList<QList<int>>& accomp);

    void makeMelody(QMidiFile& midi, const float& time, const int& track, const int& octave);

    void makeDrums(QMidiFile& midi, const float& time, const int& track);

    int stepHarmony(int step);

    QList<float> makeRithm();
};

#endif // COMPOSER_H
