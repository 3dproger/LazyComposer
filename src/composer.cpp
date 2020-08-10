#include "composer.h"
#include <QDateTime>
#include <QDebug>
#include <cmath>
#include <tuple>

Composer::Composer(QObject *parent) : QObject(parent)
{

}

void Composer::run()
{
    QMidiFile* midi = compose();
    Composition* composition = new Composition();
    composition->midi = midi;
    composition->title = _titleGenerator.generate();

    emit result(composition);
}

QMidiFile *Composer::compose()
{
    QDateTime cd = QDateTime::currentDateTime();
    qsrand(cd.toTime_t());

    _tempo = _tempoBase - _tempoDispersion / 2.0f + fmodf(qrand(), _tempoDispersion);

    _duration1_1 = 240.0f / _tempo;
    _duration1_2 = 120.0f / _tempo;
    _duration1_4 = 60.0f  / _tempo;
    _duration1_6 = 40.0f  / _tempo;
    _duration1_8 = 30.0f  / _tempo;


    _transposition = qrand() % 12;

    QList<int> progressionDrop1   = randFromArray(_progressionsDrop1);
    QList<int> progressionDrop2   = randFromArray(_progressionsDrop2);
    if (qrand() % 100 <= 60)
    {
        progressionDrop2 = progressionDrop1;
    }

    QList<int> progressionHollow1 = randFromArray(_progressionsHollow1);
    QList<int> progressionHollow2 = randFromArray(_progressionsHollow2);
    if (qrand() % 100 <= 30)
    {
        progressionHollow1 = progressionDrop1;
    }
    if (qrand() % 100 <= 60)
    {
        progressionHollow2 = progressionHollow1;
    }

    QList<QList<int>> accomponiment = {{0}, {4}, {7}, {4}, {9}, {4}, {7}, {4}};
    QList<QList<int>> accomponimentDrive1 = {{0, 4, 7}};
    QList<QList<int>> accomponimentDrive2 = {{0}, {0}, {0, 4, 7}, {0}, {0}, {0, 4, 7}, {0}, {0, 4, 7}};
    //QList<QList<int>> accomponiment = {{0, 4, 9}, {0, 4, 9}, {0, 4, 9}, {0, 4, 9}, {0, 4, 7, 9}, {0, 4, 9}, {0, 4, 9}, {0, 4, 9}};

    QMidiFile* midi = new QMidiFile();

    midi->setResolution(480);

    int instrument;

    int trackAccomp1 = midi->createTrack();
    instrument = randFromArray(_instrumentsAccomp);
    qDebug() << "Accomp1:" << instrument;
    midi->createProgramChangeEvent(trackAccomp1, 0, trackAccomp1, instrument);
    midi->createMetaEvent(trackAccomp1, 0, QMidiEvent::MetaNumbers::TrackName, "Accomp 1");

    int trackAccomp2 = midi->createTrack();
    instrument = randFromArray(_instrumentsAccomp);
    qDebug() << "Accomp2:" << instrument;
    midi->createProgramChangeEvent(trackAccomp2, 0, trackAccomp2, instrument);
    midi->createMetaEvent(trackAccomp2, 0, QMidiEvent::MetaNumbers::TrackName, "Accomp 2");

    int trackAccomp3 = midi->createTrack();
    instrument = randFromArray(_instrumentsAccomp);
    qDebug() << "Accomp3:" << instrument;
    midi->createProgramChangeEvent(trackAccomp3, 0, trackAccomp3, instrument);
    midi->createMetaEvent(trackAccomp3, 0, QMidiEvent::MetaNumbers::TrackName, "Accomp 3");

    int trackSolo1 = midi->createTrack();
    instrument = randFromArray(_instrumentsSolo);
    qDebug() << "Solo1:" << instrument;
    midi->createProgramChangeEvent(trackSolo1, 0, trackSolo1, instrument);
    midi->createMetaEvent(trackSolo1, 0, QMidiEvent::MetaNumbers::TrackName, "Solo 1");

    int trackSolo2 = midi->createTrack();
    instrument = randFromArray(_instrumentsSolo);
    qDebug() << "Solo2:" << instrument;
    midi->createProgramChangeEvent(trackSolo2, 0, trackSolo2, instrument);
    midi->createMetaEvent(trackSolo2, 0, QMidiEvent::MetaNumbers::TrackName, "Solo 2");

    int trackBass = midi->createTrack();
    instrument = randFromArray(_instrumentsBass);
    qDebug() << "Bass:" << instrument;
    midi->createProgramChangeEvent(trackBass, 0, trackBass, instrument);
    midi->createMetaEvent(trackBass, 0, QMidiEvent::MetaNumbers::TrackName, "Bass");

    int trackDrumkit = midi->createTrack();
    midi->createProgramChangeEvent(trackDrumkit, 0, 9, 0);
    midi->createMetaEvent(trackDrumkit, 0, QMidiEvent::MetaNumbers::TrackName, "Drumkit");

    //Tempo
    midi->createTempoEvent(0, 0, _tempo);


    //QList<float> rithm = {8, 16, 16, 8, 16, 16, 8, 16, 16, 8, 16, 16}; //Horse 1
    //QList<float> rithm = {6, 6, 6, 6, 8, 8}; //Club 1
    //QList<float> rithm = {1};                    //Basic 1
    //QList<float> rithm = {2, 2};                 //Basic 2
    //QList<float> rithm = {4, 4, 4, 4};           //Basic 3
    QList<float> rithm = {8, 8, 8, 8, 8, 8, 8, 8}; //Basic 4

    float time = 0.0f;

    int repeats = 2; // повторы
    int progressionSteps = 4; //размер прогрессии
    int dropSize1 = 2;
    int dropSize2 = 2;
    int hollowSize1 = 2;
    int hollowSize2 = 2;

    int tackts = ((dropSize1 + dropSize2 + hollowSize1 + hollowSize2) * progressionSteps) * repeats;
    int tacktCouter = 0;

    for (int repeat = 0; repeat < repeats; ++repeat){
        for (int i = 0; i < dropSize1; ++i)
        {
            for (int progressionStep = 0; progressionStep < progressionSteps; ++progressionStep){


                makeBass(*midi, time, trackBass, progressionDrop1, progressionStep, rithm, -3);

                makeAccomp(*midi, time, trackAccomp2, progressionDrop1, progressionStep, {8, 8, 8, 8, 8, 8, 8, 8}, accomponiment);
                makeAccomp(*midi, time, trackAccomp3, progressionDrop1, progressionStep, {8, 8, 8, 8, 8, 8, 8, 8}, accomponimentDrive1);

                makeMelody(*midi, time, trackAccomp1, 0);
                //makeMelody(*midi, time, trackPiano, 1);
                makeMelody(*midi, time, trackSolo1, 0);
                makeMelody(*midi, time, trackAccomp2, 0);

                makeDrums(*midi, time, trackDrumkit);

                time += _duration1_1;
                tacktCouter++;
                emit updateProgress(tacktCouter * 100 / tackts);
            }
        }

        for (int i = 0; i < dropSize2; ++i)
        {
            for (int progressionStep = 0; progressionStep < progressionSteps; ++progressionStep){


                makeBass(*midi, time, trackBass, progressionDrop2, progressionStep, rithm, -3);

                makeAccomp(*midi, time, trackAccomp2, progressionDrop2, progressionStep, {8, 8, 8, 8, 8, 8, 8, 8}, accomponiment);
                makeAccomp(*midi, time, trackAccomp3, progressionDrop2, progressionStep, {8, 8, 8, 8, 8, 8, 8, 8}, accomponimentDrive1);

                makeMelody(*midi, time, trackAccomp1, 0);
                //makeMelody(*midi, time, trackFlute, 0);
                makeMelody(*midi, time, trackSolo2, 0);
                makeMelody(*midi, time, trackAccomp2, randFromArray(QList<int>{0, 1}));

                makeDrums(*midi, time, trackDrumkit);

                time += _duration1_1;
                tacktCouter++;
                emit updateProgress(tacktCouter * 100 / tackts);
            }
        }

        for (int i = 0; i < hollowSize1; ++i)
        {
            for (int progressionStep = 0; progressionStep < progressionSteps; ++progressionStep){


                makeBass(*midi, time, trackBass, progressionHollow1, progressionStep, rithm, -3);

                makeAccomp(*midi, time, trackAccomp2, progressionHollow1, progressionStep, {8, 8, 8, 8, 8, 8, 8, 8}, accomponiment);

                makeMelody(*midi, time, trackAccomp1, 0);

                makeDrums(*midi, time, trackDrumkit);

                time += _duration1_1;
                tacktCouter++;
                emit updateProgress(tacktCouter * 100 / tackts);
            }
        }

        for (int i = 0; i < hollowSize2; ++i)
        {
            for (int progressionStep = 0; progressionStep < progressionSteps; ++progressionStep){


                makeBass(*midi, time, trackBass, progressionHollow2, progressionStep, rithm, -3);

                makeAccomp(*midi, time, trackAccomp2, progressionHollow2, progressionStep, {8, 8, 8, 8, 8, 8, 8, 8}, accomponiment);
                makeAccomp(*midi, time, trackAccomp3, progressionHollow2, progressionStep, {8, 8, 8, 8, 8, 8, 8, 8}, accomponimentDrive2);

                makeMelody(*midi, time, trackAccomp1, randFromArray(QList<int>{0, 1}));
                makeMelody(*midi, time, trackSolo2, 0);
                //makeMelody(*midi, time, trackGuitar, 1);

                makeDrums(*midi, time, trackDrumkit);

                time += _duration1_1;
                tacktCouter++;
                emit updateProgress(tacktCouter * 100 / tackts);
            }
        }
    }


    //midi->createNote(trackPiano, midi->tickFromTime(time), midi->tickFromTime(time + _duration1_4), 0, stepHarmony(progression.at(0)) - 24, 95, 95);
    //midi->createNote(trackPiano, midi->tickFromTime(time), midi->tickFromTime(time + _duration1_4), 0, stepHarmony(qrand() % 24), 95, 95);

    midi->sort();

    emit updateProgress(100);

    return midi;
}

