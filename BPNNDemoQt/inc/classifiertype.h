#ifndef CLASSIFIERTYPE_H
#define CLASSIFIERTYPE_H

#include <QFrame>

class ClassifierType : public QFrame
{
    Q_OBJECT
public:
    explicit ClassifierType(QWidget *parent=nullptr);
    int type() const;
    void setType(int type);

private:
    int mType;
protected:
    void paintEvent(QPaintEvent*);
};

#endif // CLASSIFIERTYPE_H
