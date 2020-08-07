#include "titlegenerator.h"
#include "defaults.h"

TitleGenerator::TitleGenerator(QObject *parent) : QObject(parent)
{

}

QString TitleGenerator::generate(){
    QString s = LazyComposer::APP_NAME;

    s += " - ";

    s += firstCharToUpper(makeWord());
    s += " ";
    s += firstCharToUpper(makeWord());

    return s;
}

QString TitleGenerator::makeWord()
{
    QString word;

    int syllablesCount = 2 + qrand() % 5;

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

