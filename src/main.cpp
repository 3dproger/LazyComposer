#include "mainwindow.h"
#include <QApplication>
#include "windowmidistructure.h"
#include "translator.h"
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

    translator = new Translator();

    windowMain = new MainWindow();
    windowMidiStructure = new WindowMidiStructure(windowMain);


    windowMain->show();

    int result = a.exec();

    delete windowMain;
    delete translator;

    return result;
}
