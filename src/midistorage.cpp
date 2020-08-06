#include "midistorage.h"
#include "utils.h"

MidiStorage::MidiStorage(QObject *parent) : QObject(parent)
{

}

void MidiStorage::append(QString fileName)
{
    QMidiFile* midiFile = new QMidiFile();
    if (!midiFile->load(fileName))
    {
        qDebug(QString("MidiStorage::append(QString): Failed to open file \"%1\"").arg(fileName).toUtf8());
        qCritical(tr("Failed to open file \"%1\"").arg(fileName).toUtf8());
        delete midiFile;
        return;
    }
    else
    {
        qDebug(QString("MidiStorage::append(QString): Successfully opened file \"%1\"").arg(fileName).toUtf8());
    }

    Composition* composition = new Composition();

    composition->midi = midiFile;
    composition->title = Utils::getFileNameFromPath(fileName);

    append(composition);
}

void MidiStorage::append(Composition* composition)
{
    _compositions.append(composition);

    emit appended(composition);
}

void MidiStorage::clear()
{
    for (Composition* composition : _compositions)
    {
        delete composition;
    }
    _compositions.clear();
}

Composition *MidiStorage::composition(const QString &title) const
{
    for (Composition* composition: _compositions)
    {
        if (composition->title == title)
        {
            return composition;
        }
    }


    return nullptr;
}

QList<Composition *> MidiStorage::compositions() const
{
    return _compositions;
}
