#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include "windowmidistructure.h"
#include "defaults.h"

WindowMidiStructure* windowMidiStructure = nullptr;
MainWindow* windowMain = nullptr;
Translator* translator = nullptr;

int main(int argc, char *argv[])
{
    QApplication::setApplicationName(LazyComposer::APP_NAME);
    QApplication::setApplicationVersion(LazyComposer::APP_VERSION);
    QApplication::setOrganizationName(LazyComposer::ORGANIZATION_NAME);
    QApplication::setOrganizationDomain(LazyComposer::ORGANIZATION_DOMAIN);

    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "LazyComposer_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    windowMain = new MainWindow();
    windowMidiStructure = new WindowMidiStructure(windowMain);


    windowMain->show();

    int result = a.exec();

    delete windowMain;

    return result;
}
