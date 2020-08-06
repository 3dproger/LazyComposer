#ifndef MIDISTORAGE_H
#define MIDISTORAGE_H

#include <QObject>
#include <QMidiFile.h>
#include "global.h"

class MidiStorage : public QObject
{
    Q_OBJECT
public:
    explicit MidiStorage(QObject *parent = nullptr);
    void append(QString fileName);
    void append(Composition* composition);
    void clear();

    Composition* composition(const QString& title) const;
    QList<Composition *> compositions() const;

signals:
    void appended (Composition*);
    void removed  (Composition*);

public slots:

private:
    QList<Composition*> _compositions;

};

#endif // MIDISTORAGE_H
