#ifndef TITLEGENERATOR_H
#define TITLEGENERATOR_H

#include <QObject>

class TitleGenerator : public QObject
{
    Q_OBJECT
public:
    explicit TitleGenerator(QObject *parent = nullptr);
    QString generate();

signals:

public slots:

protected:
    QString makeWord();
    QString firstCharToUpper(QString text);

    QStringList syllables = {"ma", "key", "io", "bra", "e", "a", "o", "un", "oe", "tion", "wa", "ex", "es", "se", "fu", "da", "xor"};
};

#endif // TITLEGENERATOR_H
