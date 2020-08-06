#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <QObject>
#include <QLocale>
#include <QTranslator>
#include "global.h"

class Translator : public QObject
{
    Q_OBJECT
public:
    explicit Translator(QObject *parent = nullptr);
    void changeLanguage(QLocale locale);
    void setSystemTranslation();

signals:

public slots:

private:
    QTranslator _appTranslator;
};

#endif // TRANSLATOR_H