int Composer::stepHarmony(int step)
{
    int note = _noteStart + _transposition + _harmony.at(step % _harmony.size()) + 12 * (step / _harmony.size());

    return note;
}

QList<float> Composer::makeRithm()
{
    QList<float> rithm;

    QList<float> fractionList = {1.0f, 0.75f, 0.5f, 0.25f, 0.125f};

    float sum = 0.0f;

    while (sum < 1.0f){
        float beat = randFromArray(fractionList);

        if (sum + beat < 1.0f)
        {
            rithm.append(1.0f / beat);
            sum += beat;
        }
        else
        {
            rithm.append(1.0f / fractionList.last());
            sum += fractionList.last();
        }

    }

    return rithm;
}

void Composer::makeBass(QMidiFile &midi, const float& time, const int& track, const QList<int>& progression, const int& progressionStep, const QList<float>& rithm, const int& octave)
{
    float preTime = time;
    for (int i = 0; i < rithm.count(); ++i)
    {
        float startNoteTime = preTime;
        float stopNoteTime  = startNoteTime + 240.0f / _tempo / rithm.at(i);

        int note = stepHarmony(progression.at(progressionStep  % progression.size()) - 1) + octave * 12;

        midi.createNote(track, midi.tickFromTime(startNoteTime), midi.tickFromTime(stopNoteTime), track, note, 95, 95);

        preTime = stopNoteTime;
    }
}

