#include "mainwindow.h"
#include "titlegenerator.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QStandardPaths>
#include <QThreadPool>
#include <cmath>
#include "global.h"
#include "windowmidistructure.h"
#include <QMessageBox>
#include "defaults.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    #ifdef QT_NO_DEBUG
    ui->btnStructure->setVisible(false);
    #endif

    //ToDo
    ui->comboBox->setVisible(false);
    ui->label->setVisible(false);

    connect(&_midiStorage, SIGNAL(appended(Composition*)), this, SLOT(compositionAppended(Composition*)));

    connect(this, SIGNAL(playSignal(Composition*)), _midiPlayer, SLOT(play(Composition*)));
    connect(this, SIGNAL(changePositionPlayer(float)), _midiPlayer, SLOT(changePosition(float)));
    connect(this, SIGNAL(setPausePlayer(bool)), _midiPlayer, SLOT(setPause(bool)));
    connect(_midiPlayer, SIGNAL(updateProgress(int, int)), this, SLOT(updatePlayerProgress(int, int)));
    connect(_midiPlayer, SIGNAL(stopPlayingSignal()), this, SLOT(playingStopSlot()));
    connect(_midiPlayer, SIGNAL(playStarted(Composition*)), this, SLOT(playStarted(Composition*)));

    ui->progressBarComposition->setValue(0);

    QThreadPool::globalInstance()->start(_midiPlayer);

    playNext();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpenMIDI_triggered(){

}

void MainWindow::on_btnPlay_clicked(){
    int index;
    Composition* composition = currentSelectedComposition(&index);
    if (composition)
    {
        _currentTrack = _midiStorage.compositions().indexOf(composition);
        playNext();
    }
}

void MainWindow::on_actionOpen_triggered()
{
      QStringList fileNames = QFileDialog::getOpenFileNames(this, ("Open File"),
                                                      QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
                                                      ("MIDI (*.mid *.midi);;All files (*)"));

      for (const QString& fileName : fileNames)
      {
          if (!fileName.isEmpty())
          {
              _midiStorage.append(fileName);
          }
      }

}

void MainWindow::on_actionSave_triggered()
{
    Composition* composition = currentSelectedComposition();
    if (composition){
        QString fileName = QFileDialog::getSaveFileName(this, ("Save MIDI"),
                                                              QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/" + composition->title,
                                                              ("MIDI (*.mid *.midi);;All files (*)"));
        if (!fileName.isEmpty())
        {
            if (!composition->midi->save(fileName))
            {
                QMessageBox::critical(this, tr("Error"), tr("Failed to save file \"%1\"").arg(fileName));
            }
        }
    }
}

void MainWindow::compositionAppended(Composition* compisition)
{
    ui->listWidgetCompositions->addItem(compisition->title);
}

void MainWindow::playingStopSlot()
{
    ui->labelCurrentPlaying->setText("---");
    ui->hsPlayerCurrentPos->setValue(0);
    ui->labelPlayerTimeCurrent->setText(tr("0:00"));
    ui->labelPlayerTimeMaximum->setText(tr("0:00"));

    playNext();
}

void MainWindow::updatePlayerProgress(int currentTime, int maxTime)
{
    ui->hsPlayerCurrentPos->setMaximum(maxTime);
    ui->hsPlayerCurrentPos->setValue(currentTime);

    int currentSec = int(ceil(double(currentTime) / 1000.0)) % 60;
    int currentMin = int(ceil(double(currentTime) / 1000.0)) / 60;

    ui->labelPlayerTimeCurrent->setText(tr("%1:%2").arg(currentMin).arg(currentSec, 2, 10, QChar('0')));

    int maximumSec = int(ceil(double(maxTime) / 1000.0)) % 60;
    int maximumMin = int(ceil(double(maxTime) / 1000.0)) / 60;

    ui->labelPlayerTimeMaximum->setText(tr("%1:%2").arg(maximumMin).arg(maximumSec, 2, 10, QChar('0')));
}

void MainWindow::on_btnPause_clicked()
{
    emit setPausePlayer(true);
}

void MainWindow::on_hsPlayerCurrentPos_sliderMoved(int position)
{
    emit changePositionPlayer(float(position) / float(ui->hsPlayerCurrentPos->maximum()) * 100.0f);
}

void MainWindow::on_btnStructure_clicked()
{
    Composition* composition = currentSelectedComposition();
    if (composition)
    {
        windowMidiStructure->load(composition);
        windowMidiStructure->show();
    }
}

Composition *MainWindow::currentSelectedComposition(int* position)
{
    if (ui->listWidgetCompositions->currentRow() >= 0 && ui->listWidgetCompositions->currentRow() < ui->listWidgetCompositions->count())
    {
        Composition* composition = _midiStorage.composition(ui->listWidgetCompositions->currentItem()->text());
        if (position){
            *position = ui->listWidgetCompositions->currentRow();
        }
        return composition;
    }
    else{
        return nullptr;
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    _midiPlayer->destroy();
    _midiStorage.clear();

    QThread::msleep(100);

    QMainWindow::closeEvent(event);
}

void MainWindow::on_actionAbout_triggered()
{
    QString s;

    s += windowTitle() + "\n";

    s += tr("Author: Alexander Kirsanov") + "\n";

    s += tr("Version: %1").arg(APP_VERSION) + "\n";

    s += APP_YEAR;

    QMessageBox::about(this, tr("About"), s);
}

void MainWindow::resultComposition(Composition *composition)
{
    _composer = nullptr;
    ui->labelComposerActivity->setText(tr("I rest"));
    _midiStorage.append(composition);
    if (!_midiPlayer->isPlaying())
    {
        playNext();
    }
}

void MainWindow::updateProgressComposition(int percentage)
{
    ui->progressBarComposition->setValue(percentage);
}

void MainWindow::playNext()
{
    qDebug(QString("MainWindow::playNext(): _currentTrack: %1").arg(_currentTrack).toUtf8());

    if (_currentTrack < _midiStorage.compositions().count())
    {
        emit playSignal(_midiStorage.compositions().at(_currentTrack));
        _currentTrack++;
    }

    if (_currentTrack >= _midiStorage.compositions().count() && !_composer)
    {
        compose();
    }
}

void MainWindow::compose()
{
    ui->progressBarComposition->setValue(0);

    Composer* composer = new Composer();
    _composer = composer;
    ui->labelComposerActivity->setText(tr("I'm writing a new track"));

    connect(composer, SIGNAL(result(Composition*)), this, SLOT(resultComposition(Composition*)));
    connect(composer, SIGNAL(updateProgress(int)), this, SLOT(updateProgressComposition(int)));

    QThreadPool::globalInstance()->start(composer);
}

void MainWindow::playStarted(Composition* composition)
{
    bool found = false;
    for (int i = 0; i < ui->listWidgetCompositions->count(); ++i)
    {
        if (composition->title != ui->listWidgetCompositions->item(i)->text())
        {
            ui->listWidgetCompositions->item(i)->setIcon(QIcon());
        }
        else
        {
            ui->listWidgetCompositions->item(i)->setIcon(_iconPlay);
            found = true;
        }
    }
    ui->labelCurrentPlaying->setText(composition->title);

    if (!found)
    {
        qDebug(QString("MainWindow::playStarted(Composition*): Title \"%1\" not found in ui->listWidgetCompositions").arg(composition->title).toUtf8());
    }
}





