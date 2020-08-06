#include "titlegenerator.h"

TitleGenerator::TitleGenerator(QObject *parent) : QObject(parent)
{

}

QString TitleGenerator::generate(){
    QString s;

    s += firstCharToUpper(makeWord());
    s += " ";
    s += firstCharToUpper(makeWord());

    s += " - ";

    s += firstCharToUpper(makeWord());
    s += " ";
    s += firstCharToUpper(makeWord());

    return s;
}

QString TitleGenerator::makeWord()
{
    QString word;

    int syllablesCount = 2 + qrand() % 3;

    for (int i = 0; i < syllablesCount; ++i){
        word += syllables.at(qrand() % syllables.count());
    }

    return word;
}

QString TitleGenerator::firstCharToUpper(QString text)
{
    if (!text.isEmpty())
    {
        QString c = text.at(0);
        c = c.toUpper();
        text = text.remove(0, 1);
        text = c + text;
    }

    return text;
}