void Composer::makeAccomp(QMidiFile &midi, const float &time, const int &track, const QList<int> &progression, const int &progressionStep, const QList<float> &rithm, const QList<QList<int>>& accomp)
{
    float preTime = time;
    for (int i = 0; i < rithm.count(); ++i)
    {
        float startNoteTime = preTime;
        float stopNoteTime  = startNoteTime + 240.0f / _tempo / rithm.at(i);

        for (int j = 0; j < accomp.at(i % accomp.size()).size(); ++j)
        {
            int note = stepHarmony(progression.at(progressionStep  % progression.size()) - 1 + accomp.at(i % accomp.size()).at(j));
            midi.createNote(track, midi.tickFromTime(startNoteTime), midi.tickFromTime(stopNoteTime), track, note - 24, 95, 95);
        }

        preTime = stopNoteTime;
    }
}

void Composer::makeMelody(QMidiFile &midi, const float &time, const int &track, const int& octave)
{
    QList<float> rithm = makeRithm();//{4, 8, 16, 16, 8, 8, 4};

    float preTime = time;
    for (int i = 0; i < rithm.count(); ++i)
    {
        float startNoteTime = preTime;
        float stopNoteTime  = startNoteTime + 240.0f / _tempo / rithm.at(i);

        int note = stepHarmony(qrand() % 12) + 12 * octave;

        midi.createNote(track, midi.tickFromTime(startNoteTime), midi.tickFromTime(stopNoteTime), track, note, 95, 95);

        preTime = stopNoteTime;
    }
}

void Composer::makeDrums(QMidiFile &midi, const float &time, const int &track)
{
    /*QList<QList<int>> drumRithm = {{35, 46}, {38, 46}, {46}, {35}, {38, 46}};
    QList<float> rithm = {4, 4, 8, 8, 4};*/

    /*QList<QList<int>> drumRithm = {{35}, {38}, {-1}, {35}, {38}};
    QList<float> rithm = {4, 4, 8, 8, 4};*/

    QList<QList<int>> drumRithm = {
        {35, 42},
        {42},
        {38, 42},
        {42},
        {35, 42},
        {35, 42},
        {38, 42},
        {42},
    };

    QList<float> rithm = {8, 8, 8, 8, 8, 8, 8, 8};

    float preTime = time;
    for (int i = 0; i < rithm.count(); ++i)
    {
        float startNoteTime = preTime;
        float stopNoteTime  = startNoteTime + 240.0f / _tempo / rithm.at(i);

        for (int j = 0; j < drumRithm.at(i % drumRithm.size()).size(); j++)
        {
            midi.createNote(track, midi.tickFromTime(startNoteTime), midi.tickFromTime(stopNoteTime), 9, drumRithm.at(i % drumRithm.size()).at(j), 95, 95);
        }

        preTime = stopNoteTime;
    }
}




template<typename T>
T randFromArray(const QList<T> &list)
{
    if (list.isEmpty())
    {
        return T();
    }

    return list.at(qrand() % list.size());
}
