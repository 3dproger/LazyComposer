#include "utils.h"
#include <QDir>
#include <cmath>
#include <QLocale>
#include <QSettings>
#include <QJsonDocument>
//#include "defaults.h"
#include "global.h"
#define PI 3.14159265359f

Utils::Utils()
{

}

typedef char* charArray;
QString Utils::fromUChar(const unsigned char *str){
   return QString(charArray(str));
}

QString Utils::getFileNameFromPath(QString filePath){
    if (filePath.contains('/')){
        filePath = filePath.mid(filePath.lastIndexOf('/') + 1);
    }
    return filePath;
}

QString Utils::getFileNameFromPathWitoutExtention(QString filePath){
    QString result;
    result = getFileNameFromPath(filePath);
    if (result.contains('.')){
        result = result.left(result.lastIndexOf('.'));
    }
    return result;
}

QString Utils::getFileExtension(QString path){
    path = path.replace('\\','/');
    if (path.contains("/")){
        path = path.remove(0,path.lastIndexOf("/")+1);
    }
    if (path.contains(".")){
        path = path.remove(0,path.lastIndexOf(".")+1);
        path = path.toLower();
        return path;
    }
    else{
        return "";
    }
}

QString Utils::removeFirstSpaces(QString s){
    //removing first spaces
    for (int i = 0; i<s.length(); ++i){
        if (s.mid(i,1)==" "){
            s = s.remove(0,1);
        }
        else{
            break;
        }
    }
    return s;
}

QString Utils::removeLastSpaces(QString s){
    //removing last spaces
    for (int i = s.length()-1; i>0; --i){
        if (s.mid(i,1)==" "){
            s = s.remove(i,1);
        }
        else{
            break;
        }
    }
    return s;
}

QByteArray Utils::removeFirstSpaces(QByteArray ba){
    //ToDo:
    //QString s = QString::fromUtf8(ba);
    //removing first spaces
    for (int i = 0; i<ba.length(); ++i){
        if (ba.mid(i,1)==" "){
            ba = ba.remove(0,1);
        }
        else{
            break;
        }
    }
    return ba;
}

QByteArray Utils::removeLastSpaces(QByteArray ba){
    //ToDo:
    //QString s = QString::fromUtf8(ba);
    //removing last spaces
    for (int i = ba.length()-1; i>0; --i){
        if (ba.mid(i,1)==" "){
            ba = ba.remove(i,1);
        }
        else{
            break;
        }
    }
    return ba;
}

float Utils::angle360(float x1, float y1, float x2, float y2){
    return -atan2f(y1 - y2, x1 - x2) * 180.0f / PI + 180.0f;
}

float Utils::angle180(float x1, float y1, float x2, float y2){
    if (y2 <= y1){
        return -atan2f(y1 - y2, x1 - x2) * 180.0f / PI + 180.0f;
    }
    else{
        return -(atan2f(y1 - y2, x1 - x2) * 180.0f / PI + 180.0f);
    }
}

void Utils::setButtonColor(QPushButton* button, const QColor& color){
    if (button == nullptr){
        return;
    }
    button->setStyleSheet(QString::fromUtf8("background-color: rgb(%1, %2, %3);"
                                            "border: 1px solid #8f8f91;"
                                            "padding: 2px;")
                          .arg(color.red())
                          .arg(color.green())
                          .arg(color.blue())
                          );
}
void Utils::setButtonColor(QToolButton* button, const QColor& color){
    if (button == nullptr){
        return;
    }
    button->setStyleSheet(QString::fromUtf8("background-color: rgb(%1, %2, %3);"
                                            "border: 1px solid #8f8f91;"
                                            "padding: 2px;")
                          .arg(color.red())
                          .arg(color.green())
                          .arg(color.blue())
                          );
}

QString Utils::fromJSONArray(const QJsonArray& array){
    QJsonDocument doc;
    doc.setArray(array);
    return doc.toJson();
}

QString Utils::prepareFloat(float value){
    QString line = QString("%1").arg(double(value));
    if (line.contains(".")){
        line = line.replace(".",QLocale::system().decimalPoint());
    }
    else{
        line = QString("%1%2%3").arg(line)
                .arg(QLocale::system().decimalPoint())
                .arg("0");
    }
    return line;
}

double Utils::getNumber(QByteArray s, int pos, bool *ok){
    const QByteArray d = "-+.0123456789";
    double num = 0;

    s = s.mid(pos, -1);
    int pos2 = 0;
    for (int i = 0; i < s.size(); ++i){
        if (d.contains(s.at(i))){
            pos2 = i;
        }
        else{
            break;
        }
    }
    s = s.left(pos2 + 1);
    num = s.toDouble(ok);

    //qInfo("mid = \"" + s + "\"");

    return num;
}

