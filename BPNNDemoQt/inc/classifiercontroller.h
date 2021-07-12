#ifndef CLASSIFIERCONTROLLER_H
#define CLASSIFIERCONTROLLER_H

#include <QWidget>

namespace Ui {
class ClassifierController;
}

class ClassifierController : public QWidget
{
    Q_OBJECT

public:
    explicit ClassifierController(QWidget *parent = nullptr);
    ~ClassifierController();
    int getClassifierNum();
    QColor getClassifierColor(int type);
    int getClassifierCnt();
    void setAddDelButtonEnabled(bool b);
    void setCurrentClassNum(int);
    void setClassifierNum(int);
public slots:
    void onBtnAddClassClicked();
    void onBtnRemoveClassClicked();
    void onColorChanged(int,QRgb);
private:
    Ui::ClassifierController *ui;
    int mCnt=2;
signals:
    void classCntChanged(int);
    void colorChanged(int,QRgb);


};

#endif // CLASSIFIERCONTROLLER_H
