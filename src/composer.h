#ifndef COMPOSER_H
#define COMPOSER_H

#include <QObject>
#include "QMidiFile.h"
#include "global.h"
#include <QRunnable>
#include "titlegenerator.h"

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

public slots:

private:
    QMidiFile* compose();

    TitleGenerator _titleGenerator;

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
        0,  1, 2, 3, 4, 5, 6, 7, 8, 11, 12, 13,
        21, 22, 23, 24, 25, 27, 29, 30, 40, 41, 42, 46, 56,
        57, 61, 62, 63, 64, 65, 66, 67, 71, 72,
        73, 74, 75, 79, 80, 81, 82, 83, 85, 85,
        96, 104, 105, 108, 109, 110, 114
    };

    const QList<int> _instrumentsAccomp =
    {
        0,  1, 2, 4, 5, 6, 7, 24, 25, 26, 27, 28,
        29, 30, 87, 104, 105
    };

    const QList<int> _instrumentsBass =
    {
        32, 33, 34, 35, 36, 37, 38, 39, 80, 81, 87
    };

    void makeBass(QMidiFile& midi, const float& time, const int& track, const QList<int>& progression, const int& progressionStep, const QList<float>& rithm, const int& octave);
    void makeAccomp(QMidiFile& midi, const float& time, const int& track, const QList<int>& progression, const int& progressionStep, const QList<float>& rithm, const QList<QList<int>>& accomp);

    void makeMelody(QMidiFile& midi, const float& time, const int& track, const int& octave);

    void makeDrums(QMidiFile& midi, const float& time, const int& track);

    int stepHarmony(int step);

    QList<float> makeRithm();
};

#endif // COMPOSER_H