QByteArrayList* Utils::prepareBAList(const QString& gcode){
    QByteArrayList* baList = new QByteArrayList();
    QString s;
    int pos = 0;
    int pos2 = 0;

    for (int i = 0; i <= gcode.count('\n'); ++i){
        pos2 = gcode.indexOf("\n", pos);
        s = gcode.mid(pos, pos2 - pos);
        pos = pos2 + 1;

        s = s.left(s.indexOf(";"));
        s = s.remove("\n");
        s = s.remove("\r");
        //removing first spaces
        s = Utils::removeFirstSpaces(s);
        //removing last spaces
        s = Utils::removeLastSpaces(s);

        if (s.isEmpty() || s.isNull()){
            continue;
        }
        //qInfo(s.toUtf8());
        baList->append(s.toUtf8());
    }

    return baList;
}

bool Utils::containsLeft  (QString string, QString substring, bool registerSensetive){
    if (!registerSensetive){
        string = string.toLower();
        substring = substring.toLower();
    }
    /*qInfo(QString("string = \"%1\" (%2), substring = \"%3\", equally = %4")
          .arg(string)
          .arg(string.left(substring.length()))
          .arg(substring)
          .arg(string.left(substring.length()) == substring)
          .toUtf8());*/
    return string.left(substring.length()) == substring;
}

bool Utils::containsLeft  (QByteArray ba, QByteArray subba, bool registerSensetive){
    if (!registerSensetive){
        ba = ba.toLower();
        subba = subba.toLower();
    }

    return ba.left(subba.length()) == subba;
}

QColor Utils::colorMix(const QList<QColor> &colors){
    if (colors.isEmpty()){
        return QColor (0, 0, 0);
    }
    int r = 0;
    int g = 0;
    int b = 0;

    for (const QColor& color : colors){
        r += color.red()   * color.alphaF();
        g += color.green() * color.alphaF();
        b += color.blue()  * color.alphaF();
    }

    return QColor(r / colors.size(), g / colors.size(), b / colors.size());
}

int Utils::getQuadrantDeg(float angle){
    float rad = angle * PI / 180.0f;
    if (cosf(rad) >= 0.0f){
        if (sinf(rad) >= 0.0f){
            return 1;
        }
        else{
            return 4;
        }
    }
    else{
        if (sinf(rad) >= 0.0f){
            return 2;
        }
        else{
            return 3;
        }
    }
}
int Utils::getQuadrantRad(float rad){
    if (cosf(rad) >= 0.0f){
        if (sinf(rad) >= 0.0f){
            return 1;
        }
        else{
            return 4;
        }
    }
    else{
        if (sinf(rad) >= 0.0f){
            return 2;
        }
        else{
            return 3;
        }
    }
}

void Utils::moveButtonCenter(QToolButton* button, int x, int y){
    if (!button){
        return;
    }
    button->move(x - button->width() / 2, y - button->height() / 2);
}

QString Utils::minimizeStringFloatNumber(QString s){
    if (s.isEmpty()){
        return QString("0");
    }
    if (s.contains('.')){
        //Right zeros
        int pointPos = s.indexOf('.');
        if (pointPos == 0){
            //.123
            s = '0' + s;
        }

        if (pointPos == s.length() - 1){
            //123.
            s = s.left(s.length() - 1);
        }
        else{
            //123.010 => 123.01
            int lastNotZero = pointPos + 1;
            for (int i = lastNotZero; i < s.length(); ++i){
                if (s.at(i) != '0'){
                    lastNotZero = i + 1;
                }
            }

            s = s.left(lastNotZero);


            if (s.indexOf('.') == s.length() - 1){
                s = s.left(s.length() - 1);
            }
        }
    }

    //Left Zeros
    int zeros = 0;
    for (int i = 0; i < s.length(); ++i){
        if (s.at(i) == '0'){
            zeros++;
        }
        else{
            break;
        }
    }
    s = s.mid(zeros);

    if (s.isEmpty() || s.at(0) == '.'){
        s = '0' + s;
    }

    /*if (s.indexOf(s.length() - 1) == '.'){
        s = s.left(s.length() - 1);
    }*/

    return s;
}

QString Utils::findFile(const QStringList& paths, const QString& fileName){
    for (const QString& path : paths){
        QDir dir(path);
        //qDebug(QString("Utils::findFile(): \"%1\"").arg(dir.absolutePath() + "/" + fileName).toUtf8());
        if (dir.exists()){
            QFile file(dir.absolutePath() + "/" + fileName);
            if (file.exists()){
                return dir.absolutePath() + "/" + fileName;
            }
        }
    }
    return QString();
}
