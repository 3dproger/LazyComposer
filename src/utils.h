#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QPushButton>
#include <QToolButton>

class Utils
{
public:
    Utils();


    static QString getFileNameFromPath(QString filePath);
    static QString getFileNameFromPathWitoutExtention(QString filePath);
    static QString fromUChar(const unsigned char *str);

    static QString getFileExtension(QString path);

    static QString removeFirstSpaces(QString s);
    static QString removeLastSpaces(QString s);
    static QByteArray removeFirstSpaces(QByteArray ba);
    static QByteArray removeLastSpaces(QByteArray ba);


    static float angle360(float x1, float y1, float x2, float y2);
    static float angle180(float x1, float y1, float x2, float y2);

    static void setButtonColor(QPushButton* button, const QColor& color);
    static void setButtonColor(QToolButton* button, const QColor& color);

    static void moveButtonCenter(QToolButton* button, int x, int y);

    static QString fromJSONArray(const QJsonArray& array);
    static QString prepareFloat(float value);
    static double getNumber(QByteArray s, int pos, bool *ok = nullptr);
    static QByteArrayList* prepareBAList(const QString& gcode);

    static bool containsLeft (QString string, QString substring, bool registerSensetive = true);
    static bool containsLeft (QByteArray ba, QByteArray subba, bool registerSensetive = true);

    static QColor colorMix(const QList<QColor> &colors);

    static int getQuadrantDeg(float angle);
    static int getQuadrantRad(float angle);

    static QString minimizeStringFloatNumber(QString text);

    static QString findFile(const QStringList& paths, const QString& fileName);
};

#endif // UTILS_H
