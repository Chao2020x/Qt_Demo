#ifndef CLASSIFERCOLOR_H
#define CLASSIFERCOLOR_H

#include <QFrame>
#include <QColor>
class ClassifierColor : public QFrame
{
    Q_OBJECT
public:
    explicit ClassifierColor(QWidget *parent=nullptr);


    QColor color() const;
    void setColor(const QColor &color);

private:
    QColor mColor;
protected:
    void paintEvent(QPaintEvent*);
};

#endif // CLASSIFERCOLOR_H
