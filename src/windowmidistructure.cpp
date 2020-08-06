#include "windowmidistructure.h"
#include "ui_windowmidistructure.h"

WindowMidiStructure::WindowMidiStructure(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WindowMidiStructure)
{
    ui->setupUi(this);

}

WindowMidiStructure::~WindowMidiStructure()
{
    delete ui;
}

void WindowMidiStructure::load(Composition* composition)
{
    if (!composition)
    {
        qDebug("WindowMidiStructure::load(QMidiFile *): midi == nullptr");
        return;
    }

    //Information
    ui->tableWidgetInfo->clear();
    ui->tableWidgetInfo->setRowCount(0);
    ui->tableWidgetInfo->setColumnCount(2);

    ui->tableWidgetInfo->insertRow(0);
    ui->tableWidgetInfo->setItem(0, 0, new QTableWidgetItem(tr("Title:")));
    ui->tableWidgetInfo->setItem(0, 1, new QTableWidgetItem(composition->title));

    ui->tableWidgetInfo->insertRow(1);
    ui->tableWidgetInfo->setItem(1, 0, new QTableWidgetItem(tr("File Format:")));
    ui->tableWidgetInfo->setItem(1, 1, new QTableWidgetItem(QString("%1").arg(composition->midi->fileFormat())));

    ui->tableWidgetInfo->insertRow(2);
    ui->tableWidgetInfo->setItem(2, 0, new QTableWidgetItem(tr("Resolution:")));
    ui->tableWidgetInfo->setItem(2, 1, new QTableWidgetItem(QString("%1").arg(composition->midi->resolution())));

    ui->tableWidgetInfo->insertRow(3);
    ui->tableWidgetInfo->setItem(3, 0, new QTableWidgetItem(tr("Division Type:")));

    QString divisionTypeText;

    switch (composition->midi->divisionType()) {
    case QMidiFile::DivisionType::PPQ:
        divisionTypeText = "PPQ";
        break;

    case QMidiFile::DivisionType::Invalid:
        divisionTypeText = "Invalid";
        break;

    case QMidiFile::DivisionType::SMPTE24:
        divisionTypeText = "SMPTE24";
        break;

    case QMidiFile::DivisionType::SMPTE25:
        divisionTypeText = "SMPTE25";
        break;

    case QMidiFile::DivisionType::SMPTE30:
        divisionTypeText = "SMPTE30";
        break;

    case QMidiFile::DivisionType::SMPTE30DROP:
        divisionTypeText = "SMPTE30DROP";
        break;
    }

    ui->tableWidgetInfo->setItem(3, 1, new QTableWidgetItem(divisionTypeText));

    //Events
    _model->clear();
    ui->tableViewStructure->setModel(nullptr);

    QStringList horizontalHeader;
    horizontalHeader.append(tr("Tick"));
    horizontalHeader.append(tr("Type"));
    horizontalHeader.append(tr("Note"));
    horizontalHeader.append(tr("Velocity"));
    horizontalHeader.append(tr("Voice"));
    //horizontalHeader.append(tr("Value"));
    //horizontalHeader.append(tr("Amount"));
    horizontalHeader.append(tr("Number"));
    //horizontalHeader.append(tr("Numerator"));
    //horizontalHeader.append(tr("Denominator"));
    horizontalHeader.append(tr("Tempo"));
    horizontalHeader.append(tr("Track"));
    horizontalHeader.append(tr("Data"));
    horizontalHeader.append(tr("Message"));

    composition->midi->tracks();

    _model->setHorizontalHeaderLabels(horizontalHeader);

    for (int i = 0; i < composition->midi->events().count(); ++i)
    {
        QMidiEvent* event = composition->midi->events().at(i);

        QStandardItem* item;
        QString text;

        //Tick
        item = new QStandardItem(QString("%1").arg(event->tick()));
        _model->setItem(i, 0, item);

        //Type
        switch (event->type())
        {
        case QMidiEvent::EventType::Meta:
            text = "Meta";
            break;

        case QMidiEvent::EventType::SysEx:
            text = "SysEx";
            break;

        case QMidiEvent::EventType::NoteOn:
            text = "NoteOn";
            break;

        case QMidiEvent::EventType::Invalid:
            text = "Invalid";
            break;

        case QMidiEvent::EventType::NoteOff:
            text = "NoteOff";
            break;

        case QMidiEvent::EventType::PitchWheel:
            text = "PitchWheel";
            break;

        case QMidiEvent::EventType::KeyPressure:
            text = "KeyPressure";
            break;

        case QMidiEvent::EventType::ControlChange:
            text = "ControlChange";
            break;

        case QMidiEvent::EventType::ProgramChange:
            text = "ProgramChange";
            break;

        case QMidiEvent::EventType::ChannelPressure:
            text = "ChannelPressure";
            break;
        }

        item = new QStandardItem(text);
        _model->setItem(i, 1, item);

        //Note
        int note = event->note();
        if (note == -1 && _makerMinusOneEmpty){
            item = new QStandardItem("");
        }
        else{
            item = new QStandardItem(QString("%1").arg(note));
        }
        _model->setItem(i, 2, item);

        //Velocity
        int velocity = event->velocity();
        if (velocity == -1 && _makerMinusOneEmpty){
            item = new QStandardItem("");
        }
        else{
            item = new QStandardItem(QString("%1").arg(velocity));
        }
        _model->setItem(i, 3, item);

        //Voice
        int voice = event->voice();
        if (voice == -1 && _makerMinusOneEmpty){
            item = new QStandardItem("");
        }
        else{
            item = new QStandardItem(QString("%1").arg(voice));
        }
        _model->setItem(i, 4, item);

        /*//Value
        int value = event->value();
        if (value == -1 && _makerMinusOneEmpty){
            item = new QStandardItem("");
        }
        else{
            item = new QStandardItem(QString("%1").arg(value));
        }
        _model->setItem(i, 5, item);

        //Amount
        int amount = event->amount();
        if (amount == -1 && _makerMinusOneEmpty){
            item = new QStandardItem("");
        }
        else{
            item = new QStandardItem(QString("%1").arg(amount));
        }
        _model->setItem(i, 6, item);*/

        //Number
        int number = event->number();
        if (number == -1 && _makerMinusOneEmpty){
            item = new QStandardItem("");
        }
        else{
            QString numberText;
            switch (number) {
            case QMidiEvent::MetaNumbers::Lyric:
                numberText = "Lyric";
                break;

            case QMidiEvent::MetaNumbers::Tempo:
                numberText = "Tempo";
                break;

            case QMidiEvent::MetaNumbers::Marker:
                numberText = "Marker";
                break;

            case QMidiEvent::MetaNumbers::TrackName:
                numberText = "TrackName";
                break;

            case QMidiEvent::MetaNumbers::TimeSignature:
                numberText = "TimeSignature";
                break;

            default:
                numberText = "0x" + QString("%1 (%2)").arg(number, 8, 16, QChar('0')).arg(number);
                break;
            }
            item = new QStandardItem(numberText);
        }
        _model->setItem(i, 5, item);

        /*//Numerator
        item = new QStandardItem(QString("%1").arg(event->numerator()));
        _model->setItem(i, 8, item);

        //Denominator
        item = new QStandardItem(QString("%1").arg(event->denominator()));
        _model->setItem(i, 9, item);*/

        //Tempo
        float tempo = event->tempo();
        if (qFuzzyCompare(tempo, -1.0f) && _makerMinusOneEmpty){
            item = new QStandardItem("");
        }
        else{
            item = new QStandardItem(QString("%1").arg(double(tempo), 0, 'f', 3));
        }
        _model->setItem(i, 6, item);

        //Track
        item = new QStandardItem(QString("%1").arg(event->track()));
        _model->setItem(i, 7, item);

        //Data
        item = new QStandardItem(QString::fromUtf8(event->data()));
        _model->setItem(i, 8, item);

        //Message
        quint32 message = event->message();
        if (message == 0 && _makerMinusOneEmpty){
            item = new QStandardItem("");
        }
        else{
            item = new QStandardItem("0x" + QString("%1").arg(message, 8, 16, QChar('0')).toUpper());
        }
        _model->setItem(i, 9, item);
    }




    ui->tableViewStructure->setModel(_model);
}
