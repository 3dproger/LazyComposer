#include "mainwindow.h"
#include "defaults.h"
#include <QApplication>
#include <QTranslator>

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

    MainWindow windowMain;
    windowMain.show();

    return a.exec();
}
