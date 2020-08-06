#include "translator.h"
#include <QApplication>
#include <QDebug>

Translator::Translator(QObject *parent) : QObject(parent)
{
    setSystemTranslation();
}

void Translator::changeLanguage(QLocale locale)
{
    QLocale().setDefault(locale);
    QString languageTag = QLocale().bcp47Name().toUtf8();

    //Change to source language
    if (languageTag.toLower() == "c" || languageTag.toLower() == "en"){
        qApp->removeTranslator(&_appTranslator);

    }
    else{
        //Change to othes languages

        QString app_lan = "app_" + languageTag;

        if (_appTranslator.load(app_lan,"translations/")){
            qApp->installTranslator(&_appTranslator);
        }
        else{
            qDebug(QString("Translator::changeLanguage(): Can't find application translation: \"%1\"").arg(languageTag).toUtf8());
            return;
        }

    }
}

void Translator::setSystemTranslation()
{
    QLocale().setDefault(QLocale::system().name());
    QString languageTag = QLocale().bcp47Name().toUtf8();

    if (languageTag.toLower() == "c" || languageTag.toLower() == "en"){
        qApp->removeTranslator(&_appTranslator);
    }
    else if (languageTag == "ru")//ToDo: автоматизировать для других языков
    {
        if (_appTranslator.load(":/LazyComposer_ru_RU.qm")){
            qApp->installTranslator(&_appTranslator);
        }
        else{
            qDebug(QString("Can't find application translation: \"%1\"").arg(languageTag).toUtf8());
        }
    }
}
