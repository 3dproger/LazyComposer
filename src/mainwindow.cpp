#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "global.h"
#include "windowmidistructure.h"
#include "defaults.h"
#include <QMessageBox>
#include <QDebug>
#include <QFileDialog>
#include <QStandardPaths>
#include <QThreadPool>
#include <cmath>

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

    connect(_midiPlayer, &MidiPlayer::updateProgress,    this, &MainWindow::updatePlayerProgress);
    connect(_midiPlayer, &MidiPlayer::stopped,           this, &MainWindow::onStopped);
    connect(_midiPlayer, &MidiPlayer::playStarted,       this, &MainWindow::playStarted);
    connect(_midiPlayer, &MidiPlayer::error,             this, &MainWindow::onMidiPlayerError);
    connect(_midiPlayer, &MidiPlayer::devicesChanged,    this, &MainWindow::onDevicesChanged);
    onDevicesChanged();

    ui->progressBarComposition->setValue(0);

    QThreadPool::globalInstance()->start(_midiPlayer);

    playNext();
}

MainWindow::~MainWindow()
{
    if (!QThreadPool::globalInstance()->waitForDone(2000))
    {
        qDebug("%s: Failed to wait stop QThreadPool", Q_FUNC_INFO);
    }
    delete ui;
}

void MainWindow::on_btnPlay_clicked(){
    int index;
    Composition* composition = currentSelectedComposition(&index);
    if (composition && composition != _midiPlayer->currentComposition())
    {
        _currentTrack = _midiStorage.compositions().indexOf(composition);
        playNext();
    }

    _midiPlayer->setPause(false);
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

void MainWindow::onStopped()
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
    MidiPlayer::Error error = _midiPlayer->setPause(true);
    if (error.type() != MidiPlayer::ErrorType::NoError)
    {
        QMessageBox::warning(this, tr("MIDI Player Error"), error.text());
    }
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
        if (!windowMidiStructure)
        {
            windowMidiStructure = new WindowMidiStructure(this);
        }

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

    s += tr("Version: %1").arg(LazyComposer::APP_VERSION) + "\n";

    s += tr("Alexander Kirsanov (c) %1").arg(LazyComposer::APP_YEAR) + "\n";

    s += LazyComposer::ORGANIZATION_DOMAIN;

    QMessageBox::about(this, tr("About"), s);
}

void MainWindow::resultComposition(Composition *composition)
{
    _composer = nullptr;
    ui->labelComposerActivity->setText(tr("I rest"));
    _midiStorage.append(composition);
    if (!_midiPlayer->isPlaying() && !_midiPlayer->isPause())
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
        MidiPlayer::Error error = _midiPlayer->play(_midiStorage.compositions().at(_currentTrack));
        if (error.type() != MidiPlayer::ErrorType::NoError)
        {
            QMessageBox::warning(this, tr("MIDI Player Error"), error.text());
        }

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

void MainWindow::onDevicesChanged()
{
    ui->menuMidiDevice->clear();

    if (!_midiPlayer->devices().isEmpty())
    {
        for (const auto& device : _midiPlayer->devices())
        {
            QString text = device.id;
            if (!device.name.isEmpty())
            {
                text = device.name;
            }

            QAction* action = new QAction(text, ui->menuMidiDevice);
            if (device.isConnected)
            {
                action->setIcon(QIcon(":/resources/images/play.svg"));
            }

            action->setData(device.id);
            connect(action, &QAction::triggered, this, &MainWindow::onSelectDeviceTriggered);
            ui->menuMidiDevice->addAction(action);
        }
    }
    else
    {
        QAction* action = new QAction(tr("MIDI devices not found"), ui->menuMidiDevice);
        action->setEnabled(false);
        ui->menuMidiDevice->addAction(action);
    }
}

void MainWindow::onSelectDeviceTriggered()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action)
    {
        QString           deviceName = action->text();
        QString           deviceId   = action->data().toString();
        MidiPlayer::Error error      = _midiPlayer->setDevice(action->data().toString());

        if (error.type() != MidiPlayer::ErrorType::NoError)
        {
            QMessageBox::critical(this, tr("Error"), error.text());
        }
    }
    else
    {
        qCritical() << Q_FUNC_INFO << ": action == nullptr";
    }
}

void MainWindow::onMidiPlayerError(MidiPlayer::Error error)
{
    QMessageBox::warning(this, tr("MIDI Player Error"), error.text());
}
