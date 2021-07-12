#ifndef CLASSIFIERSELECTORLIST_H
#define CLASSIFIERSELECTORLIST_H
#include "classifierselector.h"
#include <QListWidget>

class ClassifierSelectorList : public QListWidget
{
    Q_OBJECT

public:
    explicit ClassifierSelectorList(QWidget *parent = nullptr);
    ~ClassifierSelectorList();
    void addItemByNum(int num);
    void removeItem();

    ClassifierSelector *getItem(int row);

signals:
    void colorChanged(int,QRgb);
private:
    QStringList mColors;
    void setColorByIndex(int,QColor);

protected:
    void mouseDoubleClickEvent(QMouseEvent*);
    void mousePressEvent(QMouseEvent*);
};

#endif // CLASSIFIERSELECTORLIST_H
