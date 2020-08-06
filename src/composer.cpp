#include "composer.h"
#include <QDateTime>

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

    _tempo = 160 + qrand() % 15;// 120 + qrand() % 50;

    _duration1_1 = 240.0f / _tempo;
    _duration1_2 = 120.0f / _tempo;
    _duration1_4 = 60.0f  / _tempo;
    _duration1_6 = 40.0f  / _tempo;
    _duration1_8 = 30.0f  / _tempo;


    _transposition = qrand() % 12;
    QList<int> progression = _progressions.at(qrand() % _progressions.size());
    QList<QList<int>> accomponiment = {{0}, {4}, {7}, {4}, {9}, {4}, {7}, {4}};
    QList<QList<int>> accomponimentDrive1 = {{0, 4, 7}};
    QList<QList<int>> accomponimentDrive2 = {{0}, {0}, {0, 4, 7}, {0}, {0}, {0, 4, 7}, {0}, {0, 4, 7}};
    //QList<QList<int>> accomponiment = {{0, 4, 9}, {0, 4, 9}, {0, 4, 9}, {0, 4, 9}, {0, 4, 7, 9}, {0, 4, 9}, {0, 4, 9}, {0, 4, 9}};

    QMidiFile* midi = new QMidiFile();

    midi->setResolution(480);

    int trackPiano = midi->createTrack();
    midi->createMetaEvent(trackPiano, 0, QMidiEvent::MetaNumbers::TrackName, "Piano Accomp");

    int trackGuitar = midi->createTrack();
    midi->createProgramChangeEvent(trackGuitar, 0, trackGuitar, 24);
    midi->createMetaEvent(trackGuitar, 0, QMidiEvent::MetaNumbers::TrackName, "Guitar Accomp");

    int trackGuitarDrive = midi->createTrack();
    midi->createProgramChangeEvent(trackGuitarDrive, 0, trackGuitarDrive, 28);
    midi->createMetaEvent(trackGuitarDrive, 0, QMidiEvent::MetaNumbers::TrackName, "Guitar Drive");

    int trackBass = midi->createTrack();
    midi->createProgramChangeEvent(trackBass, 0, trackBass, 33);
    midi->createMetaEvent(trackBass, 0, QMidiEvent::MetaNumbers::TrackName, "Bass");

    int trackDrumkit = midi->createTrack();
    midi->createProgramChangeEvent(trackDrumkit, 0, 9, 0);
    midi->createMetaEvent(trackDrumkit, 0, QMidiEvent::MetaNumbers::TrackName, "Drumkit");

    //Tempo
    midi->createTempoEvent(0, 0, _tempo);


    //QList<int> rithm = {8, 16, 16, 8, 16, 16, 8, 16, 16, 8, 16, 16}; //Horse 1
    //QList<float> rithm = {6, 6, 6, 6, 8, 8}; //Club 1
    //QList<float> rithm = {1};                    //Basic 1
    //QList<float> rithm = {2, 2};                 //Basic 2
    //QList<float> rithm = {4, 4, 4, 4};           //Basic 3
    QList<float> rithm = {8, 8, 8, 8, 8, 8, 8, 8}; //Basic 4

    float time = 0.0f;

    int repeats = 2; // повторы
    int progressionSteps = 4; //размер прогрессии
    int dropSize = 4; //размер дропа
    int hollowSize1 = 2;
    int hollowSize2 = 2;

    int tackts = ((dropSize + hollowSize1 + hollowSize2) * progressionSteps) * repeats;
    int tacktCouter = 0;

    for (int repeat = 0; repeat < repeats; ++repeat){
        for (int i = 0; i < dropSize; ++i)
        {
            for (int progressionStep = 0; progressionStep < progressionSteps; ++progressionStep){


                makeBass(*midi, time, trackBass, progression, progressionStep, rithm, -3);

                makeAccomp(*midi, time, trackGuitar, progression, progressionStep, {8, 8, 8, 8, 8, 8, 8, 8}, accomponiment);
                makeAccomp(*midi, time, trackGuitarDrive, progression, progressionStep, {8, 8, 8, 8, 8, 8, 8, 8}, accomponimentDrive1);

                makeMelody(*midi, time, trackPiano, 0);
                makeMelody(*midi, time, trackPiano, 1);
                makeMelody(*midi, time, trackGuitar, 1);

                makeDrums(*midi, time, trackDrumkit);

                time += _duration1_1;
                tacktCouter++;
                emit updateProgress(tacktCouter * 100 / tackts);
            }

        }

        for (int i = 0; i < hollowSize1; ++i)
        {
            for (int progressionStep = 0; progressionStep < progressionSteps; ++progressionStep){


                makeBass(*midi, time, trackBass, progression, progressionStep, rithm, -3);

                makeAccomp(*midi, time, trackGuitar, progression, progressionStep, {8, 8, 8, 8, 8, 8, 8, 8}, accomponiment);

                makeMelody(*midi, time, trackPiano, 0);

                makeDrums(*midi, time, trackDrumkit);

                time += _duration1_1;
                tacktCouter++;
                emit updateProgress(tacktCouter * 100 / tackts);
            }
        }

        for (int i = 0; i < hollowSize2; ++i)
        {
            for (int progressionStep = 0; progressionStep < progressionSteps; ++progressionStep){


                makeBass(*midi, time, trackBass, progression, progressionStep, rithm, -3);

                makeAccomp(*midi, time, trackGuitar, progression, progressionStep, {8, 8, 8, 8, 8, 8, 8, 8}, accomponiment);
                makeAccomp(*midi, time, trackGuitarDrive, progression, progressionStep, {8, 8, 8, 8, 8, 8, 8, 8}, accomponimentDrive2);

                makeMelody(*midi, time, trackPiano, 0);
                makeMelody(*midi, time, trackGuitar, 1);

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
        float beat = fractionList.at(qrand() % fractionList.count());

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



