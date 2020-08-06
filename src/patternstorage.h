#ifndef PATTERNSTORAGE_H
#define PATTERNSTORAGE_H

#include <QObject>
#include "global.h"

struct Sequence{
    QString name;
    QString typeString = "generic";
    Signature signature;
    QList<QList<float>> list;
};

struct Rithm{
    QString name;
    QString typeString = "generic";
    Signature signature;
    QList<float> list;
};

struct Progression{
    QString name;
    Signature signature;
    QList<int> list;
};

struct Scale{
    QString name;
    QList<int> list;
};

class PatternStorage : public QObject
{
    Q_OBJECT
public:
    explicit PatternStorage(QObject *parent = nullptr);
    void load(QString fileName);

    QList<Sequence *> sequences() const;

    QList<Rithm *> rithms() const;

    QList<Progression *> progressions() const;

    QList<Scale *> scales() const;

signals:

public slots:

private:
    QList<Sequence*> _sequences;
    QList<Rithm*> _rithms;
    QList<Progression*> _progressions;
    QList<Scale*> _scales;
};

#endif // PATTERNSTORAGE_H
