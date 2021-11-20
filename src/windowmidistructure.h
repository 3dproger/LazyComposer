#ifndef WINDOWMIDISTRUCTURE_H
#define WINDOWMIDISTRUCTURE_H

#include "global.h"
#include <QMidiFile.h>
#include <QMainWindow>
#include <QStandardItemModel>

namespace Ui {
class WindowMidiStructure;
}

class WindowMidiStructure : public QMainWindow
{
    Q_OBJECT

public:
    explicit WindowMidiStructure(QWidget *parent = nullptr);
    ~WindowMidiStructure();
    void load(Composition* composition);

private:
    Ui::WindowMidiStructure *ui;
    QStandardItemModel* _model = new QStandardItemModel(this);

    bool _makerMinusOneEmpty = true;
};

#endif // WINDOWMIDISTRUCTURE_H
