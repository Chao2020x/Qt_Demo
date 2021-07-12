#ifndef CLASSIFIERSELECTOR_H
#define CLASSIFIERSELECTOR_H

#include <QWidget>

namespace Ui {
class ClassifierSelector;
}

class ClassifierSelector : public QWidget
{
    Q_OBJECT

public:
    explicit ClassifierSelector(QWidget *parent = nullptr);
    ~ClassifierSelector();
    ClassifierSelector(int type, QColor color, QWidget *parent=nullptr);

    QColor color() const;
    void setColor(const QColor &color);
    void invalidate();

private:
    Ui::ClassifierSelector *ui;
    int mType;
    QColor mColor;
};

#endif // CLASSIFIERSELECTOR_H
