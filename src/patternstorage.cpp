#include "patternstorage.h"
#include <QJsonDocument>
#include <QJsonParseError>
#include <QFile>

PatternStorage::PatternStorage(QObject *parent) : QObject(parent)
{

}

void PatternStorage::load(QString fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)){
        qDebug("Failed to open file");
        return;
    }

    QByteArray ba = file.readAll();
    file.close();

    QJsonParseError error;
    QJsonDocument::fromJson(ba, &error);

}

QList<Sequence *> PatternStorage::sequences() const
{
    return _sequences;
}

QList<Rithm *> PatternStorage::rithms() const
{
    return _rithms;
}

QList<Progression *> PatternStorage::progressions() const
{
    return _progressions;
}

QList<Scale *> PatternStorage::scales() const
{
    return _scales;
}
