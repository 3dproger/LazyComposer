#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "midi.h"
#include "midistorage.h"
#include "midiplayer.h"
#include "composer.h"
#include "titlegenerator.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void changePositionPlayer(float percentage);

private slots:
    void on_actionOpenMIDI_triggered();

    void on_btnPlay_clicked();

    void on_actionOpen_triggered();

    void on_actionSave_triggered();

    void compositionAppended(Composition*);

    void playingStopSlot();

    void updatePlayerProgress(int currentTime, int maxTime);

    void on_btnPause_clicked();

    void on_hsPlayerCurrentPos_sliderMoved(int position);

    void on_btnStructure_clicked();

    void on_actionAbout_triggered();

    void resultComposition(Composition*);

    void updateProgressComposition(int percentage);

    void playNext();

    void compose();

    void playStarted(Composition*);

    void onDevicesChanged();

    void onSelectDeviceTriggered();

    void onMidiPlayerError(MidiPlayer::Error error);

protected:
    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;

    Composition *currentSelectedComposition(int* position = nullptr);

    QSettings* _settings = new QSettings(this);//ToDo: no use pointer

    MidiStorage _midiStorage;
    MidiPlayer _midiPlayer {_settings, "midi_player"};

    QIcon _iconPlay = QIcon(":/resources/images/play.svg");

    int _currentTrack = 0;

    Composer* _composer = nullptr;

};

#endif // MAINWINDOW_H
